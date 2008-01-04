#pragma once
namespace vcg
{

class CICubeMap 
{
public:
	GLuint ti;

	GLuint oti[6];
	
	bool Load(const char *basename);
	bool LoadExt(const char *basename);
	bool LoadOld(const char *basename);
	void DrawEnvCubeExt(Matrix44f &tr);
	void DrawEnvCubeOld(Matrix44f &tr);
	void DrawEnvCube(Matrix44f &tr);
	bool GetName(int i, QString basename, QString &filename);
  bool IsValid() {
		return !( (ti == 0) && (oti[0]==0) ); 
	}
  void SetInvalid() { ti=0; oti[0]=0; }
	CICubeMap() {radius=10; ti=0; for(int i=0;i<6;++i) oti[i]=0;}
	float radius;

};

} // end namespace