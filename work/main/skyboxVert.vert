
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

uniform vec3 pointLightPosition;

// light incidence direction (calculated in vertex shader, interpolated by rasterization)
out vec3 lightDir;
// the transformed normal has been calculated per-vertex in the vertex shader
out vec3 vNormal;

out vec2 outTexture;
out float h;

out vec3 interp_UVW;

void main(){
		interp_UVW = pos;
          
		// we apply the transformations to the vertex
        vec4 pos = projection * view * vec4(pos, 1.0);
		
        gl_Position = pos.xyww;
}
