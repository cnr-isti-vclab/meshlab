/*
 * Connect3D - Reconstruction of water-tight triangulated meshes from unstructured point clouds
 *
 * please credit the following article: Stefan Ohrhallinger, Sudhir Mudur and Michael Wimmer:
 * 'Minimizing Edge Length to Connect Sparsely Sampled Unstructured Point Sets',
 * Shape Modeling International 2013, published in Computers & Graphics Journal, 2013.
 *
 * Copyright (C) 2013 Stefan Ohrhallinger, Daniel Prieler
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#pragma once

#include "Vector3D.h"


float calcCircumcenterRadiusDet(const Vector3D &p0, const Vector3D &p1, const Vector3D &p2, Vector3D &cc);

Vector3D calcCircumcenterDet(Vector3D p0, Vector3D p1, Vector3D p2);

bool comparePoints(const Vector3D &p0, const Vector3D &p1);

Vector3D calcCircumcenter(const Vector3D &a, const Vector3D &b, const Vector3D &c);

double acosSafe(double x);

float dihedralAngle(const Vector3D &v0, const Vector3D &v1, const Vector3D &v2, const Vector3D &v3);

float calcTriangleArea(const Vector3D &p0, const Vector3D &p1, const Vector3D &p2);

float calcTriangleAspectRatio(const Vector3D &p0, const Vector3D &p1, const Vector3D &p2);

float calcTriangleSemiPerimeter(const Vector3D &p0, const Vector3D &p1, const Vector3D &p2);