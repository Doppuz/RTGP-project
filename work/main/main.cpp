
// Std. Includes
#include <string>
#include <stdlib.h>
#include <ctime>
#include <cmath>

#ifdef _WIN32
    #define APIENTRY __stdcall
#endif

#include <glad/glad.h>

#include <glfw/glfw3.h>

#ifdef _WINDOWS_
    #error windows.h was included!
#endif

#include <utils/shader_v1.h>
#include <utils/model_v1.h>
#include <utils/camera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

// we include the library for images loading
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <utils/mesh_v1.h>

#include <utils/FastNoiseLite.h>

#include <utils/terrain.h>
#include <utils/terrainManagement.h>

#include <utils/waterFrameBuffer.h>

#include <utils/worldobject.h>

// dimensions of application's window
GLuint screenWidth = 1600, screenHeight = 800;

// callback function for keyboard events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// callback for mouse events
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// load image from disk and create an OpenGL texture
GLint LoadTexture(const char* path);
//FPS
void calculateFPS();
//TerrainPos
void changeTerrainPos(TerrainManagement* manager);
//SkyBox
void LoadTextureCubeSide(string path, string side_image, GLuint side_name);
GLint LoadTextureCube(string path);
//Render
void terrainRender(Shader shader, glm::vec3 fogColor, GLFWwindow* window, TerrainManagement terrainManager, glm::vec4 plane,bool boolCamera);
void skyBoxRenderer(Shader skybox_shader, glm::vec3 fogColor,Model* cubeModel);
//subroutine
void SetupShader(int program);
void PrintCurrentShader(int subroutine);
void lightShaderSetUp(Shader shaderParam);

//Movement with wasd
void apply_camera_movements(GLFWwindow* window);

//First movement bool
bool firstMouse = true;

// we need to store the previous mouse position to calculate the offset with the current frame
GLfloat lastX, lastY;

//Projection matrix
glm::mat4 projection;

//View matrix
glm::mat4 view = glm::mat4(1.0f);

//Camera
Camera camera(glm::vec3(0.0f, 300.0f, 0.0f), GL_FALSE); //25000,20000,25000

// parameters for time calculation (for animations)
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//FPS
double lastTime = glfwGetTime();
int nbFrames = 0;
int initialTime = time(NULL), finalTime, frameCount;

//light
glm::vec3 lightPos = glm::vec3(300,500,100000);
GLfloat Kd = 0.5f;

// boolean to activate/deactivate wireframe rendering
GLboolean wireframe = GL_FALSE;

// texture unit for the cube map
GLuint textureCube;

//Subroutine
GLuint current_subroutine = 0;
vector<std::string> shaders;

//texture
GLint groundTexture;
GLint sandTexture;

//Water
float waterMovement = 0.0f;
const float waterSpeed = 0.03f;
const glm::vec3 lightColour = glm::vec3({0.70f,0.90f,1.f});//glm::vec3({0.50f,0.83f,1.f});

//Near Far plane
const float near = 0.1f;
const float far = 8000.0f;

