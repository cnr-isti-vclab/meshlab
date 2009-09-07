/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2008                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef MESHLAB_FILTERPARAMETER_H
#define MESHLAB_FILTERPARAMETER_H
#include <QtCore>
#include <QtXml>

#include <QMap>
#include<QString>
#include <QPair>
#include <QAction>
#include <vcg/math/matrix44.h>
#include <meshlab/meshmodel.h>



class Value
{
public:
	virtual bool				getBool() const {assert(0);return bool();}
	virtual int					getInt() const {assert(0);return int();}
	virtual float				getFloat() const {assert(0);return float();}
	virtual QString			getString() const	{assert(0);return QString();}
	virtual vcg::Matrix44f		getMatrix44f() const {assert(0);return vcg::Matrix44f();}
	virtual vcg::Point3f getPoint3f() const {assert(0);return vcg::Point3f();}
	virtual QColor		   getColor() const {assert(0);return QColor();}
	virtual vcg::Color4b getColor4b() const {assert(0);return vcg::Color4b();}
	virtual float		     getAbsPerc() const {assert(0);return float();}
	virtual int					 getEnum() const {assert(0);return int();}
	virtual MeshModel*   getMesh() const {assert(0);return NULL;}
	virtual QList<float> getFloatList() const {assert(0);return QList<float>();}
	virtual float        getDynamicFloat() const {assert(0);return float();}
	virtual QString getFileName() const {assert(0);return QString();}

	virtual bool isBool() const {return false;}
	virtual bool isInt() const {return false;}
	virtual bool isFloat() const {return false;}
	virtual bool isString() const {return false;}
	virtual bool isMatrix44f() const {return false;}
	virtual bool isPoint3f() const {return false;}
	virtual bool isColor() const {return false;}
	virtual bool isColor4b() const {return false;}
	virtual bool isAbsPerc() const {return false;}
	virtual bool isEnum() const {return false;}
	virtual bool isMesh() const {return false;}
	virtual bool isFloatList() const {return false;}
	virtual bool isDynamicFloat() const {return false;}
	virtual bool isFileName() const {return false;}


	virtual void	set(const Value& p) = 0;
	virtual ~Value(){}
};

class BoolValue : public Value
{
public:
	BoolValue(const bool val): pval(val){};
	inline bool getBool() const {return pval;}
	inline bool isBool() const {return true;}
	inline void	set(const Value& p) {pval  = p.getBool();}
	~BoolValue() {}
private:
	bool pval;
};

class IntValue : public Value
{
public:
	IntValue(const int val) : pval(val){};
	inline int	getInt() const {return pval;}
	inline bool isInt() const {return true;}
	inline void	set(const Value& p) {pval = p.getInt();}
	~IntValue(){}
private:
	int pval;
};

class FloatValue : public Value
{
public:
	FloatValue(const float val) :pval(val){};
	inline float	getFloat() const {return pval;}
	inline bool isFloat() const {return true;}
	inline void	set(const Value& p) {pval = p.getFloat();}
	~FloatValue(){}
private:
	float pval;
};

class StringValue : public Value
{
public:
	StringValue(const QString& val) :pval(val){};
	inline QString getString() const {return pval;}
	inline bool isString() const {return true;}
	inline void	set(const Value& p) {pval = p.getString();}
	~StringValue(){}
private:
	QString pval;
};

class Matrix44fValue : public Value
{
public:
	Matrix44fValue(const vcg::Matrix44f& val) :pval(val){};
	inline vcg::Matrix44f getMatrix44f() const {return pval;}
	inline bool isMatrix44f() const {return true;}
	inline void	set(const Value& p){pval = p.getMatrix44f();}
	~Matrix44fValue(){}
private:
	vcg::Matrix44f pval;
};

