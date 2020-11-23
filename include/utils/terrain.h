#pragma once


#include <utils/mesh_v1.h>

#include <utils/FastNoiseLite.h>
#include <ctime>

class Terrain{
    public:
        
        Mesh mesh;

        Terrain(GLfloat x, glm::vec3 translate)
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
        }

    private:
        const static GLint size = 120000;
        const static GLint vertex_count = 32; //32
        FastNoiseLite noise;
        glm::mat4 modelMatrix;

        void generateBaseTerrain(){
            vector<Vertex> vertices;
		    vector<GLuint> indices;

		    int vertexPointer = 0;
            glm::vec2 vec;                    
            glm::vec3 vector;

		    for(int i=0;i<vertex_count;i++){
                Vertex vertex;
			    for(int j=0;j<vertex_count;j++){
                    vector = glm::vec3((float)j/((float)vertex_count - 1) * size, 120000 *  noise.GetNoise((float)j, (float)i)
                        ,(float)i/((float)vertex_count - 1) * size);
                    vertex.Position = vector;

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