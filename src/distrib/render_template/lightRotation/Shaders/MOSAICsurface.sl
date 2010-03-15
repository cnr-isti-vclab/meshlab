/* Beta-0.4.7: MOSAIC surface shader for Blender to RenderMan shader integration */

#define SURFACE_UTILITIES 1
#include "MOSAICfunctions.h"

//////// Begin surface shader
surface
MOSAICsurface(
    uniform float LayerBlend            = 2;
    uniform float LayerFactor           = 1;
    uniform float DiffPtcSSS            = 1;
    uniform float DiffPtcRadScale       = 0.01;
    uniform float ColPtcRadScale        = 0.01;
    uniform float PtcMaxSolidAngle      = 0.05;
    uniform float SSSOccGiBlur          = 0;
    uniform float EnvReflMaxDist        = 0;
    uniform float EnvRefrMaxDist        = 0;
    uniform float EnvFadeMode           = 1;
    uniform float EnvDispFac            = 1;
    uniform float EnvSelfOcc            = 0;
    uniform color OccColor              = 1;
    uniform float OccBendNormal[3]      = {0,0,0};
    uniform float OccEnergy             = 1;
    uniform float OccBlend              = 0;
    uniform float OccMaxDist            = 10;
    uniform float OccSamples            = 256;
    uniform float OccFalloff            = 0;
    uniform float OccMapBlur            = 0.02;
    uniform float OccMapBias            = 0;
    uniform string OccHdrMap            = "";
    uniform float OccHdrBlur            = 0;
    uniform float OccHdrSamples         = 2;
    uniform float OccHdrFactor          = 1;
    uniform color GIColor               = 1;
    uniform float GIEnergy              = 1;
    uniform float GIBlend               = 0;
    uniform float GIMaxDist             = 30;
    uniform float GISamples             = 128;
    uniform float GIDispFac             = 1;
    uniform float GIMapBlur             = 0.4;
    uniform float GISelfOcc             = 0;
    uniform string GIHdrMap             = "";
    uniform float GIHdrBlur             = 0;
    uniform float GIHdrSamples          = 2;
    uniform float GIHdrFactor           = 1;
    uniform string OccMap               = "";
    uniform string EnvMap               = "";
    uniform string IndMap               = "";
    uniform string OccPoint             = "";
    uniform string IndPoint             = "";
    uniform string DifBrick             = "";
    uniform string ColBrick             = "";
	uniform float GlobUF_X0[2]			= {0, 0};
	uniform float MatUF_X0[61]			= {0.5, 0.8, 0.5, 12.5, 1, 1, 0.0, 0.0, 0.0, 0.0, 0.5, 0.5, 1.3, 0.0, 1.0, 0.0, 1.25, 8, 0.0, 1.0, 0.0, 1.25, 8, 0.0, 0.0, 0.0, 0.0, 1.3, 0.05, 0.1, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 4.0, 0.5, 0.5, 0.5, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0};
	uniform color MatUC_X0[7]			= {color(0.8, 0.8, 0.8), color(1.0, 1.0, 1.0), color(1.0, 1.0, 1.0), color(1.0, 1.0, 1.0), color(0.0, 0.0, 0.0), color(0.0, 0.0, 0.0), color(1.0, 1.0, 1.0)};
	uniform string TexUS_X0[10]			= {"", "", "", "", "", "", "", "", "", ""};
	varying float TexVF_X0[41]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	varying float TexVF_X1[41]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	varying float TexVF_X2[41]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	varying float TexVF_X3[41]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	varying float TexVF_X4[41]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	varying float TexVF_X5[41]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	varying float TexVF_X6[41]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	varying float TexVF_X7[41]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	varying float TexVF_X8[41]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	varying float TexVF_X9[41]			= {16.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
 varying float                       s1 = 0, t1 = 0, s2 = 0, t2 = 0, s3 = 0, t3 = 0, s4 = 0, t4 = 0, s5 = 0, t5 = 0, s6 = 0, t6 = 0, s7 = 0, t7 = 0, s8 = 0, t8 = 0; // Texture uv layers
    varying float                       v0 = 1, v1 = 1, v2 = 1, v3 = 1, v4 = 1, v5 = 1, v6 = 1, v7 = 1, v8 = 1, v9 = 1; // Vertex groups
    varying float Ms                    = -1;   // Material index number from mesh (for multipass and layered shading)
    uniform string Mx                   = "";   // TexFace image names from mesh (if enabled in exporter)
    output varying float __nonfog       = 0;
    output varying float __shadowalpha  = 1;
    output varying float __useshadows   = 1;
    output varying color __surfacein    = 0;    // Input parameter for layered shading
    output varying color __surfaceout   = 0;    // Output parameter for layered shading
    output varying color _color         = 0;    // Base color channel for AOV
    output varying color _diffuse       = 0;    // Diffuse color channel for AOV
    output varying color _specular      = 0;    // Specular color channel for AOV
    output varying color _ambient       = 0;    // Ambient color channel for AOV
    output varying color _radiosity     = 0;    // Radiosity color channel for AOV
    output varying color _reflect       = 0;    // Reflection color channel for AOV
    output varying color _refract       = 0;    // Refraction color channel for AOV
    output varying color _shadow        = 0;    // Shadow color channel for AOV
    output varying color _caustic       = 0;    // Caustic color channel for AOV
    output varying color _diffshad      = 0;    // Shadow*Diffuse color channel for AOV
    output varying normal _uv           = 0;    // UV->st as normal channel for AOV
    output varying float _alpha         = 0;    // Alpha float channel for AOV
    output varying float _z             = 0;    // Z depth float channel for AOV
    output varying float _index         = 0;)   // Pass index float channel for AOV
{
    // Only shade a surface element belonging to the current material index unless disabled
    if (GlobUF_X0[MAT_UF_SETMODE] == 0 || Ms == -1 || GlobUF_X0[MAT_UF_SETINDEX] == Ms)
    {
        //// Lets setup general purpose variables...
        uniform string Name             = "";
        uniform float                   a = 0, i = 0, channels = 3;
        varying float                   sAdj, tAdj, p1, p2, p3, p4, p5, p6, p7, sssTex = 0, result = 0, mask = 1, warps = 0, warpt = 0;
        varying color                   Cp1, Cp2, Cp3, Cp4, Cp5, Cp6, Cp7;
        float stLayers[16]              = {s, t, s1, t1, s2, t2, s3, t3, s4, t4, s5, t5, s6, t6, s7, t7};
        float vGroups[10]               = {v0, v1, v2, v3, v4, v5, v6, v7, v8, v9};
        normal Nn                       = normalize(N);
        normal Nf                       = faceforward(Nn, I);
        vector In                       = normalize(I);
        vector Eye                      = -In;
        float Li                        = length(I);
        
        // These are the material channels defined in Blender's material system per texture for blending
        color                           Alpha = 1, Ref = 1, Spec = 0, Csp = 0, Cmir = 0, Amb = 0, Hard = 0, RayMir = 0, Emit = 0, Translu = 0;
        
        //// Lets setup local render layer variables...
        varying color layer_color       = 1;
        varying color layer_diffuse     = 0;
        varying color layer_specular    = 0;
        varying color layer_ambient     = 0;
        varying color layer_radiosity   = 0;
        varying color layer_reflect     = 0;
        varying color layer_refract     = 0;
        varying color layer_shadow      = 0;
        varying color layer_caustic     = 0;
        
        //// Lets grab any user attributes from RIB
        string rendertype               = "beauty";
        float renderpass                = 1;
        float rendermulti               = 1;
        float renderset                 = 1;
        string objectname               = "";
        string objecttype               = "";
        float objectid                  = 0;
        float objectscale               = 1;
        float objectpass                = 0;
        string geometrytype             = "";
        string materialname             = "";
        float materialid                = 0;
        float surfacelayer              = 0;
        attribute("user:render_type", rendertype);
        attribute("user:render_pass", renderpass);
        attribute("user:render_multi", rendermulti);
        attribute("user:render_set", renderset);
        attribute("user:object_name", objectname);
        attribute("user:object_type", objecttype);
        attribute("user:object_id", objectid);
        attribute("user:object_scale", objectscale);
        attribute("user:object_pass", objectpass);
        attribute("user:material_name", materialname);
        attribute("user:material_id", materialid);
        attribute("user:surface_layer", surfacelayer);
        
        // These are output variables to be used by other shaders
        __nonfog                        = MatUF_X0[MAT_UF_ISNOMIST];
        __shadowalpha                   = MatUF_X0[MAT_UF_SHADALPHA];
        __useshadows                    = MatUF_X0[MAT_UF_ISSHADOW];
        
        // Special setup for 3d baking passes...
        if (rendertype == "occlusion_point" || rendertype == "indirect_point" || rendertype == "diffuse_brick" || rendertype == "color_brick")
        {
            // Fake out shaders that normally use faceforward to see fully around object for baking
            Nf                          = Nn;
        }
        
        //// Lets do initial setup for material channels...
        if (rendertype != "occlusion_point" && (ColBrick == "" || rendertype == "color_brick"))
        {
            // Lets setup the initial color channel...
            if (MatUF_X0[MAT_UF_ISVCOLLIGHT] != 0)
                layer_ambient           = Cs;
            if (MatUF_X0[MAT_UF_SSSUSE] != 0)
                sssTex                  = MatUF_X0[MAT_UF_SSSTEX];
            if (MatUF_X0[MAT_UF_TEXFACE] != 0 && Mx != "")
            {
                Name                    = Mx;
                layer_color             = texture(Name, s, t, "blur", sssTex);
            }
            else if (MatUF_X0[MAT_UF_ISVCOLPAINT] == 0)
                layer_color             = MatUC_X0[MAT_UC_DIFCOL];
            if (MatUF_X0[MAT_UF_ISVCOLPAINT] != 0)
                layer_color             *= Cs;
            if (MatUF_X0[MAT_UF_ISOBJCOL] != 0)
                layer_color             *= MatUC_X0[MAT_UC_OBJCOL];
            
            // Lets setup the initial alpha channel...
            if (MatUF_X0[MAT_UF_ISTEXFACEALPHA] != 0 && Name != "")
                Alpha                   = color(float texture(Name[3], s, t, "blur", sssTex, "fill", 1));
            else if (MatUF_X0[MAT_UF_ISTEXFACEALPHA] == 0 || MatUF_X0[MAT_UF_ISVCOLPAINT] == 0)
                Alpha                   = color(MatUF_X0[MAT_UF_ALPHA]);
            if (MatUF_X0[MAT_UF_ISWIRE] != 0 && (mod(s*40, 1) > 0.1 && mod(t*40, 1) > 0.1))
                Alpha                   *= color(0);
            if (MatUF_X0[MAT_UF_ISOBJCOL] != 0)
                Alpha                   *= color(MatUF_X0[MAT_UF_OBJALPHA]);
            
            // Initial setup if using shaders...
            if (MatUF_X0[MAT_UF_ISSHADLESS] == 0)
            {
                Ref                     = color(MatUF_X0[MAT_UF_REF]);
                Spec                    = color(MatUF_X0[MAT_UF_SPEC]);
                Hard                    = color(MatUF_X0[MAT_UF_HARD]);
                Translu                 = color(MatUF_X0[MAT_UF_TRALU]);
                Amb                     = color(MatUF_X0[MAT_UF_AMB]);
                Emit                    = MatUF_X0[MAT_UF_EMIT];
                Csp                     = MatUC_X0[MAT_UC_SPECCOL];
                RayMir                  = color(MatUF_X0[MAT_UF_RAYMIR]);
                Cmir                    = MatUC_X0[MAT_UC_MIRCOL];
            }
            // Otherwise setup for shadeless...
            else
            {
                layer_diffuse           = color(1);
                layer_shadow            = color(1);
            }
            
            //// Cycle through all texture layers and blend to each material channels
            for (i = 0; i < GLOB_TEX_CHANNELS; i += 1)
            {
                Name                    = TexUS_X0[i];
                
                // Make sure theres a texture before we process it!
                if (Name != "")
                {
                    // Lets initialize shared variables...
                    float tex_paras[TEX_VF_PARALEN];
                    uniform float tex_channels  = 0;
                    textureinfo(Name, "channels", tex_channels);
                    
                    // Lets load up a temporary array of the current textures parameters
                    for (a = 0; a < TEX_VF_PARALEN; a += 1)
                    {
                        if (i == 0)         tex_paras[a] = TexVF_X0[a];
                        else if (i == 1)    tex_paras[a] = TexVF_X1[a];
                        else if (i == 2)    tex_paras[a] = TexVF_X2[a];
                        else if (i == 3)    tex_paras[a] = TexVF_X3[a];
                        else if (i == 4)    tex_paras[a] = TexVF_X4[a];
                        else if (i == 5)    tex_paras[a] = TexVF_X5[a];
                        else if (i == 6)    tex_paras[a] = TexVF_X6[a];
                        else if (i == 7)    tex_paras[a] = TexVF_X7[a];
                        else if (i == 8)    tex_paras[a] = TexVF_X8[a];
                        else if (i == 9)    tex_paras[a] = TexVF_X9[a];
                    }
                    
                    float stLayer       = tex_paras[TEX_VF_STLAYER];
                    float stCoor        = tex_paras[TEX_VF_STCOOR];
                    
                    // Adjust offset and size of st coordinates...
                    p1                  = tex_paras[TEX_VF_OFSX];
                    p2                  = tex_paras[TEX_VF_OFSY];
                    p3                  = tex_paras[TEX_VF_SIZEX];
                    p4                  = tex_paras[TEX_VF_SIZEY];
                    // Is this texture using "sticky" coordinates?
                    if (stCoor == 256)
                    {
                        sAdj            = s8;
                        tAdj            = t8;
                    }
                    // Is this texture using a UV layer?
                    else if (stCoor == 16)
                    {
                        sAdj            = stLayers[stLayer*2];
                        tAdj            = stLayers[stLayer*2+1];
                    }
                    // If using a coordinate mode not supported just use standard st
                    else
                    {
                        sAdj            = s;
                        tAdj            = t;
                    }
                    // Rotate and mirror texture?
                    if (tex_paras[TEX_VF_ROT90] != 0)
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
                    sAdj                += warps;
                    tAdj                += warpt;
                    
                    // Get and adjust color value from texture...
                    uniform float alpha_channel = tex_channels-1;
                    if (sssTex > 0) p1  = sssTex;
                    else            p1  = tex_paras[TEX_VF_FILTER];
                    if (tex_channels < 3)
                        Cp1             = color(float texture(Name[0], sAdj, tAdj, "blur", p1));
                    else
                        Cp1             = color texture(Name, sAdj, tAdj, "blur", p1);
                    p6                  = comp(ctransform("hsv", Cp1), 2);
                    
                    // Get and adjust alpha value from texture...
                    if (tex_paras[TEX_VF_USEALPHA] != 0 && (tex_channels == 2 || tex_channels == 4))
                        p1              = float texture(Name[alpha_channel], sAdj, tAdj, "blur", p1, "fill", 1);
                    else
                        p1              = 1;
                    if (tex_paras[TEX_VF_NEGALPHA] != 0)
                        p1              = 1-p1;
                    if (tex_paras[TEX_VF_CALCALPHA] != 0 || (tex_paras[TEX_VF_USEALPHA] == 0 && tex_paras[TEX_VF_ISALPHA] != 0))
                    {
                        p1              *= p6;
                        if (tex_paras[TEX_VF_ISALPHA] == 0)
                            p3          *= p1;
                    }   
                    if (tex_paras[TEX_VF_STENCIL] != 0)
                        mask            *= p1;
                    p1                  *= mask;
                    
                    // Alter texture colors before blending...
                    Cp2                 = color(tex_paras[TEX_VF_TEXR], tex_paras[TEX_VF_TEXG], tex_paras[TEX_VF_TEXB]);
                    Cp1                 *= Cp2;
                    if (tex_paras[TEX_VF_CONTR] != 1)
                        Cp1             = (Cp1-0.5*Cp2)*tex_paras[TEX_VF_CONTR]+0.5*Cp2;
                    if (tex_paras[TEX_VF_BRIGHT] != 1)
                        Cp1             = clamp(Cp1+Cp2*(tex_paras[TEX_VF_BRIGHT]-1), color(0), color(2));
                    if (tex_paras[TEX_VF_NEG] != 0)
                        Cp1             = color(1)-Cp1;
                    if (tex_paras[TEX_VF_NORGB] != 0)
                        Cp1             = colorblend(color(p6), color(tex_paras[TEX_VF_R], tex_paras[TEX_VF_G], tex_paras[TEX_VF_B]), 7, 1, 1);
                    
                    // Lets start blending texture according to assigned channel
                    p2                  = tex_paras[TEX_VF_BLENDMODE];
                    p3                  = tex_paras[TEX_VF_BLENDCOL]*p1;
                    p4                  = tex_paras[TEX_VF_BLENDVAR]*p1;
                    p5                  = tex_paras[TEX_VF_BLENDDVAR]*p1;
                    if (tex_paras[TEX_VF_ISWARP] != 0)
                    {
                        warps           = vector(1, 0, 0).(vector(Cp1)*2-vector(1))*tex_paras[TEX_VF_WARPFAC];
                        warpt           = vector(0, 1, 0).(vector(Cp1)*2-vector(1))*tex_paras[TEX_VF_WARPFAC];
                    }
                    if (tex_paras[TEX_VF_ISCOL] != 0)
                    {
                        layer_color     = colorblend(layer_color, Cp1, p2, p3, 1);
                    }
                    if (tex_paras[TEX_VF_ISCSP] != 0)
                    {
                        Csp             = colorblend(Csp, Cp1, p2, p3, 1);
                    }
                    if (tex_paras[TEX_VF_ISCMIR] != 0)
                    {
                        Cmir            = colorblend(Cmir, Cp1, p2, p3, 1);
                    }
                    if (tex_paras[TEX_VF_ISREF] != 0)
                    {
                        Cp1             = mix(Ref, color(p5), p6);
                        p7              = tex_paras[TEX_VF_ISREF];
                        Ref             = colorblend(Ref, Cp1, p2, p4, p7);
                    }
                    if (tex_paras[TEX_VF_ISSPEC] != 0)
                    {
                        Cp1             = mix(Spec, color(p5), p6);
                        p7              = tex_paras[TEX_VF_ISSPEC];
                        Spec            = colorblend(Spec, Cp1, p2, p4, p7);
                    }
                    if (tex_paras[TEX_VF_ISAMB] != 0)
                    {
                        Cp1             = mix(Amb, color(p5), p6);
                        p7              = tex_paras[TEX_VF_ISAMB];
                        Amb             = colorblend(Amb, Cp1, p2, p4, p7);
                    }
                    if (tex_paras[TEX_VF_ISHARD] != 0)
                    {
                        Cp1             = mix(Hard, color(p5), p6);
                        p7              = tex_paras[TEX_VF_ISHARD];
                        Hard            = colorblend(Hard, Cp1, p2, p4, p7);
                    }
                    if (tex_paras[TEX_VF_ISRAYMIR] != 0)
                    {
                        Cp1             = mix(RayMir, color(p5), p6);
                        p7              = tex_paras[TEX_VF_ISRAYMIR];
                        RayMir          = colorblend(RayMir, Cp1, p2, p4, p7);
                    }
                    if (tex_paras[TEX_VF_ISALPHA] != 0)
                    {
                        Cp1             = mix(Alpha, color(p5), p1);
                        p7              = tex_paras[TEX_VF_ISALPHA];
                        Alpha           = colorblend(Alpha, Cp1, p2, p4, p7);
                    }
                    if (tex_paras[TEX_VF_ISEMIT] != 0)
                    {
                        Cp1             = mix(Emit, color(p5), p6);
                        p7              = tex_paras[TEX_VF_ISEMIT];
                        Emit            = colorblend(Emit, Cp1, p2, p4, p7);
                    }
                    if (tex_paras[TEX_VF_ISTRANSLU] != 0)
                    {
                        Cp1             = mix(Translu, color(p5), p6);
                        p7              = tex_paras[TEX_VF_ISTRANSLU];
                        Translu         = colorblend(Translu, Cp1, p2, p4, p7);
                    }
                }
            }
        }
        
        
        //// Lets run shaders for channels...
        
        #ifndef AQSIS
            // If we have diffusion brickmap load it into diffuse channel...
            if(DifBrick != "" && rendertype != "diffuse_brick" && (ColBrick == "" || rendertype == "color_brick"))
            {
                point pP                = P;
                normal pNf              = Nf;
                // If not global brick map then transform to object space
                if (match("GLOBAL", DifBrick) != 1.0)
                {
                    pP                  = transform("object", pP);
                    pNf                 = transform("object", pNf);
                }
                p1                      = DiffPtcRadScale;
                layer_color             = color(1);
                layer_shadow            = color(1);
                layer_caustic           = color(0);
                layer_ambient           = color(0);
                #ifdef AIR
                    texture3d(DifBrick, pP, pNf, "blur", p1, "_diffusion", layer_diffuse);
                #endif
                #ifdef PIXIE
                    texture3d(DifBrick, pP, pNf, "radius", p1, "_diffusion", layer_diffuse);
                #endif
                #ifndef AIR
                #ifndef PIXIE
                    texture3d(DifBrick, pP, pNf, "filterradius", p1, "_diffusion", layer_diffuse);
                #endif
                #endif
                // If using point sub surface scattering...
                if (MatUF_X0[MAT_UF_SSSUSE] != 0 && DiffPtcSSS > 0)
                {
                    Cp1                 = MatUC_X0[MAT_UC_SSSCOL];
                    Cp2                 = color(MatUF_X0[MAT_UF_SSSRADIUSR], MatUF_X0[MAT_UF_SSSRADIUSG], MatUF_X0[MAT_UF_SSSRADIUSB]);
                    p1                  = MatUF_X0[MAT_UF_SSSFRONT];
                    p2                  = MatUF_X0[MAT_UF_SSSBACK];
                    p3                  = MatUF_X0[MAT_UF_SSSIOR]*0.5;
                    p4                  = (11-MatUF_X0[MAT_UF_SSSERROR])*10;
                    p5                  = MatUF_X0[MAT_UF_SSSSCALE];
                    result              = sssdiff(DifBrick, 0, p1, p2, p3, p4, p5, Cp1, Cp2, layer_diffuse, layer_shadow, layer_caustic);
                    if (result > 0)
                        layer_diffuse   *= mix(color(1), MatUC_X0[MAT_UC_SSSCOL], MatUF_X0[MAT_UF_SSSCOLBLEND]);
                }
            }
        #endif
        
        // If using only shadows...
        if (MatUF_X0[MAT_UF_ISONLYSHADOW] != 0 && rendertype != "occlusion_point" && (ColBrick == "" || rendertype == "color_brick"))
        {
            if (DifBrick == "" || rendertype == "diffuse_brick")
            {
                layer_shadow            = onlyshadow(Nf);
                // If ENV is enabled then use white diffuse and adjust output alpha by shadow channel
                if (MatUF_X0[MAT_UF_ISENV] != 0)
                {
                    layer_diffuse       = color(0);
                    layer_color         = color(1);
                    Alpha               = 1-layer_shadow;
                }
                // Else use solid horizon diffuse color to act as color key
                else
                {
                    layer_diffuse       = color(1);
                    layer_color         = MatUC_X0[MAT_UC_HORCOL];
                }
            }
        }
        // If not shadeless then do diffuse and specular
        else if (MatUF_X0[MAT_UF_ISSHADLESS] == 0 && rendertype != "occlusion_point" && (ColBrick == "" || rendertype == "color_brick"))
        {
            if (rendertype != "irradiance_cache")
            {
                // Do diffuse shading if specified
                if (layer_color != color(0))
                {
                    if ((Emit != color(0) || Ref != color(0)) && (DifBrick == "" || rendertype == "diffuse_brick"))
                    {
                        p1                  = MatUF_X0[MAT_UF_ISCUBIC];
                        p2                  = comp(ctransform("hsv", Translu), 2);
                        
                        // If using anisotropic diffusion...
                        if (MatUF_X0[MAT_UF_ISTANGENT] != 0 || (MatUF_X0[MAT_UF_ISSTRANDTAN] != 0 && objecttype == "particles"))
                        {
                            layer_diffuse   = anisodiff(Nf, p1, p2, layer_shadow, layer_caustic);
                        }
                        // If using standard lambert diffusion...
                        else if (MatUF_X0[MAT_UF_DIFFUSETYPE] == 0)
                        {
                            layer_diffuse   = lambertdiff(Nf, p1, p2, layer_shadow, layer_caustic);
                        }
                        // If using Oren-Nayar diffusion...
                        else if (MatUF_X0[MAT_UF_DIFFUSETYPE] == 1)
                        {
                            p3              = MatUF_X0[MAT_UF_ROUGHNESS];
                            layer_diffuse   = orennayardiff(Nf, In, Eye, p3, p1, p2, layer_shadow, layer_caustic);
                        }
                        // If using Toon diffusion...
                        else if (MatUF_X0[MAT_UF_DIFFUSETYPE] == 2)
                        {
                            p3              = MatUF_X0[MAT_UF_DIFFSIZE];
                            p4              = MatUF_X0[MAT_UF_DIFFSMOOTH];
                            layer_diffuse   = toondiff(Nf, p3, p4, p1, p2, layer_shadow, layer_caustic);
                        }
                        // If using Minnaert diffusion...
                        else if (MatUF_X0[MAT_UF_DIFFUSETYPE] == 3)
                        {
                            p3              = MatUF_X0[MAT_UF_DIFFDARK]-1;
                            layer_diffuse   = minnaertdiff(Nf, In, p3, p1, p2, layer_shadow, layer_caustic);
                        }
                        // If using Fresnel diffusion...
                        else if (MatUF_X0[MAT_UF_DIFFUSETYPE] == 4)
                        {
                            p3              = MatUF_X0[MAT_UF_DIFFSMOOTH];
                            p4              = MatUF_X0[MAT_UF_DIFFSIZE];
                            layer_diffuse   = fresneldiff(Nf, p3, p4, p1, p2, layer_shadow, layer_caustic);
                        }
                    
                        // If using sub surface scattering...
                        if (MatUF_X0[MAT_UF_SSSUSE] != 0)
                        {
                            Cp1             = MatUC_X0[MAT_UC_SSSCOL];
                            Cp2             = color(MatUF_X0[MAT_UF_SSSRADIUSR], MatUF_X0[MAT_UF_SSSRADIUSG], MatUF_X0[MAT_UF_SSSRADIUSB]);
                            Cp3             = mix(color(1), MatUC_X0[MAT_UC_SSSCOL], MatUF_X0[MAT_UF_SSSCOLBLEND]);
                            p1              = comp(ctransform("hsv", layer_diffuse), 2);
                            p2              = MatUF_X0[MAT_UF_SSSFRONT];
                            p3              = MatUF_X0[MAT_UF_SSSBACK];
                            p4              = MatUF_X0[MAT_UF_SSSIOR]-1.0;
                            p5              = (11-MatUF_X0[MAT_UF_SSSERROR])*10;
                            p6              = MatUF_X0[MAT_UF_SSSSCALE];
                            result          = sssdiff("", p1, p2, p3, p4, p5, p6, Cp1, Cp2, layer_diffuse, layer_shadow, layer_caustic);
                            if (result > 0)
                            {
                                layer_diffuse   *= Cp3;
                                layer_shadow    *= Cp3;
                            }
                        }                       
                        
                        // Figure shadow and diffuse layers then adjust for emit channel...
                        if (Ref != color(0))
                        {
                            float Dr        = comp(layer_diffuse, 0);
                            float Dg        = comp(layer_diffuse, 1);
                            float Db        = comp(layer_diffuse, 2);
                            float Sr        = (Dr != 0 ? comp(layer_shadow, 0)/Dr : 0);
                            float Sg        = (Dg != 0 ? comp(layer_shadow, 1)/Dg : 0);
                            float Sb        = (Db != 0 ? comp(layer_shadow, 2)/Db : 0);
                            layer_shadow    = color(Sr, Sg, Sb);
                            layer_diffuse   *= Ref;
                        }
                        else
                        {
                            layer_shadow    = color(1);
                            layer_diffuse   = color(0);
                        }
                        if (Emit != color(0))
                        {
                            float Cr        = comp(layer_color, 0);
                            float Cg        = comp(layer_color, 1);
                            float Cb        = comp(layer_color, 2);
                            float Ir        = (Cr != 0 ? 1/Cr : 0);
                            float Ig        = (Cg != 0 ? 1/Cg : 0);
                            float Ib        = (Cb != 0 ? 1/Cb : 0);
                            color InvC      = color(Ir, Ig, Ib);                            
                            layer_shadow    = mix(layer_shadow, InvC, Emit);
                            layer_diffuse   += layer_color*Emit;
                        }
                    }
                }
                
                // Do specular shading if specified
                if (Spec != color(0) && Csp != color(0) && rendertype != "indirect_map" && rendertype != "indirect_point" && rendertype != "diffuse_brick")
                {
                    // If using anisotropic specularity...
                    if (MatUF_X0[MAT_UF_ISTANGENT] != 0 || (MatUF_X0[MAT_UF_ISSTRANDTAN] != 0 && objecttype == "particles"))
                    {
                        p1              = PI/comp(Hard, 0);
                        layer_specular  = Csp*anisospec(Nf, -In, p1);
                    }
                    // If using Cooktorr specularity...
                    else if (MatUF_X0[MAT_UF_SPECULARTYPE] == 0)
                    {
                        p1              = pow(PI, 2)/comp(Hard, 0);
                        layer_specular  = Csp*cooktorrspec(Nf, -In, p1);
                    }
                    // If using Phong specularity...
                    else if (MatUF_X0[MAT_UF_SPECULARTYPE] == 1)
                    {
                        p1              = comp(Hard, 0)/PI;
                        p2              = pow(PI, 2)/comp(Hard, 0);
                        layer_specular  = Csp*phongspec(Nf, -In, p1, p2);
                    }
                    // If using Blinn specularity...
                    else if (MatUF_X0[MAT_UF_SPECULARTYPE] == 2)
                    {
                        p1              = pow(PI, 2)/comp(Hard, 0);
                        p2              = MatUF_X0[MAT_UF_SPECREFRACT];
                        layer_specular  = Csp*blinnspec(Nf, -In, p1, p2);
                    }
                    // If using Toon specularity...
                    else if (MatUF_X0[MAT_UF_SPECULARTYPE] == 3)
                    {
                        p1              = MatUF_X0[MAT_UF_SPECSIZE];
                        p2              = MatUF_X0[MAT_UF_SPECSMOOTH];
                        p3              = pow(PI, 2)/comp(Hard, 0);
                        layer_specular  = Csp*toonspec(Nf, -In, p1, p2, p3);
                    }
                    // If using Wardlso specularity...
                    else if (MatUF_X0[MAT_UF_SPECULARTYPE] == 4)
                    {
                        p1              = MatUF_X0[MAT_UF_SPECRMS];
                        p2              = pow(PI, 2)/comp(Hard, 0);
                        layer_specular  = Csp*wardspec(Nf, -In, p1, p2);
                    }
                    
                    layer_specular      *= Spec;
                }
            }           
                
            // Do any ambient shading if specified
            if (Amb != color(0) && rendertype != "indirect_point" && (DifBrick == "" || rendertype == "diffuse_brick"))
            {
                layer_ambient           += MatUC_X0[MAT_UC_AMBCOL];
                
                // If usng either occlusion map or raytracing...
                if (OccPoint != "" || OccMap != "")
                {
                    vector bend         = vector "world" (OccBendNormal[0], OccBendNormal[1], OccBendNormal[2]);
                    p1                  = OccMapBlur;
                    // If using SSS then apply percent of SSS Scale to occlusion blur...
                    if (MatUF_X0[MAT_UF_SSSUSE] != 0)
                        p1              += MatUF_X0[MAT_UF_SSSSCALE]*SSSOccGiBlur;
                    p2                  = OccMapBias;
                    p3                  = OccFalloff;
                    p4                  = OccHdrBlur;
                    p5                  = OccHdrSamples;
                    p6                  = OccHdrFactor;
                    Cp1                 = OccColor;
                    Cp2                 = hdrocclusion(OccMap, OccPoint, OccHdrMap, Nf, bend, OccSamples, OccMaxDist, PtcMaxSolidAngle, p1, p2, p3, p4, p5, p6, Cp1);
                    
                    if (OccBlend == 1)
                        Cp2             = -(1-Cp2);
                    if (OccBlend == 2)
                        Cp2             += -(1-Cp2);
                    if (OccBlend == 3)
                        layer_ambient   *= Cp2*OccEnergy;
                    else
                        layer_ambient   += Cp2*OccEnergy;
                }
                
                layer_ambient           *= Amb;
                
                // If usng either indirect diffusion map or raytracing...
                if (IndPoint != "" || (IndMap != "" && (IndMap == "raytrace" || rendertype != "indirect_map" || rendermulti > 1)))
                {
                    p1                  = GIMapBlur;
                    // If using SSS then apply percent of SSS Scale to occlusion blur...
                    if (MatUF_X0[MAT_UF_SSSUSE] != 0)
                        p1              += MatUF_X0[MAT_UF_SSSSCALE]*SSSOccGiBlur;
                    p2                  = GIDispFac;
                    p3                  = objectscale;
                    p4                  = GIHdrBlur;
                    p5                  = GIHdrSamples;
                    p6                  = GIHdrFactor;
                    Cp1                 = GIColor;
                    Cp2                 = hdrindirectdiff(IndMap, IndPoint, GIHdrMap, Nf, GISamples, GIMaxDist, PtcMaxSolidAngle, p1, p2, p3, p4, p5, p6, Cp1);
                    
                    // If self occlusion is enabled then modulate occlusion data onto diffusion map
                    if (GISelfOcc > 0)
                    {
                        Cp2             *= pow(comp(ctransform("hsv", layer_ambient), 2), GISelfOcc);
                    }
                    if (GIBlend == 1)
                        Cp2             = -(1-Cp2);
                    if (GIBlend == 2)
                        Cp2             += -(1-Cp2);
                    if (GIBlend == 3)
                        layer_radiosity *= Cp2*GIEnergy;
                    else
                        layer_radiosity += Cp2*GIEnergy;
                }
                
                layer_radiosity         *= Amb;
            }           
            
            //// Do reflection/refraction if specified
            if (rendertype != "irradiance_cache" && rendertype != "indirect_map" && rendertype != "indirect_point" && rendertype != "diffuse_brick" &&
                (EnvMap != "" || MatUF_X0[MAT_UF_ISTRANSPSHADOW] != 0))
            {
                float F                 = 1-Nf.Eye;
                float Ft                = 1-pow(pow(F, MatUF_X0[MAT_UF_TRANSPFRESNEL]), MatUF_X0[MAT_UF_TRANSPFRESFAC]-1);
                color fulldiffuse       = layer_color*layer_diffuse*layer_shadow+layer_caustic;
                RayMir                  *= pow(pow(F, MatUF_X0[MAT_UF_MIRFRESNEL]), MatUF_X0[MAT_UF_MIRFRESFAC]-1);
                if (EnvFadeMode == 0)       Cp2 = color(0);
                else if (EnvFadeMode == 1)  Cp2 = MatUC_X0[MAT_UC_HORCOL];
                else                        Cp2 = fulldiffuse;
                
                // If TranspShadow is enabled add fresnel effect to output alpha
                if (MatUF_X0[MAT_UF_ISTRANSPSHADOW] != 0)
                {
                    // If using deep shadow maps...
                    if (rendertype == "shadow_map")
                        Alpha           *= 1-(Ft*(1-RayMir));
                    // If using deep caustic maps...
                    else if (rendertype == "caustic_map")
                        Alpha           *= Ft*(1-RayMir);
                    // If already have alpha force edges opaque... 
                    else if (Alpha != color(1))
                        Alpha           += 1-(Ft*(1-RayMir));
                }
                
                // Only process reflection/refraction if an actual map is specified...
                if (EnvMap != "" && (EnvMap == "raytrace" || rendertype != "environment_map" || rendermulti > 1))
                {
                    // Lets setup for refractions...
                    if ((EnvMap != "raytrace" || MatUF_X0[MAT_UF_ISRAYTRANSP] == 1) && Ft > 0 && (1-RayMir) != color(0))
                    {
                        uniform float Tp1   = MatUF_X0[MAT_UF_TRANSPSAMPLES];
                        uniform float Tp2   = (EnvRefrMaxDist == 0 ? 1000.0 : EnvRefrMaxDist);
                        uniform float Tp3   = MatUF_X0[MAT_UF_TRANSPGLOSS];
                        p1                  = MatUF_X0[MAT_UF_IOR];
                        p2                  = MatUF_X0[MAT_UF_TRANSPRAYDEPTH];
                        p3                  = EnvDispFac;
                        p4                  = EnvFadeMode;
                        Cp1                 = mix(color(1), MatUC_X0[MAT_UC_DIFCOL], MatUF_X0[MAT_UF_TRANSPFILTER]);
                        layer_refract       = mirrortransp(EnvMap, In, Nf, 1, Tp1, Tp2, Tp3, p1, p2, p3, p4, Cp1, Cp2);
                        layer_refract       = ((layer_refract-fulldiffuse)*Ft+layer_specular*MatUF_X0[MAT_UF_TRANSPSPECTRA])*(1-RayMir);
                    }
                    
                    // Lets setup for reflections...
                    if ((EnvMap != "raytrace" || MatUF_X0[MAT_UF_ISRAYMIRROR] == 1) && RayMir != color(0))
                    {
                        uniform float Rp1   = MatUF_X0[MAT_UF_MIRSAMPLES];
                        uniform float Rp2   = (EnvReflMaxDist == 0 ? 1000.0 : EnvReflMaxDist);
                        uniform float Rp3   = MatUF_X0[MAT_UF_MIRGLOSS];
                        p1                  = MatUF_X0[MAT_UF_MIRRAYDEPTH];
                        p2                  = EnvDispFac;
                        p3                  = EnvFadeMode;
                        Cp1                 = Cmir;
                        layer_reflect       = mirrortransp(EnvMap, In, Nf, 0, Rp1, Rp2, Rp3, 1, p1, p2, p3, Cp1, Cp2);
                        
                        // If self occlusion is enabled then modulate occlusion data onto env map
                        if (EnvSelfOcc > 0 && EnvMap != "raytrace")
                        {
                            layer_reflect   *= pow(comp(ctransform("hsv", layer_ambient), 2), EnvSelfOcc);
                        }
                        
                        layer_reflect       = (layer_reflect-fulldiffuse)*RayMir;
                    }
                }
            }
        }
        
        //// If this is a standard or point baking pass then do standard channel layers
        if (rendertype != "occlusion_point" && rendertype != "indirect_point")
        {
            //// Lets blend Ci from previous shader layer into this layers ambient channel...
            if (surfacelayer > 0)
                layer_ambient           = colorblend(layer_ambient, __surfacein, LayerBlend, LayerFactor, 1);
            
            #ifndef AQSIS
                // If we have color brickmap load it in Ci...
                if(ColBrick != "" && rendertype != "color_brick")
                {
                    point pP            = P;
                    normal pNf          = Nf;
                    // If not global brick map then transform to object space
                    if (match("GLOBAL", ColBrick) != 1.0)
                    {
                        pP              = transform("object", pP);
                        pNf             = ntransform("object", pNf);
                    }
                    p1                  = ColPtcRadScale;
                    #ifdef AIR
                        texture3d(ColBrick, pP, pNf, "blur", p1, "_color", layer_color);
                    #endif
                    #ifdef PIXIE
                        texture3d(ColBrick, pP, pNf, "radius", p1, "_color", layer_color);
                    #endif
                    #ifndef AIR
                    #ifndef PIXIE
                        texture3d(ColBrick, pP, pNf, "filterradius", p1, "_color", layer_color);
                    #endif
                    #endif
                    Ci                  = layer_color;
                }
                // Otherwise lets combine channels together...
                else
            #endif
                    Ci                  = layer_color*(layer_ambient+layer_radiosity+(layer_diffuse*layer_shadow)+layer_caustic)+layer_refract+layer_reflect+layer_specular;
            
            // Lets apply output opacity and alpha modulation...
            if (Alpha != color(1))
                Oi                      = mix((color(1)-layer_color)*Alpha, color(1), Alpha);
            else
                Oi                      = Os;
            Ci                          *= Oi;
            
            #ifndef AQSIS
                // Are we baking diffusion data to point cloud?
                if (DifBrick != "" && rendertype == "diffuse_brick")
                {
                    point pP            = P;
                    normal pNn          = Nn;
                    // If not global brick map then transform to object space
                    if (match("GLOBAL", DifBrick) != 1.0)
                    {
                        pP              = transform("object", pP);
                        pNn             = ntransform("object", pNn);
                    }
                    Cp1                 = layer_color*(layer_ambient+layer_radiosity+(layer_diffuse*layer_shadow)+layer_caustic);
                    #ifdef AIR
                        bake3d(DifBrick, "_diffusion", pP, pNn, "_diffusion", Cp1);
                    #endif
                    #ifndef AIR
                        bake3d(DifBrick, "_diffusion", pP, pNn, "interpolate", 1, "_diffusion", Cp1);
                    #endif
                }
                // Are we baking all color layers to point cloud?
                if (ColBrick != "" && rendertype == "color_brick")
                {
                    point pP            = P;
                    normal pNn          = Nn;
                    // If not global brick map then transform to object space
                    if (match("GLOBAL", ColBrick) != 1.0)
                    {
                        pP              = transform("object", pP);
                        pNn             = ntransform("object", pNn);
                    }
                    #ifdef AIR
                        bake3d(ColBrick, "_color", pP, pNn, "_color", Ci);
                    #endif
                    #ifndef AIR
                        bake3d(ColBrick, "_color", pP, pNn, "interpolate", 1, "_color", Ci);
                    #endif
                }
            #endif
        }
        #ifndef AIR
        #ifndef AQSIS
            //// If this is a occlusion or indirect diffusion point cloud pass then bake out data
            else
            {           
                float a                 = area(P, "dicing");
                if (rendertype == "occlusion_point")
                {
                    Ci                  = Cs;
                    Oi                  = color(1);
                    bake3d(OccPoint, "_area", P, Nn, "coordsystem", "world", "interpolate", 1, "_area", a);
                }
                else
                {
                    Ci                  = layer_color*layer_diffuse*layer_shadow+layer_caustic;
                    Oi                  = color(1);
                    bake3d(IndPoint, "_area,_radiosity", P, Nn, "coordsystem", "world", "interpolate", 1, "_area", a, "_radiosity", Ci);
                }
            }
        #endif
        #endif
        
        // Lets apply local render layers to output parameters...
        _alpha                          = comp(ctransform("hsv", Alpha), 2);
        _color                          = layer_color;
        _diffuse                        = layer_diffuse;
        _specular                       = layer_specular;
        _ambient                        = layer_ambient;
        _radiosity                      = layer_radiosity;
        _reflect                        = layer_reflect;
        _refract                        = layer_refract;
        _shadow                         = layer_shadow;
        _caustic                        = layer_caustic;
        _diffshad                       = layer_diffuse*layer_shadow;
        _uv                             = normal(s, t, 0);
        _z                              = Li;
        _index                          = objectpass;
        __surfaceout                    = Ci;
    }
    //// If multipass material mode clear surface elements not in this material set
    else if (GlobUF_X0[MAT_UF_SETMODE] == 1)
    {
        Ci                              = 0;
        Oi                              = 0;
    }
}
