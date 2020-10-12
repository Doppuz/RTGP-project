
#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in float h;

uniform sampler2D groundTexture;
uniform sampler2D snowTexture;
uniform sampler2D grassTexture;

// ambient, diffusive and specular components (passed from the application)
uniform vec3 diffuseColor;

// weight of the components
// in this case, we can pass separate values from the main application even if Ka+Kd+Ks>1. In more "realistic" situations, I have to set this sum = 1, or at least Kd+Ks = 1, by passing Kd as uniform, and then setting Ks = 1.0-Kd

uniform float Kd;

// light incidence direction (calculated in vertex shader, interpolated by rasterization)
in vec3 lightDir;
// the transformed normal has been calculated per-vertex in the vertex shader
in vec3 vNormal;

/*vec3 Lambert(sampler2D texture){ // this name is the one which is detected by the SetupShaders() function in the main application, and the one used to swap subroutines
    // normalization of the per-fragment normal
    vec3 N = normalize(vNormal);
    // normalization of the per-fragment light incidence direction
    vec3 L = normalize(lightDir.xyz);

    // Lambert coefficient
    float lambertian = max(dot(L,N), 0.0);

    // Lambert illumination model  
    return vec3(Kd * lambertian * texture(texture, outTexture));
}*/

void main(){
    //if(h < -15.0f)
    //    FragColor = texture(groundTexture, outTexture);
    float interpolation =  h - 15; 
    FragColor = texture(grassTexture, outTexture);
    /*if (h > 35)
        FragColor = texture(snowTexture, outTexture);
    else if (h < 15)
        FragColor = texture(grassTexture, outTexture);
    else 
        FragColor = (interpolation / 20) * texture(snowTexture, outTexture) +
         (1 - (interpolation / 20)) * texture(grassTexture, outTexture) ;
    /*else if (h >= -50 && h <= -20)
        FragColor = ((abs(h) - 20) / 30) * texture(groundTexture, outTexture) +
         (1 - ((abs(h) - 20) / 30)) * texture(grassTexture, outTexture) ;
    else if (h < -50)
        FragColor = texture(groundTexture, outTexture);
    
    /*float interpolation =  h - 5; 
    if (h > 35)
        FragColor =  vec4(Lambert(snowTexture), 1.0);
    else if (h < 5)
        FragColor =  vec4(Lambert(grassTexture), 1.0);
    else //if(h >= 15 && h <= 35)
        FragColor = (interpolation / 30) * vec4(Lambert(snowTexture), 1.0) +
         (1 - (interpolation / 30)) * vec4(Lambert(grassTexture), 1.0) ;*/
    //FragColor = vec4(Lambert(), 1.0);
    //FragColor = texture(grassTexture, outTexture);
    //FragColor = vec4(Lambert(grassTexture), 1.0);
}
