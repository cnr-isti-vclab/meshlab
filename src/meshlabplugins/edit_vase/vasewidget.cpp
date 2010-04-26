#include "vasewidget.h"
#include <QPainter>

using namespace vcg;

VaseWidget::VaseWidget(QWidget *parent, MeshModel &m, GLArea* gla)
    : QDockWidget(parent), gla( gla ){
	
    // Set orthographic visualization (meshlab assumes 5 degrees...)
    gla->fov = 5;
    // Setup the widget
	ui.setupUi(this);
    this->setWidget(ui.frame);  
    setFloating(false);
    setAllowedAreas(Qt::RightDockWidgetArea);
    ((QMainWindow*) parent)->addDockWidget( Qt::RightDockWidgetArea, this );

    // Create a new balloon based on the current point cloud
    balloon = new Balloon( m.cm );
    // Init slicer
    this->update_slice();
    // Turn off the volume slicer
    this->ui.slicebox->setVisible(false);
    // Inherit the render flags from the UI
    if( ui.viewDirs->isChecked() )
        balloon->rm |= Balloon::SHOW_CLOUD;
    if( ui.surface->isChecked() )
        balloon->rm |= Balloon::SHOW_SURF;
    if( ui.surf2acc_check->isChecked() )
        balloon->rm |= Balloon::SHOW_3DDDR;
    if( ui.surf2vol_check->isChecked() )
        balloon->rm |= Balloon::SHOW_SURF_TO_VOL;

    // Open it up
    this->show();

    // Launch an init by default
    this->on_initButton_released();
}
VaseWidget::~VaseWidget(){
    delete balloon;
}
void VaseWidget::decorate(MeshModel &m, GLArea* gla){
    glPushMatrix();
        glMultMatrix(m.cm.Tr);
        balloon->render( gla );
    glPopMatrix();
}
void VaseWidget::update_slice(){
    int offset = ui.slice_offset->value();
    int max    = ui.slice_offset->maximum(); // range max: 99
    int comboidx    = ui.slice_dim_combo->currentIndex();
    float offsetf = 0;

    // No selection
    if( comboidx == 0 ){
        // Initialize the slice visualizer with image
        QPixmap pix( ui.slice_label->size() );
        QPainter painter( &pix );
        pix.fill( QColor(255,255,255) );
        painter.setPen( QColor(0,0,0) );
        painter.drawText(50, ui.slice_label->height()/2, QString("Slice Undefined") );
        ui.slice_label->setPixmap( pix );       
    }
    else{
        int dim = comboidx-1;
        // dim==0 is disabled, thus, dim-1 is dim offset in volume
        int slice = balloon->vol.size(dim)*offset/max;
        qDebug() << "Showing slice...." << slice << endl;

        // Extract a pixmap slice of the balloon
        QPixmap pix = balloon->vol.getSlice(dim, slice);
        // Scale to fit the GUI
        pix = pix.scaled(ui.slice_label->size(), Qt::KeepAspectRatio);
        // Update the GUI
        ui.slice_label->setPixmap( pix );
        // Send request to plane representation
        offsetf = balloon->vol.off2pos( slice, dim  );
    }
}

// Init ballon
void VaseWidget::on_initButton_released(){
    int gridsize = ui.gridsize->text().toInt();
    int gridpad  = ui.padsize->text().toInt();
    balloon->init( gridsize, gridpad );
    gla->log.Log(GLLogStream::FILTER, "Field refreshed with new distances");
    gla->update();

    // Pick Z slice and update slice view
    // qDebug() << "DEBUG: Automatically pick slize Z after initialization" << endl;
    // ui.slice_dim_combo->setCurrentIndex(3);
    // update_slice();
}
void VaseWidget::on_slice_dim_combo_currentIndexChanged(int){
    update_slice();
}
void VaseWidget::on_slice_offset_sliderMoved(int){
    update_slice();
}
void VaseWidget::on_iterationButton_released(){
    for(int i=0; i<ui.numItersSpin->value(); i++){
        balloon->updateViewField();
        balloon->interpolateField();
        balloon->computeCurvature();
        balloon->evolveBalloon();
        gla->log.Logf(GLLogStream::FILTER, "Finished iteration %d", balloon->numiterscompleted);
        gla->update();
    }
}
void VaseWidget::on_refreshButton_released(){
    balloon->updateViewField();
    gla->log.Log(GLLogStream::FILTER, "Refreshed view-based distance field");
    balloon->render(gla);
    gla->update();
}
void VaseWidget::on_interpButton_released(){
    balloon->interpolateField();
    balloon->rm |= Balloon::SURF_VCOLOR;
    gla->log.Log(GLLogStream::FILTER, "Distance field interpolated");
    gla->update();
}
void VaseWidget::on_evolveButton_released(){
    balloon->evolveBalloon();
    gla->log.Logf(GLLogStream::FILTER, "Finished ballon evolution iteration %d", balloon->numiterscompleted);
    gla->update();
}
void VaseWidget::on_laplButton_released(){
    balloon->computeCurvature();
    balloon->rm |= Balloon::SURF_VCOLOR;
    gla->log.Logf(GLLogStream::FILTER, "Finished iteration %d", balloon->numiterscompleted);
    gla->update();
}

void VaseWidget::on_viewDirs_toggled(bool checked){
    if( checked ) balloon->rm |=  Balloon::SHOW_CLOUD;
    else          balloon->rm &= ~Balloon::SHOW_CLOUD;
    gla->update();
}
void VaseWidget::on_surface_toggled(bool checked){
    if( checked ) balloon->rm |=  Balloon::SHOW_SURF;
    else          balloon->rm &= ~Balloon::SHOW_SURF;
    gla->update();
}
void VaseWidget::on_surf2vol_check_toggled(bool checked){
    if( checked ) balloon->rm |=  Balloon::SHOW_SURF_TO_VOL;
    else          balloon->rm &= ~Balloon::SHOW_SURF_TO_VOL;
    gla->update();
}

void VaseWidget::on_surf2acc_check_toggled(bool checked){
    if( checked ) balloon->rm |=  Balloon::SHOW_3DDDR;
    else          balloon->rm &= ~Balloon::SHOW_3DDDR;
    gla->update();
}




/*
// Draws the slicing planes
if( false ){
    const vcg::Box3f& box = balloon->vol.get_bbox();
    glDisable(GL_LIGHTING);
    // glEnable(GL_BLEND);
    glPushMatrix();
        // Meshlab loaded a mesh and put it in center view... need correct view
        glMultMatrix(m.cm.Tr);
        glColor4f(.7,.7,.7,0.9);
        switch( slice_dim ){
        // Do nothing
        case 0:
            break;
        // Slice along X
        case 1:
            glTranslatef(slice_off, 0, 0 );
            glRotatef(-90, 0, 1, 0);
            glRectf(box.min[2], box.min[1], box.max[2], box.max[1]);
            break;
        // Slice along Y
        case 2:
            glTranslatef(0,slice_off, 0 );
            glRotatef(90, 1, 0, 0);
            glRectf(box.min[0], box.min[2], box.max[0], box.max[2]);
            break;
        // Z: nothing to rotate
        case 3:
            glTranslatef(0, 0, slice_off );
            glRectf(box.min[0], box.min[1], box.max[0], box.max[1]);
            break;
        }
    glPopMatrix();
    glEnable(GL_LIGHTING);
    // glDisable(GL_BLEND);
} */
