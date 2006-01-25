#ifndef SHDRSTRUCTS
#define SHDRSTRUCTS

#include <map>
#include <QString>

struct UniformVariable {
	short type;
	short widget;
	float min;
	float max;
	float step;
	int location;
	union {
		int ival;
		float fval;
		float val2[2];
		float val3[3];
		float val4[4];
	};
};

struct ShaderInfo {
	QString vpFile;
	QString fpFile;
	std::map<QString, UniformVariable> uniformVars;
	int shaderProg;
};

enum {
	SINGLE_INT = 1,
	SINGLE_FLOAT = 5,
	ARRAY_2_FLOAT = 6,
	ARRAY_3_FLOAT = 7,
	ARRAY_4_FLOAT = 8
};

enum {
	WIDGET_NONE = 0,
	WIDGET_COLOR = 1,
	WIDGET_SLIDER = 2
};

#endif