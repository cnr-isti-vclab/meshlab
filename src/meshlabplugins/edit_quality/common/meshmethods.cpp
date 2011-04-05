/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
/****************************************************************************
History
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#include "meshmethods.h"
#include <cmath>
#include <limits>
#include <QFile>
#include <QTextStream>

// Opens a CSV file and gets its equalizer parameters
//returns a negative value if some error occurred, a positive value else
int loadEqualizerInfo(QString fileName, EQUALIZER_INFO *data)
{
	int result = fileName.size();
	QFile inFile( fileName );

	if ( !inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return -1;

	QTextStream inStream( &inFile );
	QString line;
	QStringList splittedString;

	int channel_code = 0;
	do
	{
		line = inStream.readLine();

		//if a line is a comment, it's not processed. imply ignoring it!
		if ( !line.startsWith(CSV_FILE_COMMENT) )
			channel_code ++;
	} while( (!line.isNull()) && (channel_code < NUMBER_OF_CHANNELS) );

	do
	{
		line = inStream.readLine();

		//if a line is a comment, it's not processed. imply ignoring it!
		if ( !line.startsWith(CSV_FILE_COMMENT) )
		{
			splittedString = line.split(CSV_FILE_SEPARATOR, QString::SkipEmptyParts);
			assert(splittedString.size() == 4);

			data->minQualityVal = splittedString[0].toFloat();
			data->midQualityPercentage = splittedString[1].toFloat();
			data->maxQualityVal = splittedString[2].toFloat();
			data->brightness = splittedString[3].toFloat();
			break;
		}
	} while(!line.isNull());

	inFile.close();

	return result;
}

/*
void applyColorByVertexQuality(MeshModel& mesh, TransferFunction *transferFunction, float minQuality, float maxQuality, float midHandlePercentilePosition, float brightness)
{
	CMeshO::VertexIterator vi;
	float percentageQuality;
	Color4b currentColor;

	for(vi=mesh.cm.vert.begin(); vi!=mesh.cm.vert.end(); ++vi)		
		if(!(*vi).IsD()) 
		{
			float vertexQuality = (*vi).Q();
			if (vertexQuality < minQuality)
				percentageQuality = 0.0f;
			else
				if (vertexQuality > maxQuality)
					percentageQuality = 1.0f;
				else
					// calcultating relative quality and applying exponential function: rel(Q)^exp, exp=2*midHandleRelPos
					percentageQuality = pow( ((*vi).Q() - minQuality) / (maxQuality - minQuality) , (float)(2.0f*midHandlePercentilePosition));

			currentColor = transferFunction->getColorByQuality(percentageQuality);
			
			if (brightness!=1.0f) //Applying brightness to each color channel, 0<brightness<2, 1=normale brightness, 0=white, 2=black
				if (brightness<1.0f)
					for (int i=0; i<NUMBER_OF_CHANNELS; i++) 
						currentColor[i] = relative2AbsoluteVali(pow(absolute2RelativeValf(currentColor[i],255.0f),brightness), 255.0f);
				else
					for (int i=0; i<NUMBER_OF_CHANNELS; i++) 
						currentColor[i] = relative2AbsoluteVali(1.0f-pow(1.0f-absolute2RelativeValf(currentColor[i],255.0f),2-brightness), 255.0f);

			(*vi).C() = currentColor;
		}
}*/

void applyColorByVertexQuality(MeshModel& mesh, TransferFunction *transferFunction, float minQuality, float maxQuality, float midHandlePercentilePosition, float brightness)
{
	CMeshO::VertexIterator vi;
	Color4b currentColor;

	for(vi=mesh.cm.vert.begin(); vi!=mesh.cm.vert.end(); ++vi)		
		if(!(*vi).IsD()) 
			(*vi).C() = transferFunction->getColorByQuality ((*vi).Q(), minQuality, maxQuality, midHandlePercentilePosition, brightness);
}