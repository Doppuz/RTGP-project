
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

#include <utils/terrainGeneration.h>

// dimensions of application's window
GLuint screenWidth = 800, screenHeight = 600;

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
//lerp
void lerpMesh(Mesh *a, Mesh *b);
void lerpMeshOther(Mesh *a, Mesh *b);
void lerpMeshOther2(Mesh *a);
void lerpMeshOther3(Mesh *a);
void lerpMesh2(Mesh *a);
void lerpMesh3(Mesh *a);
void lerpMesh4(Mesh *a);
void lerpMesh5(Mesh *a);
float lerp(float a, float b, float c);

//First movement bool
bool firstMouse = true;

    GLuint VBO, EBO; 
    GLuint VAO;  

// we need to store the previous mouse position to calculate the offset with the current frame
GLfloat lastX, lastY;

//Movement with wasd
void apply_camera_movements(GLFWwindow* window);
void light_movements(GLFWwindow* window);

// parameters for time calculation (for animations)
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;  

//Projection matrix
glm::mat4 projection;

//View matrix
glm::mat4 view = glm::mat4(1.0f);

//Camera
Camera camera(glm::vec3(0.0f, 50.0f, 0.0f), GL_FALSE);
//Camera camera(glm::vec3(0.0f, 600.0f, 2000.0f), GL_FALSE);

// vector for the textures IDs
vector<GLint> textureID;

//Texture
//GLint texture = LoadTexture("../../textures/UV_Grid_Sm.png");

//FPS
double lastTime = glfwGetTime();
int nbFrames = 0;
int initialTime = time(NULL), finalTime, frameCount;

//light
glm::vec3 lightPos = glm::vec3(0.0f, 1000.0f,300.0f);
GLfloat lightMovement = 10.0f;

//diffuseComponent
glm::vec3 diffuseColor = glm::vec3(1.0f,0.0f,0.0f);
GLfloat Kd = 0.7f;

// boolean to activate/deactivate wireframe rendering
GLboolean wireframe = GL_FALSE;

//Terrain
int vertices = 64;

// boolean to start/stop animated rotation on Y angle
GLboolean spinning = GL_FALSE;
// rotation angle on Y axis
GLfloat orientationY = 0.0f;
// rotation speed on Y axis
GLfloat spin_speed = 30.0f;

