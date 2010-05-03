#include <qdatetime.h>
#include <QApplication>
#include <QProgressDialog>
#include "calculatesdf.h"
#include "rayintersect.h"
#include "vcg/math/matrix33.h" // so deprecated are not called!!

const int SDF_THREADS_NUM = 4;

Ray3f get_normal_opposite(const Point3f& p, const Point3f& n){
    //direction opposite the normal
    Point3f direction = n * -1;
    //construct segment from vertex pointing towards direction
    Ray3f ray;
    ray.Set( p + direction * 1e-5, direction );
    return ray;
}
std::list<Ray3f> get_cone_rays( const Point3f& p, const Point3f& n, const float degree, const int spread){
    std::list<Ray3f> list;
    Ray3f ray = get_normal_opposite(p, n);
    Point3f rayv = ray.Direction();
    rayv.normalized();

    //now check a cone
    Plane3f plane; plane.Init(ray.Origin(), ray.Direction());
    Point3f base = plane.Direction();

    // DEBUG: this might cause problems... depending how they define the matrix
    // Enriched_kernel::Aff_transformation_3 aff = rotationMatrixAroundVertex(base, degree);
    Matrix33<float> aff = RotationMatrix(base, degree);
    // Enriched_kernel::Aff_transformation_3 aroundRay = rotationMatrixAroundVertex(rayv, 2*M_PI/spread);
    Matrix33<float> aroundRay = RotationMatrix(rayv, 2*M_PI/spread);

    Point3f spinMe = rayv;
    spinMe = aff * spinMe;
    for (int i=0;i<spread; i++){
        Ray3f coneRay( p + spinMe * 1e-5, spinMe);
        list.push_back(coneRay);
        spinMe = aroundRay*spinMe;
    }
    return list;
}

void CalculateSDF::makeFacesNVolume(bool smoothing, bool smoothingAnisotropic, int smoothingIterations){
    // TODO
}

// basically shoot rays opposite the normal and do something with the distances we receive
float CalculateSDF::traceSdfConeAt( const Point3f& p, const Point3f& n ){
    float result = 0.0;
    Ray3f ray = get_normal_opposite(p, n);
    float distanceCounter = 0.0;
    float intersectionCounter = 0.0;
    float distance = FLT_MAX;

    // Stores the values of rays in the cone
    std::vector<float> values;
    std::vector<float> weights;
    float max_value = 0.0;
    values.reserve(numCones * raysInCone + 1);

    // First check straight opposite to normal
    if( m_rayIntersect->Query(ray, distance) ) {
        if (distance > max_value)
            max_value = distance;
        values.push_back(distance);
        weights.push_back(1.0);
    }

    const float gaussianVar = 120.0 * DEG2RAD;
    float gaussianDiv2 = 1 / (2 * pow(gaussianVar,2));

    for (int i=0; i<numCones; i++) {
        //now check cone
        float degree = coneSeperation * (i+1);
        std::list<Ray3f> list = get_cone_rays(p, n, degree, raysInCone);
        for (std::list<Ray3f>::iterator it = list.begin(); it != list.end(); it++) {
            distance = FLT_MAX;
            if( m_rayIntersect->Query( *it, distance ) ){
                float weight;
                if (gaussianWeights)
                    weight = /*gaussianDiv1 * */expf(-pow(degree,2) * gaussianDiv2);
                else
                    weight = 1.0;

                //intersectionCounter += weight;
                //distanceCounter += weight * distance;
                if (distance > max_value) max_value = distance;
                values.push_back(distance);
                weights.push_back(weight);
            }
        }
    }

    //calculate the average
    for (int i=0; i<values.size();i++) {
        distanceCounter += values[i] * weights[i];
        intersectionCounter += weights[i];
    }

    if (intersectionCounter) {
        result = distanceCounter / intersectionCounter;

        //fix really insane values
        std::nth_element(values.begin(), values.begin()+(values.size()/2), values.end());
        float median = values[values.size()/2];

        float std_dev = 0.0;
        for (int i=0; i<values.size();i++) {
            std_dev += pow(values[i]-result, 2);
        }
        std_dev = sqrt(std_dev/values.size());
        distanceCounter = intersectionCounter = 0.0;

        for (int i=0; i<values.size(); i++) {
            if (fabs(values[i]-/*result*/median) <= 0.5 * std_dev) {
                distanceCounter += values[i] * weights[i];
                intersectionCounter += weights[i];
            }
        }

        if (intersectionCounter)
            result = distanceCounter / intersectionCounter;
        else
            result = 0.0;
    } else {
        result = 0.0;
    }

    return result;
}

void CalculateSDF::init(SDFMODE mode, vector<float>& results){
    //--- Prepare the data structures
    m_rayIntersect->Init(*mesh, gridsize);

    if(mode == FACES){
        //--- Allocate the memory
        origins = vector<Ray3f>( mesh->fn );
        results.resize( mesh->fn,0 );

        //--- Initialize the query (face center/normal)
        for(int i=0; i<mesh->fn; i++)
            origins[i].Set( Barycenter(mesh->face[i]), mesh->face[i].N() );
    }
    else
        assert(0);
}

void CalculateSDF::compute(vector<float>& results){
  //--- Compute the queries
    for(int i=0; i<1 /*origins.size()*/; i++)
        results[i] = traceSdfConeAt( origins[i].Origin(), origins[i].Direction() );

    //--- Only apply smoothing if on vertices
    // if (!postprocess(mesh, onVertices, results, normalize, smoothing && onVertices))
    //      return false;

    // perform smoothing on the mesh itself
    // makeFacesNVolume(smoothing, smoothingAnisotropic, smoothingIterations );
}
