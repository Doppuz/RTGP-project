
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

// dimensions of application's window
GLuint screenWidth = 1600, screenHeight = 800;

// callback function for keyboard events
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// callback for mouse events
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// load image from disk and create an OpenGL texture
GLint LoadTexture(const char* path);
void createNoise(Mesh *m);
// Create Terrain
//vector<Terrain> generateTerrains(int x, int y);
//just a try
void setupMesh(Mesh m);
//FPS
void calculateFPS();
//ComputeNormal
void calculateNormal(vector<vector<Vertex>> m, Mesh *mesh);
//TerrainPos
void changeTerrainPos(TerrainManagement* manager);
//SkyBox
void LoadTextureCubeSide(string path, string side_image, GLuint side_name);
GLint LoadTextureCube(string path);
//subroutine
void SetupShader(int program);
void PrintCurrentShader(int subroutine);

//First movement bool
bool firstMouse = true;

    GLuint VBO, EBO; 
    GLuint VAO;  

// we need to store the previous mouse position to calculate the offset with the current frame
GLfloat lastX, lastY;

//Movement with wasd
void apply_camera_movements(GLFWwindow* window);
void light_movements(GLFWwindow* window);

//Projection matrix
glm::mat4 projection;

//View matrix
glm::mat4 view = glm::mat4(1.0f);

//Camera
Camera camera(glm::vec3(0.0f, 300.0f, 0.0f), GL_FALSE); //25000,20000,25000
Camera farCamera(glm::vec3(0.0f,50000.0f,200000.0f),GL_FALSE);
Camera actualCamera = camera;

// vector for the textures IDs
vector<GLint> textureID;

// parameters for time calculation (for animations)
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//FPS
double lastTime = glfwGetTime();
int nbFrames = 0;
int initialTime = time(NULL), finalTime, frameCount;

//light
glm::vec3 lightPos = glm::vec3(0, 0.0f, 0);
GLfloat lightMovement = 100.0f;

// diffusive, specular and ambient components
glm::vec3 diffuseColor = {1.0f,0.0f,0.0f};
glm::vec3 specularColor = {0.2f,1.0f,0.7f};
glm::vec3 ambientColor = {0.5f,0.1f,0.7f};
// weights for the diffusive, specular and ambient components
GLfloat Kd = 0.5f;
GLfloat Ks = 0.4f;
GLfloat Ka = 0.1f;
// shininess coefficient for Phong and Blinn-Phong shaders
GLfloat shininess = 25.0f;

// boolean to activate/deactivate wireframe rendering
GLboolean wireframe = GL_FALSE;

// boolean to start/stop animated rotation on Y angle
GLboolean spinning = GL_FALSE;
// rotation angle on Y axis
GLfloat orientationY = 0.0f;
// rotation speed on Y axis
GLfloat spin_speed = 30.0f;

int vertexCount = 64;

int cam = 0;

// texture unit for the cube map
GLuint textureCube;