int vertexCount = 64;

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

      // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // we define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // we enable Z test
    glEnable(GL_DEPTH_TEST);

    // the "clear" color for the frame buffer
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    // we create and compile shaders (code of Shader class is in include/utils/shader_v1.h)
    Shader shader("00_basic.vert", "00_basic.frag");
    shader.Use();  

    //projection = glm::perspective(45.0f, (float)screenWidth/(float)screenHeight, 0.1f, 10000.0f);
    projection = glm::perspective(45.0f, (float)screenWidth/(float)screenHeight, 0.1f, 5000000.0f);
    shader.setMat4("projection", projection); 
    
    /*glm::mat4 modelMatrix = glm::mat4(1.0f);     
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));*/

    std::cout << "Before texture" << std::endl;

    GLint groundTexture = LoadTexture("../../textures/plane/ground.jpg");
    
    std::cout << "After 1 texture" << std::endl;

    GLint snowTexture = LoadTexture("../../textures/plane/snow.jpg");

    std::cout << "After 2 texture" << std::endl;
    
    GLint grassTexture = LoadTexture("../../textures/plane/ground2.jpg");

    std::cout << "After 3 texture" << std::endl;

    //Terrain terrain(0,0,vertices);
    //vector<Terrain> terrains = generateTerrains(10,10);
    //createNoise(&terrain.terrainMesh);
    //terrain.terrainMesh.SetupMesh();

    GLfloat orientationY = -90.0f;

    TerrainGeneration terrain(2);
    TerrainGeneration terrain2(3);
    TerrainGeneration terrain3(4);
    TerrainGeneration terrain4(5);
    TerrainGeneration terrain5(6);
    TerrainGeneration terrain6(7);
    TerrainGeneration terrain7(8);
    TerrainGeneration terrain8(9);
    TerrainGeneration terrain9(10);

    glm::mat4 modelMatrix = glm::mat4(1.0f);     
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.0f, 0));

    glm::mat4 modelMatrix2 = glm::mat4(1.0f);     
    modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(-500000, 0.0f, 0));

    glm::mat4 modelMatrix3 = glm::mat4(1.0f);     
    modelMatrix3 = glm::translate(modelMatrix3, glm::vec3(+500000, 0.0f, 0));

    glm::mat4 modelMatrix4 = glm::mat4(1.0f);    
    modelMatrix4 = glm::translate(modelMatrix4, glm::vec3(0.0f, 0.0f, -500000.0f));

    glm::mat4 modelMatrix5 = glm::mat4(1.0f);     
    modelMatrix5 = glm::translate(modelMatrix5, glm::vec3(0.0f, 0.0f, 500000.0f));
    
    glm::mat4 modelMatrix6 = glm::mat4(1.0f);     
    modelMatrix6 = glm::translate(modelMatrix6, glm::vec3(500000.0f, 0.0f, 500000.0f));

    glm::mat4 modelMatrix7 = glm::mat4(1.0f);     
    modelMatrix7 = glm::translate(modelMatrix7, glm::vec3(-500000.0f, 0.0f, 500000.0f));

    glm::mat4 modelMatrix8 = glm::mat4(1.0f);     
    modelMatrix8 = glm::translate(modelMatrix8, glm::vec3(500000.0f, 0.0f, -500000.0f));

    glm::mat4 modelMatrix9 = glm::mat4(1.0f);     
    modelMatrix9 = glm::translate(modelMatrix9, glm::vec3(-500000.0f, 0.0f, -500000.0f));

   
    //modelMatrix4 = glm::rotate(modelMatrix4,glm::radians(orientationY),glm::vec3(0.0f,1.0f,0.0f));


 /*int index = 0;
    for(int i=0;i<4;i++){
        std::cout << std::endl;
		for(int j=0;j<4;j++){
            std::cout << terrain.terrainBaseMesh.indices[index] << " ";
            index++;
		}
	}

    /*std::cout <<  std::endl;

    index = 0;
    for(int i=0;i<vertices;i++){
		for(int j=0;j<4;j++){
            std::cout << terrain.terrainYMirroredBaseMesh.vertices[index].Position.x << " " << terrain.terrainYMirroredBaseMesh.vertices[index].Position.y 
                << " " << terrain.terrainYMirroredBaseMesh.vertices[index].Position.z << std::endl;
            index++;
		}
	}

/*for(int p = 0; p < terrains.size(); p++){
    std::cout << std::endl << std::endl;
    for(int i = 0; i < 4; i++){
        std::cout << std::endl;
        for(int j = 0; j < 4; j++){
            std::cout << terrains[p].getModelMatrix()[i][j] << " ";
        }
    }
}    */
    // Rendering loop: this code is executed at each frame

    bool first = false;
    while(!glfwWindowShouldClose(window)){

        calculateFPS();

        if (spinning)
            orientationY+=(deltaTime*spin_speed);

        apply_camera_movements(window);
        light_movements(window);

        // View matrix (=camera): position, view direction, camera "up" vector
        view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        //light
        shader.setVec3("pointLightPosition",lightPos);
        shader.setVec3("diffuseColor",diffuseColor);
        shader.setFloat("Kd",Kd);

        // Check is an I/O event is happening
        glfwPollEvents();

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // we "clear" the frame and z  buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.setInt("groundTexture",0);
        shader.setInt("snowTexture",1);
        shader.setInt("grassTexture",2);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, snowTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        /*for(int i = 0; i < terrains.size(); i++){
            //std::cout << "prova " << terrains[i].getModelMatrix()[0][0] << std::endl;
            shader.setMat4("model", terrains[i].getModelMatrix());
            terrains[i].draw(); 
        } */   


        /*
        shader.setMat4("model", modelMatrix8);
        terrain.draw();

        shader.setMat4("model", modelMatrix9);
        terrain2.draw();
*/
        if(!first){
            
            /*int index = 0;
            for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrain3.terrainBaseMesh.vertices[index].Position.y << " ";
                index++;
                /*std::cout << " AA: " << terrain.terrainBaseMesh.vertices[i].Position.y << " , " << terrain2.terrainBaseMesh.vertices[i].Position.y <<
                " x1: " << terrain.terrainBaseMesh.vertices[i].Position.x << " z1:" << terrain.terrainBaseMesh.vertices[i].Position.z <<
                " x2: " << terrain2.terrainBaseMesh.vertices[i].Position.x << " z2:" << terrain2.terrainBaseMesh.vertices[i].Position.z << std::endl;*/
            /*    }
            }   
            std::cout << std::endl;
            index = 0;
             for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrain4.terrainBaseMesh.vertices[index].Position.y << " ";
                index++;
                /*std::cout << " AA: " << terrain.terrainBaseMesh.vertices[i].Position.y << " , " << terrain2.terrainBaseMesh.vertices[i].Position.y <<
                " x1: " << terrain.terrainBaseMesh.vertices[i].Position.x << " z1:" << terrain.terrainBaseMesh.vertices[i].Position.z <<
                " x2: " << terrain2.terrainBaseMesh.vertices[i].Position.x << " z2:" << terrain2.terrainBaseMesh.vertices[i].Position.z << std::endl;*/
            /*    }
            } */

            /*lerpMeshOther(&terrain3.terrainBaseMesh,&terrain4.terrainBaseMesh);
            lerpMeshOther2(&terrain3.terrainBaseMesh);
            lerpMeshOther3(&terrain4.terrainBaseMesh);

            /*std::cout << std::endl;
            index = 0;
            for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrain3.terrainBaseMesh.vertices[index].Position.y << " ";
                index++;
                /*std::cout << " AA: " << terrain.terrainBaseMesh.vertices[i].Position.y << " , " << terrain2.terrainBaseMesh.vertices[i].Position.y <<
                " x1: " << terrain.terrainBaseMesh.vertices[i].Position.x << " z1:" << terrain.terrainBaseMesh.vertices[i].Position.z <<
                " x2: " << terrain2.terrainBaseMesh.vertices[i].Position.x << " z2:" << terrain2.terrainBaseMesh.vertices[i].Position.z << std::endl;*/
            /*    }
            }   
            std::cout << std::endl;
            index = 0;
             for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrain4.terrainBaseMesh.vertices[index].Position.y << " ";
                index++;
                /*std::cout << " AA: " << terrain.terrainBaseMesh.vertices[i].Position.y << " , " << terrain2.terrainBaseMesh.vertices[i].Position.y <<
                " x1: " << terrain.terrainBaseMesh.vertices[i].Position.x << " z1:" << terrain.terrainBaseMesh.vertices[i].Position.z <<
                " x2: " << terrain2.terrainBaseMesh.vertices[i].Position.x << " z2:" << terrain2.terrainBaseMesh.vertices[i].Position.z << std::endl;*/
            /*    }
            } */

            /*lerpMesh(&terrain.terrainBaseMesh,&terrain2.terrainBaseMesh);
            lerpMesh2(&terrain2.terrainBaseMesh);
            lerpMesh3(&terrain.terrainBaseMesh);
            lerpMesh(&terrain3.terrainBaseMesh,&terrain.terrainBaseMesh);
            lerpMesh2(&terrain.terrainBaseMesh);
            lerpMesh3(&terrain3.terrainBaseMesh);*/

            /*lerpMesh(&terrain5.terrainBaseMesh,&terrain4.terrainBaseMesh);
            lerpMesh2(&terrain4.terrainBaseMesh);
            lerpMesh3(&terrain5.terrainBaseMesh);
            lerpMesh(&terrain6.terrainBaseMesh,&terrain5.terrainBaseMesh);
            lerpMesh2(&terrain5.terrainBaseMesh);
            lerpMesh3(&terrain6.terrainBaseMesh);*/
            
            lerpMesh(&terrain3.terrainBaseMesh,&terrain.terrainBaseMesh);
            lerpMesh2(&terrain.terrainBaseMesh);
            lerpMesh3(&terrain3.terrainBaseMesh);
            
            lerpMesh(&terrain4.terrainBaseMesh,&terrain5.terrainBaseMesh);
            lerpMesh2(&terrain5.terrainBaseMesh);
            lerpMesh3(&terrain4.terrainBaseMesh);
            
            lerpMesh(&terrain5.terrainBaseMesh,&terrain9.terrainBaseMesh);
            lerpMesh2(&terrain9.terrainBaseMesh);
            lerpMesh3(&terrain5.terrainBaseMesh);

            lerpMesh(&terrain.terrainBaseMesh,&terrain2.terrainBaseMesh);
            lerpMesh2(&terrain2.terrainBaseMesh);
            lerpMesh3(&terrain.terrainBaseMesh);  

            lerpMesh(&terrain6.terrainBaseMesh,&terrain7.terrainBaseMesh);
            lerpMesh2(&terrain7.terrainBaseMesh);
            lerpMesh3(&terrain6.terrainBaseMesh); 

            lerpMesh(&terrain7.terrainBaseMesh,&terrain8.terrainBaseMesh);
            lerpMesh2(&terrain8.terrainBaseMesh);
            lerpMesh3(&terrain7.terrainBaseMesh); 
   
            lerpMeshOther(&terrain3.terrainBaseMesh,&terrain4.terrainBaseMesh);
            lerpMeshOther2(&terrain3.terrainBaseMesh);
            lerpMeshOther3(&terrain4.terrainBaseMesh);
            
            lerpMeshOther(&terrain.terrainBaseMesh,&terrain5.terrainBaseMesh);
            lerpMeshOther2(&terrain.terrainBaseMesh);
            lerpMeshOther3(&terrain5.terrainBaseMesh);
            
            lerpMeshOther(&terrain2.terrainBaseMesh,&terrain9.terrainBaseMesh);
            lerpMeshOther2(&terrain2.terrainBaseMesh);
            lerpMeshOther3(&terrain9.terrainBaseMesh);

            lerpMeshOther(&terrain8.terrainBaseMesh,&terrain2.terrainBaseMesh);
            lerpMeshOther2(&terrain8.terrainBaseMesh);
            lerpMeshOther3(&terrain2.terrainBaseMesh);

            lerpMeshOther(&terrain6.terrainBaseMesh,&terrain3.terrainBaseMesh);
            lerpMeshOther2(&terrain6.terrainBaseMesh);
            lerpMeshOther3(&terrain3.terrainBaseMesh);

            lerpMeshOther(&terrain7.terrainBaseMesh,&terrain.terrainBaseMesh);
            lerpMeshOther2(&terrain7.terrainBaseMesh);
            lerpMeshOther3(&terrain.terrainBaseMesh);
            
            std::cout << std::endl;
            /*lerpMesh(&terrain3.terrainBaseMesh,&terrain.terrainBaseMesh);
            lerpMesh2(&terrain.terrainBaseMesh);
            lerpMesh3(&terrain3.terrainBaseMesh);*/
            terrain.terrainBaseMesh.SetupMesh();
            terrain2.terrainBaseMesh.SetupMesh(); 
            terrain3.terrainBaseMesh.SetupMesh();
            terrain4.terrainBaseMesh.SetupMesh();  
            terrain5.terrainBaseMesh.SetupMesh();  
            terrain6.terrainBaseMesh.SetupMesh();  
            terrain7.terrainBaseMesh.SetupMesh();  
            terrain8.terrainBaseMesh.SetupMesh();  
            terrain9.terrainBaseMesh.SetupMesh(); 
            first = true;
        }
        shader.setMat4("model", modelMatrix);
        terrain.draw();

        shader.setMat4("model", modelMatrix4);
        terrain2.draw();

        shader.setMat4("model", modelMatrix7);
        terrain4.draw();

        shader.setMat4("model", modelMatrix2);
        terrain5.draw();

        shader.setMat4("model", modelMatrix5);
        terrain3.draw();

        shader.setMat4("model", modelMatrix6);
        terrain6.draw();

        shader.setMat4("model", modelMatrix3);
        terrain7.draw();

        shader.setMat4("model", modelMatrix8);
        terrain8.draw();

        shader.setMat4("model", modelMatrix9);
        terrain9.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (wireframe)
            // Draw in wireframe
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    }

    shader.Delete();

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
    if(key == GLFW_KEY_P && action == GLFW_PRESS)
        spinning=!spinning;
}

