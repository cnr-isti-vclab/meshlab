/* Beta-0.4.7: MOSAIC default image shader for Blender integration */

#define IMAGE_UTILITIES 1
#include "MOSAICfunctions.h"

imager
MOSAICbackground(
    color bgcolor=color(1, 1, 1);
    float CompositePass=0;
    string CompositeImage="";
    float CompositeBlend=1.0;)
{
    Ci += (1 - alpha) * bgcolor;
    if (CompositePass > 0)
    {
        if (CompositeImage != "")
        {
            float res[3] = {1, 1, 1};
            option("Format",res);
            point Pcam = transform("camera",P);
            float rx = xcomp(P)/res[0];
            float ry = ycomp(P)/res[1];
            Ci = colorblend(color texture(CompositeImage,rx,ry, "samples", 1, "width", 0.00001), Ci, 7, 1, 1);
        }
        else
        {
            Ci = Ci*CompositeBlend;
        }
    }
    alpha = 1;
}

