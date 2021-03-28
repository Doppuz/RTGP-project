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

        WorldObject(glm::vec3 translate,float scale,float rotation, Model* loadedModel)
            :modelMatrix{glm::mat4(1.0f)}, model{loadedModel}{
            modelMatrix = glm::scale(modelMatrix, glm::vec3(scale,scale,scale));
            modelMatrix = glm::translate(modelMatrix,glm::vec3(translate.x/scale,translate.y/scale,translate.z/scale));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
            this->scale = scale;
            this->rotation = rotation;
        }

        WorldObject(){}

        void draw(){
           model->Draw();
        }

        glm::mat4 getModelMatrix(){
            return modelMatrix;
        }

        void translateModelMatrix(glm::vec3 translate){
            modelMatrix = glm::rotate(modelMatrix, glm::radians(-this->rotation), glm::vec3(1.0f, 0.0f, 0.0f));
            modelMatrix = glm::translate(modelMatrix,glm::vec3(translate.x/this->scale,translate.y/this->scale,translate.z/this->scale));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation), glm::vec3(1.0f, 0.0f, 0.0f));
        }

        glm::mat3 getNormalMatrix(glm::mat4 viewMatrix){
            return  glm::inverseTranspose(glm::mat3(viewMatrix*modelMatrix));
        }


    private:
        glm::mat4 modelMatrix;
        int scale;
        float rotation;
	
};