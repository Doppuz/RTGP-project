
#version 460 core

// vertex position in world coordinates
// the number used for the location in the layout qualifier is the position of the vertex attribute
// as defined in the Mesh class
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;

out vec3 vNormal;

uniform vec3 pointLightPosition;

// light incidence direction (calculated in vertex shader, interpolated by rasterization)
out vec3 lightDir;

out vec2 outTexture;
out float h;

out float visibility;

const float density = 0.0001;
const float gradient = 3;

vec4 positionRelativeToCam;

void main() {
  vec4 positionRelativeToCam = view * model * vec4(pos, 1.0f);

  vNormal = normalize( normalMatrix * normal );
  
  vec4 lightPos = view  * vec4(pointLightPosition, 1.0);

  lightDir = lightPos.xyz - positionRelativeToCam.xyz;

	outTexture = texCoord;
  
  float distance = length(positionRelativeToCam.xyz);
  visibility = exp(-pow((distance*density),gradient));
  visibility = clamp(visibility,0.0f,1.0f);

  gl_Position = projection * positionRelativeToCam;
}


