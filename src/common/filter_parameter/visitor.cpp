#include "visitor.h"
#include "rich_parameter.h"

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
    if (pd.value().getBool())
        v = "true";
    else
        v = "false";
    fillRichParameterAttribute(
                "RichBool",
                pd.name(),
                v,
                pd.fieldDescription(),
                pd.toolTip());
}

void RichParameterXMLVisitor::visit( RichInt& pd )
{
    fillRichParameterAttribute(
                "RichInt",
                pd.name(),
                QString::number(pd.value().getInt()),
                pd.fieldDescription(),
                pd.toolTip());
}

void RichParameterXMLVisitor::visit( RichFloat& pd )
{
    fillRichParameterAttribute(
                "RichFloat",
                pd.name(),
                QString::number(pd.value().getFloat()),
                pd.fieldDescription(),
                pd.toolTip());
}

void RichParameterXMLVisitor::visit( RichString& pd )
{
    fillRichParameterAttribute(
                "RichString",
                pd.name(),
                pd.value().getString(),
                pd.fieldDescription(),
                pd.toolTip());
}

void RichParameterXMLVisitor::visit( RichMatrix44f& pd )
{
    fillRichParameterAttribute(
                "RichMatrix44f",
                pd.name(),
                pd.fieldDescription(),
                pd.toolTip());
    vcg::Matrix44f mat = pd.value().getMatrix44f();
    for(unsigned int ii = 0;ii < 16;++ii)
        parElem.setAttribute(QString("val")+QString::number(ii),QString::number(mat.V()[ii]));
}

void RichParameterXMLVisitor::visit( RichPoint3f& pd )
{
    fillRichParameterAttribute(
                "RichPoint3f",
                pd.name(),
                pd.fieldDescription(),
                pd.toolTip());
    vcg::Point3f p = pd.value().getPoint3f();
    parElem.setAttribute("x",QString::number(p.X()));
    parElem.setAttribute("y",QString::number(p.Y()));
    parElem.setAttribute("z",QString::number(p.Z()));
}

void RichParameterXMLVisitor::visit( RichShotf& pd )
{
  fillRichParameterAttribute(
              "RichShotf",
              pd.name(),
              pd.fieldDescription(),
              pd.toolTip());
  assert(0); // TODO !!!!
}


void RichParameterXMLVisitor::visit( RichColor& pd )
{
    fillRichParameterAttribute(
                "RichColor",
                pd.name(),
                pd.fieldDescription(),
                pd.toolTip());
    QColor p = pd.value().getColor();
    parElem.setAttribute("r",QString::number(p.red()));
    parElem.setAttribute("g",QString::number(p.green()));
    parElem.setAttribute("b",QString::number(p.blue()));
    parElem.setAttribute("a",QString::number(p.alpha()));
}

void RichParameterXMLVisitor::visit( RichAbsPerc& pd )
{
    fillRichParameterAttribute(
                "RichAbsPerc",
                pd.name(),
                QString::number(pd.value().getAbsPerc()),
                pd.fieldDescription(),
                pd.toolTip());
    parElem.setAttribute("min",QString::number(pd.min));
    parElem.setAttribute("max",QString::number(pd.max));
}

void RichParameterXMLVisitor::visit( RichEnum& pd )
{
    fillRichParameterAttribute(
                "RichEnum",
                pd.name(),
                QString::number(pd.value().getEnum()),
                pd.fieldDescription(),
                pd.toolTip());
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
    fillRichParameterAttribute(
                "RichDynamicFloat",
                pd.name(),
                QString::number(pd.value().getDynamicFloat()),
                pd.fieldDescription(),pd.toolTip());
    parElem.setAttribute("min",QString::number(pd.min));
    parElem.setAttribute("max",QString::number(pd.max));
}

void RichParameterXMLVisitor::visit( RichOpenFile& pd )
{
    fillRichParameterAttribute(
                "RichOpenFile",
                pd.name(),
                pd.value().getFileName(),
                pd.fieldDescription(),
                pd.toolTip());
    parElem.setAttribute("exts_cardinality",pd.exts.size());
    for(int ii = 0; ii < pd.exts.size();++ii)
        parElem.setAttribute(QString("ext_val")+QString::number(ii),pd.exts[ii]);
}

void RichParameterXMLVisitor::visit( RichSaveFile& pd )
{
    fillRichParameterAttribute(
                "RichSaveFile",
                pd.name(),
                pd.value().getFileName(),
                pd.fieldDescription(),
                pd.toolTip());
    parElem.setAttribute("ext",pd.ext);
}

void RichParameterXMLVisitor::visit( RichMesh& pd )
{
//	MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(pd.pd);
    fillRichParameterAttribute(
                "RichMesh",
                pd.name(),
                QString::number(pd.meshindex),
                pd.fieldDescription(),
                pd.toolTip());
}
