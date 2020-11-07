
#version 460 core

// vertex position in world coordinates
// the number used for the location in the layout qualifier is the position of the vertex attribute
// as defined in the Mesh class
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 outTexture;

void main() {
    outTexture = texCoord;
    gl_Position = projection * view * model * vec4(pos, 1.0f);
}


