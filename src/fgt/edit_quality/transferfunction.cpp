#include "transferfunction.h"

//TRANSFER FUNCTION CHANNEL
TfChannel::TfChannel()
{}

TfChannel::TfChannel(TF_CHANNELS type) : _type(type)
{
	//no keys, they're sorted
	_sorted = true;
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
	//inserting at the head of the list
	KEYS.insert(KEYS.begin(), new_key);
	if ( KEYS.size() >= 2 )
		if ( KEYS[0] == KEYS[1] )
			this->mergeKeys(0, 1);

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

//da ottimizzare!!
TF_KEY *TfChannel::mergeKeys(int pos1, int pos2)
{	return this->mergeKeys( KEYS[pos1], KEYS[pos2] );	}

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
	int prev_key_idx = 0;
	int next_key_idx = 0;

	//not applicable if list's empty!
	assert(KEYS.size() == 0);

	//finding upper border key for x_position
	while ( (KEYS[next_key_idx].x < x_position) && (next_key_idx < (int)KEYS.size()) )
		next_key_idx ++;

	//controllare questo! MAL
	assert(next_key_idx >= (int)KEYS.size());

	//returns the correct value if next_key_idx equals to x_position or when there's only one key in the list
	if ((next_key_idx == 0) || (KEYS[next_key_idx].x == x_position))
		return KEYS[next_key_idx].junction_point();

	assert(next_key_idx == 0);

	if ( next_key_idx > 0 )
		prev_key_idx = next_key_idx - 1;

	//applying linear interpolation between two keys values

	//angular coefficient for interpolating line
	float m = ((KEYS[next_key_idx].junction_point() - KEYS[prev_key_idx].junction_point()) / (KEYS[next_key_idx].x - KEYS[prev_key_idx].x));

	//returning f(x) value for x in the interpolating line
	return (m * (x_position - KEYS[prev_key_idx].x)) + KEYS[next_key_idx].junction_point();
}


UINT8 TfChannel::getChannelValueb(float x_position)
{
	return (UINT8)relative2AbsoluteVali( this->getChannelValuef(x_position), 255.0f );
//	return (UINT8)(this->getChannelValuef(x_position) * 255.0f);
}

//tells if the keys of the channel are sorted
bool TfChannel::isSorted()
{	return _sorted;	}

void TfChannel::sortKeys()
{	sort(KEYS.begin(), KEYS.end());	}








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
	//sorting keys of the channel
	for (int i=0; i<NUMBER_OF_CHANNELS; i++)
		if ( !_channels[i].isSorted() )
			_channels[i].sortKeys();

	float relative_pos = 0.0f; 
	for (int i=0; i<COLOR_BAND_SIZE; i++)
	{
		relative_pos = absolute2RelativeValf((float)i, COLOR_BAND_SIZE);
		_color_band[i].SetRGB( _channels[RED_CHANNEL].getChannelValueb( relative_pos ),
							   _channels[GREEN_CHANNEL].getChannelValueb( relative_pos ),
							   _channels[BLUE_CHANNEL].getChannelValueb( relative_pos ) );
	}
}