//Camera key movement.
void apply_camera_movements(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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

/*vector<Terrain> generateTerrains(int x, int y){
    vector<Terrain> terrains;

    for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){
            Terrain terrain(i,j,vertices);
            createNoise(&terrain.terrainMesh);
            terrain.terrainMesh.SetupMesh();
            terrains.push_back(terrain);
        }
    }
    return terrains;
}*/

void createNoise(Mesh *m){

    int width = vertices;
    int height = vertices;
    int w, h, channels;
    unsigned char* image;

    vector<vector<Vertex>> matrix;

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    srand((unsigned) time(0));
    noise.SetSeed(rand() % 1000000);
    noise.SetFrequency(0.008f);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(5);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);

    int index = 0;
    for (int y = 0; y < width; y++){
        vector<Vertex> temp;
        for (int x = 0; x < height; x++){
            m->vertices[index].Position.y = 1500 *  noise.GetNoise((float)x, (float)y);
            temp.push_back(m->vertices[index]);
            index += 1;
        }
        matrix.push_back(temp);
    }

    

    index = 0;

    calculateNormal(matrix, m);
}


    void calculateNormal(vector<vector<Vertex>> m, Mesh *mesh){
        int index = 0;
        for(int i = 0; i < m.size(); i++){
            for(int j = 0; j < m[i].size(); j++){
                float hL,hR,hD,hU = 0;
                hL =  m[i][(j-1)%m[i].size()].Position.y;
                hR =  m[i][(j+1)%m[i].size()].Position.y;
                hD =  m[(i-1)%m.size()][j].Position.y;
                hU =  m[(i+1)%m.size()][j].Position.y;
                glm::vec3 normal = glm::vec3(hL - hR, 2.0f, hD - hU);
                glm::normalize(normal);
                mesh->vertices[index].Normal = normal;
                index = index + 1;
            }
        }
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


 //Lerp vertex
 void lerpMesh(Mesh *a, Mesh *b){
    for(int i = 0; i < b->vertices.size() / vertexCount; i++){
        //std::cout << " prima: " <<i << " , " << (b->vertices.size() - 1) - (vertexCount * ((int)(i / vertexCount) + 1) - 1) + i % vertexCount << std::endl;
        float value = lerp(a->vertices[i].Position.y,b->vertices[(b->vertices.size() - 1) - (vertexCount * ((int)(i / vertexCount) + 1) - 1) + i % vertexCount].Position.y,0.5f);          
        //std::cout << " Dopo: " << (b->vertices.size() - 2) - ((4 * ((int)i / 4) + 1)) + (i % 4 - 1) << std::endl;
        a->vertices[i].Position.y = value;
        b->vertices[(b->vertices.size() - 1) - (vertexCount * ((int)(i / vertexCount) + 1) - 1) + i % vertexCount].Position.y = value;
    }
 }

 void lerpMesh2(Mesh *a){
     int size = a->vertices.size() - vertexCount - 1;
     float value1,value2,value3,value4;
    for(int i = 0; i < size; i++){
        /*if(((size - i) - vertexCount) >= 0)
            value1 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) - vertexCount].Position.y,0.5f);
        if(((size - i) + vertexCount) <= (a->vertices.size() - 1))*/
        value2 =  lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) + vertexCount].Position.y,0.2f + ((0.8f/(vertexCount - 1))* ((int)(i / vertexCount)+1))); //((0.8f/vertexCount)(vertexCount*((int)i / vertexCount)+1)));
        /*if(((size - i) - 1) >= 0)
            value3 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) - 1].Position.y,0.5f);
        if(((size - i) + 1) <= (a->vertices.size() - 1))
            value4 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) + 1].Position.y,0.5f);*/
        a->vertices[(size - i)].Position.y = value2;
    }    
 }

  void lerpMesh3(Mesh *a){
     //int size = a->vertices.size() - vertexCount - 1;
     float value1,value2,value3,value4;
    for(int i = vertexCount; i < a->vertices.size(); i++){
        /*if(((size - i) - vertexCount) >= 0)
            value1 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) - vertexCount].Position.y,0.5f);
        if(((size - i) + vertexCount) <= (a->vertices.size() - 1))*/
        value2 =  lerp(a->vertices[i].Position.y,a->vertices[i - vertexCount].Position.y,0.2f + ((0.8f/(vertexCount - 1))* ((int)(i / vertexCount))));
        /*if(((size - i) - 1) >= 0)
            value3 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) - 1].Position.y,0.5f);
        if(((size - i) + 1) <= (a->vertices.size() - 1))
            value4 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) + 1].Position.y,0.5f);*/
        a->vertices[i].Position.y = value2;
    }    
 }

  //Lerp vertex
 void lerpMeshOther(Mesh *a, Mesh *b){
    for(int i = 0; i < b->vertices.size(); i = i + vertexCount){
        float value = lerp(a->vertices[i].Position.y,b->vertices[i + vertexCount - 1].Position.y,0.5f);          
        a->vertices[i].Position.y = value;
        b->vertices[i + vertexCount - 1].Position.y = value;
    }
 }

  void lerpMeshOther2(Mesh *a){
    for(int i = 1; i < vertexCount; i++){
        for(int j = 0; j < a->vertices.size(); j = j + vertexCount){
            float value = lerp(a->vertices[j + i].Position.y,a->vertices[j].Position.y,0.2f + ((0.8 / (vertexCount - 1)) * (i - 1)));          
            a->vertices[j + i].Position.y = value;
        }
    }
  }

  void lerpMeshOther3(Mesh *a){
    for(int i = 1; i < vertexCount; i++){
        for(int j = 0; j < a->vertices.size(); j = j + vertexCount){
            float value = lerp(a->vertices[j + vertexCount - 1 - i].Position.y,a->vertices[j + vertexCount - i].Position.y,0.2f 
                + ((0.8 / (vertexCount - 1)) * (i - 1)));          
            a->vertices[j + vertexCount - 1 - i].Position.y = value;
        }
    }
  }
 /*void lerpMesh4(Mesh *a){
     
     int size = a->vertices.size() - vertexCount - 1;
     //int size = a->vertices.size() - vertexCount - 1;
     float value2;
    for(int i = vertexCount; i < size; i++){
        /*if(((size - i) - vertexCount) >= 0)
            value1 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) - vertexCount].Position.y,0.5f);
        if(((size - i) + vertexCount) <= (a->vertices.size() - 1))*/
 //       value2 =  lerp(a->vertices[i].Position.y,a->vertices[i - vertexCount].Position.y,0.2f);
        /*if(((size - i) - 1) >= 0)
            value3 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) - 1].Position.y,0.5f);
        if(((size - i) + 1) <= (a->vertices.size() - 1))
            value4 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) + 1].Position.y,0.5f);*/
/*        a->vertices[i].Position.y = value2;
    }    
 }

 void lerpMesh5(Mesh *a){
     int size = a->vertices.size() - vertexCount - 1;
     float value1,value2,value3,value4;
    for(int i = 0; i < (size - vertexCount); i++){
        /*if(((size - i) - vertexCount) >= 0)
            value1 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) - vertexCount].Position.y,0.5f);
        if(((size - i) + vertexCount) <= (a->vertices.size() - 1))*/
  //      value2 =  lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) + vertexCount].Position.y,0.2f);
        /*if(((size - i) - 1) >= 0)
            value3 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) - 1].Position.y,0.5f);
        if(((size - i) + 1) <= (a->vertices.size() - 1))
            value4 = lerp(a->vertices[(size - i)].Position.y,a->vertices[(size - i) + 1].Position.y,0.5f);*/
   /*     a->vertices[(size - i)].Position.y = value2;
    }    
 }*/

float lerp(float a, float b, float c){
     return a * c + b * (1-c);
 }