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

#include "transferfunction.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

#include <algorithm>

#ifdef NOW_TESTING
#include <cmath>
#endif

using namespace std;

bool TfKeyPCompare(TF_KEY*k1, TF_KEY*k2)
{	return (k1->x < k2->x);	}


//TRANSFER FUNCTION CHANNEL
TfChannel::TfChannel()
{
	//da eliminare!? MAL
#ifdef NOW_TESTING
	this->testInitChannel();
#endif
}

TfChannel::TfChannel(TF_CHANNELS type) : _type(type)
{
#ifdef NOW_TESTING
	this->testInitChannel();
#endif
}

TfChannel::~TfChannel(void)
{
	KEY_LISTiterator it;
	TF_KEY *k = 0;
	for (it=KEYS.begin(); it!=KEYS.end(); it++)
	{
		k = *it;
		delete k;
		k = 0;
	}
	KEYS.clear();
}

void TfChannel::setType(TF_CHANNELS type)
{	_type = type;	}

TF_CHANNELS TfChannel::getType()
{	return _type;	}



//adds to the keys list new_key
//returns a pointer to the key just added
TF_KEY* TfChannel::addKey(float xVal, float yVal)
{
	assert(xVal>=0.0f);
	assert(yVal>=0.0f);
	return this->addKey(new TF_KEY(xVal, yVal));
}

//adds to the keys list a new keys with fields passed to the method
//returns a pointer to the key just added
TF_KEY* TfChannel::addKey(TF_KEY *newKey)
{
	assert(newKey->x>=0);
	assert(newKey->y>=0);
	for (KEY_LISTiterator it=KEYS.begin(); it!=KEYS.end(); it++)
	{
		if ( (*it)->x >= newKey->x )
		{
			KEYS.insert(it, newKey);
			return newKey;
		}
	}

	KEYS.push_back(newKey);
	return newKey;
}

//removes from keys list to_remove_key
//returns the x value of the removed key or -1 if key was not found
void TfChannel::removeKey(int keyIdx)
{
	KEY_LISTiterator it = KEYS.begin();
	if ((keyIdx >= 0) && (keyIdx<(int)KEYS.size()))
	{
		it += (keyIdx * TF_KEYsize);
		delete *it;
		KEYS.erase(it);
	}
}

//removes from keys list the key whose x value is x_val
//returns the x value of the removed key or -1 if key was not found
void TfChannel::removeKey(TF_KEY *toRemoveKey)
{
	//searching key with proper x
	for (KEY_LISTiterator it=KEYS.begin(); it!=KEYS.end(); it++)
		if ( (*it) == toRemoveKey )
		{
			delete *it;
			KEYS.erase(it);
			break;
		}
}


float TfChannel::getChannelValuef(float xVal)
{
	float result = 0.0f;

	//if a x_position is known x, its key value is returned immediately
	for (KEY_LISTiterator it=KEYS.begin(); it!=KEYS.end(); it++)
		if ( (*it)->x >= xVal )
			if ( (*it)->x == xVal )
				return (*it)->y;
			else
			{
				float x2 = (*it)->x;
				float y2 = (*it)->y;
				it--;
				float x1 = (*it)->x;
				float y1 = (*it)->y;

				if (( x1 < xVal ) && ( x2 > xVal) )
				{
					//applying linear interpolation between two keys values

					//angular coefficient for interpolating line
					float m = (y2-y1) / (x2-x1);

					//returning f(x) value for x in the interpolating line
					result = m * (xVal - x1) + y1;
				}
				break;
			}

	return result;
}


UINT8 TfChannel::getChannelValueb(float xVal)
{	return (UINT8)relative2AbsoluteVali( this->getChannelValuef(xVal), 255.0f );	}

//returns true if the key has x=0.0
bool TfChannel::isHead(TF_KEY *key)
{
	assert(key!=0);
	return ( key->x == 0.0f );
}

//returns true if the key has x=1.0
bool TfChannel::isTail(TF_KEY *key)
{
	assert(key!=0);
	return ( key->x == 1.0f );
}

//this method is called by TFHandle and is used to update the TfHandle position from graphics to logical level.
//When the key value is updated, the keys list must be checked to restore the sorting and the right alternation of LEFT\RIGHT JUNCTION SIDE keys
void TfChannel::updateKeysOrder()
{	sort(KEYS.begin(), KEYS.end(), TfKeyPCompare);	}


TF_KEY* TfChannel::operator [](float xVal)
{
	for (KEY_LISTiterator it=KEYS.begin(); it!=KEYS.end(); it++)
		if ( (*it)->x == xVal )
			return (*it);

	return 0;
}

TF_KEY* TfChannel::operator [](int i)
{
	if ((i >= 0) && (i<(int)KEYS.size()))
		return KEYS[i];

	return 0;
}

