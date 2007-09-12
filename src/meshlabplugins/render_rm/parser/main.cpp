#include "RmXmlParser.h"

#include <QString>
#include <QDir>
#include <QFileInfoList>
#include <QCoreApplication>


#define    DIRECTORY_ESEMPI   "C:/Program Files/ATI Research Inc/RenderMonkey 1.62/Examples/GL2/"

int main(int argc, char ** argv )
{
	QCoreApplication qapp( argc, argv );

	RmXmlParser par;

	QDir dir( DIRECTORY_ESEMPI );
	if( dir.exists() == false ) {
		qDebug() << "Ciao.. edita main.cpp e mettici la directory dove hai gli esempi e ricompila";
		qDebug() << "Mica ti aspettavi che scrivessi un parser di argomenti passati da linea di comando.. vero ?";
		return 1;
	}

	if( argc == 1 ){

		QFileInfoList filelist = dir.entryInfoList();
		for( int i = 0; i < filelist.size(); i++ )
			if( filelist[i].fileName().endsWith( ".rfx" )) {
				qDebug() << "Reading" << filelist[i].fileName();
				if( !par.parse( filelist[i].filePath() ) ) {
					qDebug() << par.getError();
					return 1;
				}

				par.VarDump( true );
			}


	} else {

		QString filename = QString(DIRECTORY_ESEMPI) + "/" + argv[1];

		if( !par.parse( filename ) ) {
			qDebug() << par.getError();
			return 1;
		}

		par.VarDump();
	}

	return 0;
}

