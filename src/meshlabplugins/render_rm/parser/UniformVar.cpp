#include "UniformVar.h"
#include "RmXmlParser.h"



UniformVar::UniformVar( QString & _name, QString & _typeString, enum UniformType _type )
{
	name = _name; 
	typeString = _typeString; 
	type = _type; 
	valid = true;
	minSet = false;
	maxSet = false;
	realminSet = false;
	realmaxSet = false;
}



bool UniformVar::getUniformKnownButUnimplementedTag( QDomElement & root, QString tag, QString tagname )
{
	if( RmXmlParser::getDomElement(root, tag, tagname ).isNull() == false ) {
		representerTagName = tag;
		return true;
	}
	return false;
}

// * ************************** * //
// * 3 Common Methods for       * //
// * xml variable value mining  * //
// * [boolean][number][texture] * //
// * ************************** * //
bool UniformVar::getUniformBooleanVectorFromTag( QDomElement & root, QString tag, int vecsize, bool * vec, bool * found )
{
	QDomElement el = RmXmlParser::getDomElement( root, tag, name );
	if( !el.isNull() ) {
		if( found ) *found = true;
		representerTagName = tag;
		return UniformVar::getUniformBooleanVectorFromXmlTag( el, vecsize, vec );
	}
	if( found ) *found = false;
	return false;
}

bool UniformVar::getUniformNumberVectorFromTag( QDomElement & root, QString tag, int vecsize, void * vec, bool intOrFloat, bool * found )
{
	QDomElement el = RmXmlParser::getDomElement( root, tag, name );
	if( !el.isNull() ) {
		if( found ) *found = true;
		representerTagName = tag;
		return UniformVar::getUniformNumberVectorFromXmlTag( el, vecsize, vec, intOrFloat, this );
	}
	if( found ) *found = false;
	return false;
}

bool UniformVar::getUniformTextureFromTag( QDomElement & root, QString tag, bool * found )
{
	QDomElement el = RmXmlParser::getDomElement( root, tag, textureName );
	if( !el.isNull() ) {
		if( found ) *found = true;
		representerTagName = tag;
		textureFilename = el.attribute("FILE_NAME");
		return true;
	}
	if( found ) *found = false;
	return false;
}


