/*  A class representing a set of point names that is used
 * as a template a user can fill out to create a PickedPoints
 * 
 * 
 * @author Oscar Barney
 */


#include "pickPointsTemplate.h"

#include <QtGui>
//xml stuff
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>


//Define Constants
const QString PickPointsTemplate::fileExtension = ".xml";
const QString PickPointsTemplate::rootName = "PickPointsTemplate";
const QString PickPointsTemplate::pointElementName = "point";
const QString PickPointsTemplate::pointName = "name";


bool PickPointsTemplate::save(QString filename,
		std::vector<QString> *pointNameVector){
	QDomDocument doc(rootName);
	QDomElement root = doc.createElement(rootName);
	doc.appendChild(root);
	
	//create an element for each point
	for (int i = 0; i < pointNameVector->size(); ++i) {
		QString name = pointNameVector->at(i);
		
		QDomElement tag = doc.createElement(pointElementName);
		tag.setAttribute(pointName, name);
		
		//append the element to the root
		root.appendChild(tag);
	}
	
	//create a file and write the data
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QTextStream qstream(&file);
	doc.save(qstream,1);
	file.close();
	return true;	 
}

bool PickPointsTemplate::load(QString filename,
		std::vector<QString> *pointNameVector){
	
	QDomDocument doc;
	pointNameVector->clear();
	
	QFile file(filename);
	
	QString errorMessage;
	if (file.open(QIODevice::ReadOnly) && doc.setContent(&file, &errorMessage)) 
	{
		file.close();
		QDomElement root = doc.documentElement();
		if (root.nodeName() == rootName) 
	    {
			qDebug() << "About to read a " << rootName << " xml document";
			
			for(QDomElement element = root.firstChildElement(pointElementName);
				!element.isNull();
				element = element.nextSiblingElement(pointElementName))
			{
				QString name = element.attribute(pointName);
				qDebug() << "Reading point with name " << name;
				
				pointNameVector->push_back(name);
				
			}
	    } else {
	    	//file is of unknown type
	    	qDebug() << "Failed, tried to read a " << rootName << " xml document";
	    	return false;
	    }
		
	} else {
		// problem opening the file
		qDebug() << "problem reading from the file, setContent error: " <<  errorMessage;
		return false;
	}
	return true;
}

QString PickPointsTemplate::getDefaultTemplateFileName()
{
	return QDir::homePath() + "/.pickPointsTemplate.xml";
}
