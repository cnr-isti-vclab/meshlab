#include "transferfunction.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

//da eliminare!! MAL
#ifdef NOW_TESTING
#include <cmath>
using namespace std;
#endif


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
{	KEYS.clear();	}

void TfChannel::setType(TF_CHANNELS type)
{	_type = type;	}

TF_CHANNELS TfChannel::getType()
{	return _type;	}

//adds to the keys list new_key
//returns a pointer to the key just added
TF_KEY* TfChannel::addKey(float x_pos, TF_KEY *new_key)
{
	TF_KEY *added_key = new_key;
	KEY_LISTiterator it = KEYS.find(x_pos);

	if ( it == KEYS.end() )
		//key not present yet in the list. Adding it
		KEYS[x_pos] = new_key;
	else
		//key with the same x already present in the list. Merging them
		added_key = this->mergeKeys(x_pos, new_key);

	return added_key;
}

//adds to the keys list a new keys with fields passed to the method
//returns a pointer to the key just added
TF_KEY* TfChannel::addKey(float x_pos, float y_low, float y_up)
{
	//building key
	TF_KEY *key = new TF_KEY( y_low, y_up );

	//adding it to list
	return this->addKey(x_pos, key);
}

//removes from keys list to_remove_key
//returns the x value of the removed key or -1 if key was not found
float TfChannel::removeKey(TF_KEY *to_remove_key)
{
	float result = -1.0f;

	bool found = false;
	KEY_LISTiterator it = KEYS.begin();

	while (( it != KEYS.end() ) && (! found ))
	{
		if ( it->second == to_remove_key )
		{
			found = true;
			result = it->first;
			KEYS.erase( it );
			delete to_remove_key;
			to_remove_key = 0;
		}
		else
			it ++;
	}

	return result;
}

//removes from keys list the key whose x value is x_val
//returns the x value of the removed key or -1 if key was not found
float TfChannel::removeKey(float x_val)
{
	float result = -1.0f;

	//searching key with proper x
	KEY_LISTiterator it = KEYS.find(x_val);

	TF_KEY *toRemove = 0;

	if ( it != KEYS.end() )
	{
		toRemove = it->second;
		KEYS.erase( it );
		result = x_val;
		delete toRemove;
		toRemove = 0;
	}

	return result;
}

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

float TfChannel::getChannelValuef(float x_position)
{
	float result = 0.0f;

	KEY_LISTiterator it = KEYS.find(x_position);

	//if a x_position is known x, its key value is returned immediately
	if ( it != KEYS.end())
		return it->second->getLeftJunctionPoint();

  	//finding upper border for x
  	KEY_LISTiterator up = KEYS.upper_bound( x_position );

	assert(up != KEYS.begin());

 	//finding lower border for x
	KEY_LISTiterator low = up;
	low --;

	if (( low->first < x_position ) && ( up->first > x_position) )
		if ( low != KEYS.end() && up != KEYS.end() )
		{
			//applying linear interpolation between two keys values

			//angular coefficient for interpolating line
			float m = ((up->second->getLeftJunctionPoint() - low->second->getRightJunctionPoint()) / (up->first - low->first));

			//returning f(x) value for x in the interpolating line
			result = (m * (x_position - low->first)) + low->second->getRightJunctionPoint();
		}

	return result;
}


UINT8 TfChannel::getChannelValueb(float x_position)
{
	return (UINT8)relative2AbsoluteVali( this->getChannelValuef(x_position), 255.0f );
//	return (UINT8)(this->getChannelValuef(x_position) * 255.0f);
}


TF_CHANNEL_VALUE& TfChannel::operator [](float idx)
{
	KEY_LISTiterator it= KEYS.find(idx);

	if (it!=KEYS.end())
	{
		_ret_val.x=(float*)&(it->first);
		_ret_val.y=it->second;
	}
	else
	{
		_ret_val.x = 0;
		_ret_val.y = 0;
	}
	return _ret_val;
}

