
#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in float h;
in float visibility;
in vec3 interp_UVW;

uniform vec3 skyColor;

uniform sampler2D reflTexture;
uniform sampler2D refrTexture;

uniform vec3 fogColor;

uniform vec3 cameraPosition;

in vec4 worldPosition;
in vec3 worldNormal;
in vec4 clipSpace;

/*vec4 Reflection() // this name is the one which is detected by the SetupShaders() function in the main application, and the one used to swap subroutines
{
    // vector from vertex to camera in world coordinates
    vec3 V = normalize(worldPosition.xyz - cameraPosition);

    // reflection vector of I with respect to normal
    vec3 R = reflect( V, normalize(worldNormal));

    // we sample the texture cube using the components of the reflection vector as texture coordinates. 
    return texture(waterTexture, R);
}*/

void main(){
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;
    vec2 refractionTextureCoor = vec2(ndc.x,ndc.y);
    vec2 reflectionTextureCoor = vec2(ndc.x,-ndc.y);

    vec4 reflectionTexture = texture(reflTexture, outTexture);
    vec4 refractionTexture = texture(refrTexture, outTexture);
    FragColor = reflectionTexture;
    //FragColor = Reflection();
    //FragColor = texture(waterTexture, outTexture);
    //FragColor = mix(vec4(fogColor,1),FragColor,visibility);
}
