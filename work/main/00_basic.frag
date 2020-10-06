
#version 410 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in float h;

uniform sampler2D groundTexture;
uniform sampler2D snowTexture;
uniform sampler2D grassTexture;

void main(){
    //if(h < -15.0f)
    //    FragColor = texture(groundTexture, outTexture);
     if (h > 35)
        FragColor = texture(snowTexture, outTexture);
    else
        FragColor = texture(grassTexture, outTexture);
}
