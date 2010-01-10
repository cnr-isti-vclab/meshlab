/* MOSAIC default surface shader for Blender integration */

//Simulate SSS through Cl with blurred shadows
color sssdiffuse(point P;normal N;float SSSfact,Frfact,Bkfact,Rfact,Gfact,Bfact;color SSScol,SSStex)
{
	color C = 0;
	color SSS = 0;
	illuminance(P,N,PI)
	{
		lightsource("SSSDepth",SSS); //Get shadow info
		float f = normalize(L).N; //Figure front direction
		float b = normalize(-L).N; //Figure back direction
			C += SSS*SSScol*smoothstep(-1,1,f)*Frfact*SSSfact; //Lightside sss
			C += SSS*SSScol*SSStex*color(Rfact,Gfact,Bfact)*smoothstep(-1,1,b)*Bkfact*SSSfact; //Darkside sss
	}
	return C;
}

//Adjust texture st with xy offset and size
void stAdjust(output float sAdj,tAdj;float s,t,texOfSz[4])
{
	sAdj = (s*texOfSz[2]-(texOfSz[2]-1)/2)+texOfSz[0];
	tAdj = (t*texOfSz[3]-(texOfSz[3]-1)/2)-texOfSz[1];
}

surface
MOSAICsurface(
	float Amb=1; //Standard Blender material controls
	float Ref=0.8;
	float Spec=0.5;
	float RayMir=1;
	float Hard=0.1;
	float Alpha=1;
	float Tralu=0;
	float Emit=0;
	float ColMix=1;
	float CmirMix=1;
	float CspMix=1;
	float TransLuMix=1;
	float SSSColMix=1;
	float IOR=1.5;
	float MirFresnel=1;
	float TranspFresnel=1;
	float OccMaxdist=1e38; //User non Blender controls
	float OccSamples=8;
	float OccEnergy=1;
	float OccMapBlur=0.01;
	float OccBias=0.3;
	float EnvSamples=8;
	float EnvBlur=0;
	output varying float ShadowAlpha=1.0;
	output varying float isSSS=1;
	output varying float SSSBlur=0.05;
	output varying float SSSBias=0.1;
	output varying float SSSSamples=64;
	float SSSFact=1.0;
	float SSSTexBlur=0.04;
	float SSSFront=0.2;
	float SSSBack=0.8;
	float SSSR=1;
	float SSSG=1;
	float SSSB=1;
	float isRayMirror=0; //Material state toggles
	float isRayTransp=0;
	float isShadeless=0;
	float isOcclusion=0;
	color SpecCol=1; //Standard Blender material colors
	color MirCol=1;
	color OccCol=1;
	color AmbCol=1;
	color SSSCol=1;
	string ColMap=""; //Standard Blender texture channels
	string AlphaMap="";
	string CspMap="";
	string RayMirMap="";
	string CmirMap="";
	string RefMap="";
	string SpecMap="";
	string HardMap="";
	string AmbMap="";
	string EmitMap="";
	string TransLuMap="";
	string OccMap=""; //User non Blender materials
	string IBLMap="";
	string EnvMap="";
	varying float ColOfSz[4]={0,0,1,1}; //Texture mapping arrays as offset x/y scale x/y
	varying float AlphaOfSz[4]={0,0,1,1};
	varying float CspOfSz[4]={0,0,1,1};
	varying float RayMirOfSz[4]={0,0,1,1};
	varying float CmirOfSz[4]={0,0,1,1};
	varying float RefOfSz[4]={0,0,1,1};
	varying float SpecOfSz[4]={0,0,1,1};
	varying float HardOfSz[4]={0,0,1,1};
	varying float AmbOfSz[4]={0,0,1,1};
	varying float EmitOfSz[4]={0,0,1,1};
	varying float TransLuOfSz[4]={0,0,1,1};)
{
	vector In = normalize(I);
	normal Nf = normalize(faceforward(N,In));
	float sAdj,tAdj;
	if (ColMap != "") //Are we using a color texture map?
	{
		stAdjust(sAdj,tAdj,s,t,ColOfSz);
		Ci = color mix(Cs,color texture(ColMap,sAdj,tAdj),ColMix);
	}
	else Ci = Cs;
	if (isShadeless == 0) //Only process the rest if we are not shadeless
	{
		color Uad = color "rgb" (0,0,0);
		if (EmitMap != "" || Emit > 0) //Are we using emit controls or map?
		{
			if (EmitMap != "") //Are we using a emit texture map?
			{
				stAdjust(sAdj,tAdj,s,t,EmitOfSz);
				Uad += Emit*Ci+float texture(EmitMap,sAdj,tAdj);
			}
			else Uad += Emit*Ci;
		}
		if (AmbMap != "" || Amb > 0) //Are we using ambient controls or map?
		{
			float hits = 0;
			float Uam = 1;
			color Ua = ambient();
			if (isOcclusion > 0 || OccMap != "") //Is ambience supposed to be occlusion or IBL?
			{
#if defined(DELIGHT) //Is this 3Delight style IBL?
				if (IBLMap != "") Ua += (1-indirectdiffuse(P,Nf,OccSamples,"bias",OccBias,"maxdist",OccMaxdist,"environmentmap",IBLMap))*OccEnergy;
#endif //Is this other raytrace renderers style IBL?
#if (defined(PRMAN)||defined(AIR)||defined(PIXIE))
				if (IBLMap != "") Ua += indirectdiffuse(P,Nf,OccSamples,"bias",OccBias,"maxdist",OccMaxdist,"environmentmap",IBLMap)*OccEnergy;
#endif
#if (defined(PRMAN)||defined(DELIGHT)||defined(PIXIE)) //Is this other raytrace renderers style occlusion?
				else Ua += (1-occlusion(P,Nf,OccSamples,"bias",OccBias,"maxdist",OccMaxdist))*OccCol*OccEnergy;
#endif
#if defined(AIR) //Is this Air style occlusion?
				else
				{
					if (OccMap == "") OccMap = "raytrace";
					normal Nunoccl = 0;
					Ua += (1-occlusion(OccMap,P,Nf,radians(90),Nunoccl,"samples",OccSamples,"bias",OccBias,"blur",OccMapBlur))*OccCol*OccEnergy;
				}
#endif
#if defined(AQSIS) //Is this Aqsis sytle depth map occlusion?
				if (OccMap != "") Ua += (1-occlusion(OccMap,P,Nf,0,"samples",OccSamples,"bias",OccBias,"blur",OccMapBlur))*OccCol*OccEnergy;
#endif
			}
			if (AmbMap != "") //Are we using a ambient texture map?
			{
				stAdjust(sAdj,tAdj,s,t,AmbOfSz);
				Uam = float texture(AmbMap,sAdj,tAdj);
			}
			Uad += (Ua+AmbCol)*Uam*Amb;
		}
		if (RefMap != "" || Ref > 0) //Are we using ref (diffuse) controls or map?
		{
			color Udif = diffuse(Nf);
			if (Tralu > 0) Udif += diffuse(-Nf)*Tralu;
			if (RefMap != "") //Are we using a ref texture map?
			{
				stAdjust(sAdj,tAdj,s,t,RefOfSz);
				Uad += mix(float texture(RefMap,sAdj,tAdj)*Udif,Udif,Ref);
			}
			else Uad += Ref*Udif;
		}
		color SSSTex = 1;
		if (TransLuMap != "" && TransLuMix > 0)
		{
			stAdjust(sAdj,tAdj,s,t,TransLuOfSz);
			SSSTex = mix(SSSTex,color texture(TransLuMap,sAdj,tAdj,"blur",SSSTexBlur),TransLuMix);
		}
		if (isSSS > 0) //Are we using sub surface scattering?
		{
			Uad += sssdiffuse(P,Nf,SSSFact,SSSFront,SSSBack,SSSR,SSSG,SSSB,mix(Ci,SSSCol,SSSColMix),SSSTex);
		}
		Ci *= Uad;
		//Are we using raytrace or environment controls for reflection/refraction?
		if (EnvMap != "" || isRayMirror > 0 || isRayTransp > 0)
		{
			color Umc = MirCol;
			float Um = RayMir;
			if (RayMirMap != "") //Are we using a mirror texture map?
			{
				stAdjust(sAdj,tAdj,s,t,RayMirOfSz);
				Um = mix(RayMir,float texture(RayMirMap,sAdj,tAdj),1-RayMir);
			}
			if (CmirMap != "") //Are we using a mirror color texture map?
			{
				stAdjust(sAdj,tAdj,s,t,CmirOfSz);
				Umc = mix(MirCol,color texture(CmirMap,sAdj,tAdj),CmirMix);
			}
			if (EnvMap != "" || isRayMirror > 0 || isRayTransp > 0) //Are we using reflection/refraction?
			{
				vector R,T;
				float Kmr,Kmt,result;
				fresnel(In,Nf,(In.N < 0 ? 1/IOR:IOR),Kmr,Kmt,R,T);
				if (TranspFresnel > 0 && IOR > 1.0) Kmt = clamp(Kmr/TranspFresnel*12,0,1);
				else Kmt = 1;
				if (MirFresnel > 0 && IOR > 1.0) Kmr = clamp(Kmr/(MirFresnel/12),0,1);
				else Kmr = 1;
				if (EnvMap != "")
				{
					uniform string texType = "";
					textureinfo(EnvMap,"type",texType);
					if (texType == "texture" || texType == "Plain Texture")
					{
						point Pndc = transform("NDC",P);
						float tx = xcomp(Pndc);
						float ty = ycomp(Pndc);
						if (Kmr > 0) Ci = Ci*(1-Um*Kmr)+color texture(EnvMap,tx,ty,"blur",EnvBlur)*Umc*Um*Kmr;
					}
					else
					{
						T = ntransform("world",normal(T))*(1,1,-1);
						R = ntransform("world",normal(R))*(1,1,-1);
						if (Kmt > 0) Ci = Ci*Kmt+color environment(EnvMap,T,"samples",EnvSamples,"blur",EnvBlur)*(1-Kmt);
						if (Kmr > 0) Ci = Ci*(1-Um*Kmr)+color environment(EnvMap,R,"samples",EnvSamples,"blur",EnvBlur)*Umc*Um*Kmr;
					}
				}
				else
				{
					if (isRayTransp > 0 && Kmt > 0) Ci = Ci*Kmt+trace(P,T)*(1-Kmt);
					if (isRayMirror > 0 && Kmr > 0) Ci = Ci*(1-Um*Kmr)+trace(P,R)*Umc*Um*Kmr;
				}
			}
		}
		if (SpecMap != "" || Spec > 0) //Are we using specular controls or maps?
		{
			color Usc = SpecCol;
			float Ur = Hard;
			if (HardMap != "") //Are we using a hard texture map?
			{
				stAdjust(sAdj,tAdj,s,t,HardOfSz);
				Ur += Hard*float texture(HardMap,sAdj,tAdj);
			}
			color Us = specular(Nf,-In,PI/Ur);
			if (CspMap != "") //Are we using a specular color texture map?
			{
				stAdjust(sAdj,tAdj,s,t,CspOfSz);
				Usc = mix(SpecCol,color texture(CspMap,sAdj,tAdj),CspMix);
			}
			if (SpecMap != "") //Are we using a specular map?
			{
				stAdjust(sAdj,tAdj,s,t,SpecOfSz);
				Ci += Usc*(Spec*float texture(SpecMap,sAdj,tAdj)*Us);
			}
			else Ci += Usc*Spec*Us;
		}
	}
	if (AlphaMap != "") //Are we using a alpha texture map?
	{
		stAdjust(sAdj,tAdj,s,t,AlphaOfSz);
		Oi = mix(float texture(AlphaMap,sAdj,tAdj),Alpha,Alpha);
	}
	else Oi = Os;
	Ci *= Oi;
}

