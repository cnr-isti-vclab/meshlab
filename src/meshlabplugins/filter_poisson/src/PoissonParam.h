class PoissonParam
{
public:
	PoissonParam()
	{
		Depth=8;
		SolverDivide=8;
		IsoDivide=8;
		Refine=3;
		SamplesPerNode=1.0f;		
		Scale=1.25f;
		KernelDepth = -1;
	}
	bool Verbose,NoResetSamples,NoClipTree,Confidence;
	int Depth;
	int SolverDivide;
	int IsoDivide;
	int Refine;
	int KernelDepth;
	float SamplesPerNode;
	float Scale;
//	char* paramNames[]=
//	{
//		"in","depth","out","refine","noResetSamples","noClipTree",
//		"binary","solverDivide","isoDivide","scale","verbose",
//		"kernelDepth","samplesPerNode","confidence"
//	};
//	cmdLineReadable* params[]=
//	{
//		&In,&Depth,&Out,&Refine,&NoResetSamples,&NoClipTree,
//		&Binary,&SolverDivide,&IsoDivide,&Scale,&Verbose,
//		&KernelDepth,&SamplesPerNode,&Confidence
//	};
}
;
