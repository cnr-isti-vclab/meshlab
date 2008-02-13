#include "transferfunction.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

#include <algorithm>

//da eliminare!! MAL
#ifdef NOW_TESTING
#include <cmath>
using namespace std;
#endif

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
	//da eliminare!? MAL
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
TF_KEY* TfChannel::addKey(float xVal, float yVal, TF_KEY::JUNCTION_SIDE side)
{
	assert(xVal>=0.0f);
	assert(yVal>=0.0f);
	return this->addKey(new TF_KEY(xVal, yVal, side));
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
			//RIGHT JUNCTION POINT FOR A KEY MUST BE POSITIONED BEFORE A LEFT ONE
			if ( (*it)->x == newKey->x )
				if ( ((*it)->junctionSide == TF_KEY::RIGHT_JUNCTION_SIDE ) && (newKey->junctionSide == TF_KEY::LEFT_JUNCTION_SIDE ) )
					it ++;

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

#if 0
//merges two keys together by copying opportunely y values of the keys in just one key
//returns a pointer to the "merged key"
TF_KEY *TfChannel::mergeKeys(float x_pos, TF_KEY *key)
{
	TF_KEY *result = 0;

	KEY_LISTiterator it = KEYS.find(x_pos);

	TF_KEY *new_key = new TF_KEY(x_pos);

	//be sure that key1 is really in the list!
	assert(it != KEYS.end());

	//any other case

	//setting y_lower to the minimum of y_lower of key1 and key2
	new_key->y_lower = min( it->second->y_lower, key->y_lower );

	//setting y_lower to the maximum of y_upper of key1 and key2
	new_key->y_upper = max( it->second->y_upper, key->y_upper );

	new_key->left_junction_point_code = it->second->left_junction_point_code;
	new_key->right_junction_point_code = it->second->right_junction_point_code;
	//new_key.junction_point_code = it->second.junction_point_code;

	it->second = new_key;

	//the address of new inserted in the list is returned
	result = it->second;

	return result;
}

TF_KEY* TfChannel::splitKey(float x_pos)
{
	TF_KEY *result = 0;

	KEY_LISTiterator it = KEYS.find(x_pos);

	float new_y =0.0f;

	if (it!=KEYS.end())
	{
		result = it->second;
		if (result->y_upper == result->y_lower)
		{
			if (( result->y_lower >= 0.4f) && ( result->y_lower <= 0.6f))
			{
				result->y_upper = 1.0f;
			}
			else
			{
				new_y = 1.0f - result->y_lower;
				if ( new_y > result->y_upper )
					result->y_upper = new_y;
				else
					result->y_lower = new_y;
			}
		}
	}

	return result;
}
#endif

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
	
	//if ( it != KEYS.end())		return it->second->getLeftJunctionPoint();

	/*
	//finding lower border for x
	KEY_LISTiterator low = up;
	low --;

	if (( (*low)->x < xVal ) && ( (*up)->x > xVal) )
		if ( low != KEYS.end() && up != KEYS.end() )
		{
			//applying linear interpolation between two keys values

			//angular coefficient for interpolating line
			float m = (((*up)->y - (*low)->y) / ((*up)->x - (*low)->x));

			//returning f(x) value for x in the interpolating line
			result = (m * (x_position - (*low)->x)) + low->second->getRightJunctionPoint();
		}
*/
	return result;
}


