#ifndef _UTIL_H_
#define _UTIL_H_

#include "const_types.h"


#include <cassert>

//these functions return a relative-absolute value conversion respectively in float and int (rounded to closer integer value)
float relative2AbsoluteValf(float relative_val, float max_val);
int relative2AbsoluteVali(float relative_val, float max_val);

//these functions return a absolute-relative value conversion respectively in float and int (rounded to closer integer value)
float absolute2RelativeValf(float absolute_val, float max_val);
int absolute2RelativeVali(float absolute_val, float max_val);

// Converts a relative value in an absolute one after applying an exponential function val^exp
float relative2QualityValf(float relative_val, float min_q, float max_q, float exp);




struct CHART_INFO
{
	float leftBorder;
	float rightBorder;
	float upperBorder;
	float lowerBorder;
	float chartWidth;
	float chartHeight;
	int	numOfItems;
	int	yScaleStep;
	int	maxRoundedY;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float dX;
	float dY;
	float variance;						//variance of y values

	CHART_INFO( int view_width=0, int view_height=0, int num_of_items=1, float min_X=0.0f, float max_X=0.0f, float min_Y=0.0f, float max_Y=0.0f )
	{
		assert(num_of_items != 0);

		leftBorder	= CANVAS_BORDER_DISTANCE;
		rightBorder	= view_width - CANVAS_BORDER_DISTANCE;
		upperBorder	= CANVAS_BORDER_DISTANCE;
		lowerBorder	= view_height - CANVAS_BORDER_DISTANCE;
		chartWidth = rightBorder - leftBorder;
		chartHeight = lowerBorder - upperBorder;
		numOfItems = num_of_items;
		yScaleStep = Y_SCALE_STEP;
		this->updateMinMax( min_X, max_X, min_Y, max_Y );

		dX = chartWidth / (float)numOfItems;
		dY = chartHeight / (float)numOfItems;
	}
	inline void updateMinMax( float min_X=0.0f, float max_X=0.0f, float min_Y=0.0f, float max_Y=0.0f )
	{
		minX = min_X;
		maxX = max_X;
		minY = min_Y;
		maxY = max_Y;
		maxRoundedY = (int)(maxY + Y_SCALE_STEP - (relative2AbsoluteVali(maxY, maxY) % Y_SCALE_STEP));    //the highest value represented in the y values scale
		variance = maxY - minY;          //variance of y values
	}
};


#endif
