#ifndef POVS_GENERATOR_H
#define POVS_GENERATOR_H

#include <vcg/space/point3.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PI 3.1415926535

using namespace vcg;

namespace agl
{
    namespace vrs
    {
        template< class ScalarType >
        class PovsGenerator
        {

        public:

            typedef Point3< ScalarType >    MyPoint;

            static void generatePovs( int count,
                               ScalarType radius,
                               MyPoint center,
                               vector< MyPoint >& target )
            {
                target.clear();

                srand( time(NULL) );

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

                energyMinimization( target );

                for( unsigned int i=0; i<target.size(); i++ )
                {
                    target[i] *= radius;
                    target[i] += center;
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
        };
    }
}


#endif // POVS_GENERATOR_H
