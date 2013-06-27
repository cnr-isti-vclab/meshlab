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

#include "precompiled.h"

#include "c3dDataStructures.h"

#include "c3dConstructDG.h"



void construct3DDelaunayTriangulation(c3dModelData &data)
{
	size_t i;
	data.dt.reset(new Dt());

	for (i = 0; i < data.numVertices; i++)
	{
		Point point = Point(data.vertices[i][0], data.vertices[i][1],data.vertices[i][2]);
		DVertex vh = data.dt->insert(point);
		vh->info().index = i;
	}

    if (data.dt->number_of_vertices() != data.numVertices)
	{
		// adjust indices to account for removed duplicate points (required for consistency, and no indices >= vertexcount!)
		std::map<int, int> vertexMap;
		i = 0;

		for (Dt::Vertex_iterator vhIter = data.dt->vertices_begin(); vhIter != data.dt->vertices_end(); vhIter++)
		{
			DVertex vh = vhIter;

			if (vh->info().index != -1)
			{
				vertexMap[i] = vh->info().index;
				vh->info().index = i;
				i++;
			}
		}
		
		data.numVertices = (int)data.dt->number_of_vertices();
		Vector3D *newV = new Vector3D[data.numVertices];

		for (i = 0; i < data.numVertices; i++)
		{
			newV[i] = data.vertices[vertexMap[i]];
		}

		data.vertices.reset(newV);
	}

}


void createAggregateDataStructure(c3dModelData &data)
{
	int i, j;

	//std::map<IntPair, Edge *> edgeMap;
	btree::btree_map<IntPair, Edge *> edgeMap;

	// traverse all tetrahedra and create their triangles, edges and references to them
	std::stack<Cell> tetraStack;
	tetraStack.push(data.dt->infinite_cell());


	size_t numEdges = data.dt->number_of_edges();
	size_t numTris = data.dt->number_of_facets();

	data.dtTriangles.reserve(numTris);
	data.dtEdgeList.reserve(numEdges);

	size_t initialTrianglesCapacity = data.dtTriangles.capacity();
	size_t initialEdgesCapacity = data.dtEdgeList.capacity();


	// while stack not empty
	while (tetraStack.size() > 0)
	{
		// pop tetra from stack
		Cell ch = tetraStack.top();
		tetraStack.pop();
		Tetrahedron *currTetra = &ch->info();

		bool triangleCreated[4];
		Edge *edge[6];

		for (i = 0; i < 6; i++)
			edge[i] = nullptr;

		// reference its four triangles (create if not yet existing)
		for (i = 0; i < 4; i++)
		{
			triangleCreated[i] = false;

			// look up at neighbor tetrahedron
			Cell oppCH = ch->neighbor(i);
			int oppIndex = oppCH->index(ch);
			Tetrahedron *oppTetra = &oppCH->info();

			// test if triangle exists already
			Triangle *triangle = oppTetra->triangle(oppIndex);

			if (triangle == nullptr)
			{
				triangleCreated[i] = true;

				// create new triangle
				Triangle newTriangle(ch, i);
				data.dtTriangles.push_back(newTriangle);
				triangle = &data.dtTriangles.back();

				//vector was resized.. update pointers!
				if(data.dtTriangles.capacity() != initialTrianglesCapacity)
				{
					std::cerr << "Triangles Datastructure was probably reallocated, pointers are no longer valid!!" << std::endl;
					assert(false);
				}


				// also push tetrahedron on stack as unhandled
				tetraStack.push(oppCH);

				currTetra->setTriangle(i, triangle);	// reference triangle
				oppTetra->setTriangle(oppIndex, triangle);		// also in opposite tetrahedron
			}
			else
			{
				currTetra->setTriangle(i, triangle);	// reference triangle
						
				//is probably unnecessary
				//oppTetra->setTriangle(oppIndex, triangle);		// also in opposite tetrahedron
			}


		}

		// reference its six edges (create if not yet existing)
		for (i = 0; i < 6; i++)
		{
			int edgeVIndex[2];

			for (j = 0; j < 2; j++)
				edgeVIndex[j] = ch->vertex(tetraEdgeVertexIndices[i][j])->info().index;

			std::sort(edgeVIndex, edgeVIndex + 2);

			IntPair intPair(edgeVIndex[0], edgeVIndex[1]);
			auto edgeIter = edgeMap.find(intPair);

			if (edgeIter != edgeMap.end())
				edge[i] = edgeIter->second;
			else
			{
				Edge newEdge(ch, tetraEdgeVertexIndices[i][0], tetraEdgeVertexIndices[i][1]);

				data.dtEdgeList.push_back(newEdge);
				edge[i] = &data.dtEdgeList.back();

				if(data.dtEdgeList.capacity() != initialEdgesCapacity)
				{
					std::cerr << "Edge Datastructure was probably reallocated, pointers are no longer valid!!" << std::endl;
					assert(false);
				}

				edgeMap.insert(std::make_pair(intPair, edge[i]));
			}
		}

		// only set edges into newly created triangles
		for (i = 0; i < 4; i++)
			if (triangleCreated[i])
				for (j = 0; j < 3; j++)
					currTetra->triangle(i)->setEdge(j, edge[tetraTriEdgeIndices[i][j]]);

	}

}
