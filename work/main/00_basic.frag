
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

float G1(float angle, float alpha)
{
    // in case of Image Based Lighting, the k factor is different:
    // usually it is set as k=(alpha*alpha)/2
    float r = (alpha + 1.0);
    float k = (r*r) / 8.0;

    float num   = angle;
    float denom = angle * (1.0 - k) + k;

    return num / denom;
}

float alpha = 0.5f;
float PI = 3.14;
float F0 = 0.9;

vec4 GGX() // this name is the one which is detected by the SetupShaders() function in the main application, and the one used to swap subroutines
{
    // normalization of the per-fragment normal
    vec3 N = normalize(vNormal);
    // normalization of the per-fragment light incidence direction
    vec3 L = normalize(lightDir.xyz);

    // cosine angle between direction of light and normal
    float NdotL = max(dot(N, L), 0.0);

    // diffusive (Lambert) reflection component
    vec4 lambert = (Kd*texture(grassTexture, outTexture))/PI;

    // we initialize the specular component
    vec3 specular = vec3(0.0);

    // if the cosine of the angle between direction of light and normal is positive, then I can calculate the specular component
    if(NdotL > 0.0)
    {
        // the view vector has been calculated in the vertex shader, already negated to have direction from the mesh to the camera
        vec3 V = normalize( vViewPosition );

        // half vector
        vec3 H = normalize(L + V);

        // we implement the components seen in the slides for a PBR BRDF
        // we calculate the cosines and parameters to be used in the different components
        float NdotH = max(dot(N, H), 0.0);
        float NdotV = max(dot(N, V), 0.0);
        float VdotH = max(dot(V, H), 0.0);
        float alpha_Squared = alpha * alpha;
        float NdotH_Squared = NdotH * NdotH;

        // Geometric factor G2 
        // Smith’s method (uses Schlick-GGX method for both geometry obstruction and shadowing )
        float G2 = G1(NdotV, alpha)*G1(NdotL, alpha);

        // Rugosity D
        // GGX Distribution
        float D = alpha_Squared;
        float denom = (NdotH_Squared*(alpha_Squared-1.0)+1.0);
        D /= PI*denom*denom;

        // Fresnel reflectance F (approx Schlick)
        vec3 F = vec3(pow(1.0 - VdotH, 5.0));
        F *= (1.0 - F0);
        F += F0;

        // we put everything together for the specular component
        specular = (F * G2 * D) / (4.0 * NdotV * NdotL);
    }

    // the rendering equation is:
    //integral of: BRDF * Li * (cosine angle between N and L)
    // BRDF in our case is: the sum of Lambert and GGX
    // Li is considered as equal to 1: light is white, and we have not applied attenuation. With colored lights, and with attenuation, the code must be modified and the Li factor must be multiplied to finalColor
    return (lambert + vec4(specular,1))*NdotL;
}

void main(){
    //if(h < -15.0f)
    //    FragColor = texture(groundTexture, outTexture);
    float interpolation =  h; 
    //FragColor = vec4(Lambert(), 1.0);
    //FragColor = texture(grassTexture, outTexture); 
    vec4 temp =  GGX();
    temp.a = 1;
    FragColor = temp;
    //FragColor =  BlinnPhong_ML_TX();
    /*if (h > 8015)
        FragColor = texture(snowTexture, outTexture);
    else if (h < 15)
        FragColor = texture(grassTexture, outTexture);
    else 
        FragColor = (interpolation / 8015) * texture(snowTexture, outTexture) +
         (1 - (interpolation / 8015)) * texture(grassTexture, outTexture) ;*/

    FragColor = mix(vec4(0.5f, 0.5f, 0.5f, 1.0f),FragColor,visibility);
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
