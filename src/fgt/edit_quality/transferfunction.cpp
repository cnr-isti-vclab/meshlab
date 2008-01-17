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
TF_KEY* TfChannel::addKey(TF_KEY &new_key)
{
	TF_KEY *added_key = &new_key;
	KEY_LISTiterator it = KEYS.find(new_key.x);

	if ( it != KEYS.end() )
		//key not present yet in the list. Adding it
		KEYS[new_key.x] = new_key;
	else
		//key with the same x already present in the list. Merging them
		added_key = this->mergeKeys(it->second, new_key);

// 	//inserting at the head of the list
// 	KEYS.insert(KEYS.begin(), new_key);
// 	if ( KEYS.size() >= 2 )
// 		if ( KEYS[0] == KEYS[1] )
// 			this->mergeKeys(0, 1);

	return added_key;
}

//adds to the keys list a new keys with fields passed to the method
//returns a pointer to the key just added
TF_KEY* TfChannel::addKey(float x, float y_up, float y_bot)
{
	//building key
	TF_KEY key(x, y_up, y_bot);

	//adding it to list
	return this->addKey(key);
}

//removes from keys list to_remove_key
//returns the x value of the removed key or -1 if key was not found
float TfChannel::removeKey(TF_KEY& to_remove_key)
{
	//not found by default
	float result = -1.0f;

	//searching key
	KEY_LISTiterator it = KEYS.find(to_remove_key.x);

	//if found, deleting it from list
	if ( it != KEYS.end() )
	{
		result = it->first;
		KEYS.erase(it);
	}

	return result;
}

//removes from keys list the key whose x value is x_val
//returns the x value of the removed key or -1 if key was not found
float TfChannel::removeKey(float x_val)
{
	//searching key with proper x
	TF_KEY to_find_key(x_val);

	return this->removeKey(to_find_key);
}

//merges two keys together by copying opportunely y values of the keys in just one key
//returns a pointer to the "merged key"
TF_KEY *TfChannel::mergeKeys(TF_KEY& key1, TF_KEY& key2)
{
	KEY_LISTiterator it = KEYS.find(key1.x);

	//be sure that key1 is really in the list!
	assert(it == KEYS.end());

	TF_KEY new_key(key1.x);

	if ( key1.x == 0.0f)
	{
		//if add button is pressed, a new key is built with x=0
		new_key.x = key1.x;
		if ( ( key1.y_lower >= key2.y_lower ) &&
			 (key1.y_upper >= key2.y_upper) )
		{
			new_key.y_lower = key1.y_lower;
			new_key.y_upper = key2.y_upper;
		}
		else
		{
			new_key.y_lower = key2.y_lower;
			new_key.y_upper = key1.y_upper;
		}
	}
	else
	{
		//any other case

		//setting y_lower to the minimum of y_lower of key1 and key2
		if ( key2.y_lower < key1.y_lower )
			new_key.y_lower = key2.y_lower;
		else
			new_key.y_lower = key1.y_lower;


		//setting y_lower to the maximum of y_upper of key1 and key2
		if ( key2.y_upper > key1.y_upper )
			new_key.y_upper = key2.y_upper;
		else
			new_key.y_upper = key1.y_upper;
	}

	new_key.junction_point_code = key1.junction_point_code;

	it->second = new_key;

	return &(it->second);
}

float TfChannel::getChannelValuef(float x_position)
{
	float result = 0.0f;

	KEY_LISTiterator it = KEYS.find(x_position);
	if ( it != KEYS.end())
		return it->second.junction_point();

 	//finding lower border for x
  	KEY_LISTiterator low = KEYS.lower_bound( x_position );
  	//finding upper border for x
  	KEY_LISTiterator up = KEYS.upper_bound( x_position );

	if ( low != KEYS.end() && up != KEYS.end() )
	{
		//applying linear interpolation between two keys values

		//angular coefficient for interpolating line
		float m = ((up->second.junction_point() - low->second.junction_point()) / (up->second.x - low->second.x));

		//returning f(x) value for x in the interpolating line
		result = (m * (x_position - low->second.x)) + up->second.junction_point();
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
	for (int i=0; i<num_of_keys; i++)
	{
		rand_x = ((rand() % 100) + 1) / 100.0f;
		rand_y = ((rand() % 100) + 1) / 100.0f;
		this->addKey(rand_x, rand_y, rand_y+0.1f);
	}
}
#endif








//TRANSFER FUNCTION
TransferFunction::TransferFunction(void)
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

TransferFunction::~TransferFunction(void)
{
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