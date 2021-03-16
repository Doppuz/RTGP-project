
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

uniform vec3 cameraPosition;
uniform vec3 lightPosition;

// light incidence direction (calculated in vertex shader, interpolated by rasterization)

out vec2 outTexture;
out float h;

out float visibility;

const float density = 0.00032; //0.000009
const float gradient = 3;

vec4 positionRelativeToCam;

out vec4 worldPosition;
out vec3 worldNormal;
out vec4 clipSpace;
out vec3 interp_UVW;
out vec3 toCameraVector;
out vec3 fromLightVector;

void main() {

  worldPosition = model * vec4( pos, 1.0 );

  fromLightVector = worldPosition.xyz - lightPosition;

  toCameraVector = cameraPosition - worldPosition.xyz; 
  
  vec4 positionRelativeToCam = view * worldPosition;

  interp_UVW = pos;

	outTexture = texCoord * 1;
  
  h = pos.y;
  
  float distance = length(positionRelativeToCam.xyz);
  visibility = exp(-pow((distance*density),gradient));
  visibility = clamp(visibility,0.0f,1.0f);

  worldNormal = mat3(transpose(inverse(model))) * (normal);

  clipSpace = projection * positionRelativeToCam;

  gl_Position = clipSpace;
}