#if 0
void TfChannel::updateKey(float old_x, float new_x, float new_y)
{
	KEY_LISTiterator it = KEYS.find(old_x);
	if ( it != KEYS.end())
	{
		//		assert(it == KEYS.end());

		TF_KEY *k = it->second;
		this->removeKey(old_x);

		k->y_lower = k->y_upper = new_y;
		this->addKey(new_x, k);
	}
}
#endif


#ifdef NOW_TESTING
void TfChannel::testInitChannel()
{
	int num_of_keys = (rand() % 10) + 1;
	float rand_x = 0.0f;
	float rand_y = 0.0f;
	float offset = 0.0f;

	//first node\key = 0
	this->addKey(0.0f, 0.0f, 0.0f);
	for (int i=0; i<num_of_keys; i++)
	{
		rand_x = ((rand() % 100) + 1) / 100.0f;
		rand_y = ((rand() % 100) + 1) / 100.0f;
		offset = ((rand() % 100) + 1) / 100.0f;
		if (offset > (1.0f-rand_y))
			offset = (1.0f-rand_y);
		this->addKey(rand_x,
			rand_y,
			rand_y + offset);
	}

	this->addKey(1.0f, 0.0f, 0.0f);
}
#endif







//TRANSFER FUNCTION

//declaration of static member of TransferFunction class
QString TransferFunction::defaultTFs[NUMBER_OF_DEFAULT_TF];


TransferFunction::TransferFunction(void)
{
	this->initTF();
}

TransferFunction::TransferFunction(DEFAULT_TRANSFER_FUNCTIONS code)
{
	this->initTF();

	switch(code)
	{
	case GREY_SCALE_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f);
		_channels[RED_CHANNEL].addKey(1.0f,1.0f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,1.0f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,1.0f);
		break;
	case RGB_TF:
		_channels[RED_CHANNEL].addKey(0.0f,1.0f);
		_channels[RED_CHANNEL].addKey(0.5f,0.0f);
		_channels[RED_CHANNEL].addKey(1.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.5f,1.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(0.5f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,1.0f);
		//added for test
// 		_channels[RED_CHANNEL].addKey(0.5f,0.5f,TF_KEY::LEFT_JUNCTION_SIDE);
//		_channels[GREEN_CHANNEL].addKey(0.5f,0.7f,TF_KEY::RIGHT_JUNCTION_SIDE);
// 		_channels[GREEN_CHANNEL].addKey(0.75f,1.0f,TF_KEY::LEFT_JUNCTION_SIDE);
// 		_channels[GREEN_CHANNEL].addKey(0.75f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
// 		_channels[GREEN_CHANNEL].addKey(0.2f,0.3f,TF_KEY::LEFT_JUNCTION_SIDE);

		break;
	case RED_SCALE_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f);
		_channels[RED_CHANNEL].addKey(1.0f,1.0f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,0.0f);
		break;
	case GREEN_SCALE_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f);
		_channels[RED_CHANNEL].addKey(1.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,1.0f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,0.0f);
		break;
	case BLUE_SCALE_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f);
		_channels[RED_CHANNEL].addKey(1.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,1.0f);
		break;
	case FLAT_TF:
	default:
		_channels[RED_CHANNEL].addKey(0.0f,0.5f);
		_channels[RED_CHANNEL].addKey(1.0f,0.5f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.5f);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.5f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.5f);
		_channels[BLUE_CHANNEL].addKey(1.0f,0.5f);
		break;
	}
}


