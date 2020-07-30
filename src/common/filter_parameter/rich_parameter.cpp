#include "rich_parameter.h"
#include "meshmodel.h"

RichParameter::RichParameter(const RichParameter& rp) :
    pName(rp.pName), val(rp.val->clone()), fieldDesc(rp.fieldDesc), tooltip(rp.tooltip)
{
}

RichParameter::RichParameter(RichParameter&& rp) :
    pName(std::move(rp.pName)), fieldDesc(std::move(rp.fieldDesc)), tooltip(std::move(rp.tooltip))
{
     val = rp.val;
     rp.val = nullptr;
}

RichParameter::RichParameter(const QString& nm, const Value& v, const QString& desc, const QString& tltip) :
    pName(nm), val(v.clone()), fieldDesc(desc), tooltip(tltip)
{
}

const QString& RichParameter::name() const
{
    return pName;
}

RichParameter& RichParameter::operator=(const RichParameter& rp)
{
    if (&rp != this){
        delete val;
        val = rp.val->clone();
        pName = rp.pName;
        fieldDesc = rp.fieldDesc;
        tooltip = rp.tooltip;
    }
    return *this;
}

RichParameter& RichParameter::operator=(RichParameter&& rp)
{
    assert(&rp != this);
    val = rp.val;
    rp.val = nullptr;
    pName = std::move(rp.pName);
    fieldDesc = std::move(rp.fieldDesc);
    tooltip = std::move(rp.tooltip);
}

RichParameter::~RichParameter()
{
    delete val;
}

RichBool::RichBool(const QString& nm,const bool defval,const QString& desc,const QString& tltip) :
    RichParameter(nm, BoolValue(defval), desc, tltip)
{
}


RichBool::RichBool(const QString& nm, const bool val, const bool defval, const QString& desc, const QString& tltip) :
    RichParameter(nm, BoolValue(val), desc, tltip)
{
}

void RichBool::accept( Visitor& v )
{
    v.visit(*this);
}

RichBool* RichBool::clone() const
{
    return new RichBool(*this);
}

bool RichBool::operator==( const RichParameter& rb )
{
    return (rb.val->isBool() && (pName == rb.name()) && (val->getBool() == rb.val->getBool()));
}

RichBool::~RichBool()
{
}

RichInt::RichInt( const QString& nm,const int defval,const QString& desc,const QString& tltip) :
    RichParameter(nm, IntValue(defval),desc, tltip)
{
}

RichInt::RichInt(const QString& nm, const int val, const int defval, const QString& desc, const QString& tltip) :
    RichParameter(nm, IntValue(val), desc, tltip)
{
}

void RichInt::accept( Visitor& v )
{
    v.visit(*this);
}

RichInt* RichInt::clone() const
{
    return new RichInt(*this);
}

bool RichInt::operator==( const RichParameter& rb )
{
    return (rb.val->isInt() &&(pName == rb.name()) && (val->getInt() == rb.val->getInt()));
}

RichInt::~RichInt()
{
}

RichFloat::RichFloat( const QString& nm,const float defval,const QString& desc,const QString& tltip) :
    RichParameter(nm, FloatValue(defval),desc, tltip)
{
}

RichFloat::RichFloat(const QString& nm, const float val, const float defval, const QString& desc, const QString& tltip) :
    RichParameter(nm, FloatValue(val), desc, tltip)
{
}

void RichFloat::accept( Visitor& v )
{
    v.visit(*this);
}

RichFloat* RichFloat::clone() const
{
    return new RichFloat(*this);
}

bool RichFloat::operator==( const RichParameter& rb )
{
    return (rb.val->isFloat() &&(pName == rb.name()) && (val->getFloat() == rb.val->getFloat()));
}

RichFloat::~RichFloat()
{
}

RichString::RichString( const QString& nm,const QString& defval,const QString& desc,const QString& tltip) :
    RichParameter(nm, StringValue(defval),desc,tltip)
{
}

RichString::RichString(const QString& nm, const QString& val, const QString& defval, const QString& desc, const QString& tltip) :
    RichParameter(nm, StringValue(val),desc, tltip)
{
}

void RichString::accept( Visitor& v )
{
    v.visit(*this);
}

