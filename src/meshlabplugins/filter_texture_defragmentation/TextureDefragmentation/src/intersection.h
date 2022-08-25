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

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vcg/space/segment2.h>
#include <vcg/space/point2.h>
#include <vcg/space/box2.h>

#include <vector>

#include "mesh.h"

struct HalfEdge {
    Mesh::FacePointer fp;
    int e;

    vcg::Point2d P0() const { return fp->V0(e)->T().P(); }
    vcg::Point2d P1() const { return fp->V1(e)->T().P(); }

    Mesh::VertexPointer V0() const { return fp->V0(e); }
    Mesh::VertexPointer V1() const { return fp->V1(e); }

    bool operator<(const HalfEdge& other) const { return (fp < other.fp) || (fp == other.fp && e < other.e); }
    bool operator==(const HalfEdge& other) const { return fp == other.fp && e == other.e; }
};

typedef vcg::Segment2d Segment;
typedef std::pair<HalfEdge, HalfEdge> HalfEdgePair;

bool SegmentBoxIntersection(const Segment& seg, const vcg::Box2d& box);

std::vector<HalfEdgePair> Intersection(const std::vector<HalfEdge>& heVec);
std::vector<HalfEdgePair> CrossIntersection(const std::vector<HalfEdge>& heVec1, const std::vector<HalfEdge>& heVec2);

#endif // INTERSECTION_H
