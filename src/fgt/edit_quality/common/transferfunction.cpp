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

//used just for testing
//define NOW_TESTING macro to use some testing options
#ifdef NOW_TESTING
#include <cmath>
#endif

using namespace std;
using namespace vcg;

//function used to define < relations among TF_KEYs elements
bool TfKeyPCompare(TF_KEY*k1, TF_KEY*k2)
{	return (k1->x < k2->x);	}





//TRANSFER FUNCTION CHANNEL CODE

TfChannel::TfChannel()
{
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
	//destroying TF_KEYs
	KEY_LISTiterator it;
	TF_KEY *k = 0;
	for (it=KEYS.begin(); it!=KEYS.end(); it++)
	{
		k = *it;
		delete k;
		k = 0;
	}

	//resetting keys list
	KEYS.clear();
}

//sets the type of the channel (channel code, defined using a TF_CHANNELS list member)
void TfChannel::setType(TF_CHANNELS type)
{	_type = type;	}

//returns the type of channel (channel code defined by TF_CHANNELS list)
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
	//inserting the key in the correct position
	//x value order is kept
	for (KEY_LISTiterator it=KEYS.begin(); it!=KEYS.end(); it++)
	{
		if ( (*it)->x >= newKey->x )
		{
			KEYS.insert(it, newKey);
			return newKey;
		}
	}

	//greatest x ever
	//adding new key at the end of the list
	KEYS.push_back(newKey);
	return newKey;
}

//removes from keys list the key at index keyIdx
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

//removes from keys list the key pointer is toRemoveKey
void TfChannel::removeKey(TF_KEY *toRemoveKey)
{
	//searching key in the list...
	for (KEY_LISTiterator it=KEYS.begin(); it!=KEYS.end(); it++)
		if ( (*it) == toRemoveKey )
		{
			//found it. Deleting
			delete *it;
			KEYS.erase(it);
			break;
		}
}

//returns the value (as float) of the transfer function for a certain channel in a given point (xVal)
//if the xVal value respond to the x of a key present in the list, the corresponding y value is returned,
//else linear interpolation is effected and the resulting value is returned
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
				//xVal is not the x of a key...
				//the returning value will be obtained through linear interpolation between closest x-value keys in the list

				//acquiring position of right key
				float x2 = (*it)->x;
				float y2 = (*it)->y;
				it--;

				//acquiring position of left key
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

//returns the value (as unsigned char) of the transfer function for a certain channel in a given point (xVal)
//if the xVal value respond to the x of a key present in the list, the corresponding y value is returned,
//else linear interpolation is effected and the resulting value is returned
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

//operator redefinition. Returns the key in the key list whose x-value equals xVal
TF_KEY* TfChannel::operator [](float xVal)
{
	//looking in the list for the key with the proper x
	for (KEY_LISTiterator it=KEYS.begin(); it!=KEYS.end(); it++)
		if ( (*it)->x == xVal )
			return (*it);

	return 0;
}

//operator redefinition. Returns the key in the key list whose index equals i
TF_KEY* TfChannel::operator [](int i)
{
	if ((i >= 0) && (i<(int)KEYS.size()))
		return KEYS[i];

	return 0;
}

//CODE USED FOR TESTING (define NOW_TESTING macro to use it)
#ifdef NOW_TESTING
//addes random key to channel
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







//TRANSFER FUNCTION CODE

//declaration of static member of TransferFunction class
QString TransferFunction::defaultTFs[NUMBER_OF_DEFAULT_TF];


TransferFunction::TransferFunction(void)
{
	this->initTF();
}

