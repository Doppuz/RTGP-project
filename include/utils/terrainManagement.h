#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <glfw/glfw3.h>
#include <utils/mesh_v1.h>

#include <string>

#include <utils/terrain.h>

class TerrainManagement{
public:
    vector<Terrain> terrains;

    //TerrainManagement(vector<Terrain> terrains){
    //TerrainManagement(Terrain* terrain,Terrain* terrain2,Terrain* terrain3,Terrain* terrain4,Terrain* terrain5,Terrain* terrain6,
    //    Terrain* terrain7,Terrain* terrain8,Terrain* terrain9){
    TerrainManagement()
        :size{Terrain::getSize()}{
        Terrain terrain(0, glm::vec3(-size, 0.0f, -size));
        Terrain terrain1(1, glm::vec3(0.0f, 0.0f, -size));
        Terrain terrain2(2, glm::vec3(size, 0.0f, -size));
        Terrain terrain3(3, glm::vec3(-size, 0.0f, 0));
        Terrain terrain4(4, glm::vec3(0, 0.0f, 0));
        Terrain terrain5(5, glm::vec3(+size, 0.0f, 0));
        Terrain terrain6(6, glm::vec3(-size, 0.0f, size));
        Terrain terrain7(7, glm::vec3(0.0f, 0.0f, size));
        Terrain terrain8(8, glm::vec3(size, 0.0f,size));

        terrains.push_back(terrain);
        terrains.push_back(terrain1);
        terrains.push_back(terrain2);
        terrains.push_back(terrain3);
        terrains.push_back(terrain4);
        terrains.push_back(terrain5);
        terrains.push_back(terrain6);
        terrains.push_back(terrain7);
        terrains.push_back(terrain8);
        
        this->vertexCount = Terrain::getVertexCount();
        initialSetUp();
        
        for(int i = 0; i < terrains.size();i++){
            terrains[i].mesh.SetupMesh(); 
        }

    }

private:
    GLint vertexCount;
    GLint size;

    void initialSetUp(){
        
        lerpVertical(&terrains[0].mesh,&terrains[3].mesh);
        lerpVertical(&terrains[1].mesh,&terrains[4].mesh);
        lerpVertical(&terrains[2].mesh,&terrains[5].mesh);
        lerpVertical(&terrains[3].mesh,&terrains[6].mesh);
        lerpVertical(&terrains[4].mesh,&terrains[7].mesh);
        lerpVertical(&terrains[5].mesh,&terrains[8].mesh);

        lerpVertical(&terrains[6].mesh,&terrains[0].mesh);
        lerpVertical(&terrains[7].mesh,&terrains[1].mesh);
        lerpVertical(&terrains[8].mesh,&terrains[2].mesh);

        lerpHorizontal(&terrains[0].mesh,&terrains[1].mesh);
        lerpHorizontal(&terrains[1].mesh,&terrains[2].mesh);
        lerpHorizontal(&terrains[3].mesh,&terrains[4].mesh);
        lerpHorizontal(&terrains[4].mesh,&terrains[5].mesh);
        lerpHorizontal(&terrains[6].mesh,&terrains[7].mesh);
        lerpHorizontal(&terrains[7].mesh,&terrains[8].mesh);
   
        lerpHorizontal(&terrains[2].mesh,&terrains[0].mesh);
        lerpHorizontal(&terrains[5].mesh,&terrains[3].mesh);
        lerpHorizontal(&terrains[8].mesh,&terrains[6].mesh);

    }

    void lerpVertical(Mesh *b, Mesh *a){
        lerpMesh(a,b);
        lerpMesh2(b);
        lerpMesh3(a);
    }

    void lerpHorizontal(Mesh *b, Mesh *a){
        lerpMeshOther(a,b);
        lerpMeshOther2(a);
        lerpMeshOther3(b);
    }

    //Lerp vertex
    void lerpMesh(Mesh *a, Mesh *b)
    {
        for (int i = 0; i < b->vertices.size() / vertexCount; i++)
        {
            //std::cout << " prima: " <<i << " , " << (b->vertices.size() - 1) - (vertexCount * ((int)(i / vertexCount) + 1) - 1) + i % vertexCount << std::endl;
            float value = lerp(a->vertices[i].Position.y, b->vertices[(b->vertices.size() - 1) - (vertexCount * ((int)(i / vertexCount) + 1) - 1) + i % vertexCount].Position.y, 0.5f);
            //std::cout << " Dopo: " << (b->vertices.size() - 2) - ((4 * ((int)i / 4) + 1)) + (i % 4 - 1) << std::endl;
            a->vertices[i].Position.y = value;
            b->vertices[(b->vertices.size() - 1) - (vertexCount * ((int)(i / vertexCount) + 1) - 1) + i % vertexCount].Position.y = value;
        }
    }

    void lerpMesh2(Mesh *a)
    {
        int size = a->vertices.size() - vertexCount - 1;
        float value1, value2, value3, value4;
        for (int i = 0; i < size; i++){
            value2 = lerp(a->vertices[(size - i)].Position.y, a->vertices[(size - i) + vertexCount].Position.y, 0.1f + ((0.9f / (vertexCount - 1)) * ((int)(i / vertexCount) + 1))); 
            a->vertices[(size - i)].Position.y = value2;
        }
    }

