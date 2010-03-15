/* Beta-0.4.7: MOSAIC default atmosphere shader for Blender integration */

#define VOLUME_UTILITIES 1
#include "MOSAICfunctions.h"

volume
MOSAICfog(
    uniform float HazeRadius            = 0.1;
    uniform float HazeSamples           = 2;
    uniform float HazeEnergy            = 2;
    uniform float HazeDist              = 1;
    uniform float HaloVary1             = 0;
    uniform point HaloVary1Scale        = point "world" (1, 1, 1);
    uniform point HaloVary1Offset       = point "world" (0, 0, 0);
    uniform float HaloVary2             = 0;
    uniform point HaloVary2Scale        = point "world" (4, 4, 4);
    uniform point HaloVary2Offset       = point "world" (0, 0, 0);
    uniform float HaloAtten             = 2;
    uniform float HaloFactor            = 1;
    uniform float HaloJitter            = 1;
    uniform float HaloStepSize          = 0.2;
    uniform float HaloMaxSteps          = 10000;
	uniform float MistUF_X0[6]			= {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	uniform color MistUC_X0[1]			= {color(1.0, 1.0, 1.0)};
 output varying color _mistcolor     = 0;    //Mist color channel for AOV
    output varying float _mistalpha     = 0;)   //Mist alpha depth channel for AOV
{
    //// Lets setup general purpose variables...
    varying float                       p1, p2, p3, p4, nonfog = 0, halolights = 0, atten = 1;
    varying float Li                    = length(I);
    varying float atype                 = (MistUF_X0[MIST_UF_MISTTYPE] == 0 ? 3 : MistUF_X0[MIST_UF_MISTTYPE]);
    varying color                       mist = color(0), halo = color(0);
    uniform string rendertype           = "beauty";
    
    //// Lets manage parameter passing...
    surface("__nonfog", nonfog);
    attribute("user:render_halos", halolights);
    attribute("user:render_type", rendertype);
    
    //// Lets do it...
    
    // Are we using light halos?
    if (nonfog == 0 && halolights > 0)
    {
        float                           samples = 0, step = 0, sta = 0, dis = 1;
        point WO                        = P-I;
        point O                         = transform("shader", WO);
        vector In                       = normalize(vtransform("shader", I));
        vector WIn                      = vtransform("shader", "current", In);
        
        // If attenuation is off then use clipping distances instead of mist settings...
        if (HaloAtten == 0 || (HaloAtten == 2 && MistUF_X0[MIST_UF_ISMIST] == 0))
        {
            float clip[2]               = {0, 0};
            option("Clipping", clip);
            sta                         = clip[0];
            dis                         = clip[1]-sta;
        }
        // Otherwise use mist diatance settings...
        else
        {
            sta                         = MistUF_X0[MIST_UF_MISTSTA];
            dis                         = MistUF_X0[MIST_UF_MISTDI];
        }
        
        float start                     = sta+random()*HaloJitter*HaloStepSize;
        float end                       = min(length(I), start+dis, start+HaloStepSize*HaloMaxSteps);
        float steps                     = min(dis, HaloStepSize*HaloMaxSteps)/HaloStepSize;
        
        // Step from front to back towards volume point...
        for (step = start; step <= end; step += HaloStepSize)
        {
            point Ws                    = WO+step*WIn;
            point Os                    = O+step*In;
            
            // Step through all lights at volume point...
            illuminance(Ws)
            {
                // Does current light use halo?
                float halolight         = 0;
                lightsource("__foglight", halolight);
                
                // Only process halo lights for illuminated points...
                if(halolight > 0 && Cl != color(0) && (rendertype == "beauty" || rendertype == "environment_map" || rendertype == "user_pass"))
                {
                    color hazecolor     = color(0);
                    float halovary      = 1;
                    float haloint       = 1;
                    string shadowname   = "";
                    string depthname    = "";
                    string hazename     = "";
                    string depthmap     = "";
                    lightsource("__haloint", haloint);
                    lightsource("__hazename", hazename);
                    lightsource("__shadowname", shadowname);
                    lightsource("__depthname", depthname);
                    
                    // Use special depth map if specified otherwise use standard shadow map...
                    if (depthname != "")
                        depthmap        = depthname;
                    else
                        depthmap        = shadowname;
                    
                    // Lets figure halo attenuation if used...
                    if (HaloAtten == 1 || (HaloAtten == 2 && MistUF_X0[MIST_UF_ISMIST] > 0))
                    {
                        p1              = step-start;
                        p2              = MistUF_X0[MIST_UF_MISTDI];
                        atten           = Attenuate(atype, p1, p2, 0, 0, 1);
                        // Lets mix in height attenuation if used...
                        if (MistUF_X0[MIST_UF_MISTHI] > 0)
                            atten       = mix(atten, 1, 1-clamp(zcomp(transform("world", Ws))/MistUF_X0[MIST_UF_MISTHI], 0, 1));
                        // Lets adjust overall attenuation by mist intensity...
                        if (MistUF_X0[MIST_UF_MISTINT] > 0)
                            atten       *= 1-MistUF_X0[MIST_UF_MISTINT];
                    }
                    
                    // Lets figure 3D density variance if used...
                    if (HaloVary1 > 0)
                    {
                        point S1        = transform("shader", HaloVary1Scale);
                        point O1        = transform("shader", HaloVary1Offset);
                        halovary        *= max(0, mix(1, float noise((Os+O1)*S1), HaloVary1));
                    }
                    if (HaloVary2 > 0)
                    {
                        point S2        = transform("shader", HaloVary2Scale);
                        point O2        = transform("shader", HaloVary2Offset);
                        halovary        *= max(0, mix(1, float noise((Os+O2)*S2), HaloVary2));
                    }
                    
                    // Add ray or image based haze to volume if used...
                    if (atten > 0 && halovary > 0 && hazename != "")
                        hazecolor       = volumehaze(hazename, depthmap, Ws, HazeSamples, HazeRadius, HazeDist)*HazeEnergy;
                    
                    // Lets put it all together...
                    halo                += (Cl+hazecolor)*haloint*halovary*HaloFactor*atten;
                }
            }
        }
        
        halo                            /= steps;
        
        // If attenuation is off then use halo intensity to determine mixing...
        if (HaloAtten == 0 || (HaloAtten == 2 && MistUF_X0[MIST_UF_ISMIST] == 0))
            atten                       = 1-clamp(comp(ctransform("hsv", halo), 2), 0, 1);
    }
    
    // Are we using mist?
    if (nonfog == 0 && MistUF_X0[MIST_UF_ISMIST] > 0)
    {
        mist                            = MistUC_X0[MIST_UC_MISTCOL];
        
        // Lets figure mist attenuation if used...
        if (Li > MistUF_X0[MIST_UF_MISTSTA])
        {
            p1                          = Li-MistUF_X0[MIST_UF_MISTSTA];
            p2                          = MistUF_X0[MIST_UF_MISTDI];
            atten                       = Attenuate(atype, p1, p2, 0, 0, 1);
            // Lets mix in height attenuation if used...
            if (MistUF_X0[MIST_UF_MISTHI] > 0)
                atten                   = mix(atten, 1, clamp(zcomp(transform("world", P))/MistUF_X0[MIST_UF_MISTHI], 0, 1));
            // Lets adjust overall attenuation by mist intensity...
            if (MistUF_X0[MIST_UF_MISTINT] > 0)
                atten                   *= 1-MistUF_X0[MIST_UF_MISTINT];
        }
    }
    
    //// Lets mix the results...
    mist                                += halo;
    _mistalpha                          = atten;
    _mistcolor                          = mist;
    Ci                                  = mix(mist, Ci, atten);
    Oi                                  = mix(color(1), Oi, atten);
}
