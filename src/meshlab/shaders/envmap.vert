//
// Vertex shader for environment mapping with an
// equirectangular 2D texture
//
// Authors: John Kessenich, Randi Rost
//
// Copyright (c) 2002-2004 3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//

varying vec3  Normal;
varying vec3  EyeDir;
uniform float Shininess;
uniform float SpecularVal;

vec4 Ambient;
vec4 Diffuse;
vec4 Specular;

    void directionalLight(in int i, in vec3 normal)
    {
        float nDotVP;         // normal . light direction
        float nDotHV;         // normal . light half vector
        float pf;             // power factor

        nDotVP = max(0.0, dot(normal, normalize(vec3 (gl_LightSource[i].position))));
        nDotHV = max(0.0, dot(normal, vec3 (gl_LightSource[i].halfVector)));

        if (nDotVP == 0.0)
            pf = 0.0;
        else
            pf = pow(nDotHV, Shininess);

        Ambient  += gl_LightSource[i].ambient;
        Diffuse  += gl_LightSource[i].diffuse * nDotVP;
        Specular += gl_LightSource[i].specular * pf;
    }

    vec3 fnormal(void)
    {
        //Compute the normal 
        vec3 normal = gl_NormalMatrix * gl_Normal;
        normal = normalize(normal);
        return normal;
    }

    void flight(in vec3 normal, in vec4 ecPosition, float alphaFade)
    {
        vec4 color;
        vec3 ecPosition3;
        vec3 eye;

        ecPosition3 = (vec3 (ecPosition)) / ecPosition.w;
        eye = vec3 (0.0, 0.0, 1.0);

        // Clear the light intensity accumulators
        Ambient  = vec4 (0.0);
        Diffuse  = vec4 (0.0);
        Specular = vec4 (0.0);

       directionalLight(0, normal);

        color = gl_FrontLightModelProduct.sceneColor +
       //             Ambient  * gl_FrontMaterial.ambient +
       //             Diffuse  * gl_FrontMaterial.diffuse;
                    Ambient  * gl_Color +
                    Diffuse  * gl_Color;
        color += Specular * vec4(SpecularVal,SpecularVal,SpecularVal,1.0);
        color = clamp( color, 0.0, 1.0 );
        gl_FrontColor = color;
        gl_FrontColor.a *= alphaFade;
    }  


    void main(void)
    {		
        vec3  transformedNormal;
        float alphaFade = 1.0;

        // Eye-coordinate position of vertex, needed in various calculations
        vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;

        // Do fixed functionality vertex transform
        gl_Position = ftransform();
		Normal= fnormal();
        flight(Normal, ecPosition, alphaFade);
        //EyeDir=vec3(-gl_Vertex);
        vec4 pos       = gl_ModelViewMatrix * gl_Vertex;
    	EyeDir         = pos.xyz;

    //vec4 pos       = gl_ModelViewMatrix * gl_Vertex;
    //EyeDir         = pos.xyz;
    //LightIntensity = max(dot(normalize(LightPos - EyeDir), Normal), 0.0);
    }
