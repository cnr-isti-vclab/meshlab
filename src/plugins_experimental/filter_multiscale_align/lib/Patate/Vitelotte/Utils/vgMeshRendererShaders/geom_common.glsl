/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform vec2 viewportSize;

in vec4 geom_position_obj[];
in vec3 geom_normal_obj[];
in vec3 geom_normal_view[];

flat out int frag_index;
out vec3 frag_linearBasis;
out vec3 frag_position_obj;
out vec3 frag_normal_obj;
out vec3 frag_normal_view;
out vec3 frag_edgeDist_scr;
flat out vec3 frag_vertices_obj[3];
flat out vec3 frag_normEdges_obj[3];

const vec3 basis[3] = vec3[3](
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1)
);

void main()
{
    vec2 position_scr[3];
    for(int i=0; i<3; ++i)
    {
        position_scr[i] = (viewportSize * gl_in[i].gl_Position.xy)
                        / (2.0 * gl_in[i].gl_Position.z);
    }
    float area = abs(cross(vec3(position_scr[1] - position_scr[0], 0.0),
                           vec3(position_scr[2] - position_scr[0], 0.0)).z);
    for(int i=0; i<3; ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        frag_index = gl_PrimitiveIDIn;
        frag_linearBasis = basis[i];
        frag_position_obj = geom_position_obj[i].xyz;
        frag_normal_obj = geom_normal_obj[i];
        frag_normal_view = geom_normal_view[i];
        frag_edgeDist_scr = vec3(0.0);
        frag_edgeDist_scr[i] = area / length(position_scr[(i+2)%3] - position_scr[(i+1)%3]);
        for(int j=0; j<3; ++j)
        {
            frag_vertices_obj[j] = geom_position_obj[j].xyz;//gl_in[j].gl_Position.xy;
            frag_normEdges_obj[j] = normalize(geom_position_obj[(j+2)%3].xyz
                                              - geom_position_obj[(j+1)%3].xyz);
        }
        EmitVertex();
    }
}
