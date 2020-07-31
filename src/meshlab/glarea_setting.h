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

#ifndef GLAREA_SETTING_H
#define GLAREA_SETTING_H

#include<QString>
#include <vcg/space/color4.h>
#include <common/filter_parameter/rich_parameter_list.h>

class GLAreaSetting
{
public:

	vcg::Color4b baseLightAmbientColor;
	vcg::Color4b baseLightDiffuseColor;
	vcg::Color4b baseLightSpecularColor;
	inline static QString baseLightAmbientColorParam() {return "MeshLab::Appearance::baseLightAmbientColor";}
	inline static QString baseLightDiffuseColorParam() {return "MeshLab::Appearance::baseLightDiffuseColor";}
	inline static QString baseLightSpecularColorParam() {return "MeshLab::Appearance::baseLightSpecularColor";}

	vcg::Color4b fancyBLightDiffuseColor;
	inline static QString fancyBLightDiffuseColorParam() {return "MeshLab::Appearance::fancyBLightDiffuseColor";}

	vcg::Color4b fancyFLightDiffuseColor;
	inline static QString fancyFLightDiffuseColorParam() {return "MeshLab::Appearance::fancyFLightDiffuseColor";}


	vcg::Color4b backgroundBotColor;
	vcg::Color4b backgroundTopColor;
	vcg::Color4b logAreaColor;
	vcg::Color4b textColor;
	inline static QString backgroundBotColorParam() {return "MeshLab::Appearance::backgroundBotColor";}
	inline static QString backgroundTopColorParam() {return "MeshLab::Appearance::backgroundTopColor";}
	inline static QString logAreaColorParam() {return "MeshLab::Appearance::logAreaColor";}
	inline static QString textColorParam() {return "MeshLab::Appearance::textColor";}

	int textureMagFilter;
	int textureMinFilter;
	inline static QString textureMinFilterParam() {return "MeshLab::Appearance::textureMinFilter";}
	inline static QString textureMagFilterParam() {return "MeshLab::Appearance::textureMagFilter";}

	bool pointDistanceAttenuation;
	inline static QString pointDistanceAttenuationParam() {return "MeshLab::Appearance::pointDistanceAttenuation";}
	bool pointSmooth;
	inline static QString pointSmoothParam() {return "MeshLab::Appearance::pointSmooth";}
	float pointSize;
	inline static QString pointSizeParam() {return "MeshLab::Appearance::pointSize";}
	bool wheelDirection;
	inline static QString wheelDirectionParam() {return "MeshLab::Appearance::wheelDirection";}


	void updateGlobalParameterSet( RichParameterList& rps );
	static void initGlobalParameterSet( RichParameterList * defaultGlobalParamSet);

	RichParameterList *currentGlobalParamSet;
};


#endif // GLAREA_SETTING_H
