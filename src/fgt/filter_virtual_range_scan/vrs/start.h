#ifndef START_H
#define START_H

#include <vcg/space/point3.h>
#include <wrap/gl/shot.h>
#include <vector>
#include <map>
#include "stage.h"

#define PI 3.1415926535

using namespace vcg;

namespace vrs
{
    template< class Mesh >
    class Start: public Stage
    {

    public:
        typedef typename Mesh::ScalarType           ScalarType;
        typedef typename vcg::Point3< ScalarType >  MyPoint;
        typedef typename vcg::Shot< ScalarType >             ShotType;

        Start( Mesh* src, int povs, SamplerResources* res )
            :Stage( res )
        {
            m = src;

            if( res->params->useCustomPovs )
            {
                assert( res->params->customPovs.size() > 0 );
            }
            else
            {   // distribute povs uniformly in a cone of directions
                this->povs = povs;
                radius = ( m->bbox.Diag() / 2 ) * 1.2;
                center = m->bbox.Center();
                MyPoint coneAxis( res->params->coneAxis[0],
                                  res->params->coneAxis[1],
                                  res->params->coneAxis[2] );
                coneAxis.Normalize();
                float coneGap = ( res->params->coneGap / 180.0f ) * PI;
                generatePovs( povs, radius, center, views, coneAxis, coneGap );
                generateUpVectors( views, center, upVector );
            }

            currentPov = 0;
        }

        ~Start( void ){}

        virtual void go( void )
        {
            // generates attributes screenshots
            on( "start", "start_shader" );

            if( res->params->useCustomPovs )
            {
                // there's no need to call UnsetView(), since the modelview
                // and projection matrices are resetted in the killer stage
                /*
                if( currentPov > 0 )
                {
                    GlShot< ShotType >::UnsetView();
                }
                */

                Pov& p = res->params->customPovs[ currentPov ];
                ScalarType nearPlane, farPlane;
                GlShot< ShotType >::GetNearFarPlanes( p.first, m->bbox, nearPlane, farPlane );
                GlShot< ShotType >::SetView( p.first, nearPlane, farPlane );

                glEnable( GL_SCISSOR_TEST );
                Box2i& scissorBox = p.second;
                vcg::Point2i min = scissorBox.min;
                int width = scissorBox.DimX();
                int height = scissorBox.DimY();
                glScissor( (GLint)min.X(), (GLint)min.Y(), (GLsizei)width, (GLsizei)height );
            }
            else
            {
                Point3< ScalarType >& p = views[ currentPov ];
                Point3< ScalarType >& up = upVector[ currentPov ];
                glMatrixMode( GL_PROJECTION );
                glLoadIdentity();
                glOrtho( -radius, radius, -radius, radius, -radius, radius );

                glMatrixMode( GL_MODELVIEW );
                glLoadIdentity();
                gluLookAt( center.X(), center.Y(), center.Z(),
                           p.X(), p.Y(), p.Z(),
                           up.X(), up.Y(), up.Z() );
            }

            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            render();
            //res->fbo->screenshots( buf );
            off();

            if( res->params->useCustomPovs )
            {   
                glDisable( GL_SCISSOR_TEST );
            }

            Utils::saveMatrices();

            // inverts depth background to white
            on( "depth_fixer", "depth_fixer_shader" );
            string data[] = { "startMask", "depthBlack" };
            bindPixelData( data, 2, data );
            Utils::fullscreenQuad();
            off();
        }

        bool nextPov( void )
        {
            currentPov++;
            if( res->params->useCustomPovs )
            {
                return ( currentPov < (int)res->params->customPovs.size() );
            }
            else
            {
                return ( currentPov < povs );
            }
        }

        //private:
        int currentPov;
        vector< MyPoint > views;
        Mesh* m;
        int povs;
        vector< MyPoint > upVector;
        Point3< ScalarType > center;
        ScalarType radius;


        /* ------------------ simple mesh renderer ----------------------------------- */
        void render( void )
        {
            if( m->vert.size() == 0 ) return;

            typename Mesh::FaceIterator fi;
            int i = 0;
            vcg::Point3f* pp = 0;
            vcg::Point3f* np = 0;

            glBegin( GL_TRIANGLES );

            for( fi = m->face.begin(); fi != m->face.end(); ++fi )
            {
                for( i = 0; i < 3; i++ )
                {
                    np = &( (*fi).V(i)->N() );
                    glNormal3f( np->X(), np->Y(), np->Z() );

                    pp = &( (*fi).V(i)->P() );
                    glVertex3f( pp->X(), pp->Y(), pp->Z() );
                }
            }

            glEnd();
        }
        /* --------------------------------------------------------------------------- */

