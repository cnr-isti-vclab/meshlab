/* MOSAIC default displacement shader for Blender integration */

//Adjust texture st with xy offset and size
void stAdjust(output float sAdj,tAdj;float s,t,texOfSz[4])
{
	sAdj = (s*texOfSz[2]-(texOfSz[2]-1)/2)+texOfSz[0];
	tAdj = (t*texOfSz[3]-(texOfSz[3]-1)/2)-texOfSz[1];
}

displacement
MOSAICdisplace(
	float Disp=1; //Standard Blender material controls
	float Nor=1;
	float Mid=0.5;
	string DispMap=""; //Standard Blender texture channels
	string NorMap="";
	varying float DispOfSz[4]={0,0,1,1}; //Texture mapping arrays as offset x/y scale x/y
	varying float NorOfSz[4]={0,0,1,1};)
{
	float sAdj,tAdj;
	normal Un = N;
	if (DispMap != "") //Are we using a displacement texture map?
	{
		stAdjust(sAdj,tAdj,s,t,DispOfSz);
		float amp = Disp*(float texture(DispMap,sAdj,tAdj)-Mid);
		P += amp*normalize(N);
		Un = calculatenormal(P);
	}
	if (NorMap != "") //Are we using a normal texture map?
	{
		normal Nref = normalize(Un);
		vector dPds = normalize(Deriv(P,s));
		vector dPdt = normalize(Deriv(P,t));
		stAdjust(sAdj,tAdj,s,t,NorOfSz);
		color Ncolor = color texture(NorMap,sAdj,tAdj);
		vector Nvector = (vector(Ncolor)*2)-vector(1.0,1.0,1.0);
		vector Ntransformed = vector(comp(Nvector,0)*comp(dPds,0)+comp(Nvector,1)*comp(dPdt,0)+comp(Nvector,2)*comp(Nref,0),
						comp(Nvector,0)*comp(dPds,1)+comp(Nvector,1)*comp(dPdt,1)+comp(Nvector,2)*comp(Nref,1),
						comp(Nvector,0)*comp(dPds,2)+comp(Nvector,1)*comp(dPdt,2)+comp(Nvector,2)*comp(Nref,2));
		N = mix(Un,normal normalize(Ntransformed),Nor/2);
	}
	else N = Un;
}