UINT8 TfChannel::getChannelValueb(float xVal)
{
	return (UINT8)relative2AbsoluteVali( this->getChannelValuef(xVal), 255.0f );
}

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
{
	sort(KEYS.begin(), KEYS.end(), TfKeyPCompare);
/*
		assert(newX>=0.0f);
		assert(newY>=0.0f);
	
		int result = idx;
	
		//out of boundaries
		if (( idx<0 ) || (idx>=(int)KEYS.size()))
			return -1;
	
		//updating key values
		KEYS[idx]->x = newX;
		KEYS[idx]->y = newY;
	
		sort(KEYS.begin(), KEYS.end(), TfKeyPCompare);
	
		//now checking to restore the sorting and the alternation of LEFT\RIGHT JUNCTION SIDE keys
	
		int prev = idx-1;
		int next = idx+1;
		TF_KEY *tmp = 0;
	
		if ( prev >= 0)
		{
			//swapping if prev is >= then the new value
	/ *
				 		if ( ( KEYS[prev]->x > newX ) || 
				 			 / *(* /(KEYS[prev]->x == newX) / *&& (KEYS[prev]->junctionSide == TF_KEY::LEFT_JUNCTION_SIDE) && (KEYS[idx]->junctionSide == TF_KEY::RIGHT_JUNCTION_SIDE))* / )* /
				 
			if ( KEYS[prev]->x > newX )
			{
				tmp = KEYS[prev];
				KEYS[prev] = KEYS[idx];
				KEYS[idx] = tmp;
				return prev;
			}
		}
	
		if ( next < (int)KEYS.size() )
		{
			//swapping if prev is >= then the new value
			/ *
				if ( ( KEYS[next]->x < newX ) || 
							/ *(* /(KEYS[next]->x == newX) / *&& (KEYS[next]->junctionSide == TF_KEY::RIGHT_JUNCTION_SIDE) && (KEYS[idx]->junctionSide == TF_KEY::LEFT_JUNCTION_SIDE))* / )* /
			if ( KEYS[next]->x < newX )
			{
				tmp = KEYS[next];
				KEYS[next] = KEYS[idx];
				KEYS[idx] = tmp;
				return next;
			}
		}*/
}


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
		_channels[RED_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[RED_CHANNEL].addKey(1.0f,1.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(1.0f,1.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(1.0f,1.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		break;
	case RGB_TF:
		_channels[RED_CHANNEL].addKey(0.0f,1.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[RED_CHANNEL].addKey(0.5f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[RED_CHANNEL].addKey(1.0f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(0.5f,1.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(0.5f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(1.0f,1.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		//added for test
// 		_channels[RED_CHANNEL].addKey(0.5f,0.5f,TF_KEY::LEFT_JUNCTION_SIDE);
//		_channels[GREEN_CHANNEL].addKey(0.5f,0.7f,TF_KEY::RIGHT_JUNCTION_SIDE);
// 		_channels[GREEN_CHANNEL].addKey(0.75f,1.0f,TF_KEY::LEFT_JUNCTION_SIDE);
// 		_channels[GREEN_CHANNEL].addKey(0.75f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
// 		_channels[GREEN_CHANNEL].addKey(0.2f,0.3f,TF_KEY::LEFT_JUNCTION_SIDE);

		break;
	case RED_SCALE_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[RED_CHANNEL].addKey(1.0f,1.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(1.0f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		break;
	case GREEN_SCALE_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[RED_CHANNEL].addKey(1.0f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(1.0f,1.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(1.0f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		break;
	case BLUE_SCALE_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[RED_CHANNEL].addKey(1.0f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(1.0f,1.0f,TF_KEY::LEFT_JUNCTION_SIDE);
		break;
	case FLAT_TF:
	default:
		_channels[RED_CHANNEL].addKey(0.0f,0.5f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[RED_CHANNEL].addKey(1.0f,0.5f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.5f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.5f,TF_KEY::LEFT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.5f,TF_KEY::RIGHT_JUNCTION_SIDE);
		_channels[BLUE_CHANNEL].addKey(1.0f,0.5f,TF_KEY::LEFT_JUNCTION_SIDE);
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

			TF_KEY::JUNCTION_SIDE junctSide;
			for ( int i=0; i<splittedString.size(); i+=2 )
			{
				if (i==0)
					junctSide = TF_KEY::RIGHT_JUNCTION_SIDE;
				else
					junctSide = TF_KEY::LEFT_JUNCTION_SIDE;

				_channels[channel_code].addKey( splittedString[i].toFloat(), splittedString[i+1].toFloat(), junctSide );
			}

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
		/*_color_band[i].SetRGB( _channels[RED_CHANNEL].getChannelValueb( relative_pos ),
		_channels[GREEN_CHANNEL].getChannelValueb( relative_pos ),
		_channels[BLUE_CHANNEL].getChannelValueb( relative_pos ) );*/
		_color_band[i].setRgbF(_channels[RED_CHANNEL].getChannelValuef( relative_pos),
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


QString TransferFunction::saveColorBand( QString fn )
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


TransferFunction* TransferFunction::GreyScaleTF()
{	return new TransferFunction(GREEN_SCALE_TF);	}

TransferFunction* TransferFunction::RGBTF()
{	return new TransferFunction(RGB_TF);	}

TransferFunction* TransferFunction::RedScaleTF()
{	return new TransferFunction(RED_SCALE_TF);	}

TransferFunction* TransferFunction::GreenScaleTF()
{	return new TransferFunction(GREEN_SCALE_TF);	}

TransferFunction* TransferFunction::BlueScaleTF()
{	return new TransferFunction(BLUE_SCALE_TF);	}

TransferFunction* TransferFunction::FlatTF()
{	return new TransferFunction(FLAT_TF);	}