bool UniformVar::getValueFromXmlDocument( QDomElement & root, bool echoNotFound )
{
	bool fnd;

	switch( type )
	{
		case BOOL:
		{
			bool ok = getUniformBooleanVectorFromTag( root, "RmBooleanVariable", 1, &bvalue, &fnd );
			if( fnd ) return ok;
			break;
		}
		case INT:
		{
			bool ok = getUniformNumberVectorFromTag( root, "RmIntegerVariable", 1, &ivalue, &fnd );
			if( fnd ) return ok;
			break;
		}

		case FLOAT:
		{
			bool ok = getUniformNumberVectorFromTag( root, "RmFloatVariable", 1, &fvalue, &fnd );
			if( fnd ) return ok;
			break;
		}

		case IVEC2: {
			bool ok = getUniformNumberVectorFromTag( root, "RmVectorVariable", 2, ivec2, &fnd );
			if( fnd ) return ok; break;
		}
		case IVEC3: {
			bool ok = getUniformNumberVectorFromTag( root, "RmVectorVariable", 3, ivec3, &fnd );
			if( fnd ) return ok; break;
		}
		case IVEC4: {
			bool ok = getUniformNumberVectorFromTag( root, "RmVectorVariable", 4, ivec4, &fnd );
			if( fnd ) return ok; break;
		}
		case BVEC2: {
			bool ok = getUniformBooleanVectorFromTag( root, "RmVectorVariable", 2, bvec2, &fnd );
			if( fnd ) return ok; break;
		}
		case BVEC3: {
			bool ok = getUniformBooleanVectorFromTag( root, "RmVectorVariable", 3, bvec3, &fnd );
			if( fnd ) return ok; break;
		}
		case BVEC4: {
			bool ok = getUniformBooleanVectorFromTag( root, "RmVectorVariable", 4, bvec4, &fnd );
			if( fnd ) return ok; break;
		}
		case VEC2: {
			bool ok = getUniformNumberVectorFromTag( root, "RmVectorVariable", 2, vec2, &fnd );
			if( fnd ) return ok; break;
		}
		case VEC3: {
			bool ok = getUniformNumberVectorFromTag( root, "RmVectorVariable", 3, vec3, &fnd );
			if( fnd ) return ok; break;
		}
		case VEC4: {
			// * a vec4 can be RmVectorVariable or RmColorVariable
			bool ok = getUniformNumberVectorFromTag( root, "RmVectorVariable", 4, vec4, &fnd );
			if( fnd ) return ok;
			ok = getUniformNumberVectorFromTag( root, "RmColorVariable", 4, vec4, &fnd );
			if( fnd ) return ok;
			break;
		}

		case MAT2:
		{
			bool ok = getUniformNumberVectorFromTag( root, "RmMatrixVariable", 4, (float*)mat2, &fnd );
			if( fnd ) return ok;
			break;
		}
		case MAT3:
		{
			bool ok = getUniformNumberVectorFromTag( root, "RmMatrixVariable", 8, (float*)mat3, &fnd );
			if( fnd ) return ok;
			break;
		}
		case MAT4:
		{
			bool ok = getUniformNumberVectorFromTag( root, "RmMatrixVariable", 16, (float*)mat4, &fnd );
			if( fnd ) return ok;
			break;
		}

		case SAMPLER2D:
		{
			// * sampler 2d can be Rm2DTextureVariable or not come from a texture file but 
			// * found in a RmRenderTarget xml tag
			bool ok = getUniformTextureFromTag( root, "Rm2DTextureVariable", &fnd );
			if( fnd ) return ok;
			if( getUniformKnownButUnimplementedTag( root, "RmRenderTarget", textureName ))
				return true;
			break;
		}

		case SAMPLER3D:
		{
			bool ok = getUniformTextureFromTag( root, "Rm3DTextureVariable", &fnd );
			if( fnd ) return ok;
			break;
		}
			

		case SAMPLERCUBE:
		{
			bool ok = getUniformTextureFromTag( root, "RmCubemapVariable", &fnd );
			if( fnd ) return ok;
			break;
		}

		default:
			qDebug() << "RmXmlParser for uniform variable" << name << "of type" << UniformVar::getStringFromUniformType(type);
			qDebug() << "I don't know how to read my default value.. please implement me !";
			return false;
	};

	// * any variable can be defined as RmDynamicVariable
	if( getUniformKnownButUnimplementedTag( root, "RmDynamicVariable", name ))
		return true;

	if( echoNotFound ) {
		qDebug() << "RmXmlParser for uniform variable" << name << "of type" << UniformVar::getStringFromUniformType(type);
		qDebug() << "Default value not found";
	}
	return false;
}



enum UniformVar::UniformType UniformVar::getTypeFromString( QString & type )
{
	QString typeList[] = {
		"int", "float", "bool",
		"vec2", "vec3", "vec4",
		"ivec2", "ivec3", "ivec4",
		"bvec2", "bvec3", "bvec4",
		"mat2", "mat3", "mat4",
		"sampler1D", "sampler2D", "sampler3D",
		"samplerCube",
		"sampler1Dshadow", "sampler2Dshadow"
	};

	enum UniformType enumList[] = {
		INT, FLOAT, BOOL,
		VEC2, VEC3, VEC4,
		IVEC2, IVEC3, IVEC4,
		BVEC2, BVEC3, BVEC4,
		MAT2, MAT3, MAT4,
		SAMPLER1D, SAMPLER2D, SAMPLER3D,
		SAMPLERCUBE,
		SAMPLER1DSHADOW, SAMPLER2DSHADOW,
	};

	for( int i = 0; i < 21; i++ )
		if( type == typeList[i] )
			return enumList[i];
	
	return OTHER;
}


