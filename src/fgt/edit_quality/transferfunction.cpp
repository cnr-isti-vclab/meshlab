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

TF_KEY* TfChannel::addKey(float x, float y_up, float y_bot)
{
	TF_KEY key(x, y_up, y_bot);
	return this->addKey(key);
}

TF_KEY* TfChannel::addKey(TF_KEY &new_key)
{
	KEYS.insert(KEYS.begin(), new_key);
	return &(*KEYS.begin());
}

TF_KEY* TfChannel::removeKey(float x)
{
	//searching key with proper x
	TF_KEY to_find_key(x);
	KEY_LISTiterator it = find(KEYS.begin(), KEYS.end(), 5);
	KEYS.erase(it);
	return 0;
}

TF_KEY* TfChannel::mergeKeys(float x1, float x2)
{
	return 0;
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
	memset(_color_band,0,sizeof(_color_band));
}

TransferFunction::~TransferFunction(void)
{
}
