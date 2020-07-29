#include "visitor.h"
#include "rich_parameter.h"

void RichParameterCopyConstructor::visit( RichBool& pd )
{
    lastCreated = new RichBool(pd.name,pd.val->getBool(),pd.val->getBool(),pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichInt& pd )
{
    lastCreated = new RichInt(pd.name,pd.val->getInt(),pd.val->getInt(),pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichFloat& pd )
{
    lastCreated = new RichFloat(pd.name,pd.val->getFloat(),pd.val->getFloat(),pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichString& pd )
{
    lastCreated = new RichString(pd.name,pd.val->getString(),pd.val->getString(),pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichMatrix44f& pd )
{
    lastCreated = new RichMatrix44f(pd.name,pd.val->getMatrix44f(),pd.val->getMatrix44f(),pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichPoint3f& pd )
{
    lastCreated = new RichPoint3f(pd.name,pd.val->getPoint3f(),pd.val->getPoint3f(),pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichShotf& pd )
{
  lastCreated = new RichShotf(pd.name,pd.val->getShotf(),pd.val->getShotf(),pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichColor& pd )
{
    lastCreated = new RichColor(pd.name,pd.val->getColor(),pd.val->getColor(),pd.fieldDesc,pd.tooltip);
}


void RichParameterCopyConstructor::visit( RichAbsPerc& pd )
{
    lastCreated = new RichAbsPerc(pd.name,pd.val->getAbsPerc(),pd.val->getAbsPerc(),pd.min,pd.max,pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichEnum& pd )
{
    lastCreated = new RichEnum(pd.name,pd.val->getEnum(),pd.val->getEnum(),pd.enumvalues,pd.fieldDesc,pd.tooltip);
}

//void RichParameterCopyConstructor::visit( RichFloatList& /*pd*/ )
//{
//    /*lastCreated = new FloatListWidget(par,&pd);*/
//}

void RichParameterCopyConstructor::visit( RichDynamicFloat& pd )
{
    lastCreated = new RichDynamicFloat(pd.name,pd.val->getDynamicFloat(),pd.val->getDynamicFloat(),pd.min,pd.max,pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichOpenFile& pd )
{
    lastCreated = new RichOpenFile(pd.name,pd.val->getFileName(),pd.exts,pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichSaveFile& pd )
{
    lastCreated = new RichSaveFile(pd.name,pd.val->getFileName(),pd.ext,pd.fieldDesc,pd.tooltip);
}

void RichParameterCopyConstructor::visit( RichMesh& pd )
{
    if (pd.val != NULL)
        lastCreated = new RichMesh(pd.name,pd.val->getMesh(),pd.val->getMesh(),pd.meshdoc,pd.fieldDesc,pd.tooltip);
    else
        lastCreated = new RichMesh(pd.name,pd.meshindex);
}

void RichParameterXMLVisitor::fillRichParameterAttribute(const QString& type, const QString& name, const QString& desc, const QString& tooltip)
{
    parElem = docdom.createElement("Param");
    parElem.setAttribute("name",name);
    parElem.setAttribute("type",type);
    parElem.setAttribute("description",desc);
    parElem.setAttribute("tooltip",tooltip);
}

void RichParameterXMLVisitor::fillRichParameterAttribute(const QString& type,const QString& name,const QString& val,const QString& desc,const QString& tooltip)
{
    fillRichParameterAttribute(type,name,desc,tooltip);
    parElem.setAttribute("value",val);
}

void RichParameterXMLVisitor::visit( RichBool& pd )
{
    QString v;
    if (pd.val->getBool())
        v = "true";
    else
        v = "false";
    fillRichParameterAttribute("RichBool",pd.name,v,pd.fieldDesc,pd.tooltip);
}

void RichParameterXMLVisitor::visit( RichInt& pd )
{
    fillRichParameterAttribute("RichInt",pd.name,QString::number(pd.val->getInt()),pd.fieldDesc,pd.tooltip);
}

void RichParameterXMLVisitor::visit( RichFloat& pd )
{
    fillRichParameterAttribute("RichFloat",pd.name,QString::number(pd.val->getFloat()),pd.fieldDesc,pd.tooltip);
}

void RichParameterXMLVisitor::visit( RichString& pd )
{
    fillRichParameterAttribute("RichString",pd.name,pd.val->getString(),pd.fieldDesc,pd.tooltip);
}

void RichParameterXMLVisitor::visit( RichMatrix44f& pd )
{
    fillRichParameterAttribute("RichMatrix44f",pd.name,pd.fieldDesc,pd.tooltip);
    vcg::Matrix44f mat = pd.val->getMatrix44f();
    for(unsigned int ii = 0;ii < 16;++ii)
        parElem.setAttribute(QString("val")+QString::number(ii),QString::number(mat.V()[ii]));
}

void RichParameterXMLVisitor::visit( RichPoint3f& pd )
{
    fillRichParameterAttribute("RichPoint3f",pd.name,pd.fieldDesc,pd.tooltip);
    vcg::Point3f p = pd.val->getPoint3f();
    parElem.setAttribute("x",QString::number(p.X()));
    parElem.setAttribute("y",QString::number(p.Y()));
    parElem.setAttribute("z",QString::number(p.Z()));
}

void RichParameterXMLVisitor::visit( RichShotf& pd )
{
  fillRichParameterAttribute("RichShotf",pd.name,pd.fieldDesc,pd.tooltip);
  assert(0); // TODO !!!!
}


void RichParameterXMLVisitor::visit( RichColor& pd )
{
    fillRichParameterAttribute("RichColor",pd.name,pd.fieldDesc,pd.tooltip);
    QColor p = pd.val->getColor();
    parElem.setAttribute("r",QString::number(p.red()));
    parElem.setAttribute("g",QString::number(p.green()));
    parElem.setAttribute("b",QString::number(p.blue()));
    parElem.setAttribute("a",QString::number(p.alpha()));
}

void RichParameterXMLVisitor::visit( RichAbsPerc& pd )
{
    fillRichParameterAttribute("RichAbsPerc",pd.name,QString::number(pd.val->getAbsPerc()),pd.fieldDesc,pd.tooltip);
    parElem.setAttribute("min",QString::number(pd.min));
    parElem.setAttribute("max",QString::number(pd.max));
}

void RichParameterXMLVisitor::visit( RichEnum& pd )
{
    fillRichParameterAttribute("RichEnum",pd.name,QString::number(pd.val->getEnum()),pd.fieldDesc,pd.tooltip);
    parElem.setAttribute("enum_cardinality",pd.enumvalues.size());
    for(int ii = 0; ii < pd.enumvalues.size();++ii)
        parElem.setAttribute(QString("enum_val")+QString::number(ii),pd.enumvalues.at(ii));

}

//void RichParameterXMLVisitor::visit( RichFloatList& /*pd*/ )
//{
//    assert(0);
//}

void RichParameterXMLVisitor::visit(RichDynamicFloat& pd)
{
    fillRichParameterAttribute("RichDynamicFloat",pd.name,QString::number(pd.val->getDynamicFloat()),pd.fieldDesc,pd.tooltip);
    parElem.setAttribute("min",QString::number(pd.min));
    parElem.setAttribute("max",QString::number(pd.max));
}

void RichParameterXMLVisitor::visit( RichOpenFile& pd )
{
    fillRichParameterAttribute("RichOpenFile",pd.name,pd.val->getFileName(),pd.fieldDesc,pd.tooltip);
    parElem.setAttribute("exts_cardinality",pd.exts.size());
    for(int ii = 0; ii < pd.exts.size();++ii)
        parElem.setAttribute(QString("ext_val")+QString::number(ii),pd.exts[ii]);
}

void RichParameterXMLVisitor::visit( RichSaveFile& pd )
{
    fillRichParameterAttribute("RichSaveFile",pd.name,pd.val->getFileName(),pd.fieldDesc,pd.tooltip);
    parElem.setAttribute("ext",pd.ext);
}

void RichParameterXMLVisitor::visit( RichMesh& pd )
{
//	MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(pd.pd);
    fillRichParameterAttribute("RichMesh",pd.name,QString::number(pd.meshindex),pd.fieldDesc,pd.tooltip);
}

void RichParameterValueToStringVisitor::visit( RichBool& pd )
{
    if (pd.val == NULL)
        return;
    stringvalue.clear();
    if (pd.val->getBool())
        stringvalue = "true";
    else
        stringvalue = "false";
}

void RichParameterValueToStringVisitor::visit( RichInt& pd )
{
    if (pd.val == NULL)
        return;
    stringvalue.clear();
    stringvalue = QString::number(pd.val->getInt());
}

void RichParameterValueToStringVisitor::visit( RichFloat& pd )
{
    if (pd.val == NULL)
        return;
    stringvalue.clear();
    stringvalue = QString::number(pd.val->getFloat());
}

void RichParameterValueToStringVisitor::visit( RichString& pd )
{
    if (pd.val == NULL)
        return;
    stringvalue.clear();
    stringvalue = "\"" + pd.val->getString()+"\"";
}

void RichParameterValueToStringVisitor::visit( RichMatrix44f& pd )
{
    if (pd.val == NULL)
        return;
    stringvalue.clear();
    QString tmp("[");
    vcg::Matrix44f mat = pd.val->getMatrix44f();
    for(int ii = 0;ii < 4;++ii)
    {
        for(int jj = 0;jj < 4;++jj)
            tmp = tmp + QString::number(mat[ii][jj]) + ",";
    }
    tmp.replace(tmp.lastIndexOf(","),1,"]");
    stringvalue = tmp;
}

void RichParameterValueToStringVisitor::visit( RichPoint3f& pd )
{
    if (pd.val == NULL)
        return;
    stringvalue.clear();
    QString tmp("[");
    vcg::Point3f mat = pd.val->getPoint3f();
    for(int ii = 0;ii < 3;++ii)
    {
        tmp = tmp + QString::number(mat[ii]) + ",";
    }
    tmp.replace(tmp.lastIndexOf(","),1,"]");
    stringvalue = tmp;
}

void RichParameterValueToStringVisitor::visit( RichShotf& /*pd*/ )
{

}

void RichParameterValueToStringVisitor::visit( RichColor& pd )
{
    if (pd.val == NULL)
        return;
    stringvalue.clear();
    QString tmp("[");
    QColor mat = pd.val->getColor();
    tmp = tmp + QString::number(mat.red()) + "," + QString::number(mat.green()) + "," + QString::number(mat.blue()) + "," + QString::number(mat.alpha()) + "]";
    stringvalue = tmp;
}

void RichParameterValueToStringVisitor::visit( RichAbsPerc& pd )
{
    if (pd.val == NULL)
        return;
    stringvalue.clear();
    stringvalue = QString::number(pd.val->getAbsPerc());
}

void RichParameterValueToStringVisitor::visit( RichEnum& pd )
{
    if (pd.val == NULL)
        return;
    stringvalue.clear();
    stringvalue = QString::number(pd.val->getEnum());
}

//void RichParameterValueToStringVisitor::visit( RichFloatList& /*pd*/ )
//{

//}

void RichParameterValueToStringVisitor::visit( RichDynamicFloat& pd )
{
    if (pd.val == NULL)
        return;
    stringvalue.clear();
    stringvalue = QString::number(pd.val->getDynamicFloat());
}

void RichParameterValueToStringVisitor::visit( RichOpenFile& /*pd*/ )
{

}

void RichParameterValueToStringVisitor::visit( RichSaveFile& /*pd*/ )
{

}

void RichParameterValueToStringVisitor::visit( RichMesh& /*pd*/ )
{

}