class Point3fValue : public Value
{
public:
	Point3fValue(const vcg::Point3f& val) : pval(val){};
	inline vcg::Point3f getPoint3f() const {return pval;}
	inline bool isPoint3f() const {return true;}
	inline void	set(const Value& p) {pval = p.getPoint3f();}
	~Point3fValue(){} 
private:
	vcg::Point3f pval;
};

class ColorValue : public Value
{
public:
	ColorValue(QColor val) :pval(val){};
	inline QColor getColor() const {return pval;}
	inline bool isColor() const {return true;}
	inline void	set(const Value& p) {pval = p.getColor();}
	~ColorValue(){}
private:
	QColor pval;
};

class Color4bValue : public Value
{
public:
	Color4bValue(QString name,const vcg::Color4b val) :pval(val){};
	inline vcg::Color4b getColor4b() const {return pval;}
	inline bool isColor4b() const {return true;}
	inline void	set(const Value& p) {pval = p.getColor4b();}
	~Color4bValue(){}

private:
	vcg::Color4b pval;
};

class AbsPercValue : public FloatValue
{
public:
	AbsPercValue(const float val) :FloatValue(val){};
	inline float getAbsPerc() const {return getFloat();}
	inline bool isAbsPerc() const {return true;}
	~AbsPercValue(){}
};

class EnumValue : public IntValue
{
public:
	EnumValue(const int val) :IntValue(val){};
	inline int getEnum() const {return getInt();}
	inline bool isEnum() const {return true;}
	~EnumValue(){}
};

class FloatListValue : public Value
{
public:
	FloatListValue(QList<float>& val) :pval(val){};
	inline QList<float> getFloatList() const  {return pval;}
	inline void	set(const Value& p) {pval = p.getFloatList();}
	inline bool isFloatList() const {return true;}
	~FloatListValue() {}
private:
	QList<float> pval;
};

class DynamicFloatValue : public FloatValue
{
public:
	DynamicFloatValue(const float val) :FloatValue(val){};
	inline float getDynamicFloat() const {return getFloat();}
	inline bool isDynamicFloat() const {return true;}
	~DynamicFloatValue() {}
};

class FileValue : public Value
{
public:
	FileValue(QString filename) :pval(filename){};
	inline QString getFileName() const {return pval;}
	inline bool isFileName() const {return true;}
	inline void	set(const Value& p) {pval = p.getFileName();}
	~FileValue(){}
private:
	QString pval;
};

class MeshValue : public Value
{
public:
	MeshValue(MeshModel* mesh) : pval(mesh){};
	inline MeshModel* getMesh() const {return pval;}
	inline bool isMesh() const {return true;}
	inline void	set(const Value& p) {pval = p.getMesh();}
	~MeshValue(){}

private:
	MeshModel* pval;
};

/******************************/

class ParameterDecoration
{
public:
        QString fieldDesc;
        QString tooltip;
        Value* defVal;

	ParameterDecoration(Value* defvalue,const QString desc = QString(),const QString tltip = QString())
		:fieldDesc(desc),tooltip(tltip),defVal(defvalue) {}

	virtual ~ParameterDecoration(){delete defVal;}

};

class BoolDecoration : public ParameterDecoration
{
public:
	BoolDecoration(BoolValue* defvalue,const QString desc=QString(),const QString tltip=QString())
		:ParameterDecoration(defvalue,desc,tltip) {}
	~BoolDecoration(){}
};

class IntDecoration : public ParameterDecoration
{
public:
	IntDecoration(IntValue* defvalue,const QString desc = QString(),const QString tltip = QString())
	:ParameterDecoration(defvalue,desc,tltip) {}
	~IntDecoration(){}
};

class FloatDecoration : public ParameterDecoration
{
public:
	FloatDecoration(FloatValue* defvalue,const QString desc = QString(),const QString tltip = QString())
	:ParameterDecoration(defvalue,desc,tltip) {}
	~FloatDecoration(){}
};

