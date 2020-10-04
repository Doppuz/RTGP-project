
// Std. Includes
#include <string>

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

# include <utils/FastNoiseLite.h>

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
void generateTerrain(int wRes, int lRes);
//just a try
void setupMesh(Mesh m);
//FPS
void calculateFPS();

//First movement bool
bool firstMouse = true;

    GLuint VBO, EBO; 
    GLuint VAO;  

// we need to store the previous mouse position to calculate the offset with the current frame
GLfloat lastX, lastY;

//Movement with wasd
void apply_camera_movements(GLFWwindow* window);

// parameters for time calculation (for animations)
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;  

//Projection matrix
glm::mat4 projection;

//View matrix
glm::mat4 view = glm::mat4(1.0f);

//Camera
Camera camera(glm::vec3(0.0f, 10.0f, 150.0f), GL_FALSE);

// vector for the textures IDs
vector<GLint> textureID;

//Texture
//GLint texture = LoadTexture("../../textures/UV_Grid_Sm.png");

//FPS
double lastTime = glfwGetTime();
int nbFrames = 0;

/////////////////// MAIN function ///////////////////////
int main(){
    
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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

    projection = glm::perspective(45.0f, (float)screenWidth/(float)screenHeight, 0.1f, 10000.0f);
    shader.setMat4("projection", projection); 
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);     
    modelMatrix = glm::scale(modelMatrix,glm::vec3(1.0f,1.0f,1.0f));

    GLint grassTexture = LoadTexture("../../textures/plane/ground.jpg");
    GLint snowTexture = LoadTexture("../../textures/plane/snow.jpg");

    textureID.push_back(grassTexture);
    textureID.push_back(snowTexture);

    //GLint textureNoise = createNoise();

    bool prova = false; 

    Model cubeModel("../../models/plane.obj");
    

    // Rendering loop: this code is executed at each frame
    while(!glfwWindowShouldClose(window)){

        calculateFPS();

        apply_camera_movements(window);

        // View matrix (=camera): position, view direction, camera "up" vector
        view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        // Check is an I/O event is happening
        glfwPollEvents();

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // we "clear" the frame and z  buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.setInt("grassTexture",0);
        shader.setInt("snowTexture",1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, snowTexture);

        if(!prova){
            int t = 0;
            for(int i = 0; i < cubeModel.meshes[0].indices.size(); i++){
                float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                //cubeModel.meshes[0].vertices[i].Position.y = 5 * y;
                //cubeModel.meshes[0].vertices[i].Position.x = 1;
                //cubeModel.meshes[0].vertices[i].Position.z = 1;
                //std::cout << cubeModel.meshes[0].vertices[i].Position.x << " " << cubeModel.meshes[0].vertices[i].Position.y << " " <<cubeModel.meshes[0].vertices[i].Position.z << std::endl;               
                //std::cout << "A " << cubeModel.meshes[0].indices[i] << std::endl;
                prova = true;
                t += 1;
            }
            //std::cout << "AA " << t << std::endl;
            prova = true;
            createNoise(&cubeModel.meshes[0]);
            //std::cout << cubeModel.meshes[0].vertices.size();
            cubeModel.meshes[0].SetupMesh();
            //setupMesh(cubeModel.meshes[0]);
        }
        //generateTerrain(128,128);
            //float angle = 1.0f;
            //modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", modelMatrix);

        // we render the model
        cubeModel.Draw();     
        /*glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, cubeModel.meshes[0].indices.size(), GL_UNSIGNED_INT,0);
        glBindVertexArray(0);*/

        glfwSwapBuffers(window);
        glfwPollEvents();
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

void createNoise(Mesh *m){
    int width = 128;
    int height = 128;
    /*noise::module::Perlin perlinNoise;
    // Base frequency for octave 1.
    /*perlinNoise.SetFrequency(4.0);
    GLubyte *data = new GLubyte[ width * height * 4 ];
    double xRange = 1.0;
    double yRange = 1.0;
    double xFactor = xRange / width;
    double yFactor = yRange / height;
    for( int oct = 0; oct < 4; oct++ ) {
        perlinNoise.SetOctaveCount(oct+1);
        for( int i = 0; i < width; i++ ) {
            for( int j = 0 ; j < height; j++ ) {
                double x = xFactor * i;
                double y = yFactor * j;
                double z = 0.0;
                float val = (float)perlinNoise.GetValue(x,y,z);
                // Scale and translate to roughly between 0 and 1
                val = (val + 1.0f) * 0.5f;
                // Clamp strictly between 0 and 1
                val = val> 1.0f ? 1.0f :val;
                val = val< 0.0f ? 0.0f :val;
                // Store in texture
                data[((j * width + i) * 4) + oct] =
                (GLubyte) ( val * 255.0f );
            }
        }
    }
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,
    GL_UNSIGNED_BYTE,data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    delete [] data;
    return texID;*/
    // Create and configure FastNoise object
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    // Gather noise data
    float *noiseData = new float[128 * 128];
    int index = 0;

    for (int y = 0; y < 128; y++)
    {
        for (int x = 0; x < 128; x++)
        {
            noiseData[index++] = noise.GetNoise((float)x, (float)y);
        }
    }

    for (GLuint i = 0; i < m->vertices.size(); ++i) { 
        m->vertices[i].Position.y = 20 * noiseData[i]; 
    } 

    // Do something with this data...

    // Free data later
    /*GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,
    GL_UNSIGNED_BYTE,data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    delete [] data;
    return texID;*/
    //return 1;
}

/*GLuint terrainVbo = 0,terrainVboNorm = 0,terrainEbo = 0;
static int terrainCount = 0;

void generateTerrain(int wRes = 128, int lRes = 128){
  float w = 10.0f, l = 10.0f;  
  const int size = wRes * lRes;
  vector<glm::vec3> verts(size), norms(size);  
  vector<glm::vec3> tris;

  int i = 0;

  for(int z = 0; z < lRes; z ++){
    for(int x = 0; x < wRes; x++) {
        glm::vec3 v = glm::vec3(x/(float)lRes,0,z/(float)wRes);
        v.x *= w;
        v.z *= l;
        v.x -= w/2;
        v.z -= l/2;

        verts[i] = v;
        norms[i] = glm::vec3(0,1,0);

        if(((i + 1) % (int) w) != 0 && z + 1 < lRes){
            glm::vec3 tri = glm::vec3(i, i+wRes, i+wRes+1);
            glm::vec3 tri2 = glm::vec3(i, i+wRes+1, i+1);
            tris.push_back(tri);
            tris.push_back(tri2);
        }

        i++;
    }
  }
  terrainCount = tris.size();

    glGenBuffers(1, &terrainVbo);
    glGenBuffers(1, &terrainVboNorm);
    glGenBuffers(1, &terrainEbo);

    glBindBuffer(GL_ARRAY_BUFFER,terrainVbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*verts.size(),verts.data(),GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER,terrainVboNorm);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*norms.size(),norms.data(),GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,terrainEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(glm::vec3)*tris.size(),tris.data(),GL_STATIC_DRAW);
}*/

    /*void BicubicInterpolator (int x, int y, int subsampling, vector<Vertex> vertices) {

		// build a lattice
		for (int i = 0; i < x; i += subsampling) {
			for (int j = 0; j < y; j += subsampling) {
				h [j, i] = Random.Range (0f, 1f);
			}
		}

		// cut out border effects to make code simpler
		int xCut = subsampling * ((int)Mathf.Floor (x / subsampling));
		int yCut = subsampling * ((int)Mathf.Floor (y / subsampling));

		// build bilinear interpolations
		// first direction i only on lattice points
		for (int i = 0; i < xCut; i += subsampling) { // avoid border effect
			for (int j = 0; j <= yCut; j += subsampling) {
				for (int k = 1; k < subsampling; k += 1) {
					h [j, i + k] = Mathf.Lerp (h [j, i], h [j, i + subsampling], slope((float)k / (float)subsampling)); // change here!
				}
			}
		}
		// then direction j on all points to cover all grid
		for (int i = 0; i <= xCut; i += 1) {
			for (int j = 0; j < yCut; j += subsampling) {
				for (int k = 1; k < subsampling; k += 1) {
					h [j + k, i] = Mathf.Lerp (h [j, i], h [j + subsampling, i], slope((float)k / (float)subsampling)); // change here!
				}
			}
		}

		return h;
	}*/



 void calculateFPS(){

     // Measure speed
     double currentTime = glfwGetTime();
     nbFrames++;
     if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
         // printf and reset timer
         printf("%f ms/frame\n", 1000.0/double(nbFrames));
         nbFrames = 0;
         lastTime += 1.0;
     }
 }