/* Beta-0.4.7: MOSAIC support library for Blender to RenderMan shader integration */


//////// Global Define Constants...

/* These generated constants are for simplifing use of the list based token parameters returned by MOSAIC.
   They are defined here so they are globally accessible for parameter passing between shaders. */

// Global Constants...
#define GLOB_TEX_CHANNELS 10
#define	GLOB_UF_PARALEN 2

#define	MAT_UF_SETMODE 0
#define	MAT_UF_SETINDEX 1

// MOSAICsurface Constants...
#define	MAT_UF_PARALEN 61
#define	MAT_UC_PARALEN 7
#define	TEX_US_PARALEN 10
#define	TEX_VF_PARALEN 41

#define	MAT_UF_AMB 0
#define	MAT_UF_REF 1
#define	MAT_UF_SPEC 2
#define	MAT_UF_HARD 3
#define	MAT_UF_ALPHA 4
#define	MAT_UF_OBJALPHA 5
#define	MAT_UF_TRALU 6
#define	MAT_UF_EMIT 7
#define	MAT_UF_ROUGHNESS 8
#define	MAT_UF_DIFFDARK 9
#define	MAT_UF_DIFFSIZE 10
#define	MAT_UF_DIFFSMOOTH 11
#define	MAT_UF_IOR 12
#define	MAT_UF_RAYMIR 13
#define	MAT_UF_MIRRAYDEPTH 14
#define	MAT_UF_MIRFRESNEL 15
#define	MAT_UF_MIRFRESFAC 16
#define	MAT_UF_MIRSAMPLES 17
#define	MAT_UF_MIRGLOSS 18
#define	MAT_UF_TRANSPRAYDEPTH 19
#define	MAT_UF_TRANSPFRESNEL 20
#define	MAT_UF_TRANSPFRESFAC 21
#define	MAT_UF_TRANSPSAMPLES 22
#define	MAT_UF_TRANSPGLOSS 23
#define	MAT_UF_TRANSPFILTER 24
#define	MAT_UF_TRANSPSPECTRA 25
#define	MAT_UF_SSSUSE 26
#define	MAT_UF_SSSIOR 27
#define	MAT_UF_SSSERROR 28
#define	MAT_UF_SSSSCALE 29
#define	MAT_UF_SSSTEX 30
#define	MAT_UF_SSSFRONT 31
#define	MAT_UF_SSSBACK 32
#define	MAT_UF_SSSRADIUSR 33
#define	MAT_UF_SSSRADIUSG 34
#define	MAT_UF_SSSRADIUSB 35
#define	MAT_UF_SSSCOLBLEND 36
#define	MAT_UF_SPECREFRACT 37
#define	MAT_UF_SPECSIZE 38
#define	MAT_UF_SPECSMOOTH 39
#define	MAT_UF_SPECRMS 40
#define	MAT_UF_DIFFUSETYPE 41
#define	MAT_UF_SPECULARTYPE 42
#define	MAT_UF_SHADALPHA 43
#define	MAT_UF_ISSTRANDTAN 44
#define	MAT_UF_ISCUBIC 45
#define	MAT_UF_ISOBJCOL 46
#define	MAT_UF_ISRAYMIRROR 47
#define	MAT_UF_ISRAYTRANSP 48
#define	MAT_UF_ISSHADOW 49
#define	MAT_UF_ISONLYSHADOW 50
#define	MAT_UF_ISTRANSPSHADOW 51
#define	MAT_UF_ISENV 52
#define	MAT_UF_ISWIRE 53
#define	MAT_UF_ISTANGENT 54
#define	MAT_UF_ISNOMIST 55
#define	MAT_UF_ISVCOLLIGHT 56
#define	MAT_UF_ISVCOLPAINT 57
#define	MAT_UF_TEXFACE 58
#define	MAT_UF_ISTEXFACEALPHA 59
#define	MAT_UF_ISSHADLESS 60
#define	MAT_UC_DIFCOL 0
#define	MAT_UC_SPECCOL 1
#define	MAT_UC_MIRCOL 2
#define	MAT_UC_HORCOL 3
#define	MAT_UC_AMBCOL 4
#define	MAT_UC_OBJCOL 5
#define	MAT_UC_SSSCOL 6
#define	TEX_US_TEXSLOT1 0
#define	TEX_US_TEXSLOT2 1
#define	TEX_US_TEXSLOT3 2
#define	TEX_US_TEXSLOT4 3
#define	TEX_US_TEXSLOT5 4
#define	TEX_US_TEXSLOT6 5
#define	TEX_US_TEXSLOT7 6
#define	TEX_US_TEXSLOT8 7
#define	TEX_US_TEXSLOT9 8
#define	TEX_US_TEXSLOT10 9
#define	TEX_VF_STCOOR 0
#define	TEX_VF_STLAYER 1
#define	TEX_VF_USEALPHA 2
#define	TEX_VF_CALCALPHA 3
#define	TEX_VF_NEGALPHA 4
#define	TEX_VF_FILTER 5
#define	TEX_VF_ROT90 6
#define	TEX_VF_STENCIL 7
#define	TEX_VF_NEG 8
#define	TEX_VF_NORGB 9
#define	TEX_VF_OFSX 10
#define	TEX_VF_OFSY 11
#define	TEX_VF_OFSZ 12
#define	TEX_VF_SIZEX 13
#define	TEX_VF_SIZEY 14
#define	TEX_VF_SIZEZ 15
#define	TEX_VF_ISCOL 16
#define	TEX_VF_ISCSP 17
#define	TEX_VF_ISCMIR 18
#define	TEX_VF_ISREF 19
#define	TEX_VF_ISSPEC 20
#define	TEX_VF_ISAMB 21
#define	TEX_VF_ISHARD 22
#define	TEX_VF_ISRAYMIR 23
#define	TEX_VF_ISALPHA 24
#define	TEX_VF_ISEMIT 25
#define	TEX_VF_ISTRANSLU 26
#define	TEX_VF_ISWARP 27
#define	TEX_VF_BRIGHT 28
#define	TEX_VF_CONTR 29
#define	TEX_VF_R 30
#define	TEX_VF_G 31
#define	TEX_VF_B 32
#define	TEX_VF_TEXR 33
#define	TEX_VF_TEXG 34
#define	TEX_VF_TEXB 35
#define	TEX_VF_BLENDMODE 36
#define	TEX_VF_WARPFAC 37
#define	TEX_VF_BLENDCOL 38
#define	TEX_VF_BLENDVAR 39
#define	TEX_VF_BLENDDVAR 40