class StringDecoration : public ParameterDecoration
{
public:
	StringDecoration(StringValue* defvalue,const QString desc = QString(),const QString tltip = QString())
		:ParameterDecoration(defvalue,desc,tltip) {}
	~StringDecoration(){}
};

class Matrix44fDecoration : public ParameterDecoration
{
public:
	Matrix44fDecoration(Matrix44fValue* defvalue,const QString desc = QString(),const QString tltip = QString())
		:ParameterDecoration(defvalue,desc,tltip) {}
	~Matrix44fDecoration(){}
};

class Point3fDecoration : public ParameterDecoration
{
public:
	Point3fDecoration(Point3fValue* defvalue,const QString desc = QString(),const QString tltip = QString())
		:ParameterDecoration(defvalue,desc,tltip) {}
	~Point3fDecoration(){}
};

class ColorDecoration : public ParameterDecoration
{
public:
	ColorDecoration(ColorValue* defvalue,const QString desc = QString(),const QString tltip= QString())
		:ParameterDecoration(defvalue,desc,tltip) {}
	~ColorDecoration(){}
};

class Color4bDecoration : public ParameterDecoration
{
public:
	Color4bDecoration(Color4bValue* defvalue,const QString desc = QString(),const QString tltip = QString())
		:ParameterDecoration(defvalue,desc,tltip) {}
	~Color4bDecoration(){}
};

class AbsPercDecoration : public ParameterDecoration
{
public:
	AbsPercDecoration(AbsPercValue* defvalue,const float minVal,const float maxVal,const QString desc = QString(),const QString tltip = QString())
		:ParameterDecoration(defvalue,desc,tltip),min(minVal),max(maxVal) {}
	float min;
	float max;
	~AbsPercDecoration(){}
};

class EnumDecoration : public ParameterDecoration
{
public:
	EnumDecoration(EnumValue* defvalue, QStringList values,const QString desc = QString(),const QString tltip = QString())
	:ParameterDecoration(defvalue,desc,tltip),enumvalues(values) {}
	QStringList enumvalues;
	~EnumDecoration(){}
};

class FloatListDecoration : public ParameterDecoration
{
public:
	FloatListDecoration(FloatListValue* defvalue,const QString desc = QString(),const QString tltip = QString())
	:ParameterDecoration(defvalue,desc,tltip) {}
	~FloatListDecoration(){}
};

class DynamicFloatDecoration : public ParameterDecoration
{
public:
	DynamicFloatDecoration(DynamicFloatValue* defvalue, const float minVal,const float maxVal,const QString desc = QString(),const QString tltip = QString())
	:ParameterDecoration(defvalue,desc,tltip),min(minVal),max(maxVal) {}
	~DynamicFloatDecoration(){};
	float min;
	float max;
};

class FileDecoration : public ParameterDecoration
{
public:
	FileDecoration(FileValue* defvalue,const QString extension=QString(".*"),const QString desc = QString(),const QString tltip = QString())
		:ParameterDecoration(defvalue,desc,tltip),ext(extension) {}
	~FileDecoration(){}
	
	QString ext;
};


class MeshDecoration : public ParameterDecoration
{
public:
	MeshDecoration(MeshValue* defvalue,MeshDocument* doc,const QString desc=QString(), const QString tltip=QString())
		:ParameterDecoration(defvalue,desc,tltip),meshdoc(doc) 
	{ 
		meshindex = -1;
		if (doc != NULL) meshindex = doc->meshList.indexOf(defvalue->getMesh()); 
		assert((meshindex != -1) || (doc == NULL));
	}

	MeshDecoration(int meshind,MeshDocument* doc,const QString desc=QString(), const QString tltip=QString())
		:ParameterDecoration(NULL,desc,tltip),meshdoc(doc) 
		{ 
			assert(meshind < doc->size() && meshind >= 0); 
			meshindex = meshind;
			if (doc != NULL)
				defVal = new MeshValue(doc->meshList.at(meshind));
		}

