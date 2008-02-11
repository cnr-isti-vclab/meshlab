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
#include <vector>
//#include <algorithm>
#include <cassert>

#include <QString>
#include <QColor>
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
	enum JUNCTION_SIDE
	{
		LEFT_JUNCTION_SIDE = 0,
		RIGHT_JUNCTION_SIDE
	};
	float	x;
	float	y;
	JUNCTION_SIDE		junctionSide;
// 	float	y_upper;
// 	float	y_lower;
// 	int		left_junction_point_code;
// 	int		right_junction_point_code;

// 	float getLeftJunctionPoint()
// 	{ 
// 		if (left_junction_point_code == LOWER_Y)
// 			return y_lower;
// 		else
// 			return y_upper;
// 	}
// 	float getRightJunctionPoint()
// 	{
// 		if (right_junction_point_code == LOWER_Y)
// 			return y_lower;
// 		else
// 			return y_upper;
// 	}
// 	inline void  setLeftJunctionPoint( int j_p )	{ left_junction_point_code = j_p; right_junction_point_code = NUMBER_OF_JUNCTION_Y-left_junction_point_code-1; }
// 	inline void  setRightJunctionPoint( int j_p )	{ right_junction_point_code = j_p; left_junction_point_code = NUMBER_OF_JUNCTION_Y-right_junction_point_code-1; }
	
	TF_KEY( float xVal=0.0f, float yVal=0.0f, JUNCTION_SIDE junction=LEFT_JUNCTION_SIDE )
	{
		assert(xVal>=0.0f);
		assert(yVal>=0.0f);
		x = xVal;
		y = yVal;
		junctionSide = junction;
/*
				y_upper=y_up;
				y_lower=y_low;
				if ( y_upper < y_lower)
					this->swapY();
				this->setLeftJunctionPoint(LOWER_Y);*/
	}

// 	inline void swapY() { float tmp=y_lower; y_lower=y_upper; y_upper=tmp; this->setLeftJunctionPoint(right_junction_point_code); }
// 	bool operator == (TF_KEY k)	{ return ( (y_lower == k.y_lower) && (y_upper == k.y_upper) ); }
};

#define TF_KEYsize	sizeof(TF_KEY)

// struct TF_CHANNEL_VALUE
// {
// 	float	*x;
// 	TF_KEY	*y;
// 	TF_CHANNEL_VALUE(float *x_val=0, TF_KEY *y_val=0)
// 	{	x=x_val;	y=y_val;	}
// };


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

#define COLOR_2_TYPE(COLOR, TYPE) \
if ( COLOR == Qt::red) \
{ \
	TYPE = RED_CHANNEL; \
} \
	else \
	{ \
		if ( COLOR == Qt::green) \
		{ \
			TYPE = GREEN_CHANNEL; \
		} \
		else \
		{ \
			if ( color == Qt::blue) \
			{ \
				TYPE =  BLUE_CHANNEL; \
			} \
			else \
				TYPE = -1; \
		} \
	}


//defines a to class to menage the keys for a single channel
class TfChannel
{
public:
/*	enum { LEFT_JUNCTION_SIDE = 0, RIGHT_JUNCTION_SIDE	};*/

	//container and iterator for TF KEYs
	typedef	vector<TF_KEY*> KEY_LIST;
	typedef	vector<TF_KEY*>::iterator KEY_LISTiterator;

	TfChannel(void);
	TfChannel(TF_CHANNELS type);
	~TfChannel(void);

	void	setType(TF_CHANNELS);
	TF_CHANNELS getType(void);
	TF_KEY	*addKey(float xVal, float yVal, TF_KEY::JUNCTION_SIDE side);
	TF_KEY	*addKey(TF_KEY *newKey);
	void	removeKey(int index);
	void	removeKey(TF_KEY *key);

	float	getChannelValuef(float x_position);
	UINT8	getChannelValueb(float x_position);

	bool	isHead(TF_KEY *key);
	bool	isTail(TF_KEY *key);
	void	updateKeysOrder(void);

	TF_KEY* operator [](float idx);
	TF_KEY* operator [](int idx);
	
	inline int size(void)	{	return KEYS.size();	}

//	void	updateKey( float old_x, float new_x, float new_y);

#ifdef NOW_TESTING
	void testInitChannel();
#endif

private:
	TF_CHANNELS	_type;
//	int old_iterator_idx;
//	TF_CHANNEL_VALUE _ret_val;


	//list of keys
	KEY_LIST	KEYS;
//	KEY_LISTiterator _idx_it; //reserved fo [] operator works (better you don't touch it! :-) )
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
	QColor		_color_band[COLOR_BAND_SIZE];

	void initTF(void);

public:
	TransferFunction(void);
	TransferFunction(QString csvFileName);
	TransferFunction(DEFAULT_TRANSFER_FUNCTIONS tf_code);
	~TransferFunction(void);

	static QString defaultTFs[NUMBER_OF_DEFAULT_TF];

	TfChannel &getChannel( int channel_code ) {return _channels[channel_code];}
	TfChannel& operator [](int i)	{ return _channels[_channels_order[i]];	}
	int size();
	QColor* buildColorBand();
	QString saveColorBand( QString fileName );
	Color4b getColorByQuality (float percentageQuality);
	// QColor* getColorBand(){return _color_band;};
	void moveChannelAhead( TF_CHANNELS channel_code );
	inline int	getFirstPlaneChanel(void) { return _channels_order[NUMBER_OF_CHANNELS-1]; }

	static TransferFunction *GreyScaleTF();
	static TransferFunction *RGBTF();
	static TransferFunction *RedScaleTF();
	static TransferFunction *GreenScaleTF();
	static TransferFunction *BlueScaleTF();
	static TransferFunction *FlatTF();
};

bool TfKeyPCompare(TF_KEY*k1, TF_KEY*k2);

#endif
