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
    GEOMETRIC, //Pag. 201 RISpec3.2
		MAKE,
		MAKECUBEFACEENVIRONMENT,
		OPTION,
    PROCEDURAL,
		READARCHIVE,
    SCREENWINDOW,
    SOLIDBEGIN,
    SOLIDEND,
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
    proc->insert("Blobby", ribParser::GEOMETRIC);
		proc->insert("Bound", ribParser::BOUND);
		proc->insert("Clipping", ribParser::OTHER);
		proc->insert("ClippingPlane", ribParser::OTHER);
		proc->insert("Color", ribParser::OTHER);
		proc->insert("ColorSamples", ribParser::OTHER);
		proc->insert("ConcatTransform", ribParser::OTHER);
    proc->insert("Cone", ribParser::GEOMETRIC);
		proc->insert("Context", ribParser::OTHER);
		proc->insert("CoordinateSystem", ribParser::OTHER);
		proc->insert("CoordSysTransform", ribParser::OTHER);
		proc->insert("CropWindow", ribParser::OTHER);
    proc->insert("Curves", ribParser::GEOMETRIC);
    proc->insert("Cylinder", ribParser::GEOMETRIC);
		proc->insert("Declare", ribParser::OTHER);
		proc->insert("DepthOfField", ribParser::OTHER);
		proc->insert("Detail", ribParser::OTHER);
		proc->insert("DetailRange", ribParser::OTHER);
    proc->insert("Disk", ribParser::GEOMETRIC);
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
    proc->insert("GeneralPolygon", ribParser::GEOMETRIC);
		proc->insert("GeometricApproximation", ribParser::OTHER);
    proc->insert("Geometry", ribParser::GEOMETRIC);
		proc->insert("GetContext", ribParser::OTHER);
		proc->insert("Hider", ribParser::OTHER);
    proc->insert("Hyperboloid", ribParser::GEOMETRIC);
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
    proc->insert("NuPatch", ribParser::GEOMETRIC);
		proc->insert("ObjectBegin", ribParser::OTHER);
		proc->insert("ObjectEnd", ribParser::OTHER);
    proc->insert("ObjectInstance", ribParser::GEOMETRIC);
		proc->insert("Opacity", ribParser::OTHER);
		proc->insert("Option", ribParser::OPTION);
		proc->insert("Orientation", ribParser::OTHER);
    proc->insert("Paraboloid", ribParser::GEOMETRIC);
    proc->insert("Patch", ribParser::GEOMETRIC);
    proc->insert("PatchMesh", ribParser::GEOMETRIC);
		proc->insert("Perspective", ribParser::OTHER);
		proc->insert("PixelFilter", ribParser::OTHER);
		proc->insert("PixelSamples", ribParser::OTHER);
		proc->insert("PixelVariance", ribParser::OTHER);
    proc->insert("Points", ribParser::GEOMETRIC);
    proc->insert("PointsGeneralPolygons", ribParser::GEOMETRIC);
    proc->insert("PointsPolygons", ribParser::GEOMETRIC);
    proc->insert("Polygon", ribParser::GEOMETRIC);
    proc->insert("Procedural", ribParser::PROCEDURAL);
		proc->insert("Projection", ribParser::OTHER);
		proc->insert("Quantize", ribParser::OTHER);
		proc->insert("ReadArchive", ribParser::READARCHIVE);
		proc->insert("RelativeDetail", ribParser::OTHER);
		proc->insert("ReverseOrientation", ribParser::OTHER);
		proc->insert("Rotate", ribParser::OTHER);
		proc->insert("Scale", ribParser::OTHER);
    proc->insert("ScreenWindow", ribParser::SCREENWINDOW);
		proc->insert("ShadingInterpolation", ribParser::OTHER);
		proc->insert("ShadingRate", ribParser::OTHER);
		proc->insert("Shutter", ribParser::OTHER);
		proc->insert("Sides", ribParser::OTHER);
		proc->insert("Skew", ribParser::OTHER);
    proc->insert("SolidBegin", ribParser::SOLIDBEGIN);
    proc->insert("SolidEnd", ribParser::SOLIDEND);
    proc->insert("Sphere", ribParser::GEOMETRIC);
    proc->insert("SubdivisionMesh", ribParser::GEOMETRIC);
		proc->insert("Surface", ribParser::SURFACE);
		proc->insert("TextureCoordinates", ribParser::OTHER);
    proc->insert("Torus", ribParser::GEOMETRIC);
		proc->insert("Transform", ribParser::TRANSFORM);
		proc->insert("TransformBegin", ribParser::OTHER);
		proc->insert("TransformEnd", ribParser::OTHER);
		proc->insert("TransformPoints", ribParser::OTHER);
		proc->insert("Translate", ribParser::OTHER);
    proc->insert("TrimCurve", ribParser::GEOMETRIC);
		proc->insert("version", ribParser::OTHER); //not a procedure, but a keyword
		proc->insert("WorldBegin", ribParser::WORLDBEGIN);
		proc->insert("WorldEnd", ribParser::WORLDEND);
		return proc;
	}

	//split a statement in word (strings are unique word, like so '\"' , '[' and ']' )
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