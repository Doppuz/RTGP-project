
#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in float h;
in float visibility;
in vec3 interp_UVW;

uniform sampler2D trunkTexture;

uniform vec3 fogColor;

void main(){
    //FragColor = vec4(0.0f,0.0f,1.0f,1.0f);
    FragColor = texture(trunkTexture, outTexture);
    //FragColor = mix(vec4(fogColor,1),FragColor,visibility);
}
