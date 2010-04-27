#ifndef BALLOON_H
#define BALLOON_H

#include <common/interfaces.h>
#include <common/meshmodel.h> // CMesh0
#include <volume.h> // Volumetric representation for balloon
#include "vase_utils.h" // myscale
#include "fieldinterpolator.h" // Least square interpolator
#include <meshlab/glarea.h> // Correcting meshlab bug

namespace vcg{

class Balloon{
public:
    enum RenderMode{
         DEFAULT            = 0x00000000, // Show the cloud

         SHOW_CLOUD         = 0x00000001, // Show the cloud
         SHOW_VOLUME        = 0x00000002, // Show the balloon
         SHOW_SURF          = 0x00000004, // Show the isosurface
         // SHOW_ACCEL         = 0x00000008, // Show the accelleration grid
         SHOW_3DDDR         = 0x00000010, // 3D-DDR voxels
         SHOW_SURF_TO_VOL   = 0x00000020,

         SURF_XCOLOR        = 0x00000100, // Show no color
         SURF_VCOLOR        = 0x00000200, // Show vertex color
         SURF_FCOLOR        = 0x00000400, // Show face color
    };
    Q_DECLARE_FLAGS(RenderModes, RenderMode)

    /// Pointer to the underlying point cloud
    CMeshO& cloud;
    /// Volumetric representation
    MyVolume vol;
    /// Surface representation
    CMeshO surf;
    /// Accellerator for ray-surface intersection
    GridAccell gridAccell;
    /// Scalar field interpolator
    FieldInterpolator finterp;
    /// Defines the rendering style of the ballon
    RenderModes rm;
    /// Keeps track of the number of evolution iterations done
    int numiterscompleted;

    /// Constructor: set default render mode
    Balloon( CMeshO& _cloud ) : cloud( _cloud ){}


    //--- Logic
    void init(int gridsize, int gridpad);
    void updateViewField();
    void interpolateField();
    void computeCurvature();
    void evolveBalloon();

    //--- Render
    void render(GLArea* gla);

private:
    /// Renders sample view directions on the cloud
    void render_cloud();
    /// Renders the balloon isosurface with particular RenderMode
    void render_isosurface(GLArea* gla);
    /// Renders volume boxes and correspondences to surface
    void render_surf_to_vol();
    void render_surf_to_acc();
};



} //::vcg

#endif // BALLOON_H
