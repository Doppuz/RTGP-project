
#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in float h;
in float visibility;
in vec3 interp_UVW;

uniform vec3 skyColor;

uniform sampler2D waterTexture;

uniform vec3 fogColor;

void main(){
    FragColor = texture(waterTexture, outTexture);
    //FragColor = mix(vec4(fogColor,1),FragColor,visibility);
}
