#pragma once

#include <stdlib.h>
#include <utils/mesh_v1.h>
#include <utils/model_v1.h>
#include <ctime>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

class WorldObject{
    public:
        
        Model* model;

        WorldObject(glm::vec3 translate, Model* loadedModel)
            :modelMatrix{glm::mat4(1.0f)}, model{loadedModel}{
            modelMatrix = glm::scale(modelMatrix, glm::vec3(20,20,20));
            modelMatrix = glm::translate(modelMatrix,glm::vec3(translate.x/20,translate.y/20,translate.z/20));
            std::cout << "B " << model <<std::endl; 
            //modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));
            //loadTree(path);
        }

        WorldObject(){}

        void draw(){
            std::cout << "A " << model <<std::endl; 
           model->Draw();
        }

        glm::mat4 getModelMatrix(){
            return modelMatrix;
        }

        void translateModelMatrix(glm::vec3 translate){
            //modelMatrix = glm::scale(modelMatrix, glm::vec3(1/20,1/20,1/20));
            modelMatrix = glm::translate(modelMatrix,glm::vec3(translate.x/20,translate.y/20,translate.z/20));
            //modelMatrix = glm::scale(modelMatrix, glm::vec3(20,20,20));
        }

        glm::mat3 getNormalMatrix(glm::mat4 viewMatrix){
            return  glm::inverseTranspose(glm::mat3(viewMatrix*modelMatrix));
        }


    private:
        glm::mat4 modelMatrix;

        /*void loadTree(string path){
            model = Model(path);
            for(int i = 0; i < model.meshes.size(); i++){
                model.meshes[i].SetupMesh();
            }
        }*/
	
};