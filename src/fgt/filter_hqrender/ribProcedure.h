#include <QHash>
#include <QString>

class ribParser {
public:
	enum ribProcedure {
		NOTAPROCEDURE,
		ATTRIBUTE,
		ATTRIBUTEBEGIN,
		ATTRIBUTEEND,
		BOUND,
		DISPLAY,
		FRAMEBEGIN,
		FRAMEEND,
		MAKE,
		MAKECUBEFACEENVIRONMENT,
		OPTION,
		READARCHIVE,
		SURFACE,
		TRANSFORM,
		WORLDBEGIN,
		WORLDEND,
		COMMENT,
		OTHER,
		NOMORESTATEMENT
	};

	static QHash<QString, int>* initHash() {
		QHash<QString, int>* proc = new QHash<QString,int>();
		proc->insert("ArchiveRecord", ribParser::OTHER);
		proc->insert("AreaLightSource", ribParser::OTHER);
		proc->insert("Atmosphere", ribParser::OTHER);
		proc->insert("Attribute", ribParser::ATTRIBUTE);
		proc->insert("AttributeBegin", ribParser::ATTRIBUTEBEGIN);
		proc->insert("AttributeEnd", ribParser::ATTRIBUTEEND);
		proc->insert("Basis", ribParser::OTHER);
		proc->insert("Begin", ribParser::OTHER);
		proc->insert("Blobby", ribParser::OTHER);
		proc->insert("Bound", ribParser::BOUND);
		proc->insert("Clipping", ribParser::OTHER);
		proc->insert("ClippingPlane", ribParser::OTHER);
		proc->insert("Color", ribParser::OTHER);
		proc->insert("ColorSamples", ribParser::OTHER);
		proc->insert("ConcatTransform", ribParser::OTHER);
		proc->insert("Cone", ribParser::OTHER);
		proc->insert("Context", ribParser::OTHER);
		proc->insert("CoordinateSystem", ribParser::OTHER);
		proc->insert("CoordSysTransform", ribParser::OTHER);
		proc->insert("CropWindow", ribParser::OTHER);
		proc->insert("Curves", ribParser::OTHER);
		proc->insert("Cylinder", ribParser::OTHER);
		proc->insert("Declare", ribParser::OTHER);
		proc->insert("DepthOfField", ribParser::OTHER);
		proc->insert("Detail", ribParser::OTHER);
		proc->insert("DetailRange", ribParser::OTHER);
		proc->insert("Disk", ribParser::OTHER);
		proc->insert("Displacement", ribParser::OTHER);
		proc->insert("Display", ribParser::DISPLAY);
		proc->insert("End", ribParser::OTHER);
		proc->insert("ErrorHandler", ribParser::OTHER);
		proc->insert("Exposure", ribParser::OTHER);
		proc->insert("Exterior", ribParser::OTHER);
		proc->insert("Format", ribParser::OTHER);
		proc->insert("FrameAspectRatio", ribParser::OTHER);
		proc->insert("FrameBegin", ribParser::FRAMEBEGIN);
		proc->insert("FrameEnd", ribParser::FRAMEEND);
		proc->insert("GeneralPolygon", ribParser::OTHER);
		proc->insert("GeometricApproximation", ribParser::OTHER);
		proc->insert("Geometry", ribParser::OTHER);
		proc->insert("GetContext", ribParser::OTHER);
		proc->insert("Hider", ribParser::OTHER);
		proc->insert("Hyperboloid", ribParser::OTHER);
		proc->insert("Identity", ribParser::OTHER);
		proc->insert("Illuminate", ribParser::OTHER);
		proc->insert("Imager", ribParser::OTHER);
		proc->insert("Interior", ribParser::OTHER);
		proc->insert("LightSource", ribParser::OTHER);
		proc->insert("MakeCubeFaceEnvironment", ribParser::MAKECUBEFACEENVIRONMENT);
		proc->insert("MakeLatLongEnvironment", ribParser::MAKE);
		proc->insert("MakeShadow", ribParser::MAKE);
		proc->insert("MakeTexture", ribParser::MAKE);
		proc->insert("Matte", ribParser::OTHER);
		proc->insert("MotionBegin", ribParser::OTHER);
		proc->insert("MotionEnd", ribParser::OTHER);
		proc->insert("NuPatch", ribParser::OTHER);
		proc->insert("ObjectBegin", ribParser::OTHER);
		proc->insert("ObjectEnd", ribParser::OTHER);
		proc->insert("ObjectInstance", ribParser::OTHER);
		proc->insert("Opacity", ribParser::OTHER);
		proc->insert("Option", ribParser::OPTION);
		proc->insert("Orientation", ribParser::OTHER);
		proc->insert("Paraboloid", ribParser::OTHER);
		proc->insert("Patch", ribParser::OTHER);
		proc->insert("PatchMesh", ribParser::OTHER);
		proc->insert("Perspective", ribParser::OTHER);
		proc->insert("PixelFilter", ribParser::OTHER);
		proc->insert("PixelSamples", ribParser::OTHER);
		proc->insert("PixelVariance", ribParser::OTHER);
		proc->insert("Points", ribParser::OTHER);
		proc->insert("PointsGeneralPolygons", ribParser::OTHER);
		proc->insert("PointsPolygons", ribParser::OTHER);
		proc->insert("Polygon", ribParser::OTHER);
		proc->insert("Procedural", ribParser::OTHER);
		proc->insert("Projection", ribParser::OTHER);
		proc->insert("Quantize", ribParser::OTHER);
		proc->insert("ReadArchive", ribParser::READARCHIVE);
		proc->insert("RelativeDetail", ribParser::OTHER);
		proc->insert("ReverseOrientation", ribParser::OTHER);
		proc->insert("Rotate", ribParser::OTHER);
		proc->insert("Scale", ribParser::OTHER);
		proc->insert("ScreenWindow", ribParser::OTHER);
		proc->insert("ShadingInterpolation", ribParser::OTHER);
		proc->insert("ShadingRate", ribParser::OTHER);
		proc->insert("Shutter", ribParser::OTHER);
		proc->insert("Sides", ribParser::OTHER);
		proc->insert("Skew", ribParser::OTHER);
		proc->insert("SolidBegin", ribParser::OTHER);
		proc->insert("SolidEnd", ribParser::OTHER);
		proc->insert("Sphere", ribParser::OTHER);
		proc->insert("SubdivisionMesh", ribParser::OTHER);
		proc->insert("Surface", ribParser::SURFACE);
		proc->insert("TextureCoordinates", ribParser::OTHER);
		proc->insert("Torus", ribParser::OTHER);
		proc->insert("Transform", ribParser::TRANSFORM);
		proc->insert("TransformBegin", ribParser::OTHER);
		proc->insert("TransformEnd", ribParser::OTHER);
		proc->insert("TransformPoints", ribParser::OTHER);
		proc->insert("Translate", ribParser::OTHER);
		proc->insert("TrimCurve", ribParser::OTHER);
		proc->insert("version", ribParser::OTHER); //not a procedure, but a keyword
		proc->insert("WorldBegin", ribParser::WORLDBEGIN);
		proc->insert("WorldEnd", ribParser::WORLDEND);
		return proc;
	}

	//split a statement in word (strings are unique word, like '\"' , '[' and ']' )
	static QStringList splitStatement(const QString* line) {
		QString str = line->trimmed();
		
		QStringList list = QStringList();
		QString word = "";
		bool string = false, slash = false;
		for(int i = 0; i<str.size(); i++) {
			if(str[i] != ' ' && str[i] != '\n') {
				if(str[i] == '\\') {
					slash = true;
				}
				if(!slash && str[i] == '\"') {
					if(word != "")
						list << word.simplified(); 
					list << "\"";
					word = "";
					string = !string;
				}
				else {
					if((str[i] == '[' || str[i] == ']') && !string) {
						if(word != "")
							list << word.simplified();
						list << QString(str[i]);
						word = "";					
					}
					else
						word += str[i];
				}
			}
			else { //is a ' ' or \n
				if(string) {
					word += str[i];
				}
				else {
					if(word != "") { //it's a sequence of
						list << word.simplified();
						word = "";			
					}
				}
				slash = false;
			}
		}
		word = word.simplified();
		if(word != "")
			list << word;
		//foreach(QString s,list)
		//	qDebug(qPrintable(s));
		return list;
	}
};