/*******************************************************************************
    Copyright (c) 2021, Andrea Maggiordomo, Paolo Cignoni and Marco Tarini

    This file is part of TextureDefrag, a reference implementation for
    the paper ``Texture Defragmentation for Photo-Reconstructed 3D Models''
    by Andrea Maggiordomo, Paolo Cignoni and Marco Tarini.

    TextureDefrag is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TextureDefrag is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TextureDefrag. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef SHELL_H
#define SHELL_H

class Mesh;
class FaceGroup;

/* Builds a shell for the given chart. A shell is a mesh object specifically
 * constructed to compute the parameterization of a chart. In order to support
 * the various operations that we need to perform on it, it is more convenient
 * to keep its shape as the current 2D parameter-space configuration (possibly
 * not updated). The shell has suitable attributes to retrieve information about
 * the shell-face to input mesh-face mappings, as well as the target shape
 * features of each face to guide the parameterization process. (See also the
 * comments in mesh_attribute.h). */
bool BuildShellWithTargetsFromUV(Mesh& shell, FaceGroup& fg, double downscaleFactor);

void CloseHoles3D(Mesh& shell);

/* This function synchronizes a shell with its UV coordinates, that is it
 * updates its vertex coordinates to match the parameter space configurations
 * (with z = 0). The operation is performed per-vertex. */
void SyncShellWithUV(Mesh& shell);

/* This function synchronizes a shell with the model space coordinates of its
 * chart. */
void SyncShellWith3D(Mesh& shell);

/* Removes any hole-filling face from the shell and compacts its containers */
void ClearHoleFillingFaces(Mesh& shell, bool holefill, bool scaffold);


#endif // SHELL_H