	//WARNING: IT SHOULD BE USED ONLY BY MESHLABSERVER!!!!!!!
	MeshDecoration(int meshind)
		:ParameterDecoration(NULL,QString(),QString()),meshdoc(NULL),meshindex(meshind)
	{
	}

	~MeshDecoration(){}

	MeshDocument* meshdoc;
	int meshindex;
};

/******************************/
class RichBool;
class RichInt;
class RichFloat;
class RichString;
class RichMatrix44f;
class RichPoint3f;
class RichColor;
class RichColor4b;
class RichAbsPerc;
class RichEnum;
class RichFloatList;
class RichDynamicFloat;
class RichOpenFile;
class RichSaveFile;
class RichMesh;


class Visitor
{
public:

	virtual void visit( RichBool& pd) = 0;
	virtual void visit( RichInt& pd) = 0;
	virtual void visit( RichFloat& pd) = 0;
	virtual void visit( RichString& pd) = 0;
	virtual void visit( RichMatrix44f& pd) = 0;
	virtual void visit( RichPoint3f& pd) = 0;
	virtual void visit( RichColor& pd) = 0;
	virtual void visit( RichColor4b& pd) = 0;
	virtual void visit( RichAbsPerc& pd) = 0;
	virtual void visit( RichEnum& pd) = 0;
	virtual void visit( RichFloatList& pd) = 0;
	virtual void visit( RichDynamicFloat& pd) = 0;
	virtual void visit( RichOpenFile& pd) = 0;
	virtual void visit( RichSaveFile& pd) = 0;
	virtual void visit( RichMesh& pd) = 0;

	virtual ~Visitor() {}
};

class RichParameter
{
public:
	const QString name;
	
	Value* val;

	ParameterDecoration* pd;

	RichParameter(const QString nm,Value* v,ParameterDecoration* prdec)
		:name(nm),pd(prdec),val(v) {}
	virtual void accept(Visitor& v) = 0;
	virtual bool operator==(const RichParameter& rp) = 0;
	virtual ~RichParameter() {delete val;delete pd;}
};


class RichBool : public RichParameter
{
public:
	RichBool(const QString nm,const bool defval,const QString desc=QString(),const QString tltip=QString()) 
		: RichParameter(nm,new BoolValue(defval),new BoolDecoration(new BoolValue(defval),desc,tltip)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isBool() && (name == rb.name) && (val->getBool() == rb.val->getBool()));}

	~RichBool(){}
};

class RichInt : public RichParameter
{
public:
	RichInt(const QString nm,const int defval,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new IntValue(defval),new IntDecoration(new IntValue(defval),desc,tltip)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isInt() &&(name == rb.name) && (val->getInt() == rb.val->getInt()));}
	~RichInt(){}
};

class RichFloat : public RichParameter
{
public:
	RichFloat(const QString nm,const float defval,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new FloatValue(defval),new FloatDecoration(new FloatValue(defval),desc,tltip)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isFloat() &&(name == rb.name) && (val->getFloat() == rb.val->getFloat()));}
	~RichFloat(){}
};

class RichString : public RichParameter
{
public:
	RichString(const QString nm,const QString defval,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new StringValue(defval),new StringDecoration(new StringValue(defval),desc,tltip)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isString() &&(name == rb.name) && (val->getString() == rb.val->getString()));}
	~RichString(){}
};

class RichMatrix44f : public RichParameter
{
public:
	RichMatrix44f(const QString nm,const vcg::Matrix44f& defval,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new Matrix44fValue(defval),new Matrix44fDecoration(new Matrix44fValue(defval),desc,tltip)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isMatrix44f() &&(name == rb.name) && (val->getMatrix44f() == rb.val->getMatrix44f()));}
	~RichMatrix44f() {}
};

