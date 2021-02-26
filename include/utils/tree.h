#pragma once

#include <utils/mesh_v1.h>
#include <utils/model_v1.h>
#include <ctime>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

class Tree{
    public:
        
        Model model;

        Tree(glm::vec3 translate, string path)
            :modelMatrix{glm::mat4(1.0f)}{
            modelMatrix = glm::translate(modelMatrix,translate);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(20,20,20));
            loadTree(path);
        }

        Tree(){}

        void draw(){
           //model.Draw();
           model.meshes[0].Draw();
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
        glm::mat4 modelMatrix;

        void loadTree(string path){
            Model planeModel(path);
            model = planeModel;
            model.Load(path);
            for(int i = 0; i < planeModel.meshes.size(); i++){
                model.meshes[i].SetupMesh();
            }
        }
};