QString UniformVar::getStringFromUniformType( enum UniformType type )
{
	switch(type) {
		case INT: return "int";
		case FLOAT: return "float";
		case BOOL: return "bool";
		case VEC2: return "vec2";
		case VEC3: return "vec3";
		case VEC4: return "vec4";
		case IVEC2: return "ivec2";
		case IVEC3: return "ivec3";
		case IVEC4: return "ivec4";
		case BVEC2: return "bvec2";
		case BVEC3: return "bvec3";
		case BVEC4: return "bvec4";
		case MAT2: return "mat2";
		case MAT3: return "mat3";
		case MAT4: return "mat4";
		case SAMPLER1D: return "sampler1d";
		case SAMPLER2D: return "sampler2d";
		case SAMPLER3D: return "sampler3d";
		case SAMPLERCUBE: return "samplercube";
		case SAMPLER1DSHADOW: return "sampler1dshadow";
		case SAMPLER2DSHADOW: return "sampler2dshadow";
		default: return "other";
	}
	return "I love vim 'q' macroing";
}



void UniformVar::VarDump( int indent, bool extendedVarDump ) 
{
	QString ret = "";
	
	for( int i = 0; i < indent; i++ )
		ret += " ";
	
	ret += typeString + QString(" ") + name;

	switch( type ) {
		case INT: 
			ret += " = " + QString().setNum(ivalue);
			if( minSet || maxSet ) ret += " (";
			if( minSet ) ret += "from " + QString().setNum(imin) + " ";
			if( maxSet ) ret += "to "+ QString().setNum(imax);
			if( minSet || maxSet ) ret += ")";
			break;
		case FLOAT: 
			ret += " = " + QString().setNum(fvalue);
			if( minSet || maxSet ) ret += " (";
			if( minSet ) ret += "from " + QString().setNum(fmin) + " ";
			if( maxSet ) ret += "to "+ QString().setNum(fmax);
			if( minSet || maxSet ) ret += ")";
			break;
		case BOOL: ret += bvalue ? " = true" : " = false"; break;

		case IVEC2:
			ret += " = [ "; for( int i = 0; i < 2; i++ ) ret += QString().setNum( ivec2[i] ) + " "; ret += "]"; break;
		case IVEC3:
			ret += " = [ "; for( int i = 0; i < 3; i++ ) ret += QString().setNum( ivec3[i] ) + " "; ret += "]"; break;
		case IVEC4:
			ret += " = [ "; for( int i = 0; i < 4; i++ ) ret += QString().setNum( ivec4[i] ) + " "; ret += "]"; break;
		case BVEC2:
			ret += " = [ "; for( int i = 0; i < 2; i++ ) ret += bvec2[i] ? "true " : "false "; ret += "]"; break;
		case BVEC3:
			ret += " = [ "; for( int i = 0; i < 3; i++ ) ret += bvec3[i] ? "true " : "false "; ret += "]"; break;
		case BVEC4:
			ret += " = [ "; for( int i = 0; i < 4; i++ ) ret += bvec4[i] ? "true " : "false "; ret += "]"; break;
		case VEC2:
			ret += " = [ "; for( int i = 0; i < 2; i++ ) ret += QString().setNum( vec2[i] ) + " "; ret += "]"; break;
		case VEC3:
			ret += " = [ "; for( int i = 0; i < 3; i++ ) ret += QString().setNum( vec3[i] ) + " "; ret += "]"; break;
		case VEC4:
			ret += " = [ "; for( int i = 0; i < 4; i++ ) ret += QString().setNum( vec3[i] ) + " "; ret += "]"; break;
		case MAT2:
		case MAT3:
		case MAT4:
			ret += " [matrix]";
			if( extendedVarDump ) {
				int n = type == MAT2 ? 2 : (type == MAT3 ? 3 : 4);
				float * mat = type == MAT2 ? (float*)mat2 : (type == MAT3 ? (float*)mat3 : (float*)mat4);
				ret += "\n";

				for( int i = 0; i < n; i++ ) {
					for( int k = 0; k < indent+2; k++ )
						ret += " ";
					ret += "[ ";
					for( int j = 0; j < n; j++ ) ret += QString().setNum( mat[i*n+j] ) + " ";
					ret += "]";
					if( i+1 < n ) ret += "\n";
				}
			}
			break;

		case SAMPLER1DSHADOW: 
		case SAMPLER2DSHADOW: 
		case SAMPLERCUBE: 
		case SAMPLER1D: 
		case SAMPLER2D: 
		case SAMPLER3D: 
		{
			if( textureFilename.isEmpty() ) 
				ret += " = no filename, textureName = " + textureName;
			else
				ret += " = " + textureFilename;

			if( representerTagName == "RmRenderTarget" )
				ret += " [RmRenderTarget]";

			if( extendedVarDump ) {
				if( textureGLStates.size() == 0 )
					ret += " [with no openGL states]";
				else
				{
					ret += "\n";
					for( int i = 0; i < textureGLStates.size(); i++ ) {
						for( int j = 0; j < indent+2; j++ )
							ret += " ";
						ret += "\""  + textureGLStates[i].getName() + "\" (" + QString().setNum(textureGLStates[i].state).toLatin1().data() + ") => " + QString().setNum(textureGLStates[i].getValue());
						if( i+1 < textureGLStates.size() ) ret += "\n";
					}
				}
			} else
				ret += " [openGL states: " + QString().setNum(textureGLStates.size()) + "]";

			break;
		}
		default:
			ret += " [implement me]";
	}
	
	qDebug() << ret.toLatin1().data();
}


