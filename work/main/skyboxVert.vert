#version 460 core

// vertex position in world coordinates
layout (location = 0) in vec3 position;

out vec3 interp_UVW;

// view matrix
uniform mat4 view;
// Projection matrix
uniform mat4 projection;

void main(){
		interp_UVW = position;

		// we apply the transformations to the vertex
        vec4 pos = projection * view * vec4(position, 1.0);
		
        gl_Position = pos.xyww;
}
