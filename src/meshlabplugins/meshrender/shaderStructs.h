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
		int ival[4];
		float fval[4];
	};
	/*union {
	int ival;
	float fval;
	float val2[2];
	float val3[3];
	float val4[4];
	};*/
};

struct ShaderInfo {
	QString vpFile;
	QString fpFile;
	std::map<QString, UniformVariable> uniformVars;
	std::map<int, QString> glStatus;
	int shaderProg;
};

enum {
	SINGLE_INT = 1,
	SINGLE_FLOAT = 5,
	ARRAY_2_FLOAT = 6,
	ARRAY_3_FLOAT = 7,
	ARRAY_4_FLOAT = 8
};

static int getVarsNumber(int i) {
	switch (i) {
		case SINGLE_INT: return 1; break; 
		case SINGLE_FLOAT: return 1; break;
		case ARRAY_2_FLOAT: return 2; break;
		case ARRAY_3_FLOAT: return 3; break;
		case ARRAY_4_FLOAT: return 4; break;
		default: return 0; break;
	}
}


enum {
	WIDGET_NONE = 0,
	WIDGET_COLOR = 1,
	WIDGET_SLIDER = 2
};

enum {
	SHADE = 0,
	ALPHA_TEST,
	ALPHA_FUNC,
	ALPHA_CLAMP,
	BLENDING,
	BLEND_FUNC_SRC,
	BLEND_FUNC_DST,
	BLEND_EQUATION,
	DEPTH_TEST,
	DEPTH_FUNC,
	CLAMP_NEAR,
	CLAMP_FAR,
	CLEAR_COLOR_R,
	CLEAR_COLOR_G,
	CLEAR_COLOR_B,
	CLEAR_COLOR_A
};

#endif