
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
out vec3 interp_UVW;

out vec3 vViewPosition;

const float density = 0.000009; //0.000009
const float gradient = 3;

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

vec4 positionRelativeToCam;

void main() {
	//float height = noise(pos.xz);
  interp_UVW = pos;
	outTexture = texCoord;
  vNormal = normalize( normalMatrix * normal );;
  // light incidence direction (in view coordinate)
  vec4 positionRelativeToCam = view * model * vec4(pos, 1.0f);
  vec4 lightPos = view  * vec4(pointLightPosition, 1.0);
  
  lightDir = lightPos.xyz - positionRelativeToCam.xyz;
  h = pos.y;
  
  vViewPosition = -positionRelativeToCam.xyz;
  float distance = length(positionRelativeToCam.xyz);
  visibility = exp(-pow((distance*density),gradient));
  visibility = clamp(visibility,0.0f,1.0f);

  gl_Position = projection * positionRelativeToCam;
}


