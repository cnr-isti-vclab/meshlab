#include "transferfunction.h"


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
TF_KEY* TfChannel::addKey(float x_pos, TF_KEY &new_key)
{
	TF_KEY *added_key = &new_key;
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
TF_KEY* TfChannel::addKey(float x_pos, float y_up, float y_bot)
{
	//building key
	TF_KEY key( /*x,*/ y_up, y_bot );

	//adding it to list
	return this->addKey(x_pos, key);
}

//removes from keys list to_remove_key
//returns the x value of the removed key or -1 if key was not found
float TfChannel::removeKey(TF_KEY& to_remove_key)
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

	if ( it != KEYS.end() )
	{
		KEYS.erase( it );
		result = x_val;
	}

	return result;
}

//merges two keys together by copying opportunely y values of the keys in just one key
//returns a pointer to the "merged key"
TF_KEY *TfChannel::mergeKeys(float x_pos, TF_KEY& key)
{
	TF_KEY *result = 0;

	KEY_LISTiterator it = KEYS.find(x_pos);

	TF_KEY new_key(x_pos);

	//be sure that key1 is really in the list!
	assert(it != KEYS.end());

	//any other case

	//setting y_lower to the minimum of y_lower of key1 and key2
	new_key.y_lower = min( it->second.y_lower, key.y_lower );

	//setting y_lower to the maximum of y_upper of key1 and key2
	new_key.y_upper = max( it->second.y_upper, key.y_upper );

	new_key.left_junction_point_code = it->second.left_junction_point_code;
	new_key.right_junction_point_code = it->second.right_junction_point_code;
	//new_key.junction_point_code = it->second.junction_point_code;

	it->second = new_key;

	//the address of new inserted in the list is returned
	result = &(it->second);

	return result;
}

float TfChannel::getChannelValuef(float x_position)
{
	float result = 0.0f;

	KEY_LISTiterator it = KEYS.find(x_position);

	//if a x_position is known x, its key value is returned immediately
	if ( it != KEYS.end())
		return it->second.getLeftJunctionPoint();

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
			float m = ((up->second.getLeftJunctionPoint() - low->second.getRightJunctionPoint()) / (up->first - low->first));

			//returning f(x) value for x in the interpolating line
			result = (m * (x_position - low->first)) + low->second.getRightJunctionPoint();
		}

	return result;
}


UINT8 TfChannel::getChannelValueb(float x_position)
{
	return (UINT8)relative2AbsoluteVali( this->getChannelValuef(x_position), 255.0f );
//	return (UINT8)(this->getChannelValuef(x_position) * 255.0f);
}

#ifdef NOW_TESTING
void TfChannel::testInitChannel()
{
	int num_of_keys = (rand() % 10) + 1;
	float rand_x = 0.0f;
	float rand_y = 0.0f;

	//first node\key = 0
	this->addKey(0.0f, 0.0f, 0.0f);
	for (int i=0; i<num_of_keys; i++)
	{
		rand_x = ((rand() % 100) + 1) / 100.0f;
		rand_y = ((rand() % 100) + 1) / 100.0f;
		this->addKey(rand_x, rand_y+1.0f, rand_y);
	}
	//last node\key = 0
	this->addKey(1.0f, 0.0f, 0.0f);
}
#endif








//TRANSFER FUNCTION
TransferFunction::TransferFunction(void)
{
	this->initTF();
}


TransferFunction::TransferFunction(QString colorBandFile)
{
	this->initTF();
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
}


void TransferFunction::buildColorBand()
{
	float relative_pos = 0.0f; 
	for (int i=0; i<COLOR_BAND_SIZE; i++)
	{
		relative_pos = absolute2RelativeValf((float)i, COLOR_BAND_SIZE);
		_color_band[i].SetRGB( _channels[RED_CHANNEL].getChannelValueb( relative_pos ),
							   _channels[GREEN_CHANNEL].getChannelValueb( relative_pos ),
							   _channels[BLUE_CHANNEL].getChannelValueb( relative_pos ) );
	}
}