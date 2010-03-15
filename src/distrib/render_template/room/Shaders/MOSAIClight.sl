/* Beta-0.4.7: MOSAIC default light shader for Blender integration */

#define LIGHT_UTILITIES 1
#include "MOSAICfunctions.h"

light MOSAIClight(
    uniform float SamplesFactor         = 8;
    uniform float RaySoftFactor         = 0.06;
    uniform float BufSoftFactor         = 0.004;
    uniform float SpotCenter            = 1;
    uniform float FakeSoftUse           = 0;
    uniform float FakeSoftFade          = 0;
    uniform float FakeSoftSamples       = 32;
    uniform float FakeSoftRadius        = 0.1;
    uniform float FakeSoftPower         = 1.0;
    uniform float FakeSoftMin           = 0.0001;
    uniform float FakeSoftMax           = 20;
    uniform color CausticColor          = 1;
    uniform float CausticEnergy         = 1;
    uniform float CausticBlur           = 0.05;
    uniform float CausticSamples        = 64;
    uniform float CausticThreshold      = 0.8;
    uniform float CausticNoise          = 0;
    uniform float CausticNfreq          = 1;
    uniform float CausticNpow           = 1;
    uniform string EnvMap               = "";
    uniform float EnvBlur               = 0.1;
	uniform float LampUF_X0[21]			= {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 2.0, 0.0, 1.0, 0.0, 0.0, 1.0, 30.0, 0.0, 0.0, 0.01, 0.01, 0.0, 1.0, 1.0};
	uniform color LampUC_X0[1]			= {color(1.0, 1.0, 1.0)};
	uniform string LampUS_X0[10]			= {"", "", "", "", "", "", "", "", "", ""};
	varying float LampVF_X0[25]			= {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0};
	varying float LampVF_X1[25]			= {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0};
	varying float LampVF_X2[25]			= {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0};
	varying float LampVF_X3[25]			= {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0};
	varying float LampVF_X4[25]			= {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0};
	varying float LampVF_X5[25]			= {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0};
	varying float LampVF_X6[25]			= {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0};
	varying float LampVF_X7[25]			= {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0};
	varying float LampVF_X8[25]			= {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0};
	varying float LampVF_X9[25]			= {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0};
 output uniform string __shadowname  = "";   // Name of light shadow map to use for shadows
    output uniform string __causticname = "";   // Name of light caustic map to use for caustics
    output uniform string __photonname  = "";   // Name of photon map to use for caustics
    output uniform string __hazename    = "";   // Name of light color map to use for haze
    output uniform string __depthname   = "";   // Name of min filtered shadow map used for depth calculations
    output uniform color __bypasscolor  = -1;   // Manually bypass light color retrieved from Blender
    output uniform float __bypassenergy = -1;   // Manually bypass light energy retrieved from Blender
    output uniform point __from         = point "shader" (0, 0, 0); // Lights from point
    output uniform point __to           = point "shader" (0, 0, 1); // Lights to vector
    output uniform point __up           = point "shader" (0, 1, 0); // Lights up vector
    output varying float __nondiffuse   = 0;    // Do not effect surface diffuse
    output varying float __nonspecular  = 0;    // Do not effect surface specular
    output varying float __foglight     = 1;    // Does light effect volume fog (halo in Blender)
    output varying float __lighttype    = 0;    // Type of light generated
    output varying float __lightbias    = 0.01; // Shadow's bias
    output varying float __shadowblur   = 0.01; // Output blur used by shadow (adjusted by fake soft)
    output varying float __haloint      = 1;    // Halo intensity (used by volume shader)
    output varying color __lightcolor   = 0;    // Output light color/energy/attenuation
    output varying color __shadowcolor  = 1;    // Output shadow color/alpha
    output varying color __causticcolor = 0;)   // Output caustic color
{
    //// Lets setup general purpose variables...
    uniform string Name                 = "";
    string depthmap                     = "";
    uniform float                       a = 0, i = 0, channels = 3;
    varying float                       sAdj, tAdj, p1, p2, p3, p4, p5, p6, p7, result = 0, mask = 1, Energy, Samples, ShadBlur, CausBlur = CausticBlur, ShadowAlpha = 1, UseShadows = 1;
    varying float RayBlur               = max(0.0001, LampUF_X0[LAMP_UF_RAYSOFT]*RaySoftFactor);
    varying float BufBlur               = max(0.0001, LampUF_X0[LAMP_UF_BUFSOFT]*BufSoftFactor);
    varying color                       Cp1, Cp2, vis = color(1), envcol = color(1), lightcolor = color(1);
    varying normal Nn                   = normalize(N);
    varying normal Nf                   = faceforward(Nn, I);
    uniform float a1                    = LampUF_X0[LAMP_UF_FALLOFFTYPE];
    uniform float a2                    = LampUF_X0[LAMP_UF_DIST];
    uniform float a3                    = LampUF_X0[LAMP_UF_QUAD1];
    uniform float a4                    = LampUF_X0[LAMP_UF_QUAD2];
    uniform float a5                    = LampUF_X0[LAMP_UF_ISSPHERE];
    uniform vector Z                    = normalize(__to-__from);
    Cl                                  = color(0);
    
    //// Lets manage parameter passing...
    __lightcolor                        = color(0);
    __shadowcolor                       = color(1);
    __causticcolor                      = color(0);
    __nondiffuse                        = LampUF_X0[LAMP_UF_NODIFFUSE];
    __nonspecular                       = LampUF_X0[LAMP_UF_NOSPECULAR];
    __foglight                          = LampUF_X0[LAMP_UF_ISHALO];
    __haloint                           = LampUF_X0[LAMP_UF_HALOINT];
    __lighttype                         = LampUF_X0[LAMP_UF_LAMPTYPE];
    __lightbias                         = LampUF_X0[LAMP_UF_BIAS];
    surface("__shadowalpha", ShadowAlpha);
    surface("__useshadows", UseShadows);
    
    //// Lets do initial setup based on light type...
    
    // Setup special depth map if used otherwise use primary shadow map...
    if (__depthname != "")
        depthmap                        = __depthname;
    else
        depthmap                        = __shadowname;
    // Override system light color with manual parameter if specified...
    if (__bypasscolor != color(-1))
        lightcolor                      = __bypasscolor;
    else
        lightcolor                      = LampUC_X0[LAMP_UC_LAMPCOL];
    // If using negetive light...
    if (LampUF_X0[LAMP_UF_ISNEGATIVE] != 0)
        Energy                          = -LampUF_X0[LAMP_UF_ENERGY];
    else
        Energy                          = LampUF_X0[LAMP_UF_ENERGY];
    // Override system energy with manual parameter if specified...
    if (__bypassenergy != -1)
        Energy                          = __bypassenergy;
    // If raytracing then setup using raytrace options
    if (__shadowname == "raytrace")
    {
        Samples                         = LampUF_X0[LAMP_UF_RAYSAMP]*SamplesFactor-(SamplesFactor-1);
        ShadBlur                        = RayBlur;
    }
    // Otherwise use standard options
    else
    {
        Samples                         = LampUF_X0[LAMP_UF_BUFSAMP]*SamplesFactor-(SamplesFactor-1);
        ShadBlur                        = BufBlur;
    }
    #ifdef AIR
        // If using photon caustic map for this light use caustic data regardless of light type...
        __causticcolor                  = CausticColor*caustic(P, Nf)*CausticEnergy;
    #endif
    #ifndef AIR
    #ifndef AQSIS
        // If using photon caustic map for this light use caustic data regardless of light type...
        if (__photonname != "")
        {
            illuminate (P+Nf)
            {
                __causticcolor          = CausticColor*photonmap(__photonname, Ps, Nf)*CausticEnergy;
            }
        }
    #endif
    #endif
    
    //// Lets manage light according to light type...
    
    // Are we a distant light?
    if (__lighttype == 1)
    {
        solar(__to-__from, 0.0)
        {
            float lenL                  = distance(P, Ps);
            __lightcolor                = lightcolor*Energy;
            if (FakeSoftUse != 0)
            {
                float Soft              = blockerdepth(depthmap, FakeSoftSamples, FakeSoftPower, FakeSoftRadius, FakeSoftMin, FakeSoftMax);
                if (FakeSoftFade > 0)
                    ShadowAlpha         *= 1-clamp(Soft/FakeSoftFade, 0, 1);
                ShadBlur                *= Soft;
                CausBlur                *= Soft;
            }
            if (__shadowname != "" && UseShadows != 0)
                __shadowcolor           = 1-shadow(__shadowname, Ps, "samples", Samples, "blur", ShadBlur, "bias", __lightbias)*ShadowAlpha;
            if (__causticname != "")
            {
                float Fs                = shadow(__causticname, Ps, "samples", CausticSamples, "blur", CausBlur, "bias", __lightbias);
                __causticcolor          = smoothstep(CausticThreshold, 1, Fs)*CausticColor*CausticEnergy;
                if (CausticNoise != 0)
                {
                    point Pl            = transform("shader", Ps);
                    __causticcolor      *= CausticNoise*pow(noise(Pl*CausticNfreq), CausticNpow);
                }
            }
            Cl                          = __lightcolor*__shadowcolor+__causticcolor;
        }
    }
    // Are we a hemi light?
    else if (__lighttype == 3)
    {
        solar(__to-__from, 0.0)
        {
            __lightcolor                = lightcolor*Energy;
            Cl                          = __lightcolor;
        }
    }
    // Are we a spot light or area lights array?
    else if (__lighttype == 2 || __lighttype == 4)
    {
        uniform float b1                = cos(LampUF_X0[LAMP_UF_SPOTSI]*SpotCenter);
        uniform float b2                = cos(LampUF_X0[LAMP_UF_SPOTSI]*SpotCenter-LampUF_X0[LAMP_UF_SPOTBL]);
        uniform vector Y                = normalize(Z^vector(__up));
        uniform vector X                = normalize(Y^Z);
        uniform float spread            = 1/tan(radians(LampUF_X0[LAMP_UF_SPOTSI])*57);
        illuminate(__from, Z, LampUF_X0[LAMP_UF_SPOTSI])
        {
            float lenL                  = length(L);
            float Atten                 = Attenuate(a1, lenL, a2, a3, a4, a5);
            if (Atten == 0)
            {
                Cl                      = color(0);
                __lightcolor            = color(0);
                __shadowcolor           = color(0);
                __causticcolor          = color(0);
            }
            else
            {
                if (FakeSoftUse != 0)
                {
                    float Soft          = blockerdepth(depthmap, FakeSoftSamples, FakeSoftRadius, FakeSoftPower, FakeSoftMin, FakeSoftMax);
                    if (FakeSoftFade > 0)
                        ShadowAlpha     *= 1-clamp(Soft/FakeSoftFade, 0, 1);
                    ShadBlur            *= Soft;
                    CausBlur            *= Soft;
                }
                
                //// Cycle through all texture layers and blend to projection
                for (i = 0; i < GLOB_TEX_CHANNELS; i += 1)
                {
                    Name                    = LampUS_X0[i];
                    
                    // Make sure theres a texture before we process it!
                    if (Name != "")
                    {
                        // Lets initialize shared variables...
                        float tex_paras[LAMP_VF_PARALEN];
                        uniform float tex_channels  = 0;
                        textureinfo(Name, "channels", tex_channels);
                        
                        // Lets load up a temporary array of the current textures parameters
                        for (a = 0; a < LAMP_VF_PARALEN; a += 1)
                        {
                            if (i == 0)         tex_paras[a] = LampVF_X0[a];
                            else if (i == 1)    tex_paras[a] = LampVF_X1[a];
                            else if (i == 2)    tex_paras[a] = LampVF_X2[a];
                            else if (i == 3)    tex_paras[a] = LampVF_X3[a];
                            else if (i == 4)    tex_paras[a] = LampVF_X4[a];
                            else if (i == 5)    tex_paras[a] = LampVF_X5[a];
                            else if (i == 6)    tex_paras[a] = LampVF_X6[a];
                            else if (i == 7)    tex_paras[a] = LampVF_X7[a];
                            else if (i == 8)    tex_paras[a] = LampVF_X8[a];
                            else if (i == 9)    tex_paras[a] = LampVF_X9[a];
                        }
                        
                        // Adjust offset and size of st coordinates...
                        p1                  = tex_paras[LAMP_VF_OFSX];
                        p2                  = tex_paras[LAMP_VF_OFSY];
                        p3                  = tex_paras[LAMP_VF_SIZEX];
                        p4                  = tex_paras[LAMP_VF_SIZEY];
                        sAdj                = spread*L.X/L.Z*0.5+0.5;
                        tAdj                = spread*L.Y/L.Z*0.5+0.5;
                        // Rotate and mirror texture?
                        if (tex_paras[LAMP_VF_ROT90] != 0)
                        {
                            uniform float rot   = radians(-90);
                            sAdj            -= 0.5;
                            tAdj            -= 0.5;
                            p1              = (cos(rot)*sAdj-sin(rot)*tAdj)+0.5;
                            p2              = (sin(rot)*sAdj+cos(rot)*tAdj)+0.5;
                            sAdj            = 1-p1;
                            tAdj            = p2;
                        }
                        stAdjust(sAdj, tAdj, p1, p2, p3, p4);
                        
                        // Get and adjust color value from texture...
                        uniform float alpha_channel = tex_channels-1;
                        p1                  = tex_paras[LAMP_VF_FILTER];
                        // Adjust texture blur according to distance if using fakesoft
                        if (FakeSoftUse != 0)
                            p1              *= lenL;
                        if (tex_channels < 3)
                            Cp1             = color(float texture(Name[0], sAdj, tAdj, "blur", p1));
                        else
                            Cp1             = color texture(Name, sAdj, tAdj, "blur", p1);
                        p6                  = comp(ctransform("hsv", Cp1), 2);
                        
                        // Get and adjust alpha value from texture...
                        if (tex_paras[LAMP_VF_USEALPHA] != 0 && (tex_channels == 2 || tex_channels == 4))
                            p1              = float texture(Name[alpha_channel], sAdj, tAdj, "blur", p1, "fill", 1);
                        else
                            p1              = 1;
                        if (tex_paras[LAMP_VF_NEGALPHA] != 0)
                            p1              = 1-p1;
                        if (tex_paras[LAMP_VF_CALCALPHA] != 0)
                        {
                            p1              *= p6;
                            p3              *= p1;
                        }   
                        if (tex_paras[LAMP_VF_STENCIL] != 0)
                            mask            *= p1;
                        p1                  *= mask;
                        
                        // Alter texture colors before blending...
                        Cp2                 = color(tex_paras[LAMP_VF_TEXR], tex_paras[LAMP_VF_TEXG], tex_paras[LAMP_VF_TEXB]);
                        Cp1                 *= Cp2;
                        if (tex_paras[LAMP_VF_CONTR] != 1)
                            Cp1             = (Cp1-0.5*Cp2)*tex_paras[LAMP_VF_CONTR]+0.5*Cp2;
                        if (tex_paras[LAMP_VF_BRIGHT] != 1)
                            Cp1             = clamp(Cp1+Cp2*(tex_paras[LAMP_VF_BRIGHT]-1), color(0), color(2));
                        if (tex_paras[LAMP_VF_NEG] != 0)
                            Cp1             = color(1)-Cp1;
                        if (tex_paras[LAMP_VF_NORGB] != 0)
                            Cp1             = colorblend(color(p6), color(tex_paras[LAMP_VF_R], tex_paras[LAMP_VF_G], tex_paras[LAMP_VF_B]), 7, 1, 1);
                        
                        // Lets start blending texture according to assigned channel
                        p2                  = tex_paras[LAMP_VF_BLENDMODE];
                        p3                  = tex_paras[LAMP_VF_BLENDCOL]*p1;
                        if (tex_paras[LAMP_VF_ISCOL] != 0)
                        {
                            vis             = colorblend(vis, Cp1, p2, p3, 1);
                        }
                    }
                }
                
                vis                     *= smoothstep(b1, b2, (L.Z)/lenL);
                
                if (EnvMap != "")
                {
                    vector Lw           = vtransform("world", -L);
                    envcol              = color(environment(EnvMap, Lw, "blur", EnvBlur))*lightcolor;
                }
                else
                    envcol              = lightcolor;
                __lightcolor            = envcol*Energy*Atten*vis;
                if (__shadowname != "" && UseShadows != 0)
                    __shadowcolor       = 1-shadow(__shadowname, Ps, "samples", Samples, "blur", ShadBlur, "bias", __lightbias)*ShadowAlpha;
                else
                    __shadowcolor       = color(1);
                if (__causticname != "")
                {
                    float Fs            = shadow(__causticname, Ps, "samples", CausticSamples, "blur", CausBlur, "bias", __lightbias);
                    __causticcolor      = smoothstep(CausticThreshold, 1, Fs)*CausticColor*CausticEnergy;
                    if (CausticNoise != 0)
                    {
                        point Pl        = transform("shader", Ps);
                        __causticcolor  *= CausticNoise*pow(noise(Pl*CausticNfreq), CausticNpow);
                    }
                }
                else
                    __causticcolor      = color(0);
                Cl                      = __lightcolor*__shadowcolor+__causticcolor;
            }
        }
    }
    // Are we a point light?
    else
    {
        illuminate(__from)
        {
            float lenL                  = length(L);
            float Atten                 = Attenuate(a1, lenL, a2, a3, a4, a5);
            if (Atten == 0)
            {
                Cl                      = color(0);
                __lightcolor            = color(0);
                __shadowcolor           = color(0);
                __causticcolor          = color(0);
            }
            else
            {
                if (FakeSoftUse != 0)
                {
                    float Soft          = blockerdepth(depthmap, FakeSoftSamples, FakeSoftRadius, FakeSoftPower, FakeSoftMin, FakeSoftMax);
                    if (FakeSoftFade > 0)
                        ShadowAlpha     *= 1-clamp(Soft/FakeSoftFade, 0, 1);
                    ShadBlur            *= Soft;
                    CausBlur            *= Soft;
                }
                if (EnvMap != "")
                {
                    vector Lw           = vtransform("world", -L);
                    envcol              = color(environment(EnvMap, Lw, "blur", EnvBlur))*lightcolor;
                }
                else
                    envcol              = lightcolor;
                __lightcolor            = envcol*Energy*Atten;
                if (__shadowname != "" && UseShadows != 0)
                    __shadowcolor       = 1-shadow(__shadowname, Ps, "samples", Samples, "blur", ShadBlur, "bias", __lightbias)*ShadowAlpha;
                else
                    __shadowcolor       = color(1);
                if (__causticname != "")
                {
                    float Fs            = shadow(__causticname, Ps, "samples", CausticSamples, "blur", CausBlur, "bias", __lightbias);
                    __causticcolor      = smoothstep(CausticThreshold, 1, Fs)*CausticColor*CausticEnergy;
                    if (CausticNoise != 0)
                    {
                        point Pl        = transform("shader", Ps);
                        __causticcolor  *= CausticNoise*pow(noise(Pl*CausticNfreq), CausticNpow);
                    }
                }
                else
                    __causticcolor      = color(0);
                Cl                      = __lightcolor*__shadowcolor+__causticcolor;
            }
        }
    }
    __shadowblur                        = ShadBlur;
}
