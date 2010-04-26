#include "edit_vase.h"
#include "wrap/gui/trackball.h"
#include "wrap/qt/trackball.h" //QT2VCG trackball function

Vase::Vase(){
    action = new QAction(QIcon(":/images/vase.png"),"Volume Aware Surface Extraction", this);
    action->setCheckable(true);
}






//--- Trackball controls similarly to the ones in non-edit mode
void Vase::mousePressEvent(QMouseEvent* e, MeshModel &, GLArea* gla){
    gla->trackball.MouseDown(e->x(),gla->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
    gla->update();
}
void Vase::mouseMoveEvent(QMouseEvent* e, MeshModel &, GLArea* gla){
    gla->trackball.MouseMove(e->x(),gla->height()-e->y());
    gla->update();
}
void Vase::mouseReleaseEvent(QMouseEvent* e, MeshModel &, GLArea* gla){
    gla->trackball.MouseUp(e->x(),gla->height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
    gla->update();
}

// Must be at the end of everything in CPP file or get segfault at plugin load
Q_EXPORT_PLUGIN(Vase)
