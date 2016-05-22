#ifndef RENDER_MODE_ACTIONS_H
#define RENDER_MODE_ACTIONS_H

#include <QAction>
#include "../common/ml_shared_data_context.h"


class MLRenderingAction : public QAction
{
    Q_OBJECT
public:
    MLRenderingAction(QObject* parent);
    MLRenderingAction(int meshid,QObject* parent);
    
    MLRenderingAction* copyAction(const MLRenderingAction* tocopy,QObject* newactionparent);

    virtual void updateRenderingData(MLRenderingData& rd) = 0;
    virtual bool isRenderingDataEnabled(const MLRenderingData& rd) const = 0;
    int meshId() const;
    void setMeshId(int meshid);
protected:
    virtual MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const = 0;
    void copyActionState(const MLRenderingAction* tocopy);
};

class MLRenderingBBoxAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingBBoxAction(QObject* parent);
    MLRenderingBBoxAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;

};

class MLRenderingPointsAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingPointsAction(QObject* parent);
    MLRenderingPointsAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingWireAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingWireAction(QObject* parent);
    MLRenderingWireAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingSolidAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingSolidAction(QObject* parent);
    MLRenderingSolidAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingEdgeWireAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingEdgeWireAction(QObject* parent);
    MLRenderingEdgeWireAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingHiddenLinesAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingHiddenLinesAction(QObject* parent);
    MLRenderingHiddenLinesAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingFlatAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingFlatAction(QObject* parent);
    MLRenderingFlatAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingSmoothAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingSmoothAction(QObject* parent);
    MLRenderingSmoothAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingPerVertTextCoordAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingPerVertTextCoordAction(QObject* parent);
    MLRenderingPerVertTextCoordAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingPerWedgeTextCoordAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingPerWedgeTextCoordAction(QObject* parent);
    MLRenderingPerWedgeTextCoordAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingDoubleLightingAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingDoubleLightingAction(QObject* parent);
    MLRenderingDoubleLightingAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingFancyLightingAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingFancyLightingAction(QObject* parent);
    MLRenderingFancyLightingAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingLightOnOffAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingLightOnOffAction(QObject* parent);
    MLRenderingLightOnOffAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingFaceCullAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingFaceCullAction(QObject* parent);
    MLRenderingFaceCullAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingPerMeshColorAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingPerMeshColorAction(QObject* parent);
    MLRenderingPerMeshColorAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
    void setColor(const QColor& col);
    void setColor(const vcg::Color4b& col);
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
private:
    vcg::Color4b _col;
};

class MLRenderingPerVertexColorAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingPerVertexColorAction(QObject* parent);
    MLRenderingPerVertexColorAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingPerFaceColorAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingPerFaceColorAction(QObject* parent);
    MLRenderingPerFaceColorAction(int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
};

class MLRenderingUserDefinedColorAction : public MLRenderingAction
{
    Q_OBJECT
public:
    MLRenderingUserDefinedColorAction(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm,QObject* parent);
    MLRenderingUserDefinedColorAction(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY pm,int meshid,QObject* parent);

    void updateRenderingData(MLRenderingData& rd);
    bool isRenderingDataEnabled(const MLRenderingData& rd) const;
    void setColor(const QColor& col);
    void setColor(const vcg::Color4b& col);
protected:
    MLRenderingAction* copyCtr(const MLRenderingAction* tocopy,QObject* newactionparent) const;
private:
    vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY _pm;
    vcg::Color4b _col;
};

#endif