bool UniformVar::getUniformNumberVectorFromXmlTag( QDomElement & el, int values, void * narr, bool intOrFloat, UniformVar * ptr )
{
	int * iarr = (int*)narr;
	float * farr = (float*)narr;

	bool ok1=true,ok2=true;

	for( int i = 0; i < values; i++ )
	{
		QString attrname = "VALUE";
		if( values > 1 ) 
			attrname += "_" + QString().setNum(i);
		QString attrvalue = el.attribute(attrname);

		if( intOrFloat ) {
			int val = attrvalue.toInt(&ok1);
			if( ok1 ) {
				iarr[i] = val;
				ptr -> testRealMin(val);
				ptr -> testRealMax(val);
			}
		} else {
			float val = attrvalue.toFloat(&ok1);
			if( ok1 ) {
				farr[i] = val;
				ptr -> testRealMin(val);
				ptr -> testRealMax(val);
			}
		}

		if( !ok1 ) {
			qDebug() << "RmXmlParser: error while reading default value for " << (intOrFloat ? "int" : "float" )<< " vector["<<values<<"]" << el.attribute("NAME") << ":" << attrname << "=>" << attrvalue;
			return false;
		}
	}

	if( !el.attribute( "MIN" ).isEmpty() ) {
		if( intOrFloat ) {
			int min = el.attribute("MIN").toInt(&ok1);
			if( ok1 ) ptr -> setMin(min);
		} else {
			float min = el.attribute("MIN").toFloat(&ok1);
			if( ok1 ) ptr -> setMin(min);
		}
	}

	if( !el.attribute( "MAX" ).isEmpty() ) {
		if( intOrFloat ) {
			int max = el.attribute("MAX").toInt(&ok2);
			if( ok2 ) ptr -> setMax(max);
		} else {
			float max = el.attribute("MAX").toFloat(&ok2);
			if( ok2 ) ptr -> setMax(max);
		}
	}

	if( !ok1 || !ok2 ) {
		qDebug() << "RmXmlParser: error while reading default value for float vector["<<values<<"]" << el.attribute("NAME");
		qDebug() << "Min_ok("<<ok1<<") Max_ok("<<ok2<<")";
	}

	return true;
}

bool UniformVar::getUniformBooleanVectorFromXmlTag( QDomElement & el, int values, bool * barr )
{
	for( int i = 0; i < values; i++ )
	{
		QString attrname = "VALUE";
		if( values > 1 ) 
			attrname += "_" + QString().setNum(i);

		QString attrvalue = el.attribute(attrname);
		if( attrvalue != "TRUE" && attrvalue != "FALSE ") {
			qDebug() << "RmXmlParser: error while reading default value for boolean vector["<<values<<"]" << el.attribute("NAME");
			qDebug() << attrvalue << "is not TRUE neither FALSE";
			return false;
		}
		barr[i] = attrvalue == "TRUE";
	}
	return true;
}


