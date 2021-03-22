
#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in float yPos;
in float visibility;
in vec3 interp_UVW;

uniform sampler2D groundTexture;
uniform sampler2D snowTexture;
uniform sampler2D grassTexture;

uniform float Kd;

uniform vec3 fogColor;

// light incidence direction (calculated in vertex shader, interpolated by rasterization)
in vec3 lightDir;
// the transformed normal has been calculated per-vertex in the vertex shader
in vec3 vNormal;

//Texture blending
float factor;
float upperLimit = 115.f;
float lowerLimit = 15.f;

// the "type" of the Subroutine
subroutine vec4 ill_model();

// Subroutine Uniform (it is conceptually similar to a C pointer function)
subroutine uniform ill_model Illumination_Model;

subroutine(ill_model)
vec4 Lambert() {
    // normalization of the per-fragment normal
    vec3 N = normalize(vNormal);
    // normalization of the per-fragment light incidence direction
    vec3 L = normalize(lightDir.xyz);

    // Lambert coefficient
    float lambertian = max(dot(L,N), 0.0);

    // Lambert illumination model  
    vec4 surfaceColor;

    factor = (yPos - lowerLimit) / (upperLimit - lowerLimit);
    factor = clamp(factor ,0.0, 1.0);

    if (yPos > upperLimit)
        surfaceColor = texture(snowTexture, outTexture);
    else if (yPos < lowerLimit)
        surfaceColor = texture(grassTexture, outTexture);
    else 
        surfaceColor = mix(texture(grassTexture, outTexture),texture(snowTexture, outTexture),factor);

    vec4 illumination = vec4(Kd * lambertian * surfaceColor);
    illumination.a = 1;
    return illumination;
}

subroutine(ill_model)
vec4 NoLight() {
    factor = (yPos - lowerLimit) / (upperLimit - lowerLimit);
    factor = clamp(factor ,0.0, 1.0);

    if (yPos > upperLimit)
        return texture(snowTexture, outTexture);
    else if (yPos < lowerLimit)
        return texture(grassTexture, outTexture);
    else 
        return mix(texture(grassTexture, outTexture),texture(snowTexture, outTexture),factor);
}

void main(){
    FragColor = Illumination_Model();
    //FragColor = mix(vec4(fogColor,1),FragColor,visibility);
}
