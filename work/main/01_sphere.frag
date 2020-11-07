#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;
in vec2 outTexture;

uniform sampler2D redTexture;

void main(){
    FragColor = texture(redTexture, outTexture);
}