/////////////////// MAIN function ///////////////////////
int main(){

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // we set if the window is resizable
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // we create the application's window
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Project", nullptr, nullptr);
    if (!window){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // we put in relation the window and the callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
 
    // GLAD tries to load the context set by GLFW
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // we define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // we enable Z test
    glEnable(GL_DEPTH_TEST);

    // we create and compile shaders (code of Shader class is in include/utils/shader_v1.h)
    Shader terrainShader("terrainVert.vert", "terrainFrag.frag");
    Shader waterShader("waterVert.vert", "waterFrag.frag");
    Shader objectShader("objectVert.vert", "objectFrag.frag");
    Shader skybox_shader("skyboxVert.vert", "skyboxFrag.frag");

    SetupShader(terrainShader.Program);
    // we print on console the name of the first subroutine used
    PrintCurrentShader(current_subroutine);

    sandTexture = LoadTexture("../../textures/plane/desert.jpg");
    
    groundTexture = LoadTexture("../../textures/plane/desert2.jpg");

    textureCube = LoadTextureCube("../../textures/cube/MySkyBox2/");
  
    GLint waterDuDvTexture = LoadTexture("../../textures/plane/waterDUDV.png");
    
    GLint waterNormalTexture = LoadTexture("../../textures/plane/normalMap.png");

    GLint trunkTexture = LoadTexture("../../textures/plane/trunk.jpg");
    
    GLint cactusTexture = LoadTexture("../../textures/plane/cactus.png");

    Model cubeModel("../../models/cube.obj"); // used for the environment map

    Model tree("../../models/Tree/tree.obj"); // used for the environment map

    Model cactus("../../models/cactus.3ds"); // used for the environment map
  
    TerrainManagement terrainManager(&tree,&cactus);

    bool first = false;
    
    glm::mat4 cubeModelMatrix = glm::mat4(1.0f);
    cubeModelMatrix = glm::translate(cubeModelMatrix,glm::vec3(0.0f, 8000.0f,120000));
    cubeModelMatrix = glm::scale(cubeModelMatrix, glm::vec3(500000,50000,250000));

    glm::vec3 fogColor = {0.05f,0.08f,0.13f};

    WaterFrameBuffers fbos = WaterFrameBuffers();

    GLuint index;

    while(!glfwWindowShouldClose(window)){

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        calculateFPS();

        glEnable(GL_CLIP_DISTANCE0);

        glfwPollEvents();
        
        apply_camera_movements(window);

        changeTerrainPos(&terrainManager);

        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//Reflaction

        fbos.bindReflectionFrameBuffer();

        camera.Position.y -= 30;

        terrainRender(terrainShader,fogColor,window,terrainManager, glm::vec4(0,1,0,-10),true);

        camera.Position.y += 30;

        objectShader.Use();
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);   

        lightShaderSetUp(objectShader);

        index = glGetSubroutineIndex(terrainShader.Program, GL_FRAGMENT_SHADER, shaders[current_subroutine].c_str());
        glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &index);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, trunkTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cactusTexture);
    
        objectShader.setInt("trunkTexture",0);
        objectShader.setInt("cactusTexture",1);
        
        for(int i = 0; i < terrainManager.terrains.size(); i++){
            if(terrainManager.terrains[i].hasObject){
                objectShader.setInt("objectType", terrainManager.terrains[i].objectType);
                objectShader.setMat4("model", terrainManager.terrains[i].object.getModelMatrix());    
                objectShader.setMat3("normalMatrix",terrainManager.terrains[i].object.getNormalMatrix(view));
                terrainManager.terrains[i].object.draw();
            }    
        }
        
        objectShader.setVec3("fogColor",fogColor);

        skyBoxRenderer(skybox_shader,fogColor,&cubeModel);

//Refraction

        fbos.bindRefractionFrameBuffer();

        terrainRender(terrainShader,fogColor,window,terrainManager, glm::vec4(0,-1,0,10),false);

        skyBoxRenderer(skybox_shader,fogColor,&cubeModel);

        fbos.unbindCurrentFrameBuffer();

        terrainRender(terrainShader,fogColor,window,terrainManager, glm::vec4(0,-1,0,100000),false);

//Object render

        objectShader.Use();
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);   

        lightShaderSetUp(objectShader);

        index = glGetSubroutineIndex(terrainShader.Program, GL_FRAGMENT_SHADER, shaders[current_subroutine].c_str());
        glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &index);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, trunkTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cactusTexture);

        objectShader.setInt("trunkTexture",0);
        objectShader.setInt("cactusTexture",1);

        for(int i = 0; i < terrainManager.terrains.size(); i++){
            if(terrainManager.terrains[i].hasObject){
                objectShader.setInt("objectType", terrainManager.terrains[i].objectType);
                objectShader.setMat4("model", terrainManager.terrains[i].object.getModelMatrix());    
                objectShader.setMat3("normalMatrix",terrainManager.terrains[i].object.getNormalMatrix(view));
                terrainManager.terrains[i].object.draw();
            } 
        }
        objectShader.setVec3("fogColor",fogColor);

