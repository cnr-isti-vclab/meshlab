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
#ifndef _TRANSFER_FUNCTION_H_
#define _TRANSFER_FUNCTION_H_

//eliminare questo define in fase di release
//#define NOW_TESTING


#include <vcg/math/base.h>
#include <vcg/space/color4.h>
//#include <vector>
#include <map>
//#include <algorithm>
#include <cassert>

#include <QString>
//#include <assert.h>
#include "const_types.h"

#include "util.h"

using namespace std;
using namespace vcg;


#define LOWER_Y					0
#define UPPER_Y					1
#define NUMBER_OF_JUNCTION_Y	2

//struct used to represent each point in the transfer function.
//It's composed of a position on x axis and two values on the y axis (potentially the same)
struct TF_KEY
{
	float	y_upper;
	float	y_lower;
	int		left_junction_point_code;
	int		right_junction_point_code;

	float getLeftJunctionPoint()
	{ 
		if (left_junction_point_code == LOWER_Y)
			return y_lower;
		else
			return y_upper;
	}
	float getRightJunctionPoint()
	{
		if (right_junction_point_code == LOWER_Y)
			return y_lower;
		else
			return y_upper;
	}
	inline void  setLeftJunctionPoint( int j_p )	{ left_junction_point_code = j_p; right_junction_point_code = NUMBER_OF_JUNCTION_Y-left_junction_point_code-1; }
	inline void  setRightJunctionPoint( int j_p )	{ right_junction_point_code = j_p; left_junction_point_code = NUMBER_OF_JUNCTION_Y-right_junction_point_code-1; }
	
	TF_KEY( float y_low=0.0f, float y_up=0.0f )
	{
		y_upper=y_up;
		y_lower=y_low;
		if ( y_upper < y_lower)
			this->swapY();
		this->setLeftJunctionPoint(LOWER_Y);
	}

	inline void swapY() { float tmp=y_lower; y_lower=y_upper; y_upper=tmp; this->setLeftJunctionPoint(right_junction_point_code); }
	bool operator == (TF_KEY k)	{ return ( (y_lower == k.y_lower) && (y_upper == k.y_upper) ); }
};

#define TF_KEYsize	sizeof(TF_KEY)

struct TF_CHANNEL_VALUE
{
	float	*x;
	TF_KEY	*y;
	TF_CHANNEL_VALUE(float *x_val=0, TF_KEY *y_val=0)
	{	x=x_val;	y=y_val;	}
};


//list of channels
enum TF_CHANNELS
{
	RED_CHANNEL = 0,
	GREEN_CHANNEL,
	BLUE_CHANNEL,
	NUMBER_OF_CHANNELS
};

#define TYPE_2_COLOR(TYPE, COLOR) \
	switch(TYPE) \
	{ \
	case RED_CHANNEL: \
		COLOR = Qt::red; \
		break; \
	case GREEN_CHANNEL: \
		COLOR = Qt::green; \
		break; \
	case BLUE_CHANNEL: \
		COLOR = Qt::blue; \
		break; \
	default: \
		COLOR = Qt::black; \
		break; \
	}

//defines a to class to menage the keys for a single channel
class TfChannel
{
public:
	//container and iterator for TF KEYs
	typedef	map<float, TF_KEY> KEY_LIST;
	typedef	map<float, TF_KEY>::iterator KEY_LISTiterator;

	TfChannel(void);
	TfChannel(TF_CHANNELS type);
	~TfChannel(void);

	void	setType(TF_CHANNELS);
	TF_CHANNELS getType();
	TF_KEY	*addKey(float x, float y_low, float y_up);
	TF_KEY	*addKey(float x, TF_KEY& new_key);
	float	removeKey(float x);
	float	removeKey(TF_KEY& to_remove_key);
	TF_KEY	*mergeKeys(float x_pos1, TF_KEY& x_pos2);
	TF_KEY	*splitKey(float x_pos);

	float	getChannelValuef(float x_position);
	UINT8	getChannelValueb(float x_position);

	TF_CHANNEL_VALUE& operator [](float idx);
	TF_CHANNEL_VALUE& operator [](int idx);
	inline int size()	{	return KEYS.size();	}

#ifdef NOW_TESTING
	void testInitChannel();
#endif

private:
	TF_CHANNELS	_type;
	int old_iterator_idx;
	TF_CHANNEL_VALUE _ret_val;


	//list of keys
	KEY_LIST	KEYS;
	KEY_LISTiterator _idx_it;
};


#ifdef WIN32
#define CSV_FILE_DIRECTORY		"CSV\\"
#else
#define CSV_FILE_DIRECTORY		"CSV/"
#endif
#define CSV_FILE_EXSTENSION		".csv"
#define CSV_FILE_SEPARATOR		";"
#define CSV_FILE_COMMENT		"//"

enum DEFAULT_TRANSFER_FUNCTIONS
{
	GREY_SCALE_TF = 0,
	RGB_TF,
	RED_SCALE_TF,
	GREEN_SCALE_TF,
	BLUE_SCALE_TF,
	FLAT_TF,
	NUMBER_OF_DEFAULT_TF
};

#define STARTUP_TF_TYPE		RGB_TF


//Representation of a transfer function as a triple of vectors of Keys, 
//one for each color (RGB)
class TransferFunction
{
private:
	TfChannel	_channels[NUMBER_OF_CHANNELS];			//set of channels
	int			_channels_order[NUMBER_OF_CHANNELS];	//array used to carry out virtual pivoting indexing
	Color4f		_color_band[COLOR_BAND_SIZE];

	void initTF(void);

public:
	TransferFunction(void);
	TransferFunction(QString csvFileName);
	TransferFunction(DEFAULT_TRANSFER_FUNCTIONS tf_code);
	~TransferFunction(void);

	static QString defaultTFs[NUMBER_OF_DEFAULT_TF];

	TfChannel& operator [](int i)	{ return _channels[_channels_order[i]];	}
	int size();
	void buildColorBand();
	QString saveColorBand( QString fileName );
	void moveChannelAhead( TF_CHANNELS channel_code );

	static TransferFunction *GreyScaleTF();
	static TransferFunction *RGBTF();
	static TransferFunction *RedScaleTF();
	static TransferFunction *GreenScaleTF();
	static TransferFunction *BlueScaleTF();
	static TransferFunction *FlatTF();
};

#endif
