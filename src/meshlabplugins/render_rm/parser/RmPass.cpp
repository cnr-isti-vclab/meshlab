#include "RmPass.h"

UniformVar RmPass::searchFragmentUniformVariable( QString & name) {
	return searchUniformVariable( name, RmPass::FRAGMENT);
}
UniformVar RmPass::searchVertexUniformVariable( QString & name) {
	return searchUniformVariable( name, RmPass::VERTEX);
}

UniformVar RmPass::searchUniformVariable( QString & name, enum CodeType codetype )
{
	QString & source = codetype == RmPass::FRAGMENT ? fragment : vertex;
	
	int namelen = name.length();
	int start = 0;
	int pos_name, pos_unif;

	while( (pos_name = source.indexOf( name, start )) != -1)
	{
		start += namelen;
		if(( pos_unif = source.lastIndexOf( "uniform", pos_name)) == -1 ) continue;


		QString declaration = source.mid( pos_unif, pos_name - pos_unif + namelen + 1);
		QStringList list = declaration.split(QRegExp("\\s+"));

		if( list.size() != 3 ) continue;
		if( list[0] != "uniform" ) continue;
		if( list[2] != name && list[2] != name + QString(";") ) continue;
		
		return UniformVar( name, list[1], UniformVar::getTypeFromString( list[1] ) );
	}
	return UniformVar();
}


