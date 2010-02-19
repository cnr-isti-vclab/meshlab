/* MOSAIC default light shader for Blender integration */

//Adjust texture st with xy offset and size
void stAdjust(output float sAdj,tAdj;float s,t,texOfSz[4])
{
	sAdj = (s*texOfSz[2]-(texOfSz[2]-1)/2)+texOfSz[0];
	tAdj = (t*texOfSz[3]-(texOfSz[3]-1)/2)-texOfSz[1];
}

//Returns different attenuations of light depth
float Attenuation(float Ldepth,LinAtten,SqrAtten,QuadAtten,Distance,SphereAtten;)
{
	float attenuation = 1;
	attenuation = Distance/(LinAtten*Ldepth+SqrAtten*Ldepth*2+QuadAtten*Ldepth*Ldepth+Distance);
	if (SphereAtten > 0) attenuation *= 1-Ldepth/Distance;
	return clamp(attenuation,0,1);
}

light MOSAIClight(
	float LightType=0; //Standard Blender lamp controls
	float Energy=1;
	float Dist=1;
	float SpotSi=radians(30);
	float SpotBl=radians(5);
	output float Quad1=1;
	output float Quad2=0;
	output float Quad3=0;
	point from=point "shader" (0,0,0);
	point to=point "shader" (0,0,1);
	point up=point "eye" (0,1,0);
	color LightColor= color "rgb" (1,1,1);
	float isRaytrace=0; //Lamp state toggles
	float isSphere=0;
	float Falloff=4;
	varying float ProjOfSz[4]={0,0,1,1}; //Texture mapping array as offset x/y scale x/y
	output float __nondiffuse=0; //Standard built in message passing parameters
	output float __nonspecular=0;
	output float __foglight=1;
	output float Samples=8; //User non Blender controls
	output float RaySamples=8; //User non Blender controls
	output varying float Blur=0.01;
	output varying float RayBlur=0.01;
	float Bias=0.1;
	float CausticAlpha=1.0;
	color CausticColor=1.0;
	float CausticBlur=0.05;
	float CausticSamples=64;
	float CausticThreshold=0.8;
	float CausticNoise=2.5;
	float noisefreq=0;
	float noisepow=1;
	uniform float SSSWidth=1;
	string projectionmap=""; //Projector texture
	string environmentmap=""; //Environment texture
	output string shadowname=""; //User shadow map channels
	string shadowname_px="";
	string shadowname_nx="";
	string shadowname_py="";
	string shadowname_ny="";
	string shadowname_pz="";
	string shadowname_nz="";
	string causticname=""; //User caustic map channels
	string causticname_px="";
	string causticname_nx="";
	string causticname_py="";
	string causticname_ny="";
	string causticname_pz="";
	string causticname_nz="";
	output varying color SSSDepth=0;)
{
	uniform vector Z = normalize(to-from);
	color vis = 1;
	color envcol = LightColor;
	color shad,caus = 0;
	string shadname,causname = "";
	float sAdj,tAdj,ShadowAlpha,isSSS,SSSBias,SSSBlur,SSSSamples;
	if (Falloff == 0) //Setup for constant attenuation
	{
		Quad1 = 0;
		Quad2 = 0;
		Quad3 = 0;
	}
	else if (Falloff == 1) //Setup for inverse linear attenuation
	{
		Quad1 = 1;
		Quad2 = 0;
		Quad3 = 0;
	}
	else if (Falloff == 2) //Setup for inverse square attenuation
	{
		Quad1 = 0;
		Quad2 = 0;
		Quad3 = 1;
	}
	if (LightType == 4) //If an area light manually set attenuation and spot size and blend
	{
		Quad1 = 1;
		Quad2 = 1;
		Quad3 = 1;
		RaySamples = 2;
	}
	if (isRaytrace > 0) //If raytracing then setup use "raytrace" map name method
	{
		shadowname = "raytrace";
		Samples = RaySamples;
		Blur = RayBlur;
	}
	if (LightType == 1) //Are we a distant light?
	{
		solar (to-from,0.0)
		{
			surface("ShadowAlpha",ShadowAlpha);
			surface("isSSS",isSSS);
			surface("SSSBias",SSSBias);
			surface("SSSSamples",SSSSamples);
			surface("SSSBlur",SSSBlur);
			if (shadowname != "") shad = shadow(shadowname,Ps,"samples",Samples,"blur",Blur,"bias",Bias)*ShadowAlpha;
			if (causticname != "") caus = smoothstep(CausticThreshold,1,shadow(causticname,Ps,"samples",CausticSamples,"blur",CausticBlur,"bias",Bias))*CausticColor*CausticAlpha;
			if (CausticNoise != 0)
			{
				point PL = transform("shader",Ps);
				caus *= CausticNoise*pow(noise(PL*noisefreq),noisepow);
			}
			Cl = LightColor*Energy;
			if (isSSS > 0 && shadowname != "") SSSDepth = Cl*vis*(1-shadow(shadowname,Ps,"samples",SSSSamples,"width",SSSWidth,"blur",SSSBlur,"bias",SSSBias));
			Cl *= vis*(1-shad)+caus;
		}
	}
	else if (LightType == 2 || LightType == 4) //Are we a spot light or single light in a array of area lights?
	{
		uniform vector Y = normalize(Z^vector up);
		uniform vector X = normalize(Y^Z);
		uniform float spread = 1/tan(radians(SpotSi)*57);
		illuminate (from,Z,SpotSi)
		{
			surface("ShadowAlpha",ShadowAlpha);
			surface("isSSS",isSSS);
			surface("SSSBias",SSSBias);
			surface("SSSSamples",SSSSamples);
			surface("SSSBlur",SSSBlur);
			float sloc = spread*L.X/L.Z*0.5+0.5;
			float tloc = spread*L.Y/L.Z*0.5+0.5;
			vis *= smoothstep(cos(SpotSi),cos(SpotSi-SpotBl),(L.Z)/length(L));
			if (projectionmap != "") //Are we using a projection texture map?
			{
				stAdjust(sAdj,tAdj,s,t,ProjOfSz);
				vis *= color texture(projectionmap,sAdj,tAdj);
			}
			if (causticname != "") caus = smoothstep(CausticThreshold,1,shadow(causticname,Ps,"samples",CausticSamples,"blur",CausticBlur,"bias",Bias))*CausticColor*CausticAlpha;
			if (CausticNoise != 0)
			{
				point PL = transform("shader",Ps);
				caus *= CausticNoise*pow(noise(PL*noisefreq),noisepow);
			}
			if (environmentmap != "") envcol = environment(environmentmap,-L,"blur",0.5)*LightColor;
			if (shadowname != "") shad = shadow(shadowname,Ps,"samples",Samples,"blur",Blur,"bias",Bias)*ShadowAlpha;
			Cl = envcol*Attenuation(length(L),Quad1,Quad2,Quad3,Dist,isSphere)*Energy;
			if (isSSS > 0 && shadowname != "") SSSDepth = Cl*vis*(1-shadow(shadowname,Ps,"samples",SSSSamples,"width",SSSWidth,"blur",SSSBlur,"bias",SSSBias));
			Cl *= vis*(1-shad)+caus;
		}
	}
	else //Are we a point light?
	{
		illuminate (from)
		{
			surface("ShadowAlpha",ShadowAlpha);
			surface("isSSS",isSSS);
			surface("SSSBias",SSSBias);
			surface("SSSSamples",SSSSamples);
			surface("SSSBlur",SSSBlur);
			point Lworld = transform("world",L+point "world" (0,0,0))*point (1,1,-1);
			float ax = abs(xcomp(Lworld));
			float ay = abs(ycomp(Lworld));
			float az = abs(zcomp(Lworld));
			if (shadowname != "") shadname = shadowname;
			else if ((ax > ay) && (ax > az)) //Are we using X+- shadow maps?
			{
				if (xcomp(Lworld) > 0.0)
				{
					if (shadowname_px != "") shadname = shadowname_px;
					if (causticname_px != "") causname = causticname_px;
				}
				else
				{
					if (shadowname_nx != "") shadname = shadowname_nx;
					if (causticname_nx != "") causname = causticname_nx;
				}
			}
			else if ((ay > ax) && (ay > az)) //Are we using Y+- shadow maps?
			{
				if (ycomp(Lworld) > 0.0)
				{
					if (shadowname_py != "") shadname = shadowname_py;
					if (causticname_py != "") causname = causticname_py;
				}
				else
				{
					if (shadowname_ny != "") shadname = shadowname_ny;
					if (causticname_ny != "") causname = causticname_ny;
				}
			}
			else if ((az > ay) && (az > ax)) //Are we using Z+- shadow maps?
			{
				if (zcomp(Lworld) > 0.0)
				{
					if (shadowname_pz != "") shadname = shadowname_pz;
					if (causticname_pz != "") causname = causticname_pz;
				}
				else
				{
					if (shadowname_nz != "") shadname = shadowname_nz;
					if (causticname_nz != "") causname = causticname_nz;
				}
			}
			Cl = LightColor*Attenuation(length(L),Quad1,Quad2,Quad3,Dist,isSphere)*Energy;
			if (causname != "")
			{
				caus = smoothstep(CausticThreshold,1,shadow(causname,Ps,"samples",CausticSamples,"blur",CausticBlur,"bias",Bias))*CausticColor*CausticAlpha;
				if (CausticNoise != 0)
				{
					point PL = transform("shader",Ps);
					caus *= CausticNoise*pow(noise(PL*noisefreq),noisepow);
				}
			}
			if (shadname != "")
			{
				shad = shadow(shadname,Ps,"samples",Samples,"blur",Blur,"bias",Bias)*ShadowAlpha;
				if (isSSS > 0) SSSDepth = Cl*vis*(1-shadow(shadname,Ps,"samples",SSSSamples,"width",SSSWidth,"blur",SSSBlur,"bias",SSSBias));
			}
			Cl *= vis*(1-shad)+caus;
		}
	}
}

