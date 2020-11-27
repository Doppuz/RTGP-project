#version 460 core

// output shader variable
out vec4 colorFrag;

// interpolated texture coordinates
in vec3 interp_UVW;

// texture sampler for the cube map
uniform samplerCube textureCube;

void main(){
	 // we sample the cube map
    colorFrag = texture(textureCube, interp_UVW);
}
