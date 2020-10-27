/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#version 410 core

uniform samplerBuffer nodes;
uniform float lineWidth;
uniform vec4 wireframeColor;

in vec3 frag_edgeDist_scr;

out vec4 out_color;

vec3 computeEdgeDist();
int minIndex(in vec3 dist);
float interpFactor(float dist, float radius);

void main(void)
{
    float alpha = smoothstep(
        -0.5, 0.5,
        lineWidth / 2.0 - min(frag_edgeDist_scr.x,
                              min(frag_edgeDist_scr.y,
                                  frag_edgeDist_scr.z)));
    if(alpha < 0.001)
        discard;

    out_color = vec4(wireframeColor.rgb, wireframeColor.a * alpha);
}
