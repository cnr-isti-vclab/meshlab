#ifndef __UNIFORMVAR_H__
#define __UNIFORMVAR_H__

#include <QList>
#include <QString>
#include <QStringList>
#include <QDomElement>
#include <qDebug>

#include "GlState.h"


class UniformVar
{
	public:
		enum UniformType {
			INT,								// * type integer
			FLOAT,								// * type float
			BOOL,								// * type bool
			VEC2, VEC3, VEC4,					// * vector of float
			IVEC2, IVEC3, IVEC4,				// * vector of int
			BVEC2, BVEC3, BVEC4,				// * vector of bool
			MAT2, MAT3, MAT4,					// * 2x2 3x3 4x4 float
			SAMPLER1D, SAMPLER2D, SAMPLER3D,	// * 1D, 2D and 3D texture
			SAMPLERCUBE,						// * Cube Map texture
			SAMPLER1DSHADOW, SAMPLER2DSHADOW,	// * 1D and 2D depth-component texture
			OTHER
		};


		enum UniformType type;

		QString name;
		QString typeString;

		union {
			int ivalue;
			float fvalue;
			bool bvalue;
			float vec2[2], vec3[3], vec4[4];
			int ivec2[2], ivec3[3], ivec4[4];
			bool bvec2[2], bvec3[3], bvec4[4];
			float mat2[2][2], mat3[3][3], mat4[4][4];
		};

		QString representerTagName;

		QString textureName;
		QString textureFilename;
		QList<GlState> textureGLStates;

		union { int imin; float fmin; };
		union { int imax; float fmax; };
		union { int irealmin; float frealmin; };
		union { int irealmax; float frealmax; };

		bool minSet;
		bool maxSet;
		bool realminSet;
		bool realmaxSet;


		void setMin( int min ) { if( (realminSet && min <= irealmin) || !realminSet) { imin = min; minSet = true; } }
		void setMin( float min ) { if( (realminSet && min <= frealmin) || !realminSet) { fmin = min; minSet = true;} }
		void setMax( int max ) { if( (realmaxSet && max >= irealmax) || !realmaxSet ) { imax = max; maxSet = true; } }
		void setMax( float max ) { if( (realmaxSet && max >= frealmax) || !realmaxSet ) { fmax = max; maxSet = true; } }
		void testRealMin( int min ) { if( !realminSet || min < irealmin ) { realminSet = true; irealmin = min; if( minSet && imin > irealmin ) minSet = false; } }
		void testRealMin( float min ) { if( !realminSet || min < frealmin ) { realminSet = true; frealmin = min; if( minSet && fmin > frealmin ) minSet = false; } }
		void testRealMax( int max ) { if( !realmaxSet || max > irealmax ) { realmaxSet = true; irealmax = max; if( maxSet && imax < irealmax ) maxSet = false; } }
		void testRealMax( float max ) { if( !realmaxSet || max > irealmax ) { realmaxSet = true; frealmax = max; if( maxSet && fmax < frealmax ) maxSet = false; } }

		bool valid;

		UniformVar() { valid = false; }
		UniformVar( QString & _name, QString & _typeString, enum UniformType _type );
		virtual ~UniformVar(){}

		bool isNull() { return !valid; }


		// * we search the xml tag element that has the default value of a uniform
		// * variable. It can happend a multiple declaration, so first we search
		// * in the same RmOpenGLEffect (effectElement), and then in the global document root
		bool getValueFromXmlDocument( QDomElement & root, bool echoNotFound = true );
		bool getValueFromXmlDocument( QDomElement & root, QDomElement & effectElement ) {
			if( getValueFromXmlDocument( effectElement, false ) ) return true;
			return getValueFromXmlDocument( root );
		}

		bool getUniformKnownButUnimplementedTag( QDomElement & root, QString tag, QString tagname);
		bool getUniformBooleanVectorFromTag( QDomElement & root, QString tag, int vecsize, bool * vec, bool * found = NULL);
		bool getUniformNumberVectorFromTag( QDomElement & root, QString tag, int vecsize, void * vec, bool intOrFloat, bool * found = NULL );
		bool getUniformNumberVectorFromTag( QDomElement & root, QString tag, int vecsize, int * vec, bool * found = NULL ) {
			return getUniformNumberVectorFromTag( root, tag, vecsize, (void*)vec, true, found );
		}
		bool getUniformNumberVectorFromTag( QDomElement & root, QString tag, int vecsize, float * vec, bool * found = NULL ) {
			return getUniformNumberVectorFromTag( root, tag, vecsize, (void*)vec, false, found );
		}
		bool getUniformTextureFromTag( QDomElement & root, QString tag, bool * found = NULL );
			

		static enum UniformType getTypeFromString( QString & type );
		static QString getXmlTagRomUniformType( enum UniformType type );
		static QString getStringFromUniformType( enum UniformType type );

		static bool getUniformNumberVectorFromXmlTag( QDomElement & el, int values, void * farr, bool intOrFloat, UniformVar * ptr );
		static bool getUniformBooleanVectorFromXmlTag( QDomElement & el, int values, bool * barr);


		void VarDump(int indent = 0, bool extendedVarDump = false);
};

#endif