//WATER render

        waterShader.Use();
        waterShader.setMat4("view", view);
        waterShader.setMat4("projection", projection);

        waterShader.setVec3("cameraPosition",camera.Position); 

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fbos.getReflectionTexture());

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, fbos.getRefractionTexture());

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, waterDuDvTexture);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, waterNormalTexture);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, fbos.getRefractionDepthTexture());

        waterShader.setInt("waterTexture",0);
        waterShader.setInt("reflTexture",1);
        waterShader.setInt("refrTexture",2);
        waterShader.setInt("waterDuDvTexture",3);
        waterShader.setInt("waterNormalTexture",4);
        waterShader.setInt("depthTexture",5);

        waterMovement += waterSpeed *deltaTime;
        waterMovement = fmod(waterMovement,1);

        waterShader.setFloat("waterMovement",waterMovement);
        waterShader.setFloat("near",near);
        waterShader.setFloat("far",far);
        waterShader.setVec3("lightColour",lightColour);
        waterShader.setVec3("lightPosition",lightPos);

        for(int i = 0; i < terrainManager.terrains.size(); i++){   
            waterShader.setMat4("model", terrainManager.terrains[i].water.getModelMatrix());
            waterShader.setMat3("normalMatrix",terrainManager.terrains[i].water.getNormalMatrix(view));
            terrainManager.terrains[i].water.draw(); 
        }
        
        waterShader.setVec3("fogColor",fogColor);

        glDisable(GL_BLEND);

//SKYBOX Render

        skyBoxRenderer(skybox_shader,fogColor,&cubeModel);
        glfwSwapBuffers(window);
        
    }


    fbos.cleanUp();
    terrainShader.Delete();
    waterShader.Delete();
    objectShader.Delete();
    skybox_shader.Delete();

    glfwTerminate();
    return 0;
}

//////////////////////////////////////////
// callback for keyboard events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // if ESC is pressed, we close the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // if L is pressed, we activate/deactivate wireframe rendering of models
    if(key == GLFW_KEY_L && action == GLFW_PRESS)
        wireframe=!wireframe;

    GLuint new_subroutine;
    // pressing a key number, we change the shader applied to the models
    // if the key is between 1 and 9, we proceed and check if the pressed key corresponds to
    // a valid subroutine
    if((key >= GLFW_KEY_1 && key <= GLFW_KEY_9) && action == GLFW_PRESS)
    {
        // "1" to "9" -> ASCII codes from 49 to 59
        // we subtract 48 (= ASCII CODE of "0") to have integers from 1 to 9
        // we subtract 1 to have indices from 0 to 8
        new_subroutine = (key-'0'-1);
        // if the new index is valid ( = there is a subroutine with that index in the shaders vector),
        // we change the value of the current_subroutine variable
        // NB: we can just check if the new index is in the range between 0 and the size of the shaders vector, 
        // avoiding to use the std::find function on the vector
        if (new_subroutine<shaders.size())
        {
            current_subroutine = new_subroutine;
            PrintCurrentShader(current_subroutine);
        }
    }
}

//Camera key movement.
void apply_camera_movements(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }    
}

// callback for mouse events
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
       if(firstMouse)
      {
          lastX = xpos;
          lastY = ypos;
          firstMouse = false;
      }

      // offset of mouse cursor position
      GLfloat xoffset = xpos - lastX;
      GLfloat yoffset = lastY - ypos;

      // the new position will be the previous one for the next frame
      lastX = xpos;
      lastY = ypos;

      // we pass the offset to the Camera class instance in order to update the rendering
      camera.ProcessMouseMovement(xoffset, yoffset);

}

