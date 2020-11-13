#pragma once

using namespace std;

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <utils/mesh-v1.h>


class Model{
    public:
        vector<Mesh> meshes;

        Model(const string& path){
            this->loadModel(path);
        } 

        void Draw(){
            for (GLuint i = 0; i < this->meshes.size(); i++){
                this->meshes[i].Draw();
            }
        }

        virtual ~Model(){
            for (GLuint i = 0; i < this->meshes.size(); i++){
                this->meshes[i].Delete();
            }
        }

    private:
        void loadModel(string path){
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path,aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
                cout << "Error" << importer.GetErrorString() << endl;
                return;
            }

            this->processNode(scene->mRootNode,scene);
        }

        void processNode(aiNode* node, const aiScene* scene){
            for (GLuint i = 0; i < node->mNumMeshes; i++){
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                this->meshes.push_back(this->processMesh(mesh));
            }

            for(GLuint i = 0; i < node->mNumChildren; i++){
                this->processNode(node->mChildren[i], scene);
            }

        }

        Mesh processMesh(aiMesh* mesh){

            vector<Vertex> vertices;
            vector<GLuint> indices;

            for (GLuint i = 0; i < mesh->mNumVertices; i++){
                Vertex vertex;

                glm::vec3 vector;
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.Position = vector;
            
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;

                if(mesh->mTextureCoords[0]){
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = vec;

                    vector.x = mesh->mTangents[i].x;
                    vector.y = mesh->mTangents[i].y;
                    vector.z = mesh->mTangents[i].z;
                    vertex.Tangent = vector;

                    vector.x = mesh->mBitangents[i].x;
                    vector.y = mesh->mBitangents[i].y;
                    vector.z = mesh->mBitangents[i].z;
                    vertex.Bitangent = vector;

                }else{
                    vertex.TexCoords = glm::vec2(0.0f,0.0f);
                    cout << "No uv coordinates" << endl;
                }
                vertices.push_back(vertex);
            }
            for (GLuint i = 0; i < mesh->mNumFaces; i++){
                aiFace face = mesh->mFaces[i];
                for (GLuint j = 0; j < face.mNumIndices; j++){
                    indices.push_back(face.mIndices[j]);
                }
            }
            return Mesh(vertices, indices); 

 
        }
};