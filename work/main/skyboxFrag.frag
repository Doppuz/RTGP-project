#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in vec3 interp_UVW;

uniform samplerCube textureCube;

uniform vec3 fogColor;

const float lowerLimit = 0.0f;
const float upperLimit = 0.3f;

in vec3 lightDir;
in vec3 vNormal;

void main(){
	 // we sample the cube map
    FragColor = texture(textureCube, interp_UVW);

    float factor = (interp_UVW.y - lowerLimit) / (upperLimit - lowerLimit);
    factor = clamp(factor ,0.0, 1.0);
    FragColor = mix(vec4(fogColor,1.0),FragColor, factor);
}
