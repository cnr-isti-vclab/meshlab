//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
#ifndef __CIFXUTILITIES_H__
#define __CIFXUTILITIES_H__

// here are a few utility functions for dealing with
// OpenGL texture pixels (texels)...
// Texture manager uses RGBA texels which in IA are 
// stored as (MSB)--> A B G R <--(LSB)...

// WINDOWS version
//inline U32 MakeTexel( U32 red, U32 green, U32 blue, U32 alpha) {
//	return red | (green << 8) | (blue << 16) | (alpha << 24);
//}

inline U32 MakeTexel( U32 red, U32 green, U32 blue, U32 alpha) {
	return red | (green << 8) | (blue << 16) | (alpha << 24);
}

inline U32 MakeTexelNoShiftAlpha( U32 red, U32 green, U32 blue, U32 alpha) {
	return red | (green << 8) | (blue << 16) | alpha;
}

inline U32 TexelGetRed( U32 texel) {
	return (texel & 0x000000FF);
}

inline U32 TexelGetGreen( U32 texel) {
	return ((texel & 0x0000FF00) >> 8);
}

inline U32 TexelGetBlue( U32 texel) {
	return ((texel & 0x00FF0000) >> 16);
}

inline U32 TexelGetAlpha( U32 texel) {
	return ((texel & 0xFF000000) >> 24);
}

inline U32 TexelGetAlphaNoShift( U32 texel) {
	return (texel & 0xFF000000);
}

inline U32 TexelSaturateChannel( U32 value) {
	return (value > 255) ? 255 : value;
}

#endif // !__CIFXUTILITIES_H__

// eof