    void lerpMesh3(Mesh *a)
    {
        float value1, value2, value3, value4;
        for (int i = vertexCount; i < a->vertices.size(); i++)
        {
            value2 = lerp(a->vertices[i].Position.y, a->vertices[i - vertexCount].Position.y, 0.1f + ((0.9f / (vertexCount - 1)) * ((int)(i / vertexCount))));
            a->vertices[i].Position.y = value2;
        }
    }

    //Lerp vertex
    void lerpMeshOther(Mesh *a, Mesh *b)
    {
        for (int i = 0; i < b->vertices.size(); i = i + vertexCount)
        {
            float value = lerp(a->vertices[i].Position.y, b->vertices[i + vertexCount - 1].Position.y, 0.5f);
            a->vertices[i].Position.y = value;
            b->vertices[i + vertexCount - 1].Position.y = value;
        }
    }

    void lerpMeshOther2(Mesh *a)
    {
        for (int i = 1; i < vertexCount; i++)
        {
            for (int j = 0; j < a->vertices.size(); j = j + vertexCount)
            {
                float value = lerp(a->vertices[j + i].Position.y, a->vertices[j].Position.y, 0.1f + ((0.9 / (vertexCount - 2)) * (i - 1)));
                a->vertices[j + i].Position.y = value;
            }
        }
    }

    void lerpMeshOther3(Mesh *a)
    {
        for (int i = 1; i < vertexCount; i++)
        {
            for (int j = 0; j < a->vertices.size(); j = j + vertexCount)
            {
                float value = lerp(a->vertices[j + vertexCount - 1 - i].Position.y, a->vertices[j + vertexCount - i].Position.y, 0.1f + ((0.9 / (vertexCount - 2)) * (i - 1)));
                a->vertices[j + vertexCount - 1 - i].Position.y = value;
            }
        }
    }

    float lerp(float a, float b, float c)
    {
        return a * c + b * (1 - c);
    }

    //float lerp(float a, float b, float t) { return a + t * (b - a); }
};

//------------------------------------------------------------------------------------------------------------------


/*int index = 0;
            for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrain3.terrainBaseMesh.vertices[index].Position.y << " ";
                index++;
                /*std::cout << " AA: " << terrain.terrainBaseMesh.vertices[i].Position.y << " , " << terrain2.terrainBaseMesh.vertices[i].Position.y <<
                " x1: " << terrain.terrainBaseMesh.vertices[i].Position.x << " z1:" << terrain.terrainBaseMesh.vertices[i].Position.z <<
                " x2: " << terrain2.terrainBaseMesh.vertices[i].Position.x << " z2:" << terrain2.terrainBaseMesh.vertices[i].Position.z << std::endl;*/
            /*    }
            }   
            std::cout << std::endl;
            index = 0;
             for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrain4.terrainBaseMesh.vertices[index].Position.y << " ";
                index++;
                /*std::cout << " AA: " << terrain.terrainBaseMesh.vertices[i].Position.y << " , " << terrain2.terrainBaseMesh.vertices[i].Position.y <<
                " x1: " << terrain.terrainBaseMesh.vertices[i].Position.x << " z1:" << terrain.terrainBaseMesh.vertices[i].Position.z <<
                " x2: " << terrain2.terrainBaseMesh.vertices[i].Position.x << " z2:" << terrain2.terrainBaseMesh.vertices[i].Position.z << std::endl;*/
            /*    }
            } */

            /*lerpMeshOther(&terrain3.terrainBaseMesh,&terrain4.terrainBaseMesh);
            lerpMeshOther2(&terrain3.terrainBaseMesh);
            lerpMeshOther3(&terrain4.terrainBaseMesh);

            /*std::cout << std::endl;
            index = 0;
            for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrain3.terrainBaseMesh.vertices[index].Position.y << " ";
                index++;
                /*std::cout << " AA: " << terrain.terrainBaseMesh.vertices[i].Position.y << " , " << terrain2.terrainBaseMesh.vertices[i].Position.y <<
                " x1: " << terrain.terrainBaseMesh.vertices[i].Position.x << " z1:" << terrain.terrainBaseMesh.vertices[i].Position.z <<
                " x2: " << terrain2.terrainBaseMesh.vertices[i].Position.x << " z2:" << terrain2.terrainBaseMesh.vertices[i].Position.z << std::endl;*/
            /*    }
            }   
            std::cout << std::endl;
            index = 0;
             for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrain4.terrainBaseMesh.vertices[index].Position.y << " ";
                index++;
                /*std::cout << " AA: " << terrain.terrainBaseMesh.vertices[i].Position.y << " , " << terrain2.terrainBaseMesh.vertices[i].Position.y <<
                " x1: " << terrain.terrainBaseMesh.vertices[i].Position.x << " z1:" << terrain.terrainBaseMesh.vertices[i].Position.z <<
                " x2: " << terrain2.terrainBaseMesh.vertices[i].Position.x << " z2:" << terrain2.terrainBaseMesh.vertices[i].Position.z << std::endl;*/
            /*    }
            } */