#ifndef EDIT_VIRTUAL_SCAN_PLUGIN_H
#define EDIT_VIRTUAL_SCAN_PLUGIN_H

/*  Qt includes */
#include    <QObject>
#include    <QDockWidget>

/*  meshlab includes */
#include    <common/interfaces.h>

/*  my includes */
#include    "edit_vs_widget.h"
#include    "vs/sampler.h"

using namespace vs;

class EditVirtualScanPlugin: public QObject, public MeshEditInterface
{

    Q_OBJECT
    Q_INTERFACES(MeshEditInterface)

public:

    EditVirtualScanPlugin           ( void );
    virtual ~EditVirtualScanPlugin  ( void );

    static const QString    Info    ( void );

    /* MeshEditInterface implementation */
    virtual bool    StartEdit           ( MeshModel& m, GLArea* parent );
    virtual void    EndEdit             ( MeshModel& m, GLArea* parent );
    virtual void    Decorate            ( MeshModel& m, GLArea* parent );
    virtual void    mousePressEvent     ( QMouseEvent* event, MeshModel& m, GLArea* parent );
    virtual void    mouseMoveEvent      ( QMouseEvent* event, MeshModel& m, GLArea* parent );
    virtual void    mouseReleaseEvent   ( QMouseEvent* event, MeshModel& m, GLArea* parent );

private:

    VSParameters    params;
    MeshModel*      inputMeshModel;
    bool            unifyClouds;
    GLArea*         glArea;

    QDockWidget*    dockWidget;
    EditVsWidget*   editVsWidget;

    void    connectWidgetEvents     ( void );
    void    disconnectWidgetEvents  ( void );

private slots:

    void    customPovsMode      ( bool on );
    void    povsNumberChanged   ( int povsNumber );
    void    xAxisChanged        ( float newVal );
    void    yAxisChanged        ( float newVal );
    void    zAxisChanged        ( float newVal );
    void    povsConeGapChanged  ( int newGap );
    void    uResChanged         ( int newRes );
    void    uConeGapChanged     ( int newGap );
    void    fResChanged         ( int newRes );
    void    bigJumpChanged      ( float newJump );
    void    borderFGapChanged   ( int newGap );
    void    smallJumpChanged    ( float newJump );
    void    normalsAngleChanged ( int newAngle );
    void    unifyPointClouds    ( bool unify );
    void    go                  ( void );

};

#endif