TransferFunction::TransferFunction(QString fileName)
{
	this->initTF();

	//	QString fileName = QFileDialog::getSaveFileName( 0, "Save Transfer Function File", fn + CSV_FILE_EXSTENSION, "CSV File (*.csv)" );

	QFile inFile( fileName );

	if ( !inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream inStream( &inFile );
	QString line;
	QStringList splittedString;

	int channel_code = 0;
	do
	{
		line = inStream.readLine();

		//if a line is a comment, it's not processed. imply ignoring it!
		if ( !line.startsWith(CSV_FILE_COMMENT) )
		{
			splittedString = line.split(CSV_FILE_SEPARATOR, QString::SkipEmptyParts);
			assert( (splittedString.size() % 2) == 0 );

			for ( int i=0; i<splittedString.size(); i+=2 )
				_channels[channel_code].addKey( splittedString[i].toFloat(), splittedString[i+1].toFloat() );

			channel_code ++;
		}
	} while( (!line.isNull()) && (channel_code < NUMBER_OF_CHANNELS) );

	inFile.close();
}

TransferFunction::~TransferFunction(void)
{
}


void TransferFunction::initTF()
{
	//Initializing channels types and pivoting indexes.
	//Each index in channel order has the same value of the enum
	for (int i=0; i<NUMBER_OF_CHANNELS; i++)
	{
		_channels[i].setType((TF_CHANNELS)i);
		_channels_order[i] = i;
	}

	//resetting color band value
	memset(_color_band,0,sizeof(_color_band));

	defaultTFs[GREY_SCALE_TF] = "Grey Scale";
	defaultTFs[RGB_TF] = "RGB";
	defaultTFs[RED_SCALE_TF] = "Red Scale";
	defaultTFs[GREEN_SCALE_TF] = "Green Scale";
	defaultTFs[BLUE_SCALE_TF] = "Blue Scale";
	defaultTFs[FLAT_TF] = "Flat";
}


int TransferFunction::size()
{
	int result = 0;
	for (int i=0; i<NUMBER_OF_CHANNELS; i++)
		if ( _channels[i].size() > result )
			result = _channels[i].size();

	return result;
}

QColor* TransferFunction::buildColorBand()
{
	float relative_pos = 0.0f; 
	for (int i=0; i<COLOR_BAND_SIZE; i++)
	{
		relative_pos = absolute2RelativeValf((float)i, COLOR_BAND_SIZE);
		_color_band[i].setRgbF( _channels[RED_CHANNEL].getChannelValuef( relative_pos),
								_channels[GREEN_CHANNEL].getChannelValuef( relative_pos ),
								_channels[BLUE_CHANNEL].getChannelValuef( relative_pos ) );
	}
	return _color_band;
}

Color4b TransferFunction::getColorByQuality (float percentageQuality)
{
	return Color4b(_channels[RED_CHANNEL].getChannelValueb( percentageQuality ), 
		_channels[GREEN_CHANNEL].getChannelValueb( percentageQuality ), 
		_channels[BLUE_CHANNEL].getChannelValueb( percentageQuality ),
		255 );
}

QString TransferFunction::saveColorBand( QString fn, EQUALIZER_INFO& info  )
{
	QString fileName = QFileDialog::getSaveFileName( 0, "Save Transfer Function File", fn + CSV_FILE_EXSTENSION, "CSV File (*.csv)" );

	QFile outFile( fileName );

	if ( !outFile.open(QIODevice::WriteOnly | QIODevice::Text))
		return fileName;

	QTextStream outStream( &outFile );
	outStream << CSV_FILE_COMMENT << " COLOR BAND FILE STRUCTURE - first row: RED CHANNEL DATA - second row GREEN CHANNEL DATA - third row: BLUE CHANNEL DATA" << endl;
	outStream << CSV_FILE_COMMENT << " CHANNEL DATA STRUCTURE - the channel structure is grouped in many triples. The items of each triple represent respectively: X VALUE, Y_LOWER VALUE, Y_UPPER VALUE of each node-key of the transfer function" << endl;

	TF_KEY *val = 0;
	for ( int i=0; i<NUMBER_OF_CHANNELS; i++)
	{
		for (int j=0; j<_channels[i].size(); j++)
		{
			val = _channels[i][j];
			assert(val != 0);
			outStream << val->x << CSV_FILE_SEPARATOR << val->y << CSV_FILE_SEPARATOR;
		}
		outStream << endl;
	}

	outStream << CSV_FILE_COMMENT << "THE FOLLOWING 4 VALUES REPRESENT EQUALIZER SETTINGS - the first and the third values represent respectively the minimum and the maximum quality values used in histogram, the second one represent the position (in percentage) of the middle quality, and the last one represent the level of brightness as a floating point number (0 copletely dark, 1 original brightness, 2 completely white)" << endl;
	outStream << info.minQualityVal << CSV_FILE_SEPARATOR << info.midQualityPercentage << CSV_FILE_SEPARATOR << info.maxQualityVal << CSV_FILE_SEPARATOR << info.brightness << CSV_FILE_SEPARATOR << endl;

	outFile.close();

	return fileName;
}


void TransferFunction::moveChannelAhead(TF_CHANNELS ch_code)
{
	int ch_code_int = (int)ch_code;
	assert( (ch_code_int>=0) && (ch_code_int<NUMBER_OF_CHANNELS) );

	if ( _channels_order[NUMBER_OF_CHANNELS-1] == ch_code_int )
		return ;

	int tmp = 0;
	do 
	{
		tmp = _channels_order[NUMBER_OF_CHANNELS-1];
		for (int i=NUMBER_OF_CHANNELS-1; i>=1; i--)
			//_channels_order[i] = _channels_order[i-1] % (NUMBER_OF_CHANNELS -1);
			_channels_order[i] = _channels_order[i-1];

		_channels_order[0] = tmp;
	} while( _channels_order[NUMBER_OF_CHANNELS-1] != ch_code_int );
}
