#ifndef POVS_GENERATOR_H
#define POVS_GENERATOR_H

#include <vcg/space/point3.h>
#include <vector>
#include <map>

#define PI 3.1415926535

using namespace vcg;
using namespace std;

namespace vs
{
    template< class ScalarType >
    class PovsGenerator
    {

    public:

        typedef typename vcg::Point3< ScalarType > MyPoint;

        static void generatePovs
                (
                        int count,
                        ScalarType radius,
                        MyPoint center,
                        MyPoint coneAxis,
                        ScalarType coneAngle,
                        vector< MyPoint >& target
                        )
        {
            target.clear();
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

    private:

        static void randomPointsOnAUnitSphere( vector< MyPoint >& target, int count )
        {
            srand( 12345 );
            float dtheta = (2 * PI) / 100.0f;
            int r = 0;
            float theta = 0.0f;
            float du = 0.02f, u = 0.0f;
            float px = 0.0f, py = 0.0f;

            for( int i=0; i<count; i++ )
            {
                r = rand() % 100;
                theta = dtheta * r;

                r = rand() % 100;
                u = r * du - 1.0f;

                px = cos( theta ) * sqrt( 1.0f - pow( u, 2 ) );
                py = sin( theta ) * sqrt( 1.0f - pow( u, 2 ) );

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
    };
}

#endif // POVS_GENERATOR_H
