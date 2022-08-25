/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#version 410 core

uniform mat4 viewMatrix;
uniform mat3 normalMatrix;

in vec4 vx_position;
in vec3 vx_normal;

out vec4 ctrl_position_obj;
out vec3 ctrl_normal_obj;
out vec3 ctrl_normal_view;

void main(void)
{
    gl_Position = viewMatrix * vx_position;
    ctrl_position_obj = vx_position;
    ctrl_normal_obj = vx_normal;
    ctrl_normal_view = normalMatrix * vx_normal;
}
