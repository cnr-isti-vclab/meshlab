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

struct c3dModelData;
class Triangle;

typedef bool ProgressCallback(const int pos, const std::string &msg);

class Connect3D {

public:
	Connect3D();
	~Connect3D();

	static std::vector<Triangle *> reconstruct(c3dModelData &modelData, ProgressCallback *p);

	static std::string info();

};



