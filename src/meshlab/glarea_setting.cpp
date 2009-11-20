#include "glarea.h"
#include "glarea_setting.h"

void GLAreaSetting::initGlobalParameterSet( RichParameterSet * defaultGlobalParamSet)
{
    defaultGlobalParamSet->addParam(new RichColor(backgroundBotColorParam(),QColor(128,128,255),"MeshLab Bottom BackGround Color ","MeshLab GLarea's BackGround Color(bottom corner)"));
    defaultGlobalParamSet->addParam(new RichColor(backgroundTopColorParam(),QColor(  0, 0,   0),"MeshLab Top BackGround Color(top corner)","MeshLab GLarea's BackGround Color(top corner)"));
    defaultGlobalParamSet->addParam(new RichColor(logAreaColorParam(),   QColor(255, 32, 32),"MeshLab GLarea's Log Area Color(bottom corner)","MeshLab GLarea's BackGround Color(bottom corner)"));


    defaultGlobalParamSet->addParam(new RichColor(baseLightAmbientColorParam()	,QColor( 32, 32, 32),"MeshLab Base Light Ambient Color","MeshLab GLarea's BackGround Color(bottom corner)"));
    defaultGlobalParamSet->addParam(new RichColor(baseLightDiffuseColorParam()	,QColor(204,204,204),"MeshLab Base Light Diffuse Color","MeshLab GLarea's BackGround Color(top corner)"));
    defaultGlobalParamSet->addParam(new RichColor(baseLightSpecularColorParam() ,QColor(255,255,255),"MeshLab Base Light Specular Color","MeshLab GLarea's BackGround Color(bottom corner)"));

    defaultGlobalParamSet->addParam(new RichColor(fancyBLightDiffuseColorParam()	,QColor(255,204,204),"MeshLab Base Light Diffuse Color","MeshLab GLarea's BackGround Color(top corner)"));
    defaultGlobalParamSet->addParam(new RichColor(fancyFLightDiffuseColorParam()	,QColor(204,204,255),"MeshLab Base Light Diffuse Color","MeshLab GLarea's BackGround Color(top corner)"));
}


void GLAreaSetting::updateGlobalParameterSet( RichParameterSet& rps )
{
    logAreaColor = rps.getColor4b(logAreaColorParam());
    backgroundBotColor =  rps.getColor4b(backgroundBotColorParam());
    backgroundTopColor =  rps.getColor4b(backgroundTopColorParam());

    baseLightAmbientColor =  rps.getColor4b(baseLightAmbientColorParam()	);
    baseLightDiffuseColor =  rps.getColor4b(baseLightDiffuseColorParam()	);
    baseLightSpecularColor =  rps.getColor4b(baseLightSpecularColorParam() );

    fancyBLightDiffuseColor =  rps.getColor4b(fancyBLightDiffuseColorParam());
    fancyFLightDiffuseColor =  rps.getColor4b(fancyFLightDiffuseColorParam());

}