RichString* RichString::clone() const
{
    return new RichString(*this);
}

bool RichString::operator==( const RichParameter& rb )
{
    return (rb.val->isString() &&(pName == rb.name()) && (val->getString() == rb.val->getString()));
}

RichString::~RichString()
{

}

RichMatrix44f::RichMatrix44f( const QString& nm,const vcg::Matrix44f& defval,const QString& desc,const QString& tltip ) :
    RichParameter(nm, Matrix44fValue(defval),desc, tltip) { }
RichMatrix44f::RichMatrix44f( const QString& nm,const vcg::Matrix44d& defval,const QString& desc,const QString& tltip ) :
    RichParameter(nm, Matrix44fValue(defval),desc, tltip) { }

RichMatrix44f::RichMatrix44f(const QString& nm, const vcg::Matrix44f& val, const vcg::Matrix44f& defval, const QString& desc /*= QString()*/, const QString& tltip /*= QString()*/)
    : RichParameter(nm, Matrix44fValue(val),desc,tltip) { }

void RichMatrix44f::accept( Visitor& v )
{
    v.visit(*this);
}

RichMatrix44f* RichMatrix44f::clone() const
{
    return new RichMatrix44f(*this);
}

bool RichMatrix44f::operator==( const RichParameter& rb )
{
    return (rb.val->isMatrix44f() &&(pName == rb.name()) && (val->getMatrix44f() == rb.val->getMatrix44f()));
}

RichMatrix44f::~RichMatrix44f()
{

}

RichPoint3f::RichPoint3f( const QString& nm,const vcg::Point3f& defval,const QString& desc,const QString& tltip ) :
    RichParameter(nm, Point3fValue(defval),desc, tltip){}
RichPoint3f::RichPoint3f( const QString& nm,const vcg::Point3d& defval,const QString& desc,const QString& tltip ) :
    RichParameter(nm, Point3fValue(defval),desc, tltip){}

RichPoint3f::RichPoint3f(const QString& nm, const vcg::Point3f& val, const vcg::Point3f& defval, const QString& desc, const QString& tltip) :
    RichParameter(nm, Point3fValue(val), desc, tltip)
{

}

void RichPoint3f::accept( Visitor& v )
{
    v.visit(*this);
}

RichPoint3f* RichPoint3f::clone() const
{
    return new RichPoint3f(*this);
}

bool RichPoint3f::operator==( const RichParameter& rb )
{
    return (rb.val->isPoint3f() &&(pName == rb.name()) && (val->getPoint3f() == rb.val->getPoint3f()));
}

RichPoint3f::~RichPoint3f()
{

}
//----
RichShotf::RichShotf( const QString& nm,const vcg::Shotf& defval,const QString& desc,const QString& tltip ) :
    RichParameter(nm, ShotfValue(defval),desc, tltip)
{}

RichShotf::RichShotf(const QString& nm, const vcg::Shotf& val, const vcg::Shotf& defval, const QString& desc, const QString& tltip) :
    RichParameter(nm, ShotfValue(val), desc, tltip)
{}

void RichShotf::accept( Visitor& v )
{
    v.visit(*this);
}

RichShotf* RichShotf::clone() const
{
    return new RichShotf(*this);
}

bool RichShotf::operator==( const RichParameter& rb )
{
  return (rb.val->isShotf() &&(pName == rb.name()) ); // TODO REAL TEST OF EQUALITY // && (val->getShotf() == rb.val->getShotf()));
}

RichShotf::~RichShotf()
{ }
//----
RichColor::RichColor( const QString& nm,const QColor& defval,const QString& desc,const QString& tltip ) :
    RichParameter(nm, ColorValue(defval),desc, tltip)
{

}

RichColor::RichColor(const QString& nm, const QColor& val, const QColor& defval, const QString& desc, const QString& tltip) :
    RichParameter(nm, ColorValue(val), desc, tltip)
{
}

void RichColor::accept( Visitor& v )
{
    v.visit(*this);
}

RichColor* RichColor::clone() const
{
    return new RichColor(*this);
}

bool RichColor::operator==( const RichParameter& rb )
{
    return (rb.val->isColor() &&(pName == rb.name()) && (val->getColor() == rb.val->getColor()));
}

