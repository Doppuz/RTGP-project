
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

out vec2 outTexture;

out float visibility;

const float density = 0.000009; //0.000008
const float gradient = 10;

vec4 positionRelativeToCam;

void main() {
	//float height = noise(pos.xz);
	outTexture = texCoord;
  
  vec4 positionRelativeToCam = view * model * vec4(pos, 1.0f);
  float distance = length(positionRelativeToCam.xyz);
  visibility = exp(-pow((distance*density),gradient));
  visibility = clamp(visibility,0.0f,1.0f);

  gl_Position = projection * positionRelativeToCam;
}


