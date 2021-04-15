#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <cstdlib>
#include <ctime>

#include <glfw/glfw3.h>
#include <utils/mesh_v1.h>

#include <string>

#include <utils/terrain.h>
#include <utils/worldobject.h>

class TerrainManagement{
public:

    enum direction{
        BACKWARD,
        FORWARD,
        RIGHT,
        LEFT
    };

    vector<Terrain> terrains;

    TerrainManagement(Model* tree,Model *cactus)
        :size{Terrain::getSize()},lastXPosition{5000}, lastZPosition{5000}{ //4500 4500
        
        rowLength = 7;
        indexLastRow  = rowLength * (rowLength - 1);
        indexLastColumn = (rowLength - 1);
        int index = 0;
        srand((unsigned) time(0));
        for(int j = 0; j < rowLength; j++){
            for(int i = 0; i < rowLength; i++){
                int rndNum = rand() % 3;
                if(rndNum == 0 || rndNum == 1)
                    terrains.push_back(Terrain(index, glm::vec3(-size + i *size, 0.0f, -size + j *size), tree,0));
                else
                    terrains.push_back(Terrain(index, glm::vec3(-size + i *size, 0.0f, -size + j *size), cactus,1));    
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

        for(int i = 0; i < terrains.size(); i++){
            calculateNormal(&terrains[i].mesh,i);
            calculateHeights(&terrains[i]);
            terrains[i].createTree();
        }  
    }

    void lerpVertical(Mesh *b, Mesh *a){
        connectMeshHorizontally(a,b);
        propagateUpdown(b);
        propagateDownUp(a);
    }

    void lerpHorizontal(Mesh *b, Mesh *a){
        connectMeshVertically(a,b);
        propagateLeftRight(a);
        propagateRightLeft(b);
    }

    float lerp(float a, float b, float c){
        return a * c + b * (1 - c);
    }

    //Lerp vertex
    void connectMeshHorizontally(Mesh *a, Mesh *b)
    {
        for (int i = 0; i < b->vertices.size() / vertexCount; i++)
        {
            float value = lerp(a->vertices[i].Position.y, b->vertices[(b->vertices.size() - 1) - (vertexCount * ((int)(i / vertexCount) + 1) - 1) + i % vertexCount].Position.y, 0.5f);
            a->vertices[i].Position.y = value;
            b->vertices[(b->vertices.size() - 1) - (vertexCount * ((int)(i / vertexCount) + 1) - 1) + i % vertexCount].Position.y = value;
        }
        
    }

    void propagateUpdown(Mesh *a)
    {
        int size = a->vertices.size() - vertexCount - 1;
        float value1, value2, value3, value4;
        for (int i = 0; i < size; i++){
            value2 = lerp(a->vertices[(size - i)].Position.y, a->vertices[(size - i) + vertexCount].Position.y, 0.1f + ((0.9f / (vertexCount - 1)) * ((int)(i / vertexCount) + 1))); 
            a->vertices[(size - i)].Position.y = value2;
        }
    }

    void propagateDownUp(Mesh *a)
    {
        float value1, value2, value3, value4;
        for (int i = vertexCount; i < a->vertices.size(); i++)
        {
            value2 = lerp(a->vertices[i].Position.y, a->vertices[i - vertexCount].Position.y, 0.1f + ((0.9f / (vertexCount - 1)) * ((int)(i / vertexCount))));
            a->vertices[i].Position.y = value2;
        }
    }

    //Lerp vertex
    void connectMeshVertically(Mesh *a, Mesh *b)
    {
        for (int i = 0; i < b->vertices.size(); i = i + vertexCount)
        {
            float value = lerp(a->vertices[i].Position.y, b->vertices[i + vertexCount - 1].Position.y, 0.5f);
            a->vertices[i].Position.y = value;
            b->vertices[i + vertexCount - 1].Position.y = value;
        }
    }

    void propagateLeftRight(Mesh *a)
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

    void propagateRightLeft(Mesh *a)
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

    void calculateNormal(Mesh *mesh, int pos){
        int predecessorLeft = pos % rowLength == 0 ? pos + (rowLength -1): pos -1;
        int predecessorRight = pos % rowLength == (rowLength -1) ? pos - (rowLength -1) : pos + 1;
        int predecessorDown = pos < rowLength ? pos + (terrains.size() - rowLength) : pos - rowLength;
        int predecessorUp = pos > (terrains.size() - rowLength - 1) ? pos - (terrains.size() - rowLength): pos + rowLength;

        vector<Vertex> vertex = mesh -> vertices;
        for(int i = 0; i < vertex.size(); i++){
            float heightL = 0,heightR = 0,heightD = 0,heightU = 0;

            heightL = i % vertexCount != 0 ? vertex[i-1].Position.y : this->terrains[predecessorLeft]
                .mesh.vertices[(vertexCount)*((int)(i/vertexCount)+1) - 2].Position.y;
            
            heightL = lerp(heightL,vertex[i].Position.y,0.1f);

            heightR = i % vertexCount != (vertexCount - 1) ? vertex[i+1].Position.y : this->terrains[predecessorRight]
                .mesh.vertices[(vertexCount)*((int)(i/vertexCount))+1].Position.y;

            heightR = lerp(heightR,vertex[i].Position.y,0.1f);

            heightD = i >= vertexCount ? vertex[i - vertexCount].Position.y : this->terrains[predecessorDown]
                .mesh.vertices[(vertexCount * vertexCount) - 2 * vertexCount + i].Position.y;
                
            heightD = lerp(heightD,vertex[i].Position.y,0.1f);
             
            heightU = i < (vertex.size() - vertexCount) ? vertex[i + vertexCount].Position.y :  this->terrains[predecessorUp]
                .mesh.vertices[vertexCount + i - (vertex.size() - vertexCount)].Position.y;
    
            heightU = lerp(heightU, vertex[i].Position.y ,0.1f);    

            glm::vec3 normal = glm::vec3(heightL - heightR, 3.0f, heightD - heightU );
            normal = glm::normalize(normal);
            mesh->vertices[i].Normal = normal;
        }
    }

    void calculateHeights(Terrain* t){
        int index = 0;
        for(int i=0;i<t->vertex_count;i++){
			for(int j=0;j < t->vertex_count;j++){
                t->heights[i][j] = t->mesh.vertices[index].Position.y;
                index += 1;
            }
        }    
    }
};