//this overloaded constructor configures the Transfer Function object according to the transfer function code passed to it
//(the code passed must be an item of the DEFAULT_TRANSFER_FUNCTIONS values list)
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
	case MESHLAB_RGB_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f);
		_channels[RED_CHANNEL].addKey(0.125f,0.0f);
		_channels[RED_CHANNEL].addKey(0.375f,0.0f);
		_channels[RED_CHANNEL].addKey(0.625f,1.0f);
		_channels[RED_CHANNEL].addKey(0.875f,1.0f);
		_channels[RED_CHANNEL].addKey(1.0f,0.5f);

		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.125f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.375f,1.0f);
		_channels[GREEN_CHANNEL].addKey(0.625f,1.0f);
		_channels[GREEN_CHANNEL].addKey(0.875f,0.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f);

		_channels[BLUE_CHANNEL].addKey(0.0f,0.5f);
		_channels[BLUE_CHANNEL].addKey(0.125f,1.0f);
		_channels[BLUE_CHANNEL].addKey(0.375f,1.0f);
		_channels[BLUE_CHANNEL].addKey(0.625f,0.0f);
		_channels[BLUE_CHANNEL].addKey(0.875f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,0.0f);
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
		break;
			case FRENCH_RGB_TF:
		_channels[RED_CHANNEL].addKey(0.0f,1.0f);
		_channels[RED_CHANNEL].addKey(0.5f,1.0f);
		_channels[RED_CHANNEL].addKey(1.0f,0.0f);

		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.5f,1.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f);

		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(0.5f,1.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,1.0f);
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
	case SAW_4_TF:
	for(int i=0;i<4;++i)
	{
		_channels[RED_CHANNEL].addKey(0.25f*i,           0.0f);
		_channels[RED_CHANNEL].addKey(0.25f*(i+1)-0.0001,1.0f);
		_channels[GREEN_CHANNEL].addKey(0.25f*i,           0.0f);
		_channels[GREEN_CHANNEL].addKey(0.25f*(i+1)-0.0001,1.0f);
		_channels[BLUE_CHANNEL].addKey(0.25f*i,           0.0f);
		_channels[BLUE_CHANNEL].addKey(0.25f*(i+1)-0.0001,1.0f);
	}		break;
	case SAW_8_TF:
	for(int i=0;i<8;++i)
	{
		_channels[RED_CHANNEL].addKey(0.125f*i,           0.0f);
		_channels[RED_CHANNEL].addKey(0.125f*(i+1)-0.0001,1.0f);
		_channels[GREEN_CHANNEL].addKey(0.125f*i,           0.0f);
		_channels[GREEN_CHANNEL].addKey(0.125f*(i+1)-0.0001,1.0f);
		_channels[BLUE_CHANNEL].addKey(0.125f*i,           0.0f);
		_channels[BLUE_CHANNEL].addKey(0.125f*(i+1)-0.0001,1.0f);
	}		break;
	}
}

//this overloaded constructor configures the Transfer Function using the info present in an external CSV file
TransferFunction::TransferFunction(QString fileName)
{
	this->initTF();

	QFile inFile( fileName );

	if ( !inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream inStream( &inFile );
	QString line;
	QStringList splittedString;

	//each not-commented line of the file represent the values to build-up a channel
	int channel_code = 0;
	do
	{
		line = inStream.readLine();

		//if a line is a comment, it's not processed. imply ignoring it!
		if ( !line.startsWith(CSV_FILE_COMMENT) )
		{
			//a channel line found. Splitting it to find the values
			splittedString = line.split(CSV_FILE_SEPARATOR, QString::SkipEmptyParts);
			assert( (splittedString.size() % 2) == 0 );

			//for each couple of values a key is built and added to the current channel
			for ( int i=0; i<splittedString.size(); i+=2 )
				_channels[channel_code].addKey( splittedString[i].toFloat(), splittedString[i+1].toFloat() );

			//trying to load data for the next channel
			channel_code ++;
		}
	} while( (!line.isNull()) && (channel_code < NUMBER_OF_CHANNELS) );

	inFile.close();
}

TransferFunction::~TransferFunction(void)
{
}

//initializes the Transfer Function at startup
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

	//setting default transfer functions names
	defaultTFs[GREY_SCALE_TF] = "Grey Scale";
	defaultTFs[MESHLAB_RGB_TF] = "Meshlab RGB";
	defaultTFs[FRENCH_RGB_TF] = "Red-White-Blue Scale";
	defaultTFs[RGB_TF] = "RGB";
	defaultTFs[RED_SCALE_TF] = "Red Scale";
	defaultTFs[GREEN_SCALE_TF] = "Green Scale";
	defaultTFs[BLUE_SCALE_TF] = "Blue Scale";
	defaultTFs[SAW_4_TF] = "SawTooth Gray 4";
	defaultTFs[SAW_8_TF] = "SawTooth Gray 8";
	defaultTFs[FLAT_TF] = "Flat";
}

//returns the size of the TF. It's defined as the maximum size of each channel
int TransferFunction::size()
{
	int result = 0;
	for (int i=0; i<NUMBER_OF_CHANNELS; i++)
		if ( _channels[i].size() > result )
			result = _channels[i].size();

	return result;
}

//Builds the color band by setting the proper color for each item
//returns a pointer to the color band built
QColor* TransferFunction::buildColorBand()
{
	float relative_pos = 0.0f; 
	for (int i=0; i<COLOR_BAND_SIZE; i++)
	{
		//converting the index in relative TF x-coordinate
		relative_pos = absolute2RelativeValf((float)i, COLOR_BAND_SIZE);

		//setting the color of the color band with the color resulting by evaluation of the TF for each channel
		_color_band[i].setRgbF( _channels[RED_CHANNEL].getChannelValuef( relative_pos),
								_channels[GREEN_CHANNEL].getChannelValuef( relative_pos ),
								_channels[BLUE_CHANNEL].getChannelValuef( relative_pos ) );
	}

	return _color_band;
}

