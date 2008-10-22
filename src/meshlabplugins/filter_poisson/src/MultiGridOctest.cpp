/*
Copyright (c) 2006, Michael Kazhdan and Matthew Bolitho
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "MarchingCubes.h"
#include "Octree.h"
#include "SparseMatrix.h"
#include "FunctionData.h"
#include "PPolynomial.h"
#include "PoissonParam.h"

#include <wrap/callback.h>

#define SCALE 1.25

#include <stdarg.h>

#include "MultiGridOctreeData.h"

void ShowUsage(char* ex)
{
	printf("Usage: %s\n",ex);
	printf("\t--in  <input points>\n");

	printf("\t--out <ouput triangle mesh>\n");

	printf("\t[--depth <maximum reconstruction depth>]\n");
	printf("\t\t Running at depth d corresponds to solving on a 2^d x 2^d x 2^d\n");
	printf("\t\t voxel grid.\n");

	printf("\t[--scale <scale factor>]\n");
	printf("\t\t Specifies the factor of the bounding cube that the input\n");
	printf("\t\t samples should fit into.\n");

	printf("\t[--binary]\n");
	printf("\t\t If this flag is enabled, the point set is read in in\n");
	printf("\t\t binary format.\n");

	printf("\t[--solverDivide <subdivision depth>]\n");
	printf("\t\t The depth at which a block Gauss-Seidel solver is used\n");
	printf("\t\t to solve the Laplacian.\n");

	printf("\t[--samplesPerNode <minimum number of samples per node>\n");
	printf("\t\t This parameter specifies the minimum number of points that\n");
	printf("\t\t should fall within an octree node.\n");

	printf("\t[--confidence]\n");
	printf("\t\t If this flag is enabled, the size of a sample's normals is\n");
	printf("\t\t used as a confidence value, affecting the sample's\n");
	printf("\t\t constribution to the reconstruction process.\n");

	printf("\t[--verbose]\n");
}
template<int Degree>
int Execute(PoissonParam &Par, std::vector<Point3D<Real> > Pts, std::vector<Point3D<Real> > Nor, 	CoredVectorMeshData &mesh, Point3D<Real> &newCenter, Real &newScale, vcg::CallBackPos *cb)
{
		int i;
//	int paramNum=sizeof(paramNames)/sizeof(char*);
	//int commentNum=0;
	//char **comments;

	//comments=new char*[paramNum+7];
	//for(i=0;i<=paramNum+7;i++){comments[i]=new char[1024];}

//	const char* Rev = "Rev: V2 ";
//	const char* Date = "Date: 2006-11-09 (Thur, 09 Nov 2006) ";

	// cmdLineParse(argc-1,&argv[1],paramNames,paramNum,params,0);
  
	double t;
	Point3D<float> center;
	Real scale=1.0;
	Real isoValue=0;
	Octree<Degree> tree;
	PPolynomial<Degree> ReconstructionFunction=PPolynomial<Degree>::GaussianApproximation();

	center.coords[0]=center.coords[1]=center.coords[2]=0;
	
	TreeOctNode::SetAllocator(MEMORY_ALLOCATOR_BLOCK_SIZE);

	int kernelDepth=Par.Depth-2;
	if(Par.KernelDepth>=0){kernelDepth=Par.KernelDepth;}

	tree.setFunctionData(ReconstructionFunction,Par.Depth,0,Real(1.0)/(1<<Par.Depth));
//	DumpOutput("Memory Usage: %.3f MB\n",float(MemoryInfo::Usage())/(1<<20));
	if(kernelDepth>Par.Depth){
		fprintf(stderr,"KernelDepth can't be greater than Depth: %d <= %d\n",kernelDepth,Par.Depth);
		return EXIT_FAILURE;
	}


#if 1
	tree.setTree(Pts,Nor,Par.Depth,kernelDepth,Real(Par.SamplesPerNode),Par.Scale,center,scale,!Par.NoResetSamples,Par.Confidence);
#else
if(Confidence.set){
	tree.setTree(Pts,Nor,Depth.value,kernelDepth,Real(SamplesPerNode.value),Scale.value,center,scale,!NoResetSamples.set,0,1);
}
else{
	tree.setTree(Pts,Nor,Depth.value,kernelDepth,Real(SamplesPerNode.value),Scale.value,center,scale,!NoResetSamples.set,0,0);
}
#endif
	printf("Leaves/Nodes: %d/%d\n",tree.tree.leaves(),tree.tree.nodes());
	printf("   Tree Size: %.3f MB\n",float(sizeof(TreeOctNode)*tree.tree.nodes())/(1<<20));

	if(!Par.NoClipTree){
		tree.ClipTree();
		printf("Leaves/Nodes: %d/%d\n",tree.tree.leaves(),tree.tree.nodes());
	}

	tree.finalize1(Par.Refine);
	printf("Leaves/Nodes: %d/%d\n",tree.tree.leaves(),tree.tree.nodes());

	tree.maxMemoryUsage=0;
	tree.SetLaplacianWeights();

	tree.finalize2(Par.Refine);

	tree.maxMemoryUsage=0;
	tree.LaplacianMatrixIteration(Par.SolverDivide);

	tree.maxMemoryUsage=0;
	isoValue=tree.GetIsoValue();
	printf("IsoValue is %f \n",isoValue);
  isoValue = isoValue * Par.Offset; 
	printf("IsoValue is %f \n",isoValue);

	if(Par.IsoDivide){tree.GetMCIsoTriangles(isoValue,Par.IsoDivide,&mesh);}
	else{tree.GetMCIsoTriangles(isoValue,&mesh);}
//	PlyWriteTriangles(Out.value,&mesh,PLY_BINARY_NATIVE,center,scale,comments,commentNum);
newCenter=center;
newScale=scale;
	return 1;
}

int Execute2(PoissonParam &Par, std::vector<Point3D<Real> > Pts, std::vector<Point3D<Real> > Nor, 	CoredVectorMeshData &mesh, Point3D<Real> &newCenter, Real &newScale, vcg::CallBackPos *cb )
{
	return Execute<2>(Par,Pts,Nor,mesh,newCenter,newScale,cb);
}
