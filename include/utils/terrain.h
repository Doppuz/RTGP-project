#pragma once


#include <utils/mesh_v1.h>

#include <utils/FastNoiseLite.h>
#include <ctime>

#include <utils/water.h>
#include <utils/tree.h>
#include <math.h>

class Terrain{
    public:
        
        Mesh mesh;
        Water water;
        Tree tree;
        const static GLint vertex_count = 8; //32
        float heights[vertex_count][vertex_count];
        glm::vec3 initialTranslate;
        Model* model;
        bool hasTree = true;

        Terrain(GLfloat x, glm::vec3 translate,Model* treeModel)
            :modelMatrix{glm::mat4(1.0f)}, initialTranslate{translate}, model{treeModel}{
            modelMatrix = glm::translate(modelMatrix,translate);
            noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            srand((unsigned) time(0));
            int random = (rand() % 1000000) * x;
            noise.SetSeed(random);
            noise.SetFrequency(0.008f);
            noise.SetFractalType(FastNoiseLite::FractalType_FBm);
            noise.SetFractalOctaves(5);
            noise.SetFractalLacunarity(2.0f);
            noise.SetFractalGain(0.5f);
            generateBaseTerrain();
            water = Water(x,translate,10);
        }

        void createTree(){
            float height = getTerrainHeight();
            if(height > 10)
                tree = Tree(glm::vec3(initialTranslate.x+250,height - 25,initialTranslate.z+250),model);
            else{
                Tree* tempTree = &tree;
                tempTree = NULL;
                hasTree = false;
            }
        }

        Terrain(){}

        void draw(){
           mesh.Draw();
        }

        GLint static getVertexCount(){
            return vertex_count;
        }
        
        GLint static getSize(){
            return size;
        }

        glm::mat4 getModelMatrix(){
            return modelMatrix;
        }

        void translateModelMatrix(glm::vec3 translate){
            modelMatrix = glm::translate(modelMatrix,translate);
            water.translateModelMatrix(translate);
            tree.translateModelMatrix(translate);
            if(translate.x != 0.0f)
                xPos += translate.x;
            else
                zPos += translate.z;
            //std::cout << xPos << " " << zPos << std::endl;
        }

        glm::mat3 getNormalMatrix(glm::mat4 viewMatrix){
            return  glm::inverseTranspose(glm::mat3(viewMatrix*modelMatrix));
        }

        float getTerrainHeight(){
            float terrainX = 250;
            float terrainZ = 250;
            float gridSquareSize = size / ((float)vertex_count - 1);
            int gridX = (int) floor(terrainX / gridSquareSize);
            int gridZ = (int) floor(terrainZ / gridSquareSize);

            if(gridX > (vertex_count - 1) || gridZ > (vertex_count - 1) || gridX < 0 || gridZ < 0)
                return 0;

            float xCoord = fmod(terrainX,gridSquareSize)/gridSquareSize;
            float zCoord = fmod(terrainZ,gridSquareSize)/gridSquareSize;
            float answer;

            if(xCoord <= (1 - zCoord)){
                answer = barryCentric(glm::vec3(0, heights[gridX][gridZ], 0), glm::vec3(1,
							heights[gridX + 1][gridZ], 0), glm::vec3(0,
							heights[gridX][gridZ + 1], 1), glm::vec2(xCoord, zCoord)); 
            }else{
                answer = barryCentric(glm::vec3(1, heights[gridX + 1][gridZ], 0), glm::vec3(1,
							heights[gridX + 1][gridZ + 1], 1), glm::vec3(0,
							heights[gridX][gridZ + 1], 1), glm::vec2(xCoord, zCoord));
            }

            //std::cout << gridX << " " << gridZ << std::endl;
            //std::cout << xCoord << " " << zCoord << std::endl;
            //std::cout << " a " << answer << std::endl;
            return answer;
        }

    private:
        const static GLint size = 500; //500
        FastNoiseLite noise;
        glm::mat4 modelMatrix;
        float height = 6000; //6000 
        int xPos = 0;
        int zPos = 0;

        void generateBaseTerrain(){
            vector<Vertex> vertices;
		    vector<GLuint> indices;

		    int vertexPointer = 0;
            glm::vec2 vec;                    
            glm::vec3 vector;

		    for(int i=0;i<vertex_count;i++){
                Vertex vertex;
			    for(int j=0;j<vertex_count;j++){
                    vector = glm::vec3((float)j/((float)vertex_count - 1) * size, height *  noise.GetNoise((float)j, (float)i)
                        ,(float)i/((float)vertex_count - 1) * size);
                    vertex.Position = vector;
                    heights[i][j] = vertex.Position.y;
                    vector = glm::vec3(0,1,0);
                    vertex.Normal = vector;

                    vec = glm::vec2((float)j/((float)vertex_count - 1),(float)i/((float)vertex_count - 1));
				    vertex.TexCoords = vec;
				    vertexPointer++;
                    vertices.push_back(vertex);

			    }
		    }

		    for(int gz=0;gz<vertex_count-1;gz++){
			    for(int gx=0;gx<vertex_count-1;gx++){
				    int topLeft = (gz*vertex_count)+gx;
				    int topRight = topLeft + 1;
				    int bottomLeft = ((gz+1)*vertex_count)+gx;
				    int bottomRight = bottomLeft + 1;
                    indices.push_back(topLeft);
                    indices.push_back(bottomLeft);
                    indices.push_back(topRight);
                    indices.push_back(topRight);
                    indices.push_back(bottomLeft);
                    indices.push_back(bottomRight);
			    }
		    }
            
		 this->mesh = Mesh(vertices, indices);
    }

    float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos) {
	   	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	    float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	    float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	    float l3 = 1.0f - l1 - l2;
	    return l1 * p1.y + l2 * p2.y + l3 * p3.y;
    }
};