//Subroutine
GLuint current_subroutine = 0;
vector<std::string> shaders;

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

    // we disable the mouse cursor
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    // GLAD tries to load the context set by GLFW
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // we define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // we enable Z test
    glEnable(GL_DEPTH_TEST);

    // the "clear" color for the frame buffer
    glClearColor(0.07f, 0.07f, 0.07f, 1.0f);

    // we create and compile shaders (code of Shader class is in include/utils/shader_v1.h)
    Shader shader("00_basic.vert", "00_basic.frag");
    Shader skybox_shader("skyboxVert.vert", "skyboxFrag.frag");

    SetupShader(shader.Program);
    // we print on console the name of the first subroutine used
    PrintCurrentShader(current_subroutine);

    std::cout << "Before texture" << std::endl;

    GLint groundTexture = LoadTexture("../../textures/plane/ground.jpg");
    
    std::cout << "After 1 texture" << std::endl;

    GLint snowTexture = LoadTexture("../../textures/plane/snow.jpg");

    std::cout << "After 2 texture" << std::endl;
    
    GLint grassTexture = LoadTexture("../../textures/plane/ground4.jpg");

    std::cout << "After 3 texture" << std::endl;

    GLint redTexture = LoadTexture("../../textures/plane/redTexture.jpg");

    textureCube = LoadTextureCube("../../textures/cube/MySkyBox/");;

    GLfloat orientationY = -90.0f;


    Model planeModel("../../models/plane.obj");
    planeModel.meshes[0].SetupMesh();
    Model cubeModel("../../models/cube.obj"); // used for the environment map
    cubeModel.meshes[0].SetupMesh();

    Model cubeModel2("../../models/cube.obj"); // used for the environment map
    cubeModel2.meshes[0].SetupMesh();

    //terrain
    TerrainManagement terrainManager;

    bool first = false;
    
    
    glm::mat4 cubeModelMatrix = glm::mat4(1.0f);
    cubeModelMatrix = glm::translate(cubeModelMatrix,glm::vec3(0.0f, 8000.0f,120000));
    cubeModelMatrix = glm::scale(cubeModelMatrix, glm::vec3(500000,50000,250000));

    while(!glfwWindowShouldClose(window)){

        //std::cout << actualCamera.Position.x << " " <<  actualCamera.Position.y << " " <<  actualCamera.Position.z << std::endl;

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check is an I/O event is happening
        glfwPollEvents();

        apply_camera_movements(window);

        changeTerrainPos(&terrainManager);
            
        shader.Use();  

        shader.setFloat("kd",Kd);
        shader.setFloat("ks",Ks);
        shader.setFloat("ka",Ka);
        shader.setVec3("ambientColor",ambientColor);
        shader.setVec3("diffuseColor",diffuseColor);
        shader.setVec3("specularColor",specularColor);
        shader.setFloat("shininess",shininess);

        projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 10.0f,20000.0f); //150000
        shader.setMat4("projection", projection);   

        GLuint index = glGetSubroutineIndex(shader.Program, GL_FRAGMENT_SHADER, shaders[current_subroutine].c_str());
        // we activate the subroutine using the index (this is where shaders swapping happens)
        glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &index);

        calculateFPS();

        light_movements(window);

        // View matrix (=camera): position, view direction, camera "up" vector
        view = actualCamera.GetViewMatrix();
        shader.setMat4("view", view);

        //light
        shader.setVec3("pointLightPosition",lightPos);
        shader.setVec3("diffuseColor",diffuseColor);
        shader.setFloat("Kd",Kd); 

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
        glBindTexture(GL_TEXTURE_2D, snowTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        for(int i = 0; i < terrainManager.terrains.size(); i++){     
            shader.setMat4("model", terrainManager.terrains[i].getModelMatrix());
            shader.setMat3("normalMatrix",terrainManager.terrains[i].getNormalMatrix(view));
            terrainManager.terrains[i].draw(); 
        }

       /*shaderSphere.Use();

        projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f,10000000.0f);
        shaderSphere.setMat4("projection", projection); 

        view = camera.GetViewMatrix();
        shaderSphere.setMat4("view", view);  

        shaderSphere.setInt("redTexture",0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, redTexture);

        glm::mat4 modelSphereMatrix = glm::mat4(1.0f);     
        modelSphereMatrix = glm::translate(modelSphereMatrix, glm::vec3(0.0f, 25000.0f, 1500.0f));
        modelSphereMatrix = glm::scale(modelSphereMatrix, glm::vec3(100.0f, 100.0f, 100.0f));

        shaderSphere.setMat4("model", modelSphereMatrix);
        sphere.Draw();*/
        
        if (wireframe)
            // Draw in wireframe
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (spinning)
            orientationY+=(deltaTime*spin_speed);


        lightPos = glm::vec3(actualCamera.Position.x,lightPos.y,actualCamera.Position.z);

        /*fog_shader.Use();
        fog_shader.setMat4("projection", projection);  
        fog_shader.setMat4("view", view);
        fog_shader.setMat4("model", cubeModelMatrix);
        cubeModel2.Draw();*/
        
        /////////////////// SKYBOX ////////////////////////////////////////////////
        // we use the cube to attach the 6 textures of the environment map.
        // we render it after all the other objects, in order to avoid the depth tests as much as possible.
        // we will set, in the vertex shader for the skybox, all the values to the maximum depth. Thus, the environment map is rendered only where there are no other objects in the image (so, only on the background). Thus, we set the depth test to GL_LEQUAL, in order to let the fragments of the background pass the depth test (because they have the maximum depth possible, and the default setting is GL_LESS)
        glDepthFunc(GL_LEQUAL);
        skybox_shader.Use();
        // we activate the cube map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureCube);
         // we pass projection and view matrices to the Shader Program of the skybox
        skybox_shader.setMat4("projection", projection);   
        // to have the background fixed during camera movements, we have to remove the translations from the view matrix
        // thus, we consider only the top-left submatrix, and we create a new 4x4 matrix
        view =  glm::mat4(glm::mat3(actualCamera.GetViewMatrix()));    // Remove any translation component of the view matrix
        skybox_shader.setMat4("view", view);

        // we determine the position in the Shader Program of the uniform variables
        skybox_shader.setInt("textureCube",0);

        // we render the cube with the environment map
        cubeModel.Draw();
        // we set again the depth test to the default operation for the next frame
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);

    }

    shader.Delete();
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

         // if P is pressed, we start/stop the animated rotation of models
    if(key == GLFW_KEY_P && action == GLFW_PRESS){
        if(cam == 0)
            actualCamera = farCamera;
        else  
            actualCamera = camera;
        cam = (cam + 1) % 2;    
    }    
}

//Camera key movement.
void apply_camera_movements(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        actualCamera.ProcessKeyboard(FORWARD, deltaTime);
    }if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        actualCamera.ProcessKeyboard(BACKWARD, deltaTime);
    }if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        actualCamera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        actualCamera.ProcessKeyboard(RIGHT, deltaTime);
    }    
}

//Light key movement.
void light_movements(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        lightPos.y += lightPos.y * lightMovement * deltaTime; 
    if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        lightPos.y -= lightPos.y * lightMovement * deltaTime; 
    if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        lightPos.z += lightPos.z * lightMovement * deltaTime; 
    if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        lightPos.z -= lightPos.z * lightMovement * deltaTime; 
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
      actualCamera.ProcessMouseMovement(xoffset, yoffset);

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
    
    if(actualCamera.Position.z - manager->getLastZ() <= 0){
        manager->translateTerrain(manager->BACKWARD);
        manager->increaseZ(true);
    }else if(actualCamera.Position.z - manager->getLastZ() >= Terrain::getSize()){
        manager->translateTerrain(manager->FORWARD);
        manager->increaseZ(false);
    }

    if(actualCamera.Position.x - manager->getLastX() <= 0){
        manager->translateTerrain(manager->RIGHT);
        manager->increaseX(true);
    }else if(actualCamera.Position.x - manager->getLastX() >= Terrain::getSize()){
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