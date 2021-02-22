#pragma once

#include <utils/mesh_v1.h>
#include <ctime>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

class Water{
    public:
        
        Mesh mesh;

        Water(GLfloat x, glm::vec3 translate,float ht)
            :modelMatrix{glm::mat4(1.0f)},height{ht}{
            
            modelMatrix = glm::translate(modelMatrix,translate);
            generatePlane();

        }

        Water(){}

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

        glm::mat3 getNormalMatrix(glm::mat4 viewMatrix){
            return  glm::inverseTranspose(glm::mat3(viewMatrix*modelMatrix));
        }


    private:
        const static GLint size = 500; //10000
        const static GLint vertex_count = 2; //32
        float height;
        glm::mat4 modelMatrix;

        void generatePlane(){
            vector<Vertex> vertices;
		    vector<GLuint> indices;

		    int vertexPointer = 0;
            glm::vec2 vec;                    
            glm::vec3 vector;

		    for(int i=0;i<vertex_count;i++){
                Vertex vertex;
			    for(int j=0;j<vertex_count;j++){
                    vector = glm::vec3((float)j/((float)vertex_count - 1) * size, height
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
         this->mesh.SetupMesh();
    }
};