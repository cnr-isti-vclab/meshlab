#ifndef RENDER_MODE_ACTIONS_H
#define RENDER_MODE_ACTIONS_H

#include "../common/meshmodel.h"
#include <QAction>

class RenderModeAction : public QAction
{
    Q_OBJECT
public:
    RenderModeAction(const QString& title,QObject* parent);
    RenderModeAction(const unsigned int meshid,const QString& title,QObject* parent);
    RenderModeAction(const QIcon& icn,const QString& title,QObject* parent);
    RenderModeAction(const unsigned int meshid,const QIcon& icn,const QString& title,QObject* parent);

    void updateRenderMode(QList<RenderMode>& rmlist);
    virtual void updateRenderMode(RenderMode& rm) = 0;
    virtual bool isRenderModeEnabled(const RenderMode& rm) const = 0;
    virtual bool isBufferObjectUpdateRequired() const;

private:
    void commonInit(const unsigned int meshid);

};

class RenderModeBBoxAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeBBoxAction(QObject* parent);
    RenderModeBBoxAction(const unsigned int meshid,QObject* parent);

protected:
    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};

class RenderModePointsAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModePointsAction(QObject* parent);
    RenderModePointsAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};

class RenderModeWireAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeWireAction(QObject* parent);
    RenderModeWireAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};

class RenderModeHiddenLinesAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeHiddenLinesAction(QObject* parent);
    RenderModeHiddenLinesAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeFlatLinesAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeFlatLinesAction(QObject* parent);
    RenderModeFlatLinesAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};


class RenderModeFlatAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeFlatAction(QObject* parent);
    RenderModeFlatAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};

class RenderModeSmoothAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeSmoothAction(QObject* parent);
    RenderModeSmoothAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};

class RenderModeTexturePerVertAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeTexturePerVertAction(QObject* parent);
    RenderModeTexturePerVertAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};

class RenderModeTexturePerWedgeAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeTexturePerWedgeAction(QObject* parent);
    RenderModeTexturePerWedgeAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};

class RenderModeDoubleLightingAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeDoubleLightingAction(QObject* parent);
    RenderModeDoubleLightingAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeFancyLightingAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeFancyLightingAction(QObject* parent);
    RenderModeFancyLightingAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeLightOnOffAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeLightOnOffAction(QObject* parent);
    RenderModeLightOnOffAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeFaceCullAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeFaceCullAction(QObject* parent);
    RenderModeFaceCullAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
};

class RenderModeColorModeNoneAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeColorModeNoneAction(QObject* parent);
    RenderModeColorModeNoneAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};

class RenderModeColorModePerMeshAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeColorModePerMeshAction(QObject* parent);
    RenderModeColorModePerMeshAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};

class RenderModeColorModePerVertexAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeColorModePerVertexAction(QObject* parent);
    RenderModeColorModePerVertexAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};

class RenderModeColorModePerFaceAction : public RenderModeAction
{
    Q_OBJECT
public:
    RenderModeColorModePerFaceAction(QObject* parent);
    RenderModeColorModePerFaceAction(const unsigned int meshid,QObject* parent);

    void updateRenderMode(RenderMode& rm);
    bool isRenderModeEnabled(const RenderMode& rm) const;
    bool isBufferObjectUpdateRequired() const;
};


#endif
