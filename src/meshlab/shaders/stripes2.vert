vec4 Ambient;
vec4 Diffuse;
vec4 Specular;
varying vec3 pos;

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
            pf = pow(nDotHV, gl_FrontMaterial.shininess);

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
        color += Specular * gl_FrontMaterial.specular;
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
        transformedNormal = fnormal();
        flight(transformedNormal, ecPosition, alphaFade);
        pos=vec3(gl_Vertex);
    }
