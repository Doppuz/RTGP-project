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

        Terrain(GLfloat x, glm::vec3 translate,Model* treeModel)
            :modelMatrix{glm::mat4(1.0f)}{
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
            tree = Tree(translate,treeModel);
        }

        void createTree(){
            //t = Tree(glm::vec3(0.0f, 300.0f,-800),"../../models/Tree/Tree.obj");
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
            float terrainX = 230;
            float terrainZ = 230;
            float gridSquareSize = size / ((float)vertex_count - 1);
            int gridX = (int) floor(terrainX / gridSquareSize);
            int gridZ = (int) floor(terrainZ / gridSquareSize);

            if(gridX > (vertex_count - 1) || gridZ > (vertex_count - 1) || gridX < 0 || gridZ < 0)
                return 0;

            float xCoord = fmod(terrainX,gridSquareSize)/gridSquareSize;
            float zCoord = fmod(terrainZ,gridSquareSize)/gridSquareSize;
            float answer;

            if(xCoord <= (1 - zCoord)){
                answer = (heights[gridX][gridZ] + heights[gridX + 1][gridZ] + heights[gridX][gridZ + 1])/3; 
            }else{
                answer = (heights[gridX + 1][gridZ] + heights[gridX + 1][gridZ + 1] + heights[gridX][gridZ + 1])/3;
            }

            //std::cout << gridX << " " << gridZ << std::endl;
            //std::cout << xCoord << " " << zCoord << std::endl;
            return answer;
        }

    private:
        const static GLint size = 500; //10000
        const static GLint vertex_count = 12; //32
        FastNoiseLite noise;
        glm::mat4 modelMatrix;
        float height = 3000; 
        float heights[vertex_count][vertex_count];
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
                    heights[i][j] = vector.y;
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

};