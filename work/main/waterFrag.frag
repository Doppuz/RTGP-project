
#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in float h;
in float visibility;
in vec3 interp_UVW;
in vec3 toCameraVector;

uniform vec3 skyColor;

uniform sampler2D reflTexture;
uniform sampler2D refrTexture;
uniform sampler2D waterDuDvTexture;
uniform sampler2D waterNormalTexture;
uniform sampler2D depthTexture;

uniform samplerCube waterTexture;

uniform vec3 fogColor;

uniform vec3 cameraPosition;

uniform float waterMovement;

// Eta (= ratio between the 2 refraction indices)
uniform float Eta;
// exponent of Fresnel equation (=5 in literature, but we can change its value to have more artistic control on the final effect)
uniform float mFresnelPower;

uniform vec3 lightColour;

in vec4 worldPosition;
in vec3 worldNormal;
in vec4 clipSpace;
in vec3 fromLightVector;

const float waveStrength = 0.05; 
const float shineDamper = 20.0; 
const float reflectivity = 0.6;

void main(){
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;
    vec2 refractionTextureCoor = vec2(ndc.x, ndc.y);
    vec2 reflectionTextureCoor = vec2(ndc.x, ndc.y);

    float near = 10.0;
    float far = 10000;
    float depth = texture(depthTexture,refractionTextureCoor).r;
    float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    depth = gl_FragCoord.z;
    float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    float waterDepth = floorDistance - waterDistance;


    vec2 distortedTexCoords = texture(waterDuDvTexture, vec2(outTexture.x + waterMovement, outTexture.y)).rg*0.1;
	distortedTexCoords = outTexture + vec2(distortedTexCoords.x, distortedTexCoords.y+waterMovement);
	vec2 totalDistortion = (texture(waterDuDvTexture, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;// * clamp(waterDepth/20.0,0.0,1.0);

    refractionTextureCoor += totalDistortion;
    refractionTextureCoor = clamp(refractionTextureCoor,0.001,0.999);

    reflectionTextureCoor += totalDistortion;
    reflectionTextureCoor = clamp(reflectionTextureCoor,0.001,0.999);

    vec4 normalMapColour = texture(waterNormalTexture,distortedTexCoords);
    vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b * 3.0, normalMapColour.g * 2.0 - 1.0);
    normal = normalize(normal); 

    vec3 viewVector = normalize(toCameraVector);
    float refractiveFactor = dot(viewVector, normal);

    vec4 reflectionTexture = texture(reflTexture, reflectionTextureCoor);
    vec4 refractionTexture = texture(refrTexture, refractionTextureCoor);

    vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColour * specular * reflectivity;// * clamp(waterDepth/50.0,0.0,1.0);
    
    FragColor = mix(refractionTexture,reflectionTexture,refractiveFactor);
    FragColor = mix(FragColor, vec4(0.0,0.3,0.5,1),0.2) + vec4(specularHighlights,0.0); 
    FragColor = mix(vec4(fogColor,1),FragColor,visibility);
    //FragColor = vec4(waterDepth/50);
    //FragColor.a = 1;
    //FragColor.a = clamp(waterDepth/50,0.0,1.0);
}