// MOSAICdisplace constants...
#define	DISP_US_PARALEN 10
#define	DISP_VF_PARALEN 27

#define	DISP_US_TEXSLOT1 0
#define	DISP_US_TEXSLOT2 1
#define	DISP_US_TEXSLOT3 2
#define	DISP_US_TEXSLOT4 3
#define	DISP_US_TEXSLOT5 4
#define	DISP_US_TEXSLOT6 5
#define	DISP_US_TEXSLOT7 6
#define	DISP_US_TEXSLOT8 7
#define	DISP_US_TEXSLOT9 8
#define	DISP_US_TEXSLOT10 9
#define	DISP_VF_STCOOR 0
#define	DISP_VF_STLAYER 1
#define	DISP_VF_USEALPHA 2
#define	DISP_VF_CALCALPHA 3
#define	DISP_VF_NEGALPHA 4
#define	DISP_VF_FILTER 5
#define	DISP_VF_ROT90 6
#define	DISP_VF_STENCIL 7
#define	DISP_VF_NEG 8
#define	DISP_VF_OFSX 9
#define	DISP_VF_OFSY 10
#define	DISP_VF_OFSZ 11
#define	DISP_VF_SIZEX 12
#define	DISP_VF_SIZEY 13
#define	DISP_VF_SIZEZ 14
#define	DISP_VF_ISNOR 15
#define	DISP_VF_MID 16
#define	DISP_VF_VERTGRP 17
#define	DISP_VF_ISDISP 18
#define	DISP_VF_BRIGHT 19
#define	DISP_VF_CONTR 20
#define	DISP_VF_TEXR 21
#define	DISP_VF_TEXG 22
#define	DISP_VF_TEXB 23
#define	DISP_VF_BLENDMODE 24
#define	DISP_VF_NORFAC 25
#define	DISP_VF_DISPFAC 26

// MOSAIClight constants...
#define	LAMP_UF_PARALEN 21
#define	LAMP_UC_PARALEN 1
#define	LAMP_US_PARALEN 10
#define	LAMP_VF_PARALEN 25

#define	LAMP_UF_NODIFFUSE 0
#define	LAMP_UF_NOSPECULAR 1
#define	LAMP_UF_ISNEGATIVE 2
#define	LAMP_UF_ISONLYSHAD 3
#define	LAMP_UF_ISSQUARE 4
#define	LAMP_UF_ISHALO 5
#define	LAMP_UF_HALOINT 6
#define	LAMP_UF_FALLOFFTYPE 7
#define	LAMP_UF_ISSPHERE 8
#define	LAMP_UF_QUAD1 9
#define	LAMP_UF_QUAD2 10
#define	LAMP_UF_LAMPTYPE 11
#define	LAMP_UF_ENERGY 12
#define	LAMP_UF_DIST 13
#define	LAMP_UF_SPOTSI 14
#define	LAMP_UF_SPOTBL 15
#define	LAMP_UF_BIAS 16
#define	LAMP_UF_BUFSOFT 17
#define	LAMP_UF_RAYSOFT 18
#define	LAMP_UF_BUFSAMP 19
#define	LAMP_UF_RAYSAMP 20
#define	LAMP_UC_LAMPCOL 0
#define	LAMP_US_TEXSLOT1 0
#define	LAMP_US_TEXSLOT2 1
#define	LAMP_US_TEXSLOT3 2
#define	LAMP_US_TEXSLOT4 3
#define	LAMP_US_TEXSLOT5 4
#define	LAMP_US_TEXSLOT6 5
#define	LAMP_US_TEXSLOT7 6
#define	LAMP_US_TEXSLOT8 7
#define	LAMP_US_TEXSLOT9 8
#define	LAMP_US_TEXSLOT10 9
#define	LAMP_VF_USEALPHA 0
#define	LAMP_VF_CALCALPHA 1
#define	LAMP_VF_NEGALPHA 2
#define	LAMP_VF_FILTER 3
#define	LAMP_VF_ROT90 4
#define	LAMP_VF_STENCIL 5
#define	LAMP_VF_NEG 6
#define	LAMP_VF_NORGB 7
#define	LAMP_VF_OFSX 8
#define	LAMP_VF_OFSY 9
#define	LAMP_VF_OFSZ 10
#define	LAMP_VF_SIZEX 11
#define	LAMP_VF_SIZEY 12
#define	LAMP_VF_SIZEZ 13
#define	LAMP_VF_ISCOL 14
#define	LAMP_VF_BRIGHT 15
#define	LAMP_VF_CONTR 16
#define	LAMP_VF_R 17
#define	LAMP_VF_G 18
#define	LAMP_VF_B 19
#define	LAMP_VF_TEXR 20
#define	LAMP_VF_TEXG 21
#define	LAMP_VF_TEXB 22
#define	LAMP_VF_BLENDMODE 23
#define	LAMP_VF_BLENDCOL 24

// MOSAICfog constants...
#define	MIST_UF_PARALEN 6
#define	MIST_UC_PARALEN 1

#define	MIST_UF_ISMIST 0
#define	MIST_UF_MISTTYPE 1
#define	MIST_UF_MISTINT 2
#define	MIST_UF_MISTSTA 3
#define	MIST_UF_MISTDI 4
#define	MIST_UF_MISTHI 5
#define	MIST_UC_MISTCOL 0

// MOSAICbackground constants...




//////// Global Define Functions...

#define CUBICDIFFUSE 1.4
#define MINFILTERWIDTH  1e-7
#define filterwidthf(x) (max(abs(Du(x) * (du)) + (Dv(x) * (dv)),MINFILTERWIDTH))
#define filterwidthp(x) (max(sqrt(area(x)), MINFILTERWIDTH))


//////// Global Utility Functions...

//// Adjust sAdj and tAdj by xy offset and size
void stAdjust(
    output float sAdj;      // s texture coordinate to adjust
    output float tAdj;      // t texture coordinate to adjust
    float offX;             // +/- texture offset along s
    float offY;             // +/- texture offset along t
    float scaleX;           // scale factor along s
    float scaleY;)          // scale factor along t
{
    sAdj    = (sAdj*scaleX-(scaleX-1)/2)+offX;
    tAdj    = (tAdj*scaleY-(scaleY-1)/2)-offY;
}


