/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#version 410 core

layout(triangles) in;

uniform mat4 viewMatrix;

uniform samplerBuffer nodes;
uniform int baseNodeIndex;
uniform bool singularTriangles;

in vec4 eval_position_obj[];
in vec3 eval_normal_obj[];
in vec3 eval_normal_view[];
in ScreenSpaceBlock {
    flat vec3 heights_scr;
    flat vec3 vertices_obj[3];
    flat vec3 normEdges_obj[3];
} eval_ss[];

flat out int frag_index;
out vec3 frag_linearBasis;
out vec4 frag_position_obj;
out vec3 frag_normal_obj;
out vec3 frag_normal_view;
out vec3 frag_edgeDist_scr;
flat out vec3 frag_vertices_obj[3];
flat out vec3 frag_normEdges_obj[3];

void main(void)
{
    vec3 c1 = gl_TessCoord;
    vec3 c2 = c1 * c1;
    vec3 c3 = c2 * c1;
    vec4 mid = vec4(0);
    for(int i = 0; i < 9; ++i) mid += eval_position_obj[i];
    mid /= 9;
    frag_position_obj = eval_position_obj[0] * c3[0]
                      + eval_position_obj[1] * c2[0] * c1[1] * 3
                      + eval_position_obj[2] * c1[0] * c2[1] * 3
                      + eval_position_obj[3] * c3[1]
                      + eval_position_obj[4] * c2[1] * c1[2] * 3
                      + eval_position_obj[5] * c1[1] * c2[2] * 3
                      + eval_position_obj[6] * c3[2]
                      + eval_position_obj[7] * c2[2] * c1[0] * 3
                      + eval_position_obj[8] * c1[2] * c2[0] * 3
                      + mid                  * c1[0] * c1[1] * c1[2] * 6;

    gl_Position = viewMatrix * frag_position_obj;
    frag_index         = gl_PrimitiveID;
    frag_linearBasis   = gl_TessCoord;
    frag_normal_obj    = mat3(eval_normal_obj[0],
                              eval_normal_obj[3],
                              eval_normal_obj[6]) * gl_TessCoord;
    frag_normal_view   = mat3(eval_normal_view[0],
                              eval_normal_view[3],
                              eval_normal_view[6]) * gl_TessCoord;
    frag_edgeDist_scr  = eval_ss[0].heights_scr * gl_TessCoord;
    frag_vertices_obj  = eval_ss[0].vertices_obj;
    frag_normEdges_obj = eval_ss[0].normEdges_obj;
}