//Load texture
GLint LoadTexture(const char* path){
    GLuint textureImage;
    int w, h, channels;
    unsigned char* image;
    image = stbi_load(path, &w, &h, &channels, STBI_rgb);

    if (image == nullptr)
        std::cout << "Failed to load texture!" << std::endl;

    glGenTextures(1, &textureImage);
    glBindTexture(GL_TEXTURE_2D, textureImage);
    // 3 channels = RGB ; 4 channel = RGBA
    if (channels==3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (channels==4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    // we set how to consider UVs outside [0,1] range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // we set the filtering for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    // we free the memory once we have created an OpenGL texture
    stbi_image_free(image);

    // we set the binding to 0 once we have finished
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureImage;

}

 void calculateFPS(){
        frameCount++;
        finalTime = time(NULL);
        if((finalTime - initialTime) > 0){
            std::cout << "FPS: " << frameCount / (finalTime - initialTime) << std::endl;
            frameCount = 0;
            initialTime = finalTime;
        }
 }

void changeTerrainPos(TerrainManagement* manager){
    if(camera.Position.z - manager->getLastZ() <= 0){
        manager->translateTerrain(manager->BACKWARD);
        manager->increaseZ(true);
    }else if(camera.Position.z - manager->getLastZ() >= Terrain::getSize()){
        manager->translateTerrain(manager->FORWARD);
        manager->increaseZ(false);
    }

    if(camera.Position.x - manager->getLastX() <= 0){
        manager->translateTerrain(manager->RIGHT);
        manager->increaseX(true);
    }else if(camera.Position.x - manager->getLastX() >= Terrain::getSize()){
        manager->translateTerrain(manager->LEFT);
        manager->increaseX(false);
    }
}

void LoadTextureCubeSide(string path, string side_image, GLuint side_name)
{
    int w, h;
    unsigned char* image;
    string fullname;

    // full name and path of the side of the cubemap
    fullname = path + side_image;
    // we load the image file
    image = stbi_load(fullname.c_str(), &w, &h, 0, STBI_rgb);
    if (image == nullptr)
        std::cout << "Failed to load texture!" << std::endl;
    // we set the image file as one of the side of the cubemap (passed as a parameter)
    glTexImage2D(side_name, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    // we free the memory once we have created an OpenGL texture
    stbi_image_free(image);
}

//////////////////////////////////////////
// we load the 6 images from disk and we create an OpenGL cube map
GLint LoadTextureCube(string path)
{
    GLuint textureImage;
    
    // we create and activate the OpenGL cubemap texture
    glGenTextures(1, &textureImage);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureImage);

    // we load and set the 6 images corresponding to the 6 views of the cubemap
    // we use as convention that the names of the 6 images are "posx, negx, posy, negy, posz, negz", placed at the path passed as parameter
    // we load the images individually and we assign them to the correct side of the cube map
    LoadTextureCubeSide(path, std::string("posx.png"), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    LoadTextureCubeSide(path, std::string("negx.png"), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    LoadTextureCubeSide(path, std::string("posy.png"), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    LoadTextureCubeSide(path, std::string("negy.png"), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    LoadTextureCubeSide(path, std::string("posz.png"), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    LoadTextureCubeSide(path, std::string("negz.png"), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    // we set the filtering for minification and magnification
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // we set how to consider the texture coordinates outside [0,1] range
    // in this case we have a cube map, so
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // we set the binding to 0 once we have finished
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureImage;

}

void SetupShader(int program)
{
    int maxSub,maxSubU,countActiveSU;
    GLchar name[256]; 
    int len, numCompS;
    
    // global parameters about the Subroutines parameters of the system
    glGetIntegerv(GL_MAX_SUBROUTINES, &maxSub);
    glGetIntegerv(GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS, &maxSubU);
    std::cout << "Max Subroutines:" << maxSub << " - Max Subroutine Uniforms:" << maxSubU << std::endl;

    // get the number of Subroutine uniforms (only for the Fragment shader, due to the nature of the exercise)
    // it is possible to add similar calls also for the Vertex shader
    glGetProgramStageiv(program, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &countActiveSU);
    
    // print info for every Subroutine uniform
    for (int i = 0; i < countActiveSU; i++) {
        
        // get the name of the Subroutine uniform (in this example, we have only one)
        glGetActiveSubroutineUniformName(program, GL_FRAGMENT_SHADER, i, 256, &len, name);
        // print index and name of the Subroutine uniform
        std::cout << "Subroutine Uniform: " << i << " - name: " << name << std::endl;

        // get the number of subroutines
        glGetActiveSubroutineUniformiv(program, GL_FRAGMENT_SHADER, i, GL_NUM_COMPATIBLE_SUBROUTINES, &numCompS);
        
        // get the indices of the active subroutines info and write into the array s
        int *s =  new int[numCompS];
        glGetActiveSubroutineUniformiv(program, GL_FRAGMENT_SHADER, i, GL_COMPATIBLE_SUBROUTINES, s);
        std::cout << "Compatible Subroutines:" << std::endl;
        
        // for each index, get the name of the subroutines, print info, and save the name in the shaders vector
        for (int j=0; j < numCompS; ++j) {
            glGetActiveSubroutineName(program, GL_FRAGMENT_SHADER, s[j], 256, &len, name);
            std::cout << "\t" << s[j] << " - " << name << "\n";
            shaders.push_back(name);
        }
        std::cout << std::endl;
        
        delete[] s;
    }
}

void PrintCurrentShader(int subroutine)
{
    std::cout << "Current shader subroutine: " << shaders[subroutine]  << std::endl;
}

void terrainRender(Shader shader, glm::vec3 fogColor, GLFWwindow* window, TerrainManagement terrainManager, glm::vec4 plane, bool boolCamera){

    shader.Use();  

        //glm::vec3 fogColor = glm::vec3(0.1f,0.3f,0.1f);
        shader.setVec3("fogColor",fogColor);
        lightShaderSetUp(shader);
        shader.setVec4("clippingPlane", plane);

        projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, near,far); //10000
        shader.setMat4("projection", projection);   

        GLuint index = glGetSubroutineIndex(shader.Program, GL_FRAGMENT_SHADER, shaders[current_subroutine].c_str());
        // we activate the subroutine using the index (this is where shaders swapping happens)
        glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &index);

        // View matrix (=camera): position, view direction, camera "up" vector
        if(boolCamera){
            //view = actualCamera->GetViewMatrix();
            //actualCamera->InvertPitch();
            view = camera.GetViewMatrixInverted();
        }else
            view = camera.GetViewMatrix();

        shader.setMat4("view", view);

        // we "clear" the frame and z  buffer
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.setVec3("skyColor",glm::vec3(0.5f,0.5f,0.5f));

        shader.setInt("groundTexture",0);
        shader.setInt("snowTexture",1);
        shader.setInt("grassTexture",2);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, sandTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, groundTexture);

        for(int i = 0; i < terrainManager.terrains.size(); i++){     
            shader.setMat4("model", terrainManager.terrains[i].getModelMatrix());
            shader.setMat3("normalMatrix",terrainManager.terrains[i].getNormalMatrix(view));
            terrainManager.terrains[i].draw(); 
        }

}

void skyBoxRenderer(Shader skybox_shader, glm::vec3 fogColor,Model* cubeModel){
        glDepthFunc(GL_LEQUAL);
        skybox_shader.Use();
        // we activate the cube map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube);
        // we pass projection and view matrices to the Shader Program of the skybox
        skybox_shader.setMat4("projection", projection);   
        // to have the background fixed during camera movements, we have to remove the translations from the view matrix
        // thus, we consider only the top-left submatrix, and we create a new 4x4 matrix
        view =  glm::mat4(glm::mat3(camera.GetViewMatrix()));    // Remove any translation component of the view matrix
        skybox_shader.setMat4("view", view);

        // we determine the position in the Shader Program of the uniform variables
        skybox_shader.setInt("textureCube",0);
        
        skybox_shader.setVec3("fogColor",fogColor);

        // we render the cube with the environment map
        cubeModel->Draw();
        glDepthFunc(GL_LESS);

}

void lightShaderSetUp(Shader shaderParam){

        shaderParam.setVec3("pointLightPosition",lightPos);
        shaderParam.setFloat("Kd",Kd); 
}