class RichPoint3f : public RichParameter
{
public:
	RichPoint3f(const QString nm,const vcg::Point3f defval,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new Point3fValue(defval),new Point3fDecoration(new Point3fValue(defval),desc,tltip)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isPoint3f() &&(name == rb.name) && (val->getPoint3f() == rb.val->getPoint3f()));}
	~RichPoint3f(){}
};

class RichColor : public RichParameter
{
public:
	RichColor(const QString nm,const QColor defval,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new ColorValue(defval),new ColorDecoration(new ColorValue(defval),desc,tltip)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isColor() &&(name == rb.name) && (val->getColor() == rb.val->getColor()));}
	~RichColor(){}

};

class RichColor4b : public RichParameter
{
public:
	RichColor4b(const QString nm,Color4bValue* v,Color4bDecoration* prdec)
		:RichParameter(nm,v,prdec){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isColor4b() &&(name == rb.name) && (val->getColor4b() == rb.val->getColor4b()));}
	~RichColor4b() {}
};

class RichAbsPerc : public RichParameter
{
public:
	RichAbsPerc(const QString nm,const float defval,const float minval,const float maxval,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new AbsPercValue(defval),new AbsPercDecoration(new AbsPercValue(defval),minval,maxval,desc,tltip)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isAbsPerc() &&(name == rb.name) && (val->getAbsPerc() == rb.val->getAbsPerc()));}
	~RichAbsPerc() {}
};

class RichEnum : public RichParameter
{
public:
	RichEnum(const QString nm,const int defval,const QStringList values,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new EnumValue(defval),new EnumDecoration(new EnumValue(defval),values,desc,tltip)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isEnum() &&(name == rb.name) && (val->getEnum() == rb.val->getEnum()));}
	~RichEnum(){}
};

class RichMesh : public RichParameter
{
public:
	RichMesh(const QString nm,MeshModel* defval,MeshDocument* doc,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new MeshValue(defval),new MeshDecoration(new MeshValue(defval),doc,desc,tltip)){}

	RichMesh(const QString nm,int meshindex,MeshDocument* doc,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,NULL,new MeshDecoration(meshindex,doc,desc,tltip))
	{ 
		assert(meshindex < doc->size() && meshindex >= 0); 
		val = new MeshValue(doc->meshList.at(meshindex));
	}

	//WARNING: IT SHOULD BE USED ONLY BY MESHLABSERVER!!!!!!!
	RichMesh(const QString nm,int meshindex)
		:RichParameter(nm,new MeshValue(NULL),new MeshDecoration(meshindex)) {}
	
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isMesh() &&(name == rb.name) && (val->getMesh() == rb.val->getMesh()));}
	~RichMesh(){}
};

class RichFloatList : public RichParameter
{
public:
	RichFloatList(const QString nm,FloatListValue* v,FloatListDecoration* prdec)
		:RichParameter(nm,v,prdec){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isFloatList() &&(name == rb.name) && (val->getFloatList() == rb.val->getFloatList()));}
	~RichFloatList(){}
};

class RichDynamicFloat : public RichParameter
{
public:
	RichDynamicFloat(const QString nm,const float defval,const float minval,const float maxval,const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new DynamicFloatValue(defval),new DynamicFloatDecoration(new DynamicFloatValue(defval),minval,maxval,desc,tltip)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isDynamicFloat() &&(name == rb.name) && (val->getDynamicFloat() == rb.val->getDynamicFloat()));}
	~RichDynamicFloat(){}

};

class RichOpenFile : public RichParameter
{
public:
	RichOpenFile(const QString nm,const QString defval,const QString ext = QString("*.*"),const QString desc=QString(),const QString tltip=QString())
		:RichParameter(nm,new FileValue(defval),new FileDecoration(new FileValue(defval),tltip,desc)){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isFileName() &&(name == rb.name) && (val->getFileName() == rb.val->getFileName()));}
	~RichOpenFile(){}
};

