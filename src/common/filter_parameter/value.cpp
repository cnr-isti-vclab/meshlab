#include "value.h"

#include "../mesh_data_structures/meshmodel.h"

void BoolValue::fillToXMLElement(QDomElement& element) const
{
	QString v =  pval ? "true" : "false";
	element.setAttribute("value", v);
}

MeshValue::MeshValue(MeshDocument* doc, int meshind)
{
	if (doc != nullptr)
		pval = doc->meshList.at(meshind);
	else
		pval = nullptr;
}

void IntValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("value", QString::number(pval));
}

void FloatValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("value", QString::number(pval));
}

void StringValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("value", pval);
}

void Matrix44fValue::fillToXMLElement(QDomElement& element) const
{
	for(unsigned int ii = 0;ii < 16;++ii)
		element.setAttribute(QString("val")+QString::number(ii),QString::number(pval.V()[ii]));
}

void Point3fValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("x",QString::number(pval.X()));
	element.setAttribute("y",QString::number(pval.Y()));
	element.setAttribute("z",QString::number(pval.Z()));
}

void ShotfValue::fillToXMLElement(QDomElement&) const
{
	assert(0);
	//TODO!!!
}

void ColorValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("r",QString::number(pval.red()));
	element.setAttribute("g",QString::number(pval.green()));
	element.setAttribute("b",QString::number(pval.blue()));
	element.setAttribute("a",QString::number(pval.alpha()));
}

void FileValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("value", pval);
}

void MeshValue::fillToXMLElement(QDomElement&) const
{
	//nothing to do on this side... Everything is done on RichMesh side
	//maybe breaking retrocompatibility for mlx and do something here
	//could be a good idea.
}

