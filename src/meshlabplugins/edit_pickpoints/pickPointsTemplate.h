/*  A class representing a set of point names that is used
 * as a template a user can fill out to create a PickedPoints
 * 
 * 
 * @author Oscar Barney
 */

#include <QString>
#include <vector>

class PickPointsTemplate
{
public:
	
	static bool save(QString filename, std::vector<QString> *pointNameVector);
	
	static bool load(QString filename, std::vector<QString> *pointNameVector);
	
	static QString getDefaultTemplateFileName();
	
	//extension of the filetype for PickPointsTemplate
	static const QString fileExtension;
	
private:
	PickPointsTemplate(){};
	
	//root name of the xml document
	static const QString rootName;

	//point element's name
	static const QString pointElementName;

	//point's name
	static const QString pointName;

};