        /* -------------------- povs generation -------------------------------------- */
        static void generatePovs( int count,
                                  ScalarType radius,
                                  MyPoint center,
                                  vector< MyPoint >& target,
                                  MyPoint& coneAxis,
                                  float coneAngle )
        {
            target.clear();

            // NB: the cone axis must be inverted due to the ortographic projection,
            // that sets a negative near plane
            coneAxis = -coneAxis;
            vector< MyPoint > v;

            if( coneAngle < 2 * PI )
            {
                // computes the number of points to be generated on the unit sphere
                int n = ceil((float)(2 * count)/(1 - cos(coneAngle/2)));

                // generates n random points on the unit sphere
                randomPointsOnAUnitSphere( v, n );

                // rejects point with the energy minimization algorithm
                energyMinimization( v );


                // fills the target points vector
                computePointsInsideCone( v, target, count, coneAxis, coneAngle );
            }
            else
            {
                // generates n random points on the unit sphere
                randomPointsOnAUnitSphere( target, count );

                // rejects point with the energy minimization algorithm
                energyMinimization( target );
            }

            // scales and translates points to distribute them
            // onto the desired sphere's surface
            for( unsigned int i=0; i<target.size(); i++ )
            {
                target[i] *= radius;
                target[i] += center;
            }
        }

        static void randomPointsOnAUnitSphere( vector< MyPoint >& target, int count )
        {
            srand( 12345 );
            float dtheta = (2 * PI) / 100.0;
            int r = 0;
            float theta = 0.0;
            float du = 0.02, u = 0.0;
            float px = 0.0, py = 0.0;

            for( int i=0; i<count; i++ )
            {
                r = rand() % 100;
                theta = dtheta * r;

                r = rand() % 100;
                u = r * du - 1;

                px = cos( theta ) * sqrt( 1 - pow( u, 2 ) );
                py = sin( theta ) * sqrt( 1 - pow( u, 2 ) );

                target.push_back( MyPoint( px, py, u ) );
            }
        }

        static void energyMinimization( vector< MyPoint >& points )
        {
            int iterations = 100;
            while( iterations-- )
            {
                for( unsigned int i=0; i<points.size(); i++ )
                {
                    MyPoint force;
                    MyPoint res( 0.0, 0.0, 0.0 );
                    MyPoint vec;
                    ScalarType fac;

                    vec = points[i];

                    // minimize with other samples
                    for( unsigned int j=0; j<points.size(); j++ )
                    {
                        force = vec - points[j];
                        fac = force.dot( force );
                        if( fac != 0.0 )
                        {
                            fac = 1.0 / fac;
                            res += ( force * fac );
                        }
                    }

                    res *= 0.5;
                    points[i] += res;
                    points[i] = Normalize( points[i] );
                }
            }
        }

        static void computePointsInsideCone( vector< MyPoint >& src,
                                             vector< MyPoint >& dst,
                                             int count,
                                             MyPoint& coneAxis,
                                             float coneAngle )
        {
            float alpha = coneAngle / 2.0f;
            float thresholdCosine = cos( alpha );
            map< float, MyPoint* > outerPoints;
            float dotResult = 0.0f;

            for( unsigned int i=0; i<src.size(); i++ )
            {
                dotResult = coneAxis.dot( src[i] );
                if( dotResult > thresholdCosine && (int)dst.size() < count )
                {
                    dst.push_back( src[i] );
                }
                else
                {
                    outerPoints[ dotResult ] = &(src[i]);
                }
            }

            int missingPoints = count - dst.size();

            if( missingPoints > 0 )
            {
                typename map< float, MyPoint* >::reverse_iterator ri = outerPoints.rbegin();
                int i = 0;
                while( i < missingPoints && ri != outerPoints.rend() )
                {
                    dst.push_back( *((*ri).second) );
                    ++i;
                    ++ri;
                }
            }
        }
        /* --------------------------------------------------------------------------- */

        void generateUpVectors( vector< MyPoint >& povs, MyPoint& center,
                                vector< MyPoint >& target )
        {
            srand( 12345 );
            target.clear();
            MyPoint dir;
            MyPoint up;
            bool ok = false;

            for( unsigned int i=0; i<povs.size(); i++ )
            {
                dir = (povs[i] - center).Normalize();

                ok = false;
                while( !ok )
                {
                    up.X() = rand() % 1000;
                    up.Y() = rand() % 1000;
                    up.Z() = rand() % 1000;
                    up.Normalize();
                    ok = ( vcg::math::Abs(up.dot( dir )) < 0.8 );
                }

                target.push_back( up );
            }
        }
    };
}

#endif // START_H
