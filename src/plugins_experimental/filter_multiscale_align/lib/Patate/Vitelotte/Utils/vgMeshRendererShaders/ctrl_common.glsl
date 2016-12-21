/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#version 410 core

layout(vertices = 9) out;

uniform vec2 viewportSize;
uniform float smoothness;

in vec4 ctrl_position_obj[];
in vec3 ctrl_normal_obj[];
in vec3 ctrl_normal_view[];

out vec4 eval_position_obj[];
out vec3 eval_normal_obj[];
out vec3 eval_normal_view[];
out ScreenSpaceBlock {
    flat vec3 heights_scr;
    flat vec3 vertices_obj[3];
    flat vec3 normEdges_obj[3];
} eval_ss[];

bool isLinear(int i) {
    vec4 p0 = eval_position_obj[3*i + 0];
    vec4 p1 = eval_position_obj[3*i + 1];
    vec4 p2 = eval_position_obj[3*i + 2];
    vec4 p3 = eval_position_obj[(3*i + 3)%9];

    vec4 p1off = p1 - (2*p0 + p3) / 3;
    vec4 p2off = p2 - (p0 + 2*p3) / 3;

    return dot(p1off, p1off) < 0.0001
        && dot(p2off, p2off) < 0.0001;
}

float curviness(int i) {
    vec4 p0 = eval_position_obj[3*i + 0];
    vec4 p1 = eval_position_obj[3*i + 1];
    vec4 p2 = eval_position_obj[3*i + 2];
    vec4 p3 = eval_position_obj[(3*i + 3)%9];

    vec4 p1off = p1 - (2*p0 + p3) / 3;
    vec4 p2off = p2 - (p0 + 2*p3) / 3;

    return max(length(p1off), length(p2off)) / distance(p0, p3);
}

void main(void)
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    eval_position_obj[gl_InvocationID]  = ctrl_position_obj[gl_InvocationID];
    eval_normal_obj[gl_InvocationID]    = ctrl_normal_obj[gl_InvocationID];
    eval_normal_view[gl_InvocationID]   = ctrl_normal_view[gl_InvocationID];

    vec2 position_scr[3];
    for(int i=0; i<3; ++i)
    {
        position_scr[i] = (viewportSize * gl_in[i*3].gl_Position.xy)
                        / (2.0 * gl_in[i*3].gl_Position.z);
    }
    float area = abs(cross(vec3(position_scr[1] - position_scr[0], 0.0),
                           vec3(position_scr[2] - position_scr[0], 0.0)).z);

    for(int i=0; i<3; ++i)
    {
        eval_ss[gl_InvocationID].heights_scr[i]
                = area / length(position_scr[(i+2)%3] - position_scr[(i+1)%3]);
        eval_ss[gl_InvocationID].vertices_obj[i]
                = ctrl_position_obj[i*3].xyz;
        eval_ss[gl_InvocationID].normEdges_obj[i]
                = normalize(ctrl_position_obj[((i+2)%3)*3].xyz - ctrl_position_obj[((i+1)%3)*3].xyz);
    }

    bvec3 isEdgeLinear = bvec3(isLinear(0), isLinear(1), isLinear(2));
    gl_TessLevelOuter = float[4](
                isEdgeLinear[1]? 1: smoothness,
                isEdgeLinear[2]? 1: smoothness,
                isEdgeLinear[0]? 1: smoothness,
                1);
    gl_TessLevelInner = float[2](all(isEdgeLinear)? 1: smoothness, 1);
    // Uncomment this for adaptive smoothing (using a very rough heristic).
//    gl_TessLevelOuter = float[4](
//                max(curviness(1) * smoothness, 1),
//                max(curviness(2) * smoothness, 1),
//                max(curviness(0) * smoothness, 1),
//                1);
}
