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

#ifndef _TRANSFER_FUNCTION_H_
#define _TRANSFER_FUNCTION_H_

#include <vcg/math/base.h>
#include <vcg/space/color4.h>
#include <vector>
#include <cassert>
#include <QString>
#include <QColor>
#include "const_types.h"
#include "util.h"


#define LOWER_Y					0
#define UPPER_Y					1
#define NUMBER_OF_JUNCTION_Y	2

//struct used to represent each point in the transfer function.
//It's composed of a position on x axis and two values on the y axis (potentially the same)
struct TF_KEY
{
	float	x;
	float	y;
	
	TF_KEY( float xVal=0.0f, float yVal=0.0f )
	{
		assert(xVal>=0.0f);
		assert(yVal>=0.0f);
		x = xVal;
		y = yVal;
	}
};
#define TF_KEYsize	sizeof(TF_KEY)


//list of channels
enum TF_CHANNELS
{
	RED_CHANNEL = 0,
	GREEN_CHANNEL,
	BLUE_CHANNEL,
	NUMBER_OF_CHANNELS
};

//macro to convert a type (channel code) into the respective color
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

//macro to convert a color (channel color) into the respective channel code
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
	//container and iterator for TF KEYs
	typedef	std::vector<TF_KEY*> KEY_LIST;
	typedef	std::vector<TF_KEY*>::iterator KEY_LISTiterator;

	TfChannel(void);
	TfChannel(TF_CHANNELS type);
	~TfChannel(void);

	void		setType(TF_CHANNELS);
	TF_CHANNELS	getType(void);
	TF_KEY		*addKey(float xVal, float yVal);
	TF_KEY		*addKey(TF_KEY *newKey);
	void		removeKey(int index);
	void		removeKey(TF_KEY *key);

	float	getChannelValuef(float x_position);
	UINT8	getChannelValueb(float x_position);

	bool	isHead(TF_KEY *key);
	bool	isTail(TF_KEY *key);
	void	updateKeysOrder(void);

	TF_KEY* operator [](float idx);
	TF_KEY* operator [](int idx);
	
	inline int size(void)	{	return KEYS.size();	}

#ifdef NOW_TESTING
	void testInitChannel();
#endif

private:
	//this code represent a unique id used for each channel.
	TF_CHANNELS	_type;

	//list of keys
	KEY_LIST	KEYS;
};


#ifdef WIN32
#define CSV_FILE_DIRECTORY		"CSV\\"
#else
#define CSV_FILE_DIRECTORY		"CSV/"
#endif
#define CSV_FILE_EXSTENSION		".qmap"
#define CSV_FILE_SEPARATOR		";"
#define CSV_FILE_COMMENT		"//"

//list of default transfer function
//An overloaded constructor of Transfer Function takes a value of this list as parameter
//to build the correspondent default Transfer Function
enum DEFAULT_TRANSFER_FUNCTIONS
{
	GREY_SCALE_TF = 0,
	MESHLAB_RGB_TF,
	RGB_TF,
	FRENCH_RGB_TF,
	RED_SCALE_TF,
	GREEN_SCALE_TF,
	BLUE_SCALE_TF,
	FLAT_TF,
	SAW_4_TF,
	SAW_8_TF,
	NUMBER_OF_DEFAULT_TF
};

//code of the default startup Transfer Function
//When the plugin is launched, a TF is built using this code
#define STARTUP_TF_TYPE		MESHLAB_RGB_TF


//Representation of a Transfer Function as a set of channels
//At the moment, the Transfer Function contains 3 channels (RGB)
class TransferFunction
{
private:
	TfChannel	_channels[NUMBER_OF_CHANNELS];			//set of channels
	int			_channels_order[NUMBER_OF_CHANNELS];	//array used to carry out virtual pivoting indexing
	QColor		_color_band[COLOR_BAND_SIZE];			//array of colors used for preview color band

	void initTF(void);

public:
	TransferFunction(void);
	TransferFunction(QString csvFileName);
	TransferFunction(DEFAULT_TRANSFER_FUNCTIONS tf_code);
	~TransferFunction(void);

	static QString defaultTFs[NUMBER_OF_DEFAULT_TF];

	TfChannel&	getChannel(int channel_code)	{return _channels[channel_code];}
	TfChannel&	operator [](int i)				{ return _channels[_channels_order[i]];	}
	int size();
	QColor* buildColorBand(void);
	QString saveColorBand(QString fileName, EQUALIZER_INFO& equalizerInfo);
	vcg::Color4b getColorByQuality(float percentageQuality);
	vcg::Color4b getColorByQuality(float absoluteQuality, float minQuality, float maxQuality, float midRelativeQuality, float brightness);
	void moveChannelAhead(TF_CHANNELS channel_code);
	inline int	getFirstPlaneChanel(void)		{ return _channels_order[NUMBER_OF_CHANNELS-1]; }
};

bool TfKeyPCompare(TF_KEY*k1, TF_KEY*k2);

#endif
