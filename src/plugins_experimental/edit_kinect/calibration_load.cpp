#include "calibration_data.h"

vcg::Matrix33f rgb_intrinsics,depth_intrinsics,R;
float rgb_distortion[5];
float depth_distortion[5];
vcg::Point3f T;
int rgb_size[2],raw_rgb_size[2],depth_size[2],raw_depth_size[2];


void DefaultCalibrationData(){

    rgb_intrinsics[0][0] = 5.2113739925590016e+02;
    rgb_intrinsics[0][2] = 3.1491244343525125e+02;
    rgb_intrinsics[1][1] = 5.1991686025545800e+02;
    rgb_intrinsics[1][2] = 2.4026344772264517e+02;

    float mR[9] = {9.9969879814870832e-01, -4.7653773427246268e-03,
                   2.4074969549503451e-02, 4.3910927443567907e-03,
                   9.9986905022011940e-01, 1.5575644976267967e-02,
                   -2.4146040763208549e-02, -1.5465238139056611e-02,
                   9.9958881302501768e-01};
    R = vcg::Matrix33f(&mR[0]);
    T = vcg::Point3f(2.1080184637889673e-02, 5.3046733066992597e-04,
                     4.0461983367107872e-02 );
}


FILE * f;

bool SkipS(int n){
    char waste[255];
    for(unsigned int i = 0; i < n; ++i)
        if(fscanf(f,"%s",&waste[0])<=0) return false;
    return true;
}

bool Read33Matrix(vcg::Matrix33f& m ){
    char waste[255];
    if(!SkipS(10)) return false;
    for(unsigned int i = 0; i < 3; ++i)
        for(unsigned int j = 0; j < 3; ++j)
            if(fscanf(f,"%f %s",& m[i][j],&waste[0])<=0) return false;
    return true;
}
bool ReadDistortion( float d[5]){
    char waste[255];
    if(!SkipS(10)) return false;
    for(unsigned int i = 0; i < 5; ++i)
        if(fscanf(f,"%f %s",& d[i],&waste[0])<=0) return false;
    return true;
}
bool Read3Vector( vcg::Point3f v){
    char waste[255];
    if(!SkipS(10)) return false;
    for(unsigned int i = 0; i < 3; ++i)
        if(fscanf(f,"%f %s",& v[i],&waste[0])<=0) return false;
    return true;
}
bool Read2Vector( int d[2]){
    char waste[255];
    if(!SkipS(10)) return false;
    for(unsigned int i = 0; i < 2; ++i)
        if(fscanf(f,"%d %s",&d[i],&waste[0])<=0) return false;
    return true;
}
bool LoadCalibrationData(QString name){


    f = fopen(qPrintable(name),"r");
    SkipS(1);//header
    if(!Read33Matrix(rgb_intrinsics)) return false;
    if(!ReadDistortion(rgb_distortion)) return false;
    if(!Read33Matrix(depth_intrinsics)) return false;
    if(!ReadDistortion(depth_distortion)) return false;
    if(!Read33Matrix(R)) return false;
    if(!Read3Vector(T)) return false;
    if(!Read2Vector(rgb_size)) return false;
    if(! Read2Vector(raw_rgb_size)) return false;
    if(!Read2Vector(depth_size)) return false;
    if(!Read2Vector(raw_depth_size)) return false;

    fclose(f);
    return true;
}