//// Blend color1 into color2 by value according to mode and return result, modes are:
//// 0-13 (mix,mult,add,sub,divide,darken,diff,lighten,screen,overlay,hue,saturation,value,color)
color colorblend(
    color color1;           // First color to blend
    color color2;           // Second color to blend
    float mode;             // Blend mode see above
    float value;            // Blend first color into second color by this factor
    float pos_neg;)         // Positive or negative (inverted) result
{
    color blendcolor    = color(1);
    color outputcolor   = color(1);
    if (mode == 0)          // mix
        blendcolor      = color2;
    else if (mode == 1)     // mult
        blendcolor      = color1*color2;
    else if (mode == 2)     // add
        blendcolor      = color1+color2;
    else if (mode == 3)     // sub
        blendcolor      = max(color1-color2, color(0));
    else if (mode == 4)     // divide
        blendcolor      = max(color1/color2, color(0));
    else if (mode == 5)     // darken
        blendcolor      = min(color1, color2);
    else if (mode == 6)     // diff
        blendcolor      = max(color1, color2)-min(color1, color2);
    else if (mode == 7)     // lighten
        blendcolor      = max(color1, color2);
    else if (mode == 8)     // screen
        blendcolor      = color(1)-((color(1)-color2)*(color(1)-color1));
    else if (mode == 9)     // overlay
        blendcolor      = (color1*(color(1)-((color(1)-color2)*(color(1)-color1))))+((color(1)-color1)*color1*color2);
    else                    // setup for hsv modes
    {
        float hue1      = comp(ctransform("hsv", color1), 0);
        float sat1      = comp(ctransform("hsv", color1), 1);
        float val1      = comp(ctransform("hsv", color1), 2);
        float hue2      = comp(ctransform("hsv", color2), 0);
        float sat2      = comp(ctransform("hsv", color2), 1);
        float val2      = comp(ctransform("hsv", color2), 2);
        float lit1      = comp(ctransform("hsl", color1), 2);
        blendcolor      = ctransform("rgb", "hsv", blendcolor);
        if (mode == 10)     // hue
        {
            setcomp(blendcolor, 0, hue2);
            setcomp(blendcolor, 1, sat1);
            setcomp(blendcolor, 2, val1);
        }
        else if (mode == 11)// saturation
        {
            setcomp(blendcolor, 0, hue1);
            setcomp(blendcolor, 1, sat2);
            setcomp(blendcolor, 2, val1);
        }
        else if (mode == 12)// value
        {
            setcomp(blendcolor, 0, hue1);
            setcomp(blendcolor, 1, sat1);
            setcomp(blendcolor, 2, val2);
        }
        else if (mode == 13)// color
        {
            setcomp(blendcolor, 0, hue2);
            setcomp(blendcolor, 1, sat2);
            setcomp(blendcolor, 2, lit1);
        }
        blendcolor      = ctransform("hsv", "rgb", blendcolor);
    }
    outputcolor         = mix(color1, blendcolor, value);
    if (pos_neg < 0)
        outputcolor     = max(color(0), 1-outputcolor);
    return outputcolor;
}


//// Returns attenuation factor for Length based on Type and controlled using Linear and Quad
float Attenuate(
    float Type;             // Specifies attenuation type (0 = constant, 1 = inverse linear, 2 = inverse square, 3 = Lin/Quad weighted)
    float Length;           // Length to attenuate
    float Distance;         // Distance to attenuate by
    float Linear;           // Linear factor for Lin/Quad type
    float Quad;             // Quadratic factor for Lin/Quad type
    float Sphere;)          // Use Distance as absolute term for all attenuation types (0 || 1)
{
    float                   q1, q2, q3, a1 = 1, a2 = 1, a3 = 1, Atten = 1;
    if (Type > 0)           // Are we attenuating at all?
    {
        if (Type == 1)      // inverse linear setup
        {
            q1          = 1;
            q2          = 0;
            q3          = 0;
        }
        else if (Type == 2) // inverse square setup
        {
            q1          = 0;
            q2          = 0;
            q3          = 1;
        }
        else                // Linear/Quadratic weighted
        {
            q1          = Linear;
            q2          = Quad;
            q3          = 0;
        }
        if (q1 > 0)         // Linear equation
            a1          = Distance/(Distance+q1*Length);
        if (q2 > 0)         // Quadratic equation
            a2          = pow(Distance, 2)/(pow(Distance, 2)+q2*pow(Length, 2));
        if (q3 > 0)         // Inverse Square Equation
            a3          = Distance/(Distance+q3*pow(Length, 2));
        Atten           = a1*a2*a3;
    }
    if (Sphere > 0)         // Are we using sphere attenuation?
        Atten           *= (Distance-Length)/Distance;
    return max(0, Atten);
}


//////// Surface Shader Utility Functions...

#ifdef SURFACE_UTILITIES