class RichSaveFile : public RichParameter
{
public:
	RichSaveFile(const QString nm,FileValue* v,FileDecoration* prdec)
		:RichParameter(nm,v,prdec){}
	void accept(Visitor& v) {v.visit(*this);}
	bool operator==(const RichParameter& rb) {return (rb.val->isFileName() &&(name == rb.name) && (val->getFileName() == rb.val->getFileName()));}
	~RichSaveFile(){}
};



/******************************/

class RichParameterCopyConstructor : public Visitor
{
public:
	RichParameterCopyConstructor(){}

	void visit(RichBool& pd);
	void visit(RichInt& pd);
	void visit(RichFloat& pd);
	void visit(RichString& pd);
	void visit(RichMatrix44f& pd);
	void visit(RichPoint3f& pd);
	void visit(RichColor& pd);
	void visit(RichColor4b& pd);
	void visit(RichAbsPerc& pd);

	void visit(RichEnum& pd);
	void visit(RichFloatList& pd);

	void visit(RichDynamicFloat& pd);

	void visit(RichOpenFile& pd);
	void visit(RichSaveFile& pd);
	void visit(RichMesh& pd);

	~RichParameterCopyConstructor() {}

	RichParameter* lastCreated;
};

class RichParameterFactory
{
public:
	static void create(const QDomElement& np,RichParameter** par);
};

class RichParameterXMLVisitor : public Visitor
{
public:
	RichParameterXMLVisitor(QDomDocument& doc) : docdom(doc){}

	void visit(RichBool& pd);
	void visit(RichInt& pd);
	void visit(RichFloat& pd);
	void visit(RichString& pd);
	void visit(RichMatrix44f& pd);
	void visit(RichPoint3f& pd);
	void visit(RichColor& pd);
	void visit(RichColor4b& pd);
	void visit(RichAbsPerc& pd);

	void visit(RichEnum& pd);
	void visit(RichFloatList& pd);

	void visit(RichDynamicFloat& pd);

	void visit(RichOpenFile& pd);
	void visit(RichSaveFile& pd);
	void visit(RichMesh& pd);
	
	~RichParameterXMLVisitor(){}

	QDomDocument docdom;
	QDomElement parElem;
private:
	void fillRichParameterAttribute(const QString& type,const QString& name);
	void fillRichParameterAttribute(const QString& type,const QString& name,const QString& val);
};

class RichParameterSet
{

public:
	RichParameterSet():paramList(){}	
	RichParameterSet(const RichParameterSet& rps);
	// The data is just a list of Parameters
	//QMap<QString, FilterParameter *> paramMap;  
	QList<RichParameter*> paramList;  
	bool isEmpty() const {return paramList.isEmpty();} 
	RichParameter* findParameter(QString name);
	const RichParameter* findParameter(QString name) const;
	bool hasParameter(QString name);


	RichParameterSet& operator=(const RichParameterSet& rps);
	RichParameterSet& copy(const RichParameterSet& rps);
	bool operator==(const RichParameterSet& rps);

	RichParameterSet& addParam(RichParameter* pd);

	//remove a parameter from the set by name
	RichParameterSet& removeParameter(QString name);

	void clear() { paramList.clear(); }

	void setValue(const QString name,const Value& val);

	bool				getBool(QString name) const;
	int					getInt(QString name) const;
	float				getFloat(QString name) const;
	QString			getString(QString name) const;
	vcg::Matrix44f		getMatrix44(QString name) const;
	vcg::Point3f getPoint3f(QString name) const;
	QColor		   getColor(QString name) const;
	vcg::Color4b getColor4b(QString name) const;
	float		     getAbsPerc(QString name) const;
	int					 getEnum(QString name) const;	
	MeshModel*   getMesh(QString name) const;
	QList<float> getFloatList(QString name) const;
	float        getDynamicFloat(QString name) const;
	QString getOpenFileName(QString name) const;
	QString getSaveFileName(QString name) const;


	~RichParameterSet();
};

/****************************/



#endif