//converts a quality percentage value into a color depending on the transfer function channels values
Color4b TransferFunction::getColorByQuality (float percentageQuality)
{
	return Color4b(_channels[RED_CHANNEL].getChannelValueb( percentageQuality ), 
		_channels[GREEN_CHANNEL].getChannelValueb( percentageQuality ), 
		_channels[BLUE_CHANNEL].getChannelValueb( percentageQuality ),
		255 );
}

//converts a quality value into a color depending on the transfer function channels values, min quality, max quality, mid quality and brightness
Color4b TransferFunction::getColorByQuality (float absoluteQuality, float minQuality, float maxQuality, float midRelativeQuality, float brightness)
{
	float percentageQuality;
	Color4b currentColor;

	if (absoluteQuality < minQuality)
		percentageQuality = 0.0f;
	else
		if (absoluteQuality > maxQuality)
			percentageQuality = 1.0f;
		else
			// calcultating relative quality and applying exponential function: rel(Q)^exp, exp=2*midHandleRelPos
			percentageQuality = pow( (absoluteQuality - minQuality) / (maxQuality - minQuality) , (float)(2.0f*midRelativeQuality));

	currentColor = getColorByQuality(percentageQuality);
	
	if (brightness!=1.0f) //Applying brightness to each color channel, 0<brightness<2, 1=normale brightness, 0=white, 2=black
		if (brightness<1.0f)
			for (int i=0; i<NUMBER_OF_CHANNELS; i++) 
				currentColor[i] = relative2AbsoluteVali(pow(absolute2RelativeValf(currentColor[i],255.0f),brightness), 255.0f);
		else
			for (int i=0; i<NUMBER_OF_CHANNELS; i++) 
				currentColor[i] = relative2AbsoluteVali(1.0f-pow(1.0f-absolute2RelativeValf(currentColor[i],255.0f),2-brightness), 255.0f);

	return currentColor;
}

//saves the current color band onto an external file
//moreover it saves info about the equalizer state
//returns the name of the file
QString TransferFunction::saveColorBand( QString fn, EQUALIZER_INFO& info  )
{
	//acquiring save file
	QString fileName = QFileDialog::getSaveFileName( 0, "Save Transfer Function File", fn + CSV_FILE_EXSTENSION, QString("Quality Mapper File (*") + QString(CSV_FILE_EXSTENSION) + QString(")") );

	QFile outFile( fileName );

	if ( !outFile.open(QIODevice::WriteOnly | QIODevice::Text))
		return fileName;

	QTextStream outStream( &outFile );
	//writing file header (info about file structure)
	outStream << CSV_FILE_COMMENT << " COLOR BAND FILE STRUCTURE - first row: RED CHANNEL DATA - second row GREEN CHANNEL DATA - third row: BLUE CHANNEL DATA" << endl;
	outStream << CSV_FILE_COMMENT << " CHANNEL DATA STRUCTURE - the channel structure is grouped in many triples. The items of each triple represent respectively: X VALUE, Y_LOWER VALUE, Y_UPPER VALUE of each node-key of the transfer function" << endl;

	TF_KEY *val = 0;
	//for each channel...
	for ( int i=0; i<NUMBER_OF_CHANNELS; i++)
	{
		//...for each key of the channel...
		for (int j=0; j<_channels[i].size(); j++)
		{
			//saving the values couple
			val = _channels[i][j];
			assert(val != 0);
			outStream << val->x << CSV_FILE_SEPARATOR << val->y << CSV_FILE_SEPARATOR;
		}
		//one channel-per-row
		outStream << endl;
	}

	//saving equalizer info too (only one line is needed)
	outStream << CSV_FILE_COMMENT << "THE FOLLOWING 4 VALUES REPRESENT EQUALIZER SETTINGS - the first and the third values represent respectively the minimum and the maximum quality values used in histogram, the second one represent the position (in percentage) of the middle quality, and the last one represent the level of brightness as a floating point number (0 copletely dark, 1 original brightness, 2 completely white)" << endl;
	outStream << info.minQualityVal << CSV_FILE_SEPARATOR << info.midQualityPercentage << CSV_FILE_SEPARATOR << info.maxQualityVal << CSV_FILE_SEPARATOR << info.brightness << CSV_FILE_SEPARATOR << endl;

	outFile.close();

	return fileName;
}

//"moving" the channel identified by ch_code to first plane.
//This operation simply implies the circular shift of the channel_order pivot indexes untill the selected channel code is in the last position of the array
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