RichColor::~RichColor()
{

}

RichAbsPerc::RichAbsPerc( const QString& nm,const float defval,const float minval,const float maxval,const QString& desc,const QString& tltip) :
    RichParameter(nm, AbsPercValue(defval), desc, tltip), min(minval), max(maxval)
{

}

RichAbsPerc::RichAbsPerc(const QString& nm, const float val, const float defval, const float minval, const float maxval, const QString& desc, const QString& tltip) :
    RichParameter(nm, AbsPercValue(val), desc, tltip), min(minval), max(maxval)
{

}

void RichAbsPerc::accept( Visitor& v )
{
    v.visit(*this);
}

RichAbsPerc* RichAbsPerc::clone() const
{
    return new RichAbsPerc(*this);
}

bool RichAbsPerc::operator==( const RichParameter& rb )
{
    return (rb.val->isAbsPerc() &&(pName == rb.name()) && (val->getAbsPerc() == rb.val->getAbsPerc()));
}

RichAbsPerc::~RichAbsPerc()
{

}

RichEnum::RichEnum( const QString& nm,const int defval,const QStringList& values,const QString& desc,const QString& tltip) :
    RichParameter(nm, EnumValue(defval),desc, tltip), enumvalues(values)
{
}

RichEnum::RichEnum(const QString& nm, const int val, const int defval, const QStringList& values, const QString& desc, const QString& tltip) :
    RichParameter(nm, EnumValue(val), desc, tltip), enumvalues(values)
{

}

void RichEnum::accept( Visitor& v )
{
    v.visit(*this);
}

RichEnum* RichEnum::clone() const
{
    return new RichEnum(*this);
}

bool RichEnum::operator==( const RichParameter& rb )
{
    return (rb.val->isEnum() &&(pName == rb.name()) && (val->getEnum() == rb.val->getEnum()));
}

RichEnum::~RichEnum()
{

}

RichMesh::RichMesh( const QString& nm,MeshModel* defval,MeshDocument* doc,const QString& desc,const QString& tltip )
    :RichParameter(nm, MeshValue(defval), desc, tltip), meshdoc(doc)
{
    meshindex = -1;
    if (meshdoc != nullptr)
        meshindex = meshdoc->meshList.indexOf(defval);
    assert((meshindex != -1) || (meshdoc == nullptr));
}

RichMesh::RichMesh( const QString& nm,int meshind,MeshDocument* doc,const QString& desc,const QString& tltip )
    :RichParameter(nm,MeshValue(doc, meshind), desc, tltip), meshdoc(doc)
{
    assert(meshind < meshdoc->size() && meshind >= 0);
    meshindex = meshind;
    if (meshdoc != nullptr)
        val = new MeshValue(meshdoc->meshList.at(meshindex));
    else
        val = nullptr;
}

RichMesh::RichMesh(const QString& nm, MeshModel* val, MeshModel* defval, MeshDocument* doc, const QString& desc, const QString& tltip)
    : RichParameter(nm, MeshValue(val), desc, tltip), meshdoc(doc)
{
    meshindex = -1;
    if (meshdoc != nullptr)
        meshindex = doc->meshList.indexOf(val);
}

RichMesh::RichMesh(const QString& nm, int meshind, const QString& desc /*= QString()*/, const QString& tltip /*= QString()*/)
	: RichParameter(nm, MeshValue(nullptr), desc, tltip)
{
	meshdoc = nullptr;
	meshindex = meshind;
}

void RichMesh::accept( Visitor& v )
{
	v.visit(*this);
}

RichMesh* RichMesh::clone() const
{
	return new RichMesh(*this);
}

bool RichMesh::operator==( const RichParameter& rb )
{
	return (rb.val->isMesh() &&(pName == rb.name()) && (val->getMesh() == rb.val->getMesh()));
}

RichMesh::~RichMesh()
{

}

//RichFloatList::RichFloatList( const QString& nm,FloatListValue* v,FloatListDecoration* prdec ) :
//	RichParameter(nm,v,prdec)
//{

//}

//RichFloatList::RichFloatList(const QString& nm, FloatListValue* /*val*/, FloatListValue* v, FloatListDecoration* prdec) : RichParameter(nm, v, prdec)
//{

