/*
** point_based_renderer.h Point Based Render header.
**
**
**   history:	created  02-Jul-07
*/


#ifndef __POINT_BASED_RENDERER_H__
#define __POINT_BASED_RENDERER_H__

#include <iostream>

#include "surfel.hpp"
#include "materials.h"
#include "object.h"

/**
 * Base class for rendering algorithms.
 **/
class PointBasedRenderer
{
 private:

 public:
  /**
   * Default constructor, creates an 1024x1024 screen size.
   **/
  PointBasedRenderer() :
    canvas_width(1024), canvas_height(1024), scale_factor(1.0),
    material_id(0), depth_test(1), back_face_culling(1), elliptical_weight(0),
    reconstruction_filter_size(1.0), prefilter_size(1.0)
    {}

  /**
   * Constructor for given screen size.
   * @param w Screen width.
   * @param h Screen height.
   **/
  PointBasedRenderer(int w, int h) :
    canvas_width(w), canvas_height(h), scale_factor(1.0),
    material_id(0), depth_test(1), back_face_culling(1), elliptical_weight(0),
    reconstruction_filter_size(1.0), prefilter_size(1.0)
    {}
  
  virtual ~PointBasedRenderer() {}

  virtual void init ( void ) {}

   /**
    * Render point based model using deferred shading (per pixel shading).
    **/
	virtual void draw( void ) {}

	/**
 	 * Interpolate samples in screen space using pyramid method.
	**/
	virtual void interpolate( void ) {}

	/**
	 * Projects samples to screen space.
	 * @param p Point to primitives instance containing samples.
	**/
	virtual void projectSamples(Object* ) {}

   /**
    * Clears all buffers, including those of the framebuffer object.
    **/
   virtual void clearBuffers( void ) {}

   /**
    **/
   virtual void setMinimumRadiusSize(double s) { minimum_radius_size = s; }


   /**
    * Sets the size of the prefilter (default = 1.0).
    * This filter works as an increment of the radius size in screen space.
    * @param s Prefilter size.
    **/
   virtual void setPrefilterSize(double s) { prefilter_size = s; }

   /**
    * Sets the size of the reconstruction filter (default = 1.0).
    * This filter works as a multiplier of the radius size in screen space.
    * @param s Reconstruction filter size.
    **/
   virtual void setReconstructionFilterSize(double s) { reconstruction_filter_size = s; }

   /**
    * Sets the size of the reconstruction filter (default = 1.0).
    * This filter works as a multiplier of the radius size in screen space.
    * @param s Reconstruction filter size.
    **/
   virtual double getReconstructionFilterSize(void) { return reconstruction_filter_size; }

   /**
    * Sets the quality threshold for interpolating samples.
    * @param q Quality threshold.
    **/
   virtual void setQualityThreshold(double q) { quality_threshold = q; }

   /**
    * Sets the quality per vertex flag.
    * @param q Quality flag.
    **/
   virtual void setQualityPerVertex(bool q) { quality_per_vertex = q; }

   /** 
    * Sets the kernel size, for templates rendering only.
    * @param Kernel size.
    **/
   virtual void setGpuMaskSize ( int ) {}

   /** 
    * Sets eye vector used mainly for backface culling.
    * @param e Given eye vector.
    **/
   void setEye (Point3f e) {
     eye = e;
   }

   /** 
    * Sets scale factor for zooming, scales sample's radius size.
    * @param s Given scale factor.
    **/
   void setScaleFactor (double s) {
     scale_factor = s;
   }

   /**
    * Sets the material id number for rendering.
    * @param m Material id.
    **/
   void setMaterial (const int m) {
     if (m < NUM_MATERIALS)
       material_id = m;
   }

   /**
    * Gets the material id number for rendering.
    * @return Current material id.
    **/
   const int getMaterial ( void ) {
     return material_id;
   }

   void upMaterial ( void ) {
     ++material_id;
     if (material_id == NUM_MATERIALS)
       material_id = 0;

   }
   void downMaterial ( void ) {
     --material_id;
     if (material_id < 0)
       material_id = NUM_MATERIALS - 1;
   }

   /**
    * Sets the depth test flag on/off.
    * @param d Given depth test state.
    **/
   void setDepthTest( const bool d ) {
     depth_test = d;
   }

   /**
    * Sets the backface culling flag on/off.
    * @param b Given backface culling state.
    **/
   void setBackFaceCulling( const bool b ) {
     back_face_culling = b;
   }

   void setEllipticalWeight( const bool w ) {
     elliptical_weight = w;
   }

 protected:

   /// Canvas width.
   int canvas_width;
   /// Canvas height.
   int canvas_height;

   /// Eye position.
   Point3f eye;

   /// Scale factor (camera zooming)
   double scale_factor;

   /// Identification of the material from materials.h table.
   int material_id;

   /// Flag to turn on/off depth test
   bool depth_test;

   /// Flag to turn on/off back_face_culling
   bool back_face_culling;

   /// Flag to turn on/off elliptical weight
   bool elliptical_weight;

   /// Flag to turn on/off quality per vertex interpolation.
   bool quality_per_vertex;

   /// Size of quality threshold for interpolation.
   double quality_threshold;

   /// Size of reconstruction filter.
   double reconstruction_filter_size;
   /// Size of antialising filter.
   double prefilter_size;
   /// Minimum smallest radius size.
   double minimum_radius_size;

};

//inline void check_for_ogl_error( char * from = 0) {
inline void check_for_ogl_error( string from = "") {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
	cerr << from << endl;
	cerr << __FILE__ << " (" << __LINE__ << ") " << gluErrorString(err) << endl;
  }
}

#endif
