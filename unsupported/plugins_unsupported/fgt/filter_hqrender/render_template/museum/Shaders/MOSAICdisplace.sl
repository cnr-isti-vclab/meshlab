/* Beta-0.4.7: MOSAIC default displacement shader for Blender integration */

#define DISPLACE_UTILITIES 1
#include "MOSAICfunctions.h"

displacement
MOSAICdisplace(
    uniform float Mid                   = 0.5;
    uniform float DispFac               = 1.0;
	uniform float GlobUF_X0[2]			= {0, 0};
	uniform string DispUS_X0[10]			= {"", "", "", "", "", "", "", "", "", ""};
	varying float DispVF_X0[27]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0};
	varying float DispVF_X1[27]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0};
	varying float DispVF_X2[27]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0};
	varying float DispVF_X3[27]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0};
	varying float DispVF_X4[27]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0};
	varying float DispVF_X5[27]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0};
	varying float DispVF_X6[27]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0};
	varying float DispVF_X7[27]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0};
	varying float DispVF_X8[27]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0};
	varying float DispVF_X9[27]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0};
 varying float                       s1 = 0, t1 = 0, s2 = 0, t2 = 0, s3 = 0, t3 = 0, s4 = 0, t4 = 0, s5 = 0, t5 = 0, s6 = 0, t6 = 0, s7 = 0, t7 = 0, s8 = 0, t8 = 0; // Texture uv layers
    varying float                       v0 = 1, v1 = 1, v2 = 1, v3 = 1, v4 = 1, v5 = 1, v6 = 1, v7 = 1, v8 = 1, v9 = 1; // Vertex groups
    varying float Ms                    = -1;   //Material index number from mesh (for multipass and layered shading)
    uniform string Mx                   = "";   //TexFace image names from mesh (if enabled in exporter)
    output varying point __displacein   = 0;    // Input parameter for layered shading
    output varying point __displaceout  = 0;)   // Output parameter for layered shading
{
    // Only shade a surface element belonging to the current material index unless disabled
    if (GlobUF_X0[MAT_UF_SETMODE] == 0 || Ms == -1 || GlobUF_X0[MAT_UF_SETINDEX] == Ms)
    {
        //// Lets setup general purpose variables...
        uniform string Name             = "";
        uniform float                   a = 0, i = 0, channels = 3, isDisp = 0, isNor = 0;
        varying float                   sAdj, tAdj, p1, p2, p3, p4, p5, p6, p7, mask = 1;
        varying color                   Cp1, Cp2;
        float stLayers[16]              = {s, t, s1, t1, s2, t2, s3, t3, s4, t4, s5, t5, s6, t6, s7, t7};
        float vGroups[10]               = {v0, v1, v2, v3, v4, v5, v6, v7, v8, v9};
        normal Un                       = N;
        
        //// Lets grab any user attributes from RIB
        float displacelayer             = 0;
        attribute("user:displace_layer", displacelayer);
        
        // These are the material channels defined in Blender's material system per texture for blending
        color                           Nor = color(0), NorFac = color(0), Disp = color(0);
        
        //// Cycle through all texture layers and blend to each material channels
        for (i = 0; i < GLOB_TEX_CHANNELS; i += 1)
        {
            Name                        = DispUS_X0[i];
            
            // Make sure theres a texture before we process it!
            if (Name != "")
            {               
                // Lets initialize shared variables...
                float disp_paras[DISP_VF_PARALEN];
                uniform float tex_channels  = 0;
                textureinfo(Name, "channels", tex_channels);
                
                // Lets load up a temporary array of the current textures parameters
                for (a = 0; a < DISP_VF_PARALEN; a += 1)
                {
                    if (i == 0)         disp_paras[a] = DispVF_X0[a];
                    else if (i == 1)    disp_paras[a] = DispVF_X1[a];
                    else if (i == 2)    disp_paras[a] = DispVF_X2[a];
                    else if (i == 3)    disp_paras[a] = DispVF_X3[a];
                    else if (i == 4)    disp_paras[a] = DispVF_X4[a];
                    else if (i == 5)    disp_paras[a] = DispVF_X5[a];
                    else if (i == 6)    disp_paras[a] = DispVF_X6[a];
                    else if (i == 7)    disp_paras[a] = DispVF_X7[a];
                    else if (i == 8)    disp_paras[a] = DispVF_X8[a];
                    else if (i == 9)    disp_paras[a] = DispVF_X9[a];
                }
                
                float stLayer           = disp_paras[DISP_VF_STLAYER];
                float stCoor            = disp_paras[DISP_VF_STCOOR];
                float gIndex            = disp_paras[DISP_VF_VERTGRP];
                float vertGrp           = 1;
                float midLevel          = Mid;
                
                // Use vertex group value if specified...
                if (gIndex != -1 && gIndex < 10)
                    vertGrp             = vGroups[gIndex];
                
                // Use mid level value if specified...
                if (disp_paras[DISP_VF_MID] != -1)
                    midLevel            = disp_paras[DISP_VF_MID];
                
                // Adjust offset and size of st coordinates...
                p1                      = disp_paras[DISP_VF_OFSX];
                p2                      = disp_paras[DISP_VF_OFSY];
                p3                      = disp_paras[DISP_VF_SIZEX];
                p4                      = disp_paras[DISP_VF_SIZEY];
                // Is this texture using "sticky" coordinates?
                if (stCoor == 256)
                {
                    sAdj                = s8;
                    tAdj                = t8;
                }
                // Is this texture using a UV layer?
                else if (stCoor == 16)
                {
                    sAdj                = stLayers[stLayer*2];
                    tAdj                = stLayers[stLayer*2+1];
                }
                // If using a coordinate mode not supported just use standard st
                else
                {
                    sAdj                = s;
                    tAdj                = t;
                }
                // Rotate and mirror texture?
                if (disp_paras[DISP_VF_ROT90] != 0)
                {
                    uniform float rot   = radians(-90);
                    sAdj                -= 0.5;
                    tAdj                -= 0.5;
                    p1                  = (cos(rot)*sAdj-sin(rot)*tAdj)+0.5;
                    p2                  = (sin(rot)*sAdj+cos(rot)*tAdj)+0.5;
                    sAdj                = 1-p1;
                    tAdj                = p2;
                }
                stAdjust(sAdj, tAdj, p1, p2, p3, p4);
                
                // Get and adjust color value from texture...
                uniform float alpha_channel = tex_channels-1;
                p1                      = disp_paras[DISP_VF_FILTER];
                if (tex_channels < 3)
                    Cp1                 = color(float texture(Name[0], sAdj, tAdj, "blur", p1));
                else
                    Cp1                 = color texture(Name, sAdj, tAdj, "blur", p1);
                p6                      = comp(ctransform("hsv", Cp1), 2);
                
                // Get and adjust alpha value from texture...
                if (disp_paras[DISP_VF_USEALPHA] != 0 && (tex_channels == 2 || tex_channels == 4))
                    p1                  = float texture(Name[alpha_channel], sAdj, tAdj, "blur", p1, "fill", 1);
                else
                    p1                  = 1;
                if (disp_paras[DISP_VF_NEGALPHA] != 0)
                    p1                  = 1-p1;
                if (disp_paras[DISP_VF_CALCALPHA] != 0)
                    p1                  *= p6;
                if (disp_paras[TEX_VF_STENCIL] != 0)
                    mask                *= p1;
                p1                      *= mask;
                
                // Alter texture colors before blending...
                Cp2                 = color(disp_paras[DISP_VF_TEXR], disp_paras[DISP_VF_TEXG], disp_paras[DISP_VF_TEXB]);
                Cp1                 *= Cp2;
                if (disp_paras[DISP_VF_CONTR] != 1)
                    Cp1             = (Cp1-0.5*Cp2)*disp_paras[DISP_VF_CONTR]+0.5*Cp2;
                if (disp_paras[DISP_VF_BRIGHT] != 1)
                    Cp1             = clamp(Cp1+Cp2*(disp_paras[DISP_VF_BRIGHT]-1), color(0), color(2));
                if (disp_paras[DISP_VF_NEG] != 0)
                    Cp1                 = color(1)-Cp1;
                
                // Lets start blending texture according to assigned channel
                p2                      = disp_paras[DISP_VF_BLENDMODE];
                p3                      = disp_paras[DISP_VF_NORFAC]*p1;
                p4                      = disp_paras[DISP_VF_DISPFAC]*p1;
                if (disp_paras[DISP_VF_ISNOR] != 0)
                {
                    isNor               = 1;
                    p7                  = disp_paras[DISP_VF_ISNOR];
                    Nor                 = colorblend(Nor, Cp1, 7, 1, p7);
                    NorFac              = colorblend(NorFac, color(p7), p2, p3, p7);
                }
                if (disp_paras[DISP_VF_ISDISP] != 0)
                {
                    isDisp              = 1;
                    p7                  = disp_paras[DISP_VF_ISDISP];
                    Disp                = colorblend(Disp, (Cp1-midLevel)*vertGrp, p2, p4, p7);
                }
            }
        }
        
        //Are we using a displacement texture map?
        if (isDisp != 0)
        {
            float Amp                   = comp(ctransform("hsv", Disp), 2)*DispFac;
            P                           += normalize(Un)*Amp;
            Un                          = calculatenormal(P);
        }
        //Are we using a normal texture map?
        if (isNor != 0)
        {
            normal Nref                 = normalize(Un);
            vector dPds                 = normalize(Deriv(P, s));
            vector dPdt                 = normalize(Deriv(P, t));
            vector Nvector              = vector(Nor)*2-vector(1);
            vector Ntransformed         = vector(comp(Nvector, 0)*comp(dPds, 0)+comp(Nvector, 1)*comp(dPdt, 0)+comp(Nvector, 2)*comp(Nref, 0),
                                                 comp(Nvector, 0)*comp(dPds, 1)+comp(Nvector, 1)*comp(dPdt, 1)+comp(Nvector, 2)*comp(Nref, 1),
                                                 comp(Nvector, 0)*comp(dPds, 2)+comp(Nvector, 1)*comp(dPdt, 2)+comp(Nvector, 2)*comp(Nref, 2));
            N                           = mix(Un, normal normalize(Ntransformed), comp(ctransform("hsv", NorFac), 2)/25.0);
        }
        else N                          = Un;
    }
    //// If multipass material mode clear surface elements not in this material set
    else if (GlobUF_X0[MAT_UF_SETMODE] == 1)
    {
        N                               = 0;
        P                               = 0;
    }
}