TF_CHANNEL_VALUE& TfChannel::operator [](int i)
{
	assert((i>=0) && (i<=this->size()));

	if ( i == 0)
	{
		//indexing the first item of the list
		_idx_it=KEYS.begin();
	}
	else
	{
		//now indexing the successive item in the list respect to the previous one
		if ( old_iterator_idx == i-1 )
		{
			_idx_it ++;
		}
		else
		{
			//now indexing the previous item in the list respect to the previous one
			if ( old_iterator_idx == i+1 )
			{
				_idx_it --;
			}
			else
			{
				_idx_it = KEYS.begin();
				for(int k=0; k<i; k++)
					_idx_it ++;
			}
		}
	}
	old_iterator_idx = i;
	_ret_val.x = (float*)&(_idx_it->first);
	_ret_val.y = _idx_it->second;

	return _ret_val;
}


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
		_channels[RED_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[RED_CHANNEL].addKey(1.0f,1.0f,1.0f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,1.0f,1.0f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,1.0f,1.0f);
			break;
	case RGB_TF:
		_channels[RED_CHANNEL].addKey(0.0f,1.0f,1.0f);
		_channels[RED_CHANNEL].addKey(0.5f,0.0f,0.0f);
		_channels[RED_CHANNEL].addKey(1.0f,0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.5f,1.0f,1.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(0.5f,0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,1.0f,1.0f);
		break;
	case RED_SCALE_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[RED_CHANNEL].addKey(1.0f,1.0f,1.0f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,0.0f,0.0f);
		break;
	case GREEN_SCALE_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[RED_CHANNEL].addKey(1.0f,0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,1.0f,1.0f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,0.0f,0.0f);
		break;
	case BLUE_SCALE_TF:
		_channels[RED_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[RED_CHANNEL].addKey(1.0f,0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.0f,0.0f);
		_channels[BLUE_CHANNEL].addKey(1.0f,1.0f,1.0f);
		break;
	case FLAT_TF:
	default:
		_channels[RED_CHANNEL].addKey(0.0f,0.5f,0.5f);
		_channels[RED_CHANNEL].addKey(1.0f,0.5f,0.5f);
		_channels[GREEN_CHANNEL].addKey(0.0f,0.5f,0.5f);
		_channels[GREEN_CHANNEL].addKey(1.0f,0.5f,0.5f);
		_channels[BLUE_CHANNEL].addKey(0.0f,0.5f,0.5f);
		_channels[BLUE_CHANNEL].addKey(1.0f,0.5f,0.5f);
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
			assert( (splittedString.size() % NUMBER_OF_CHANNELS) == 0 );

			for ( int i=0; i<splittedString.size(); i+=3 )
				_channels[channel_code].addKey( splittedString[i].toFloat(), splittedString[i+1].toFloat(), splittedString[i+2].toFloat() );

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


QString TransferFunction::saveColorBand( QString fn )
{
	QString fileName = QFileDialog::getSaveFileName( 0, "Save Transfer Function File", fn + CSV_FILE_EXSTENSION, "CSV File (*.csv)" );

	QFile outFile( fileName );

	if ( !outFile.open(QIODevice::WriteOnly | QIODevice::Text))
		return fileName;

	QTextStream outStream( &outFile );
	outStream << CSV_FILE_COMMENT << " COLOR BAND FILE STRUCTURE - first row: RED CHANNEL DATA - second row GREEN CHANNEL DATA - third row: BLUE CHANNEL DATA" << endl;
	outStream << CSV_FILE_COMMENT << " CHANNEL DATA STRUCTURE - the channel structure is grouped in many triples. The items of each triple represent respectively: X VALUE, Y_LOWER VALUE, Y_UPPER VALUE of each node-key of the transfer function" << endl;

	TF_CHANNEL_VALUE val;
	for ( int i=0; i<NUMBER_OF_CHANNELS; i++)
	{
		for (int j=0; j<_channels[i].size(); j++)
		{
			val = _channels[i][j];

			assert((val.x != 0) && (val.y != 0));

			outStream << (*val.x) << CSV_FILE_SEPARATOR << val.y->y_lower << CSV_FILE_SEPARATOR << val.y->y_upper << CSV_FILE_SEPARATOR;
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