//}

//void RichFloatList::accept( Visitor& v )
//{
//    v.visit(*this);
//}

//bool RichFloatList::operator==( const RichParameter& rb )
//{
//    return (rb.val->isFloatList() &&(name == rb.name) && (val->getFloatList() == rb.val->getFloatList()));
//}

//RichFloatList::~RichFloatList()
//{

//}

RichDynamicFloat::RichDynamicFloat( const QString& nm,const float defval,const float minval,const float maxval,const QString& desc,const QString& tltip ) :
    RichParameter(nm, DynamicFloatValue(defval),desc, tltip), min(minval), max(maxval)
{
}

RichDynamicFloat::RichDynamicFloat(const QString& nm, const float val, const float defval, const float minval, const float maxval, const QString& desc, const QString& tltip) :
    RichParameter(nm, DynamicFloatValue(val), desc, tltip), min(minval), max(maxval)
{
}

void RichDynamicFloat::accept( Visitor& v )
{
    v.visit(*this);
}

RichDynamicFloat* RichDynamicFloat::clone() const
{
    return new RichDynamicFloat(*this);
}

bool RichDynamicFloat::operator==( const RichParameter& rb )
{
    return (rb.val->isDynamicFloat() &&(pName == rb.name()) && (val->getDynamicFloat() == rb.val->getDynamicFloat()));
}

RichDynamicFloat::~RichDynamicFloat()
{

}

RichOpenFile::RichOpenFile(const QString& nm, const QString& directorydefval, const QStringList& exts, const QString& desc, const QString& tltip) :
    RichParameter(nm, FileValue(directorydefval), desc, tltip), exts(exts)
{
}

void RichOpenFile::accept( Visitor& v )
{
    v.visit(*this);
}

RichOpenFile* RichOpenFile::clone() const
{
    return new RichOpenFile(*this);
}

bool RichOpenFile::operator==( const RichParameter& rb )
{
    return (rb.val->isFileName() &&(pName == rb.name()) && (val->getFileName() == rb.val->getFileName()));
}

RichOpenFile::~RichOpenFile()
{

}

RichSaveFile::RichSaveFile(const QString& nm, const QString& filedefval, const QString& ext, const QString& desc, const QString& tltip) :
    RichParameter(nm, FileValue(filedefval), desc, tltip), ext(ext)
{
}

void RichSaveFile::accept( Visitor& v )
{
    v.visit(*this);
}

RichSaveFile* RichSaveFile::clone() const
{
    return new RichSaveFile(*this);
}

bool RichSaveFile::operator==( const RichParameter& rb )
{
    return (rb.val->isFileName() &&(pName == rb.name()) && (val->getFileName() == rb.val->getFileName()));
}

RichSaveFile::~RichSaveFile()
{

}

