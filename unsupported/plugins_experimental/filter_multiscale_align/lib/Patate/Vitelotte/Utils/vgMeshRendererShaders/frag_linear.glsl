/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#version 410 core

uniform samplerBuffer nodes;
uniform int baseNodeIndex;
uniform bool singularTriangles;
uniform bool enableShading;
uniform int meshColorSpace;
uniform int screenColorSpace;

flat in int frag_index;
in vec3 frag_linearBasis;
in vec4 frag_position_obj;
in vec3 frag_normal_obj;
in vec3 frag_normal_view;
flat in vec3 frag_vertices_obj[3];
flat in vec3 frag_normEdges_obj[3];

out vec4 out_color;

float irlerp(in vec3 vx, in vec3 v1, in vec3 v2);
vec4 quadraticInterp(in vec4 colors[6]);
float diffuse(in vec3 n, in vec3 l);
vec3 convertColor(in vec3 fromColor, in int from, in int to);

int baseVxIndex = baseNodeIndex + frag_index * (3 + int(singularTriangles));

vec4 linearInterp(in vec4 colors[3])
{
    return
        colors[0] * frag_linearBasis.x +
        colors[1] * frag_linearBasis.y +
        colors[2] * frag_linearBasis.z;
}

void main(void)
{
    vec4 colorNodes[] = vec4[3](
        texelFetch(nodes, baseVxIndex + 0),
        texelFetch(nodes, baseVxIndex + 1),
        texelFetch(nodes, baseVxIndex + 2)
    );

    if(singularTriangles)
    {
        colorNodes[0] = mix(colorNodes[0],
                            texelFetch(nodes, baseVxIndex + 3),
                            irlerp(normalize(frag_position_obj.xyz - frag_vertices_obj[0]),
                                   frag_normEdges_obj[2], -frag_normEdges_obj[1]));
    }

    out_color = linearInterp(colorNodes);

    if(enableShading) {
        // Shading is done in linear RGB
        out_color.rgb = convertColor(out_color.rgb, meshColorSpace, 2);

        vec3 n = normalize(frag_normal_view);
        vec3 light = vec3(0.);
        light = diffuse(n, normalize(vec3(-.2, 0, -1.))) * vec3(1., .9, .8) * .8
              + diffuse(n, normalize(vec3( 1, .2,  .2))) * vec3(.8, .9, 1.) * .6;

        out_color.rgb = convertColor(light * out_color.rgb,
                                     1, screenColorSpace);
    } else {
        out_color.rgb = convertColor(out_color.rgb, meshColorSpace, screenColorSpace);
    }
}
