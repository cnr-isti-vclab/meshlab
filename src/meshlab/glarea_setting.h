#ifndef GLAREA_SETTING_H
#define GLAREA_SETTING_H

class GLAreaSetting
{
public:

    vcg::Color4b baseLightAmbientColor;
    vcg::Color4b baseLightDiffuseColor;
    vcg::Color4b baseLightSpecularColor;
    inline static QString baseLightAmbientColorParam()		{return "MeshLab::Appearance::baseLightAmbientColor";}
    inline static QString baseLightDiffuseColorParam()		{return "MeshLab::Appearance::baseLightDiffuseColor";}
    inline static QString baseLightSpecularColorParam()		{return "MeshLab::Appearance::baseLightSpecularColor";}

    vcg::Color4b fancyBLightDiffuseColor;
    inline static QString fancyBLightDiffuseColorParam()		{return "MeshLab::Appearance::fancyBLightDiffuseColor";}

    vcg::Color4b fancyFLightDiffuseColor;
    inline static QString fancyFLightDiffuseColorParam()		{return "MeshLab::Appearance::fancyFLightDiffuseColor";}


    vcg::Color4b backgroundBotColor;
    vcg::Color4b backgroundTopColor;
    vcg::Color4b logAreaColor;
    inline static QString backgroundBotColorParam()		{return "MeshLab::Appearance::backgroundBotColor";}
    inline static QString backgroundTopColorParam()		{return "MeshLab::Appearance::backgroundTopColor";}
    inline static QString logAreaColorParam()           {return "MeshLab::Appearance::logAreaColor";}

    int textureMagFilter;
    int textureMinFilter;
    inline static QString textureMinFilterParam()           {return "MeshLab::Appearance::textureMinFilter";}
    inline static QString textureMagFilterParam()           {return "MeshLab::Appearance::textureMagFilter";}

    bool pointDistanceAttenuation;
    inline static QString pointDistanceAttenuationParam()           {return "MeshLab::Appearance::pointDistanceAttenuation";}
    bool pointSmooth;
    inline static QString pointSmoothParam()           {return "MeshLab::Appearance::pointSmooth";}
    float pointSize;
    inline static QString pointSizeParam()           {return "MeshLab::Appearance::pointSize";}


    void updateGlobalParameterSet( RichParameterSet& rps );
    static void initGlobalParameterSet( RichParameterSet * defaultGlobalParamSet);

    RichParameterSet *currentGlobalParamSet;
};


#endif // GLAREA_SETTING_H