//// Returns Oren-Nayer diffusion model modified from Larry Gritz's source
color orennayardiff(
    normal Nf;              // Normalized faceforward surface normal
    vector In;              // Normalized camera vector
    vector Eye;             // Eye vector
    float sigma;            // Roughness
    float isCubic;          // Use cubic terminator blend
    float translu;          // Translucent diffuse factor
    output color _shadow;   // Shadow layer for composite
    output color _caustic;) // Caustic layer for composite
{
    extern point        P;
    extern vector       L;
    vector              Ln;
    float               Fd;
    float               C1, C2, C3, L1, L2;
    float               theta_r, theta_i, cos_theta_i;
    float               alpha, beta, sigma2, cos_phi_diff;
    float nondiff       = 0;
    float lighttype     = 0;
    color diffuse       = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    color causticcolor  = color(0);
    theta_r             = acos(abs(Eye.Nf));
    sigma2              = sigma*sigma;
    illuminance(P)
    {
        lightsource("__nondiffuse", nondiff);
        if (nondiff == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            lightsource("__causticcolor", causticcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
            {
                Fd      = min((Nf.Ln+1)/2, 1);
            }
            else
            {
                cos_theta_i = max(0, Ln.Nf);
                cos_phi_diff= normalize(Eye-Nf*(Eye.Nf)).normalize(Ln-Nf*(max(0, Ln.Nf)));
                theta_i = acos(cos_theta_i);
                alpha   = max(theta_i, theta_r);
                beta    = min(theta_i, theta_r);
                C1      = 1-0.5*sigma2/(sigma2+0.33);
                C2      = 0.45*sigma2/(sigma2 + 0.09);
                if (cos_phi_diff >= 0)  C2 *= sin(alpha);
                else                    C2 *= (sin(alpha)-pow(2*beta/PI,3));
                C3      = 0.125*sigma2/(sigma2+0.09)*pow((4*alpha*beta)/(PI*PI),2);
                L1      = cos_theta_i*(C1+cos_phi_diff*C2*tan(beta)+(1-abs(cos_phi_diff))*C3*tan((alpha+beta)/2));
                L2      = 0.17*cos_theta_i*sigma2/(sigma2+0.13)*(1-cos_phi_diff*(4*beta*beta)/(PI*PI));
                Fd      = L1+L2;
            }
            if (translu > 0)
                Fd      += max(0, -Nf.Ln)*translu;
            if (isCubic)
                Fd      = pow(max(Fd, 0), CUBICDIFFUSE);
            diffuse     += lightcolor*Fd;
            if (Fd > 0)
            {
                _shadow     += lightcolor*shadowcolor*Fd;
                _caustic    += causticcolor*Fd;
            }
        }
    }
    return diffuse;
}


//// Returns simple toon cell shading for diffusion
color toondiff(
    normal Nf;              // Normalized faceforward surface normal
    float diffSize;         // Size of solid diffusion color
    float diffSmooth;       // Edge smoothing of solid color
    float isCubic;          // Use cubic terminator blend
    float translu;          // Translucent diffuse factor
    output color _shadow;   // Shadow layer for composite
    output color _caustic;) // Caustic layer for composite
{
    extern point        P;
    vector              Ln;
    float               Fd;
    float nondiff       = 0;
    float lighttype     = 0;
    color diffuse       = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    color causticcolor  = color(0);
    illuminance(P)
    {
        lightsource("__nondiffuse", nondiff);
        if (nondiff == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            lightsource("__causticcolor", causticcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
                Fd      = min((Nf.Ln+1)/2, 1);
            else
                Fd      = max(0, smoothstep(cos(diffSize)-sin(diffSmooth)/1.3, cos(diffSize), comp(color(Ln.Nf), 0)));
            if (translu > 0)
                Fd      += max(0, -Nf.Ln)*translu;
            if (isCubic)
                Fd      = pow(max(Fd, 0), CUBICDIFFUSE);
            diffuse     += lightcolor*Fd;
            if (Fd > 0)
            {
                _shadow     += lightcolor*shadowcolor*Fd;
                _caustic    += causticcolor*Fd;
            }
        }
    }
    return diffuse;
}


//// Returns Minnaert diffusion model
color minnaertdiff(
    normal Nf;              // Normalized faceforward surface normal
    vector In;              // Normalized camera vector
    float dark;             // Minnaert darkness setting
    float isCubic;          // Use cubic terminator blend
    float translu;          // Translucent diffuse factor
    output color _shadow;   // Shadow layer for composite
    output color _caustic;) // Caustic layer for composite
{
    extern point        P;
    vector              Ln;
    float               Fd;
    float nondiff       = 0;
    float lighttype     = 0;
    float Dd            = 0;
    color diffuse       = color(0);
    if (dark > 0)   Dd  = 1;
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    color causticcolor  = color(0);
    illuminance(P)
    {
        lightsource("__nondiffuse", nondiff);
        if (nondiff == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            lightsource("__causticcolor", causticcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
                Fd      = min((Nf.Ln+1)/2, 1);
            else
                Fd      = max(0, (Nf.Ln)*pow(max(0, (Nf.Ln)*abs(In.Nf+Dd)), dark));
            if (translu > 0)
                Fd      += max(0, -Nf.Ln)*translu;
            if (isCubic)
                Fd      = pow(max(Fd, 0), CUBICDIFFUSE);
            diffuse     += lightcolor*Fd;
            if (Fd > 0)
            {
                _shadow     += lightcolor*shadowcolor*Fd;
                _caustic    += causticcolor*Fd;
            }
        }
    }
    return diffuse;
}


//// Returns Lambert diffusion model
color lambertdiff(
    normal Nf;              // Normalized faceforward surface normal
    float isCubic;          // Use cubic terminator blend
    float translu;          // Translucent diffuse factor
    output color _shadow;   // Shadow layer for composite
    output color _caustic;) // Caustic layer for composite
{
    extern point        P;
    vector              Ln;
    float               Fd;
    float nondiff       = 0;
    float lighttype     = 0;
    color diffuse       = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    color causticcolor  = color(0);
    illuminance(P)
    {
        lightsource("__nondiffuse", nondiff);
        if (nondiff == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            lightsource("__causticcolor", causticcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
                Fd      = min((Nf.Ln+1)/2, 1);
            else
                Fd      = max(0, Nf.Ln);
            if (translu > 0)
                Fd      += max(0, -Nf.Ln)*translu;
            if (isCubic)
                Fd      = pow(max(Fd, 0), CUBICDIFFUSE);
            diffuse     += lightcolor*Fd;
            if (Fd > 0)
            {
                _shadow     += lightcolor*shadowcolor*Fd;
                _caustic    += causticcolor*Fd;
            }
        }
    }
    return diffuse;
}


//// Returns fresnel diffusion model
color fresneldiff(
    normal Nf;              // Normalized faceforward surface normal
    float power;            // Fresnel power
    float factor;           // Fresnel factor
    float isCubic;          // Use cubic terminator blend
    float translu;          // Translucent diffuse factor
    output color _shadow;   // Shadow layer for composite
    output color _caustic;) // Caustic layer for composite
{
    extern point        P;
    vector              Ln;
    float               Fd;
    float nondiff       = 0;
    float lighttype     = 0;
    color diffuse       = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    color causticcolor  = color(0);
    illuminance(P)
    {
        lightsource("__nondiffuse", nondiff);
        if (nondiff == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            lightsource("__causticcolor", causticcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
                Fd      = min((Nf.Ln+1)/2, 1);
            else
                Fd      = pow(clamp(Ln.reflect(Ln, normalize(Nf)), 0, 1), pow(power, factor));
            if (translu > 0)
                Fd      += max(0, -Nf.Ln)*translu;
            if (isCubic)
                Fd      = pow(max(Fd, 0), CUBICDIFFUSE);
            diffuse     += lightcolor*Fd;
            if (Fd > 0)
            {
                _shadow     += lightcolor*shadowcolor*Fd;
                _caustic    += causticcolor*Fd;
            }
        }
    }
    return diffuse;
}


//// Returns anisotropic diffusion model
color anisodiff(
    normal Nf;              // Normalized faceforward surface normal
    float isCubic;          // Use cubic terminator blend
    float translu;          // Translucent diffuse factor
    output color _shadow;   // Shadow layer for composite
    output color _caustic;) // Caustic layer for composite
{
    extern point        P;
    extern float        t;
    vector              Ln;
    float               Fd;
    float nondiff       = 0;
    float lighttype     = 0;
    vector tangent      = normalize(Deriv(P, t));
    color diffuse       = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    color causticcolor  = color(0);
    illuminance(P)
    {
        lightsource("__nondiffuse", nondiff);
        if (nondiff == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            lightsource("__causticcolor", causticcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
                Fd      = min((Nf.Ln+1)/2, 1);
            else
                Fd      = sqrt(1-pow(Ln.tangent, 2));
            if (translu > 0)
                Fd      += max(0, -Nf.Ln)*translu;
            if (isCubic)
                Fd      = pow(max(Fd, 0), CUBICDIFFUSE);
            diffuse     += lightcolor*Fd;
            if (Fd > 0)
            {
                _shadow     += lightcolor*shadowcolor*Fd;
                _caustic    += causticcolor*Fd;
            }
        }
    }
    return diffuse;
}


//// Returns only shadow data from lights for shadow mattes
color onlyshadow(
    normal Nf;)             // Normalized surface normal
{
    extern point        P;
    float samples       = 0;
    float Fd            = 0;
    vector Ln           = 0;
    color matte         = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    illuminance(P)
    {
        lightsource("__lightcolor", lightcolor);
        lightsource("__shadowcolor", shadowcolor);
        Ln              = normalize(L);
        Fd              = Nf.Ln;
        if (Fd > 0)
            matte       += comp(ctransform("hsv", min(lightcolor*shadowcolor, color(1))), 2);
        else
            matte       += color(1);
        samples         += 1;
    }
    return min(matte/(samples == 0 ? 1 : samples), color(1));
}



//Simulate SSS using blurred shadows or diffuse brickmap, returns number of lights used for SSS or 1 for brickmap use
float sssdiff(
    string BrickMap;        // Name of brickmap containing diffusion to use for SSS (instead of using light depth maps) 
    float Diffuse;          // Colorless diffusion with shadows to blend on top of SSS at low obsorbtion
    float FrFact;           // Front factor of SSS light obsorbtion
    float BkFact;           // Back factor of SSS light obsorbtion
    float SSSBias;          // Bias to use for SSS depth maps
    float Samples;          // Number of samples used for shadow lookup
    float Scale;            // Obsorbtion scale (technically a blurring scale)
    color SSSCol;           // Surface color off SSS (this color used on front and inverse used on back)
    color SSSRad;           // SSS Radius simulating scattering path of light for each color channel
    output color _diffuse;  // Diffuse layer for composite
    output color _shadow;   // Shadow layer for composite
    output color _caustic;) // Caustic layer for composite
{
    extern point        P;
    extern vector       I;
    string              shadowname = "", depthname = "", depthmap = "";
    float               LgBlur, frdiff, indiff, nondiff = 0, shadowblur = 0, lightCount = 0;
    color               InDepth, FrDepth, BkDepth, SSS = color(0), lightcolor = color(1);
    color InCol         = (1.004-SSSCol)*SSSRad;
    float DiffBlend     = 0.65; // Factor of diffuse blending at low SSS scales
    if (comp(ctransform("hsv", SSSRad), 2) > 1)
        InCol           = color(normalize(vector(InCol)));
    color FrCol         = color(1)-InCol;
    float InBlur        = clamp(comp(ctransform("hsv", InCol), 2)*0.3*Scale, 0.001, 1);
    float FrBlur        = clamp((1-comp(ctransform("hsv", FrCol), 2))*0.2*Scale, 0.001, 1);
    float InDiff        = InBlur*DiffBlend*20;
    float FrDiff        = FrBlur*DiffBlend*20;
    
    if (BrickMap != "")     // If brickmap is specified use it for SSS calculations...
    {
        point pP        = P;
        // If not global brick map then transform to object space
        if (match("GLOBAL", BrickMap) != 1.0)
            pP          = transform("object", pP);
        BkDepth         = color(0);
        #ifndef AQSIS
        #ifdef AIR
            texture3d(BrickMap, pP, normal(0), "blur", InBlur, "_diffusion", InDepth);
            texture3d(BrickMap, pP, normal(0), "blur", FrBlur, "_diffusion", FrDepth);
        #endif
        #ifdef PIXIE
            texture3d(BrickMap, pP, normal(0), "radius", InBlur, "_diffusion", InDepth);
            texture3d(BrickMap, pP, normal(0), "radius", FrBlur, "_diffusion", FrDepth);
        #endif
        #ifndef AIR
        #ifndef PIXIE
            texture3d(BrickMap, pP, normal(0), "filterradius", InBlur, "_diffusion", InDepth);
            texture3d(BrickMap, pP, normal(0), "filterradius", FrBlur, "_diffusion", FrDepth);
        #endif
        #endif
        #endif
        if (BkFact != 1) illuminance(P) {BkDepth += max(pow(normalize(I).normalize(L), 16)*InDepth, color(0))*(BkFact-1);}
        SSS             = min(color(1), (InCol*InDepth + FrCol*FrDepth)*FrFact+BkDepth);
        lightCount      = 1;
    }
    else                    // Otherwise use light depth maps for calculations...
    {
        illuminance(P)
        {
            lightsource("__nondiffuse", nondiff);
            if (nondiff == 0)
            {
                lightsource("__shadowname", shadowname);
                lightsource("__depthname", depthname);
                if (depthname != "")
                    depthmap    = depthname;
                else
                    depthmap    = shadowname;
                if (depthmap != "" && depthmap != "raytrace")
                {
                    lightCount  += 1;
                    lightsource("__lightcolor", lightcolor);
                    lightsource("__shadowblur", shadowblur);
                    indiff      = clamp((clamp(Diffuse, 0, 1)*(1-InDiff))+InDiff, 0, 1);
                    frdiff      = clamp((clamp(Diffuse, 0, 1)*(1-FrDiff))+FrDiff, 0, 1);
                    LgBlur      = InBlur*comp(ctransform("hsv", lightcolor), 2)+shadowblur;
                    FrBlur      += shadowblur;
                    InDepth     = comp(ctransform("hsv", 1-shadow(depthmap, P, "samples", Samples, "bias", SSSBias, "blur", LgBlur)), 2);
                    FrDepth     = comp(ctransform("hsv", 1-shadow(depthmap, P, "samples", Samples, "bias", SSSBias, "blur", FrBlur)), 2);
                    BkDepth     = InDepth*max(normalize(I).normalize(L)*0.2, 0)*(BkFact-1);
                    SSS         += min(color(1), lightcolor*(InCol*InDepth*indiff + FrCol*FrDepth*frdiff)*FrFact+BkDepth + _caustic);
                }
            }
        }
        if (lightCount > 0)
        {
            _caustic    = color(0);
            _shadow     = SSS;
        }
    }
    if (lightCount > 0)
        _diffuse        = SSS;
    return lightCount;
}


//// Returns Cooktorr specular model
color cooktorrspec(
    normal Nf;              // Normalized faceforward surface normal
    vector In;              // Normalized camera vector
    float hard;)            // Specular hard factor
{
    extern point        P;
    vector              Ln;
    color               Fs;
    float nonspec       = 0;
    color spec          = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    illuminance(P, Nf, PI/2)
    {
        lightsource("__nonspecular", nonspec);
        if (nonspec == 0)
        {
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            Ln          = normalize(L);
            Fs          = specularbrdf(normalize(Ln), normalize(Nf), normalize(In), hard);
            if (Fs != color(0))
                spec    += lightcolor*shadowcolor*Fs;
        }
    }
    return spec;
}


//// Returns Phong specular model
color phongspec(
    normal Nf;              // Normalized faceforward surface normal
    vector In;              // Normalized camera vector
    float size;             // Specular size
    float hard;)            // Specular hard factor
{
    extern point        P;
    vector              Ln;
    color               Fs;
    float nonspec       = 0;
    float lighttype     = 0;
    vector R            = reflect(-In, Nf);
    color spec          = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    illuminance(P, Nf, PI/2)
    {
        lightsource("__nonspecular", nonspec);
        if (nonspec == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
                Fs      = specularbrdf(normalize(Ln), normalize(Nf), normalize(In), hard);
            else
                Fs      = pow(max(0.0, R.Ln), size);
            if (Fs != color(0))
                spec    += lightcolor*shadowcolor*Fs;
        }
    }
    return spec;
}


//// Returns Blinn specular model
color blinnspec(
    normal Nf;              // Normalized faceforward surface normal
    vector In;              // Normalized camera vector
    float hard;             // Specular hard factor
    float refr;)            // Index of refraction
{
    extern point        P;
    vector              Ln, R, T;
    color               Fs;
    float               Kr, Kt;
    float nonspec       = 0;
    float lighttype     = 0;
    float ior           = (refr-1)/9;
    color spec          = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    fresnel(-In, Nf, 0.1, Kr, Kt, R, T);
    illuminance(P, Nf, PI/2)
    {
        lightsource("__nonspecular", nonspec);
        if (nonspec == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
                Fs      = specularbrdf(normalize(Ln), normalize(Nf), normalize(In), hard);
            else
                Fs      = max((specularbrdf(normalize(Ln), normalize(Nf), normalize(In), hard)-Kt)*2, color(0))*ior;
            if (Fs != color(0))
                spec    += lightcolor*shadowcolor*Fs;
        }
    }
    return spec;
}


//// Returns simple toon cell shading for specularity
color toonspec(
    normal Nf;              // Normalized faceforward surface normal
    vector In;              // Normalized camera vector
    float specSize;         // Size of specular solid color
    float specSmooth;       // Smoothing or solid color's edges
    float hard;)            // Specular hard factor
{
    extern point        P;
    vector              Ln;
    color               brdf;
    color               Fs;
    float nonspec       = 0;
    float lighttype     = 0;
    color spec          = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    illuminance(P, Nf, PI/2)
    {
        lightsource("__nonspecular", nonspec);
        if (nonspec == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
            {
                Fs      = specularbrdf(normalize(Ln), normalize(Nf), normalize(In), hard);
            }
            else
            {
                brdf    = specularbrdf(normalize(Ln), normalize(Nf), normalize(In), 20);
                Fs      = smoothstep(cos(specSize)-sin(specSmooth)/1.3, cos(specSize), comp(brdf, 0));
            }
            if (Fs != color(0))
                spec    += lightcolor*shadowcolor*Fs;
        }
    }
    return spec;
}


//// Returns Wardlso specular model (poorly hacked variant from Cook model)
color wardspec(
    normal Nf;              // Normalized faceforward surface normal
    vector In;              // Normalized camera vector
    float rms;              // Simulation or Wardlso deviation of surface slope
    float hard;)            // Specular hard factor
{
    extern point        P;
    vector              Ln;
    color               Fs;
    float nonspec       = 0;
    float lighttype     = 0;
    color spec          = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    illuminance(P, Nf, PI/2)
    {
        lightsource("__nonspecular", nonspec);
        if (nonspec == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
                Fs      = specularbrdf(normalize(Ln), normalize(Nf), normalize(In), hard);
            else
                Fs      = min(comp(specularbrdf(normalize(Ln), normalize(Nf), normalize(In), rms)*(0.8-rms), 0)*(9-(20*rms)), 1);
            if (Fs != color(0))
                spec    += lightcolor*shadowcolor*Fs;
        }
    }
    return spec;
}


//// Returns anisotropic specular model
color anisospec(
    normal Nf;              // Normalized faceforward surface normal
    vector In;              // Normalized camera vector
    float hard;)            // Specular hard factor
{
    extern point        P;
    extern float        t;
    vector              Ln;
    color               Fs;
    float nonspec       = 0;
    float lighttype     = 0;
    vector tangent      = normalize(Deriv(P, t));
    color spec          = color(0);
    color lightcolor    = color(1);
    color shadowcolor   = color(1);
    illuminance(P, Nf, PI/2)
    {
        lightsource("__nonspecular", nonspec);
        if (nonspec == 0)
        {
            lightsource("__lighttype", lighttype);
            lightsource("__lightcolor", lightcolor);
            lightsource("__shadowcolor", shadowcolor);
            Ln          = normalize(L);
            if (lighttype == 3)
                Fs      = specularbrdf(normalize(Ln), normalize(Nf), normalize(In), hard);
            else
                Fs      = pow(max(0, sqrt(1-pow(Ln.tangent, 2))*sqrt(1-pow(In.tangent, 2))-(Ln.tangent)*(In.tangent)), 1/hard);
            if (Fs != color(0))
                spec    += lightcolor*shadowcolor*Fs;
        }
    }
    return spec;
}


//// Returns reflection and refraction model for both environment maps or raytracing if "raytrace" is used for map name
color mirrortransp(
    string Environment;
    vector In;              // Normalized camera vector
    vector Nf;              // Normalized faceforward surface normal
    uniform float dirMode;  // Direction mode: 0 == reflect, 1 == refract
    uniform float Samples;  // Number of samples for environment lookup
    uniform float maxDist;  // Maximum distance for ray visibility, also serves as env map lookup correction radius
    uniform float Gloss;    // Level of environment blur
    float IOR;              // Index of refraction
    float maxDepth;         // Maximum number of ray bounces
    float dispFactor;       // Factor displacement effect env map lookups
    float fadeMode;         // Non ray hit or env map alpha color mode: 0 = black, 1 = horizon color, 2 = diffuse color
    color mirCol;           // Color modulated on reflection
    color fogCol;)          // Color for sky (non ray hit) used for fadeMode 1
{
    extern point        P;
    extern normal       N;
    vector              R;
    color Col           = mirCol;
    color fadeCol       = fogCol*mirCol;
    string mapName      = Environment;
    if (dirMode == 0)
        R               = reflect(In, Nf);
    else
        R               = refract(In, Nf, (In.Nf < 0 ? 1/IOR : IOR));
    if (Environment != "raytrace")
    {
        if (match("planar", mapName))
        {
            //Transform N to same space as reflection map (shader->camera->-object) to correct for displacements, then transform to NDC for texture lookup
            point Pndc  = transform("NDC", P+vtransform("object", vtransform("camera", vector(Nf)))*dispFactor);
            float tx    = xcomp(Pndc);
            float ty    = ycomp(Pndc);
            Col         *= texture(mapName, tx, ty, "samples", Samples, "blur", Gloss);
            if (fadeMode != 1)
            {
                float alpha = texture(mapName[3], tx, ty, "samples", Samples, "blur", Gloss);
                Col         = Col*alpha + fadeCol*(1-alpha);
            }
        }
        else
        {
            //Adjust N to compensate for point position P during map lookup by mapRadius for more accurate reflection and refraction mapping
            R           = (vtransform("world", R*dispFactor)+(transform("world", P)-transform("object", "world", (0, 0, 0)))/maxDist)*vector(1, 1, -1);
            Col         *= environment(mapName, R, "samples", Samples, "blur", Gloss);
            if (fadeMode != 1)
            {
                float alpha = environment(mapName[3], R, "samples", Samples, "blur", Gloss);
                Col         = Col*alpha + fadeCol*(1-alpha);
            }
        }
    }
    else
    {
        float rayLen    = 0;
        float rayDepth  = 0;
        float avgLen    = 0;
        color rayCol    = color(0);
        color avgCol    = color(0);
        #ifdef AIR
            string catagory = "environment";
        #else
            string catagory = "illuminance";
        #endif
        gather(catagory, P, R, PI*0.5*Gloss, Samples, "maxdist", maxDist, "surface:Ci", rayCol, "ray:length", rayLen)
        {
            rayinfo("depth", rayDepth);
            if (rayDepth >= maxDepth)
            {
                avgLen  += maxDist;
                avgCol  += fadeCol;
            }
            else
            {
                avgLen  += rayLen;
                avgCol  += rayCol;
            }
        }
        else
        {
            avgLen      += maxDist;
            avgCol      += fadeCol;
        }
        avgLen          = min(avgLen/Samples/maxDist, 1);
        Col             *= (avgCol/Samples)*(1-avgLen)+fadeCol*avgLen;
    }
    
    return Col;
}


//// Returns occlusion model for both occlusion maps or raytracing if "raytrace" is used for map name
color hdrocclusion(
    string occMap;              // Occlusion depth map or set as "raytrace"
    string occPoint;            // Occlusion point cloud
    string hdrMap;              // HDR map to use for non ray hit color lookup
    vector Nf;                  // Normalized faceforward surface normal
    vector bendN;               // Bend surface normals in this direction
    uniform float occSamples;   // Number of samples to use for occlusion
    uniform float maxDist;      // Maximum distance for occlusion
    uniform float maxSolidAngle;// Quality versus speed for point data
    float mapBlur;              // Blur for occlusion map lookup
    float mapBias;              // Occlusion map bias
    float occFalloff;           // Falloff factor
    float hdrBlur;              // Blur for HDR map lookup
    float hdrSamples;           // Samples for HDR map lookup
    float hdrFactor;            // Factor of HDR color blended into occlusion
    color occCol;)              // Color modulated into occlusion
{
    extern point        P;
    float Hits          = 0;
    normal Nocc         = 0;
    color Occ           = color(1);;
    if (occMap != "raytrace")
    {
        if (occPoint != "")
        {
            #ifndef AIR
            #ifndef AQSIS       
                Hits    = comp(ctransform("hsv", color(1)-occlusion(P, Nf+bendN, 0, "pointbased", 1, "filename", occPoint, "maxdist", maxDist, "maxsolidangle", maxSolidAngle)), 2);
            #endif
            #endif
        }
        else
        {
            #ifdef AIR
                Hits    = comp(ctransform("hsv", color(1)-occlusion(occMap, P, Nf+bendN, PI/2, Nocc, "samples", occSamples, "bias", mapBias, "blur", mapBlur)), 2);
            #endif
            #ifdef AQSIS
                Hits    = comp(ctransform("hsv", color(1)-occlusion(occMap, P, Nf+bendN, PI/2, "samples", occSamples, "bias", mapBias, "blur", mapBlur)), 2);
            #endif
        }
    }
    else
    {
        #ifdef AIR
            Hits        = comp(ctransform("hsv", color(1)-occlusion(P, Nf+bendN, PI/2, Nocc, "samples", occSamples, "maxdist", maxDist)), 2);
        #else
        #ifndef AQSIS
            Hits        = comp(ctransform("hsv", color(1)-occlusion(P, Nf+bendN, occSamples, "maxdist", maxDist)), 2);
        #endif
        #endif
    }
    if (occFalloff > 0)
        Hits            = min(Hits*(occFalloff+1), 1);
    if (hdrMap != "")
    {
        color hdr       = environment(hdrMap, vtransform("world", Nf)*vector(1,1,-1), "samples", hdrSamples, "blur", hdrBlur);
        Occ             = occCol*mix(color(1), hdr, hdrFactor)*Hits;
    }
    else
    {
        Occ             = occCol*Hits;
    }
    return Occ;
}


//// Returns indirect diffusion model for both env maps or raytracing if "raytrace" is used for map name
color hdrindirectdiff(
    string diffMap;             // Diffusion env map or "raytrace"
    string diffPoint;           // Indirect diffusion point cloud
    string hdrMap;              // HDR map to use for non ray hit color lookup
    vector Nf;                  // Normalized faceforward surface normal
    uniform float diffSamples;  // Number of samples to use for indirect diffusion
    uniform float maxDist;      // Maximum distance for indirect diffusion
    uniform float maxSolidAngle;// Quality versus speed for point data
    float mapBlur;              // Blur for occlusion map lookup
    float dispFactor;           // Factor displacement effect env map lookups
    float  objScale;            // Scale of object to map planar diffusion map to
    float  hdrBlur;             // Blur for HDR map lookup
    float  hdrSamples;          // Samples for HDR map lookup
    float  hdrFactor;           // Factor of HDR color blended into indirect diffusion
    color diffCol;)             // Color modulated into indirect diffusion
{
    extern point            P;
    color Hits              = 0;
    color Diff              = color(0);
    string mapName          = diffMap;
    if (diffMap != "raytrace")
    {
        if (diffPoint != "")
        {
            #ifndef AIR
            #ifndef AQSIS
                Hits        = indirectdiffuse(P, Nf, 0, "pointbased", 1, "filename", diffPoint, "maxdist", maxDist, "maxsolidangle", maxSolidAngle);
            #endif
            #endif
        }
        else
        {
            if (match("planar", mapName))
            {
                //Transform P plus displacements to object space and adjust coordinates to texture ortho scale to map planar map directly to surface
                point Pobj  = transform("object", P+Nf*dispFactor);
                float       tx = 0.5-xcomp(-Pobj)/objScale;
                float       ty = 0.5-ycomp(Pobj)/objScale;
                Hits        = texture(mapName, tx, ty, "samples", diffSamples, "blur", mapBlur);
            }
            else
            {
                float envBlur   = mapBlur/4;
                //Adjust N to componsate for point position P during map lookup to directly map env to surface
                vector D    = (vtransform("world", Nf*dispFactor)+(transform("world", P)-transform("object", "world", (0, 0, 0))))*vector(1, 1, -1);
                Hits        = environment(mapName, D, "samples", diffSamples, "blur", envBlur);
            }
        }
    }
    #ifndef AQSIS
        else
        {
            Hits            = indirectdiffuse(P, Nf, diffSamples, "maxdist", maxDist);
        }
    #endif
    
    if (hdrMap != "")
    {
        color hdr           = environment(hdrMap, vtransform("world", Nf)*vector(1,1,-1), "samples", hdrSamples, "blur", hdrBlur);
        Diff                = diffCol*mix(color(1), hdr, hdrFactor)*Hits;
    }
    else
    {
        Diff                = diffCol*Hits;
    }
    return Diff;
}

#endif


//////// Displacement Shader Utility Functions...

#ifdef DISPLACE_UTILITIES
    // None
#endif


//////// Light Shader Utility Functions...

#ifdef LIGHT_UTILITIES

//// Returns avarage distance from specified point to nearest light blocker referenced in depth map
float blockerdepth(
    string depthmap;        // Depth map to check blocker depth from
    uniform float Samples;  // Number of samples to average depth by
    uniform float Radius;   // Sample radius to average depth by
    uniform float Power;    // Power final depth value
    uniform float MinAvg;   // Minimum average depth returned
    uniform float MaxAvg;)  // Maximum average depth returned
{
    extern point            Ps;
    float depth             = 0;
    if (depthmap != "" && depthmap != "raytrace")
    {
        uniform matrix      shadProjSpace;
        uniform matrix      shadCamSpace;
        textureinfo(depthmap, "projectionmatrix", shadProjSpace);
        textureinfo(depthmap, "viewingmatrix", shadCamSpace);
        point shadProjP     = transform(shadProjSpace, Ps);
        point shadCamP      = transform(shadCamSpace, Ps);
        float PsDepth       = zcomp(shadCamP);
        float sloc          = (1+xcomp(shadProjP))*0.5;
        float tloc          = (1-ycomp(shadProjP))*0.5;
        float weight        = 0;
        float step          = 0;
        for (step = 0; step < Radius; step += Radius/Samples)
        {
            float angle     = random()*360.0;
            float sstep     = sloc+sin(angle)*step;
            float tstep     = tloc+cos(angle)*step;
            float Blocker   = float texture(depthmap, sstep, tstep, "samples", 1, "width", 0);
            if (Blocker < 3.402823466e+38 && Blocker < PsDepth)
            {
                float w     = 1-0.9*(step/Radius);
                weight      += w;
                depth       += (PsDepth-Blocker)*w;
            }
        }
        if (weight > 0)
        {
            depth           = clamp(depth/weight, MinAvg, MaxAvg);
            if (Power > 1)
                depth       = pow(depth, Power);
        }
    }
    return depth;
}

#endif


//////// Volume Shader Utility Functions...

#ifdef VOLUME_UTILITIES

//// Uses a light passes environment and depth map to project blurred and attenuated surface color into surrounding volume
color volumehaze(
    string colormap;        // The name of an image containing a color render from the lights perpective
    string depthmap;        // The name of an image containing a depth render from the lights perpective
    point Pv;               // Point shaded in volume in world coordinates
    uniform float samples;  // Number of random samples in volume
    uniform float radius;   // Size of radius around shading point to sample
    uniform float dist;)    // Distance of attenuation from surface torwards light
{
    color haze              = color(0);
    if (dist > 0 && colormap != "" && depthmap != "")
    {
        uniform matrix      shadProjSpace;
        uniform matrix      shadCamSpace;
        textureinfo(depthmap, "projectionmatrix", shadProjSpace);
        textureinfo(depthmap, "viewingmatrix", shadCamSpace);
        point shadProjP     = transform(shadProjSpace, Pv);
        point shadCamP      = transform(shadCamSpace, Pv);
        float PvDepth       = zcomp(shadCamP);
        float sloc          = (1+xcomp(shadProjP))*0.5;
        float tloc          = (1-ycomp(shadProjP))*0.5;
        float step          = 0;
        for (step = 0; step < samples; step += 1)
        {
            float sstep     = sloc+(1-random()*2)*radius;
            float tstep     = tloc+(1-random()*2)*radius;
            float Blocker   = float texture(depthmap, sstep, tstep, "samples", 1, "width", 0);
            if (Blocker < 3.402823466e+38 && Blocker > PvDepth)
            {
                float atten = 1-min((Blocker-PvDepth)/dist, 1);
                if (atten > 0)
                    haze    += color texture(colormap, sstep, tstep, "samples", 1, "width", 0)*atten;
            }
        }
        if (samples > 1)
            haze            /= samples;
    }
    return haze;
}

#endif


//////// Image Shader Utility Functions...

#ifdef IMAGE_UTILITIES
    // None
#endif

