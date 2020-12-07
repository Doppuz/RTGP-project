
#version 460 core

// output variable for the fragment shader. Usually, it is the final color of the fragment
out vec4 FragColor;

in vec2 outTexture;

in float h;
in float visibility;
in vec3 interp_UVW;

uniform vec3 skyColor;

uniform sampler2D groundTexture;
uniform sampler2D snowTexture;
uniform sampler2D grassTexture;

uniform samplerCube textureCube;

// ambient, diffusive and specular components (passed from the application)
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
// weight of the components
// in this case, we can pass separate values from the main application even if Ka+Kd+Ks>1. In more "realistic" situations, I have to set this sum = 1, or at least Kd+Ks = 1, by passing Kd as uniform, and then setting Ks = 1.0-Kd
uniform float Ka;
uniform float Kd;
uniform float Ks;

// shininess coefficients (passed from the application)
uniform float shininess;

// light incidence direction (calculated in vertex shader, interpolated by rasterization)
in vec3 lightDir;
// the transformed normal has been calculated per-vertex in the vertex shader
in vec3 vNormal;

in vec3 vViewPosition;

vec3 Lambert() // this name is the one which is detected by the SetupShaders() function in the main application, and the one used to swap subroutines
{
    // normalization of the per-fragment normal
    vec3 N = normalize(vNormal);
    // normalization of the per-fragment light incidence direction
    vec3 L = normalize(lightDir.xyz);

    // Lambert coefficient
    float lambertian = max(dot(L,N), 0.0);

    // Lambert illumination model  
    return vec3(Kd * lambertian * texture(grassTexture, outTexture));
}

vec4 BlinnPhong() // this name is the one which is detected by the SetupShaders() function in the main application, and the one used to swap subroutines
{
    // ambient component can be calculated at the beginning
    vec4 color = vec4(Ka*ambientColor,1);

    // normalization of the per-fragment normal
    vec3 N = normalize(vNormal);
    
    // normalization of the per-fragment light incidence direction
    vec3 L = normalize(lightDir.xyz);

    // Lambert coefficient
    float lambertian = max(dot(L,N), 0.0);

    // if the lambert coefficient is positive, then I can calculate the specular component
    if(lambertian > 0.0)
    {
      // the view vector has been calculated in the vertex shader, already negated to have direction from the mesh to the camera
      vec3 V = normalize( vViewPosition );

      // in the Blinn-Phong model we do not use the reflection vector, but the half vector
      vec3 H = normalize(L + V);

      // we use H to calculate the specular component
      float specAngle = max(dot(H, N), 0.0);
      // shininess application to the specular component
      float specular = pow(specAngle, shininess);

      // We add diffusive and specular components to the final color
      // N.B. ): in this implementtion, the sum of the components can be different than 1
      color += Kd * lambertian * texture(grassTexture, outTexture) +
                      vec4(Ks * specular * specularColor,1);
    }
    return color;
}

vec4 BlinnPhong_ML_TX() // this name is the one which is detected by the SetupShaders() function in the main application, and the one used to swap subroutines
{
    // we repeat the UVs and we sample the texture
    //vec2 repeated_Uv = mod(outTexture*80, 1.0);
    vec4 surfaceColor = texture(grassTexture, outTexture);
    
    // ambient component can be calculated at the beginning
    vec4 color = vec4(Ka*ambientColor,1.0);

    // normalization of the per-fragment normal
    vec3 N = normalize(vNormal);
    
    //for all the lights in the scene
    for(int i = 0; i < 1; i++)
    {
        // normalization of the per-fragment light incidence direction
        vec3 L = normalize(lightDir.xyz);

        // Lambert coefficient
        float lambertian = max(dot(L,N), 0.0);

        // if the lambert coefficient is positive, then I can calculate the specular component
        if(lambertian > 0.0)
        {
            // the view vector has been calculated in the vertex shader, already negated to have direction from the mesh to the camera
            vec3 V = normalize( vViewPosition );

            // in the Blinn-Phong model we do not use the reflection vector, but the half vector
            vec3 H = normalize(L + V);

            // we use H to calculate the specular component
            float specAngle = max(dot(H, N), 0.0);
            // shininess application to the specular component
            float specular = pow(specAngle, shininess);

            // We add diffusive (= color sampled from texture) and specular components to the final color
            // N.B. ): in this implementation, the sum of the components can be different than 1
            color += Kd * lambertian * surfaceColor + vec4(Ks * specular * specularColor,1.0);
        }
    }
    return color;
}

void main(){
    //if(h < -15.0f)
    //    FragColor = texture(groundTexture, outTexture);
    float interpolation =  h; 
    //FragColor = vec4(Lambert(), 1.0);
    //FragColor = texture(grassTexture, outTexture);  
    FragColor = BlinnPhong_ML_TX();
    /*if (h > 8015)
        FragColor = texture(snowTexture, outTexture);
    else if (h < 15)
        FragColor = texture(grassTexture, outTexture);
    else 
        FragColor = (interpolation / 8015) * texture(snowTexture, outTexture) +
         (1 - (interpolation / 8015)) * texture(grassTexture, outTexture) ;*/

    //FragColor = mix(vec4(0.7f, 0.7f, 0.7f, 1.0f),FragColor,visibility);
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
    //FragColor = texture(grassTexture, outTexture);
    //FragColor = vec4(Lambert(grassTexture), 1.0);
}
