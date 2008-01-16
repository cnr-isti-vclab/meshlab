#include "transferfunction.h"

//TRANSFER FUNCTION CHANNEL
TfChannel::TfChannel()
{}

TfChannel::TfChannel(TF_CHANNELS type) : _type(type)
{}

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
	//inserting at the head of the list
	KEYS.insert(KEYS.begin(), new_key);

	return &(*KEYS.begin());
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
	KEY_LISTiterator it = find(KEYS.begin(), KEYS.end(), to_remove_key);

	//if found, deleting it from list
	if ( it != KEYS.end() )
	{
		result = (*it).x;
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
TF_KEY *TfChannel::mergeKeys(TF_KEY key1, TF_KEY key2)
{
	KEY_LISTiterator it1 = std::find(KEYS.begin(), KEYS.end(), key1);
	KEY_LISTiterator it2 = find(KEYS.begin(), KEYS.end(), key2);

	//key1 or key2 not found!
	assert(( it1 == KEYS.end() ) || ( it2 == KEYS.end() ));

	//at least one of two y_lower should be 0
	if ( ( (*it1).y_lower * key2.y_lower) == 0 )
		if ( (*it1).y_lower == 0 )
			(*it1).y_lower = key2.y_lower;

	//at least one of two y_upper should be 0
	if ( ( (*it1).y_upper * key2.y_upper) == 0 )
		if ( (*it1).y_upper == 0 )
			(*it1).y_upper = key2.y_upper;

	//merge done, deleting key2
	KEYS.erase(it2);

	return &(*it1);
}

//merges two keys together by copying opportunely y values of the keys in just one key
//returns a pointer to the "merged key"
TF_KEY* TfChannel::mergeKeys(float x1, float x2)
{
	TF_KEY key1(x1);
	TF_KEY key2(x2);

	//calling merge keys on TF_KEY objects
	return this->mergeKeys(key1, key2);
}

float TfChannel::getChannelValuef(float x_position)
{
	return x_position;
}


UINT8 TfChannel::getChannelValueb(float x_position)
{
	return (UINT8)relative2AbsoluteVali( this->getChannelValuef(x_position), 255.0f );
//	return (UINT8)(this->getChannelValuef(x_position) * 255.0f);
}








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
	for (int i=0; i<COLOR_BAND_SIZE; i++)
		_color_band[i].SetRGB( _channels[RED_CHANNEL].getChannelValueb( absolute2RelativeValf((float)i, COLOR_BAND_SIZE) ),
							   _channels[GREEN_CHANNEL].getChannelValueb( absolute2RelativeValf((float)i, COLOR_BAND_SIZE) ),
							   _channels[BLUE_CHANNEL].getChannelValueb( absolute2RelativeValf((float)i, COLOR_BAND_SIZE) ) );
}