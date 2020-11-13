
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

//Projection matrix
glm::mat4 projection;

//View matrix
glm::mat4 view = glm::mat4(1.0f);

//Camera
Camera camera(glm::vec3(25000.0f, 25000.0f, 25000.0f), GL_FALSE);
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

int cam = 0;

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

    Shader shaderSphere("01_sphere.vert", "01_sphere.frag");

    std::cout << "Before texture" << std::endl;

    GLint groundTexture = LoadTexture("../../textures/plane/ground.jpg");
    
    std::cout << "After 1 texture" << std::endl;

    GLint snowTexture = LoadTexture("../../textures/plane/snow.jpg");

    std::cout << "After 2 texture" << std::endl;
    
    GLint grassTexture = LoadTexture("../../textures/plane/ground2.jpg");

    std::cout << "After 3 texture" << std::endl;

    GLint redTexture = LoadTexture("../../textures/plane/redTexture.jpg");

    GLfloat orientationY = -90.0f;

    Model sphere = Model("../../models/sphere.obj");
    sphere.meshes[0].SetupMesh();

    //terrain
    TerrainManagement terrainManagement;

    bool first = false;

    while(!glfwWindowShouldClose(window)){

        std::cout << actualCamera.Position.x << " " <<  actualCamera.Position.y << " " <<  actualCamera.Position.z << std::endl;

        if(actualCamera.Position.x >= +25000 && !first){
            first = true;
            //up -25000
            /*modelMatrix5 = glm::translate(modelMatrix5, glm::vec3(0.0f, 0.0f, size - 200000));
            modelMatrix6 = glm::translate(modelMatrix6, glm::vec3(0, 0.0f,size - 200000));
            modelMatrix7 = glm::translate(modelMatrix7, glm::vec3(0, 0.0f, +size - 200000));*/

            //down 50000
            /*modelMatrix9 = glm::translate(modelMatrix9, glm::vec3(0.0f, 0.0f, size + 100000));
            modelMatrix4 = glm::translate(modelMatrix4, glm::vec3(0, 0.0f,size + 100000));
            modelMatrix8 = glm::translate(modelMatrix8, glm::vec3(0, 0.0f, +size + 100000));*/

            //right +25000
            /*terrainManagement.terrains[0].translateModelMatrix(glm::vec3(+150000.0f, 0.0f, 0.0f));
            terrainManagement.terrains[3].translateModelMatrix(glm::vec3(+150000.0f, 0.0f, 0.0f));
            terrainManagement.terrains[6].translateModelMatrix(glm::vec3(+150000.0f, 0.0f, 0.0f));*/

            //left -25000
            /*modelMatrix8 = glm::translate(modelMatrix8, glm::vec3(-150000.0f, 0.0f, 0.0f));
            modelMatrix3 = glm::translate(modelMatrix3, glm::vec3(-150000.0f, 0.0f, 0.0f));
            modelMatrix6 = glm::translate(modelMatrix6, glm::vec3(-150000.0f, 0.0f, 0.0f));*/
            
        }
            

        shader.Use();  

        projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f,10000000.0f);
        shader.setMat4("projection", projection);   

        //calculateFPS();

        if (spinning)
            orientationY+=(deltaTime*spin_speed);

        apply_camera_movements(window);
        light_movements(window);

        // View matrix (=camera): position, view direction, camera "up" vector
        view = actualCamera.GetViewMatrix();
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

        shader.setMat4("model", terrainManagement.terrains[0].getModelMatrix());
        terrainManagement.terrains[0].draw(); 

        shader.setMat4("model", terrainManagement.terrains[1].getModelMatrix());
        terrainManagement.terrains[1].draw(); 

        shader.setMat4("model",terrainManagement.terrains[2].getModelMatrix());
        terrainManagement.terrains[2].draw(); 

        shader.setMat4("model", terrainManagement.terrains[3].getModelMatrix());
        terrainManagement.terrains[3].draw(); 

        shader.setMat4("model", terrainManagement.terrains[4].getModelMatrix());
        terrainManagement.terrains[4].draw(); 

        shader.setMat4("model", terrainManagement.terrains[5].getModelMatrix());
        terrainManagement.terrains[5].draw();

        shader.setMat4("model", terrainManagement.terrains[6].getModelMatrix());
        terrainManagement.terrains[6].draw(); 

        shader.setMat4("model", terrainManagement.terrains[7].getModelMatrix());
        terrainManagement.terrains[7].draw(); 

        shader.setMat4("model", terrainManagement.terrains[8].getModelMatrix());
        terrainManagement.terrains[8].draw(); 

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

void changeTerrainPos(){
    if(abs(actualCamera.Position.x - )){

    }
}