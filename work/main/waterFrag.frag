
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

uniform samplerCube waterTexture;

uniform vec3 fogColor;

uniform vec3 cameraPosition;

// Eta (= ratio between the 2 refraction indices)
uniform float Eta;
// exponent of Fresnel equation (=5 in literature, but we can change its value to have more artistic control on the final effect)
uniform float mFresnelPower;

uniform vec3 pointLightPosition;

in vec4 worldPosition;
in vec3 worldNormal;
in vec4 clipSpace;

vec4 Reflection() // this name is the one which is detected by the SetupShaders() function in the main application, and the one used to swap subroutines
{
    // vector from vertex to camera in world coordinates
    vec3 V = normalize(worldPosition.xyz - cameraPosition);

    // reflection vector of I with respect to normal
    vec3 R = reflect( V, normalize(worldNormal));

    // we sample the texture cube using the components of the reflection vector as texture coordinates. 
    return texture(waterTexture, R);
}

/*vec4 Fresnel() // this name is the one which is detected by the SetupShaders() function in the main application, and the one used to swap subroutines
{
    vec3 N = normalize(worldNormal);

    // vector from vertex to camera in world coordinates
    vec3 V = normalize(worldPosition.xyz - cameraPosition);

    // incidence light direction in world coordinates
    vec3 L = normalize(worldPosition.xyz - pointLightPosition);

    // half vector in world coordinates
    vec3 H = normalize(L + V);

    // reflection vector of V with respect to normal
    vec3 R = reflect(V, N);

    // we sample the texture cube using the components of the reflection vector as texture coordinates.
    vec4 reflectedColor = texture( waterTexture, R);

    // refraction vectors
    // we use 3 vectors, one for each RGB channel, to simulate chromatic aberration:
    // we use a slightly different value of Eta in the second and third vector, in order to have 3 slightly different colors
    vec3 refractDir[3];
    refractDir[0] = refract( V , N, Eta );
    refractDir[1] = refract( V , N, Eta * 0.99 );
    refractDir[2] = refract( V , N, Eta * 0.98 );

    // we sample 3 times the cube map for the refraction, and we use only 1 channel for each for each sample
    vec4 refractedColor = vec4( 1.0 );
    refractedColor.r = texture( waterTexture,refractDir[0]).r;
    refractedColor.g = texture( waterTexture,refractDir[1]).g;
    refractedColor.b = texture( waterTexture,refractDir[2]).b;

    // Fresnel equation with Schlik's approximation
    // F(0°) factor
    float F0 = ((1.0-Eta)*(1.0-Eta)) / ((1.0+Eta)*(1.0+Eta));
    // ratio between reflection and refraction
    float Ratio = F0 + (1.0-F0) * pow( 1.0 - max(dot(V, H),0.0), mFresnelPower );

    // we merge the 2 colors, using the ratio calculated with the Fresnel equation
    return mix( refractedColor, reflectedColor, clamp( Ratio, 0.0, 1.0 ));
}*/


void main(){
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;
    vec2 refractionTextureCoor = vec2(ndc.x,ndc.y);
    vec2 reflectionTextureCoor = vec2(ndc.x,-ndc.y);

    vec4 reflectionTexture = texture(reflTexture, refractionTextureCoor);
    vec4 refractionTexture = texture(refrTexture, refractionTextureCoor);
    FragColor = reflectionTexture;
    //FragColor = mix(vec4(0.1,0.5,0.7,0.5),Reflection(),0.5);
    //FragColor = texture(waterTexture, outTexture);
    //FragColor = vec4(0.1,0.5,0.7,0.5);//mix(vec4(0.1,0.5,0.7,0.5),refractionTexture,0.4);
    //FragColor = Reflection();
    //FragColor = Reflection();
    //FragColor.a = 0.4;
    //FragColor = mix(vec4(0.1,0.5,0.7,0.5),FragColor,0.5);
}
