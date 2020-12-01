
#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in vec3 interp_UVW;

uniform vec3 skyColor;

uniform samplerCube textureCube;

// ambient, diffusive and specular components (passed from the application)
uniform vec3 diffuseColor;

// weight of the components
// in this case, we can pass separate values from the main application even if Ka+Kd+Ks>1. In more "realistic" situations, I have to set this sum = 1, or at least Kd+Ks = 1, by passing Kd as uniform, and then setting Ks = 1.0-Kd

uniform float Kd;

// light incidence direction (calculated in vertex shader, interpolated by rasterization)
in vec3 lightDir;
// the transformed normal has been calculated per-vertex in the vertex shader
in vec3 vNormal;

void main(){
	 // we sample the cube map
    FragColor = texture(textureCube, interp_UVW);
}