bool RichParameterAdapter::create( const QDomElement& np,RichParameter** par )
{
    QString name=np.attribute("name");
    QString type=np.attribute("type");
    QString desc=np.attribute("description");
    QString tooltip=np.attribute("tooltip");

    // qDebug("    Reading Param with name %s : %s", qUtf8Printable(name), qUtf8Printable(type));

    bool corrconv = false;
    if(type=="RichBool")
    {
        QString val = np.attribute("value").toLower();
        if ((val != QString("true")) && (val != QString("false")))
            return false;
        *par = new RichBool(name,np.attribute("value")!=QString("false"),desc,tooltip);
        return true;
    }

    if(type=="RichInt")
    {
        int val = np.attribute("value").toInt(&corrconv);
        if (!corrconv)
            return false;
        *par = new RichInt(name,val,desc,tooltip);
        return true;
    }

    if(type=="RichFloat")
    {
        float val = np.attribute("value").toFloat(&corrconv);
        if (!corrconv)
            return false;
        *par = new RichFloat(name,val,desc,tooltip);
        return true;
    }

    if(type=="RichString")
    {
        *par = new RichString(name,np.attribute("value"),desc,tooltip);
        return true;
    }

    if(type=="RichAbsPerc")
    {
        float val = np.attribute("value").toFloat(&corrconv);
        if ((!corrconv) && (val >= 0.0f) && (val <= 100.0f))
            return false;
        float min = np.attribute("min").toFloat(&corrconv);
        if (!corrconv)
            return false;
        float max = np.attribute("max").toFloat(&corrconv);
        if (!corrconv)
            return false;
        *par = new RichAbsPerc(name,val,min,max,desc,tooltip);
        return true;
    }

    if(type=="RichColor")
    {
        unsigned int r = np.attribute("r").toUInt(&corrconv);
    if ((!corrconv) && (r <= 255))
            return false;
        unsigned int g = np.attribute("g").toUInt(&corrconv);
    if ((!corrconv) && (g <= 255))
            return false;
        unsigned int b = np.attribute("b").toUInt(&corrconv);
    if ((!corrconv) && (b <= 255))
            return false;
        unsigned int a = np.attribute("a").toUInt(&corrconv);
    if ((!corrconv) && (a <= 255))
            return false;
        QColor col(r,g,b,a);
        *par= new RichColor(name,col,desc,tooltip);
        return true;
    }

    if(type=="RichMatrix44f")
    {
        vcg::Matrix44f mm;
        for(int i=0;i<16;++i)
        {
            float val = np.attribute(QString("val")+QString::number(i)).toFloat(&corrconv);
            if (!corrconv)
                return false;
            mm.V()[i]=val;
        }
        *par = new RichMatrix44f(name,mm,desc,tooltip);
        return true;
    }

    if(type=="RichEnum")
    {
    QStringList list;
        int enum_card = np.attribute(QString("enum_cardinality")).toUInt(&corrconv);
        if (!corrconv)
            return false;

        for(int i=0;i<enum_card;++i)
            list<<np.attribute(QString("enum_val")+QString::number(i));

        int val = np.attribute("value").toInt(&corrconv);
        if ((!corrconv) && (val >=0) && (val < enum_card))
            return false;
        *par = new RichEnum(name,val,list,desc,tooltip);
        return true;
    }

    if(type == "RichMesh")
    {
        int val = np.attribute("value").toInt(&corrconv);

        if (!corrconv)
            return false;

        *par = new RichMesh(name, val,desc,tooltip);
        return true;
    }

    if(type == "RichFloatList")
    {
        //to be implemented
        assert(0);
    }

    if(type == "RichDynamicFloat")
    {
        float min = np.attribute("min").toFloat(&corrconv);
        if (!corrconv)
            return false;
        float max = np.attribute("max").toFloat(&corrconv);
        if (!corrconv)
            return false;

        float val = np.attribute("value").toFloat(&corrconv);
        if ((!corrconv) && (val >= min) && (val <= max))
            return false;

        *par = new RichDynamicFloat(name, val, min, max, desc, tooltip);
        return true;
    }

    if(type == "RichOpenFile")
    {
        QStringList list;
        int exts_card = np.attribute(QString("exts_cardinality")).toUInt(&corrconv);
        if (!corrconv)
            return false;

        for(int i=0;i<exts_card;++i)
            list<<np.attribute(QString("exts_val")+QString::number(i));

        QString defdir = np.attribute("value");

        *par = new RichOpenFile(name,defdir,list,desc,tooltip);
        return true;
    }

    if(type == "RichSaveFile")
    {
        QString deffile = np.attribute("value");
        QString ext = np.attribute("ext");
        *par = new RichSaveFile(name,deffile,ext,desc,tooltip);
        return true;
    }

    if(type=="RichPoint3f")
    {
        vcg::Point3f val;
        val[0]=np.attribute("x").toFloat(&corrconv);
        if (!corrconv)
            return false;
        val[1]=np.attribute("y").toFloat(&corrconv);
        if (!corrconv)
            return false;
        val[2]=np.attribute("z").toFloat(&corrconv);
        if (!corrconv)
            return false;

        *par = new RichPoint3f(name, val,desc,tooltip);
        return true;
    }
  if(type=="RichShotf")
  {
    vcg::Shotf val;
    assert(0); //TODO!!!!
    *par = new RichShotf(name, val,desc,tooltip);
    return true;
  }

    return false;
}

QString RichParameterAdapter::convertToStringValue(RichParameter& rp)
{
	RichParameterValueToStringVisitor v;
	rp.accept(v);
	return v.stringvalue;
}
