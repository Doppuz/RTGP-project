
#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in float visibility;
in vec3 interp_UVW;

uniform sampler2D trunkTexture;
uniform sampler2D cactusTexture;

uniform vec3 fogColor;
uniform int objectType;

uniform float Kd;

in vec3 lightDir;
in vec3 vNormal;

subroutine vec4 ill_model();

subroutine uniform ill_model Illumination_Model;

subroutine(ill_model)
vec4 Lambert(){

    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightDir.xyz);
    float lambertian = max(dot(L,N), 0.0);
    vec4 surfaceColor;

    if(objectType == 0)
        surfaceColor = texture(trunkTexture, outTexture);
    else
        surfaceColor = texture(cactusTexture, outTexture);

    vec4 illumination = vec4(Kd * lambertian * surfaceColor);
    illumination.a = 1;
    return illumination;
}

subroutine(ill_model)
vec4 NoLight() {
    if(objectType == 0)
        return texture(trunkTexture, outTexture);
    else
        return texture(cactusTexture, outTexture);
}

void main(){
    FragColor = Illumination_Model();
    FragColor = mix(vec4(fogColor,1),FragColor,visibility);
}
