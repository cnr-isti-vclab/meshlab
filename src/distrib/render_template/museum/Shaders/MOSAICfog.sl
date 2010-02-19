/* MOSAIC default atmosphere shader for Blender integration */
volume
MOSAICfog(
	float Sta=0; //Standard Blender world controls
	float Di=0;
	float Hi=0;
	float Misi=0;
	float isMist=0;
	float MistType=0;
	color MistCol=0;)
{
	if (isMist > 0 && Sta < Di) //Are we using mist?
	{
		float LI = length(I)-Sta;
		float DL = Di-Sta;
		if (MistType == 0) DL = DL/clamp(LI/DL,0,1);
		else if (MistType == 1) DL = DL;
		else if (MistType == 2) DL = (DL+LI)/2;
		float d = 1-clamp(LI/DL,0,1);
		if (Hi > 0) d = mix(d,1,clamp(zcomp(transform("world",point I))/Hi,0,1));
		d =  mix(d,0,Misi);
		Ci = mix(MistCol,Ci,d);
		Oi = mix(color(1,1,1),Oi,d);
	}
}

