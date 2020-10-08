
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
    float interpolation =  h - 15; 
    if (h > 35)
        FragColor = texture(snowTexture, outTexture);
    else if (h < 15 && h > -20)
        FragColor = texture(grassTexture, outTexture);
    else if(h >= 15 && h <= 35)
        FragColor = (interpolation / 20) * texture(snowTexture, outTexture) +
         (1 - (interpolation / 20)) * texture(grassTexture, outTexture) ;
    else if (h >= -50 && h <= -20)
        FragColor = ((abs(h) - 20) / 30) * texture(groundTexture, outTexture) +
         (1 - ((abs(h) - 20) / 30)) * texture(grassTexture, outTexture) ;
    else if (h < -50)
        FragColor = texture(groundTexture, outTexture);
    
}
