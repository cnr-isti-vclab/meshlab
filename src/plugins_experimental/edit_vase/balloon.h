#ifndef BALLOON_H
#define BALLOON_H

#include <common/interfaces.h>
#include <common/meshmodel.h>   // CMesh0
#include <volume.h>             // Volumetric representation for balloon
#include "vase_utils.h"         // myscale
#include "fieldinterpolator.h"  // Interpolator
#include <meshlab/glarea.h>     // Correcting meshlab bug (??)

namespace vcg{

class Balloon{
public:
    /// Constructor: set default render mode
    Balloon( CMeshO& _cloud ) : cloud( _cloud ){}

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

    /// Interpolation parameter that guides view field interpolation.
    /// Lower levels of omega might cause overshoothing problems
    static const float OMEGA_VIEW_FIELD   = 1e8;

    /// Interpolation parameter which guides weight field
    /// This can be much smoother
    static const float OMEGA_WEIGHT_FIELD = 1e-1;

    /// Pointer to the underlying point cloud
    CMeshO& cloud;

    /// Volumetric representation
    MyVolume vol;

    /// Surface representation + handles to extra fields
    CMeshO surf;

    /// Accellerator for ray-surface intersection
    GridAccell gridAccell;

    /// Scalar field interpolator (one constraint per poking ray)
    FieldInterpolator dinterp;

    /// Extra field to keep track of init/interpolated distance field
    /// Refer to "User Defined Attributes" http://vcg.sourceforge.net/index.php/Tutorial
    CMeshO::PerVertexAttributeHandle<float> surf_df;

    /// How close am I to the data? (1: close 0: far)
    FieldInterpolator winterp;

    /// Extra field to keep track of how geometrically supported a piece of data is
    CMeshO::PerVertexAttributeHandle<float> surf_wf;
    
    /// Defines the rendering style of the ballon
    RenderModes rm;

    /// Keeps track of the number of evolution iterations done
    int numiterscompleted;

    /// Have these quantities been updated?
    bool isCurvatureUpdated;
    bool isDistanceFieldUpdated;
    bool isWeightFieldUpdated;
    bool isDistanceFieldInit;

    /// Creates a new volume of given size and padding
    void init(int gridsize, int gridpad);

    /// initializes a distance field according to view-point directions. Constraints are created
    /// on both sides of the isosurface to cope with noise but especially to guarantee convergence
    /// of the surface. If we go through a sample, a negative force field will be generated that will
    /// push the isosurface back close to the sample.bool initializeField();
    bool init_fields();

    /// Interpolates the fields defined in "init_fields"
    bool interp_fields();

    /// Creates a curvature field defiened on vertices and stored in Kh
    /// This will be used to create a mean-curvature flow type of evolutoin
    bool compute_curvature();

    /// Based on the distance + curvature fields, evolves the surface in an implicit way.
    /// ASSUMPTION: a correspondence has already been executed once!
    /// TODO: more documentation
    bool evolve();

    //--- Render & debug visualization
    void render(GLArea* gla);
#ifdef DEBUG
    void KhToVertexColor();
    void dfieldToVertexColor();
    void wfieldToVertexColor();
    void selectedFacesQualityToColor();
#endif

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
