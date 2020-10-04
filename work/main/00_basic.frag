
#version 410 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in float h;

uniform sampler2D grassTexture;
uniform sampler2D snowTexture;

void main(){
    //if(h < 12.0f)
        FragColor = texture(grassTexture, outTexture);
    //else
    //    FragColor = texture(snowTexture, outTexture);
}
