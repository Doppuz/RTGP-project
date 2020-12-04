#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <glfw/glfw3.h>
#include <utils/mesh_v1.h>

#include <string>

#include <utils/terrain.h>

class TerrainManagement{
public:

    enum direction{
        BACKWARD,
        FORWARD,
        RIGHT,
        LEFT
    };

    vector<Terrain> terrains;

    TerrainManagement()
        :size{Terrain::getSize()},lastXPosition{100000}, lastZPosition{100000}{
        
        rowLength = 21;
        indexLastRow  = rowLength * (rowLength - 1);
        indexLastColumn = (rowLength - 1);

        int index = 0;

        for(int j = 0; j < rowLength; j++){
            for(int i = 0; i < rowLength; i++){
                Terrain terrain(index, glm::vec3(-size + i *size, 0.0f, -size + j *size));
                terrains.push_back(terrain);
                index ++;
            }
        }
        
        this->vertexCount = Terrain::getVertexCount();
        initialSetUp();
        
        for(int i = 0; i < terrains.size();i++){
            terrains[i].mesh.SetupMesh(); 
        }

    }

    GLint getLastX(){
        return lastXPosition;
    }

    GLint getLastZ(){
        return lastZPosition;
    }

    void increaseZ(bool negative) {
        negative ? lastZPosition -= size : lastZPosition += size;
    }

    void increaseX(bool negative) {
        negative ? lastXPosition -= size : lastXPosition += size;
    }

    void translateTerrain(int index){
        switch(index){
            case BACKWARD:
                for(int i = 0; i < rowLength; i++){
                    terrains[indexLastRow+i].translateModelMatrix( glm::vec3(0.0f, 0.0f, -(size * rowLength)));
                }
                indexLastRow = (indexLastRow + rowLength * (rowLength - 1)) % terrains.size();
                break;
            case FORWARD:
                {
                   int indexFirstRow = (indexLastRow + rowLength) % terrains.size();
                    for(int i = 0; i < rowLength; i++){
                        terrains[indexFirstRow+i].translateModelMatrix( glm::vec3(0.0f, 0.0f, +(size * rowLength)));
                    }
                    indexLastRow = indexFirstRow;
                }
                break;
            case RIGHT:
                for(int i = 0; i < rowLength; i++){
                    terrains[indexLastColumn + rowLength *i].translateModelMatrix( glm::vec3(-(size * rowLength), 0.0f,0.0f));
                }
                indexLastColumn = (indexLastColumn + (rowLength - 1)) % rowLength;
                break;
            case LEFT:
                {
                    int indexFirstColoumn = (indexLastColumn + 1) % rowLength;
                    for(int i = 0; i < rowLength; i++){
                        terrains[indexFirstColoumn + rowLength *i].translateModelMatrix( glm::vec3(+(size * rowLength), 0.0f, 0.0f));
                    }
                    indexLastColumn = indexFirstColoumn;
                }
                break;
        }
    }

private:
    GLint vertexCount;
    GLint size;
    GLint lastXPosition;
    GLint lastZPosition;
    GLint indexLastRow;
    GLint indexLastColumn;
    int rowLength;
    
    void initialSetUp(){

        for(int i=0; i < terrains.size(); i++){
            if(i >= (rowLength - 1) && (i+1) % (rowLength) == 0){
                lerpHorizontal(&terrains[i].mesh,&terrains[i - (rowLength - 1)].mesh);
            }else{
                lerpHorizontal(&terrains[i].mesh,&terrains[i + 1].mesh);
            }       
        }

        for(int i=0; i < terrains.size(); i++){
            if(i >= rowLength * (rowLength - 1)){
                lerpVertical(&terrains[i].mesh,&terrains[i - rowLength * (rowLength - 1)].mesh);
            }else{
                lerpVertical(&terrains[i].mesh,&terrains[i + rowLength].mesh);
            }       
        }

        //for(int i = 0; i < terrains.size(); i++)
            //calculateNormal(&terrains[i].mesh,i);


        /*int index = 0;
            for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrains[0].mesh.vertices[index].Position.y << " ";
                index++;
                }
            }   
            
            std::cout << std::endl;

         index = 0;
            for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrains[3].mesh.vertices[index].Position.y << " ";
                index++;
                }
            } 

            std::cout << std::endl;
            index = 0;
            for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " " << terrains[6].mesh.vertices[index].Position.y << " ";
                index++;
                }
            } 

        calculateNormal(&terrains[0].mesh,0);

         std::cout << std::endl;
            index = 0;
             for(int i = 0; i < vertexCount; i++){
                std::cout << std::endl;
                for(int j = 0; j < vertexCount; j++){
                std::cout << " ( " << terrains[0].mesh.vertices[index].Normal.x << " " << terrains[0].mesh.vertices[index].Normal.y << " " 
                    << terrains[0].mesh.vertices[index].Normal.z << " ) ";
                index++;
                }
            }*/
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

    void calculateNormal(Mesh *mesh, int pos){
        int predecessorLeft = pos % 3 == 0 ? pos + 2: pos -1;
        int predecessorRight = pos % 3 == 2 ? pos - 2 : pos + 1;
        int predecessorDown = pos < 3 ? pos + 6: pos - 3;
        int predecessorUp = pos > 5 ? pos - 6: pos + 3;

        vector<Vertex> vertex = mesh -> vertices;
        for(int i = 0; i < vertex.size(); i++){
            float heightL = 0,heightR = 0,heightD = 0,heightU = 0;

            heightL = i % vertexCount != 0 ? vertex[i-1].Position.y : this->terrains[predecessorLeft]
                .mesh.vertices[(vertexCount)*((int)(i/vertexCount)+1) - 2].Position.y;

            heightR = i % vertexCount != (vertexCount - 1) ? vertex[i+1].Position.y : this->terrains[predecessorRight]
                .mesh.vertices[(vertexCount)*((int)(i/vertexCount))+1].Position.y;

            heightD = i >= vertexCount ? vertex[i - vertexCount].Position.y : this->terrains[predecessorDown]
                .mesh.vertices[(vertexCount * vertexCount) - 2 * vertexCount + i].Position.y;
             
            heightU = i < (vertex.size() - vertexCount) ? vertex[i + vertexCount].Position.y :  this->terrains[predecessorUp]
                .mesh.vertices[vertexCount + i - (vertex.size() - vertexCount)].Position.y;

            glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightU - heightD);
            glm::normalize(normal);
            mesh->vertices[i].Normal = normal;
        }
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