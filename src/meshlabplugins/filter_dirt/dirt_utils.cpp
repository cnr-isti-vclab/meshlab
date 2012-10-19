/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "dirt_utils.h"

/**
Return a random direction
*/

CMeshO::CoordType getRandomDirection(){
    CMeshO::CoordType dir;
    dir = Point3f(rand()/float(RAND_MAX),rand()/float(RAND_MAX),rand()/float(RAND_MAX))-Point3f(0.5f,0.5f,0.5f);
    dir = dir * 0.3f;
    return dir;
}
/**
Verify if a point on that face fall out because of the inclination
@param FacePointer f -  Pointer to the face
@param Point3f     g -  Direction of the gravity
@param float       a -  Adhesion Factor

return true if a particle of that face fall out
*/
bool CheckFallPosition(CMeshO::FacePointer f,Point3f g,float a){
    Point3f n=f->N();
    if(a>1) return false;
    if(acos(n.dot(g)/(n.Norm()*g.Norm()))<((PI/2)*(1-a))) return true;
    return false;
}

/**

  @param Point3f p1 - Position of the initial point
  @param Point3f p2 - Position of the intersection point
  @param Point3f p3 - Position of the final point
  @param float   t  - Time length

  @return elapsed time
*/
float GetElapsedTime(CMeshO::CoordType p1,CMeshO::CoordType p2, CMeshO::CoordType p3, float t){
    float d1= Distance(p1,p2);
    float d2= Distance(p2,p3);
    if(d1+d2==0) return 0;
    return (d1/(d1+d2))*t;
}

float GetVelocity(CMeshO::CoordType o_p,CMeshO::CoordType n_p,CMeshO::FacePointer f,CMeshO::CoordType g,float m,float v){
    Point3f n=f->N();
    float b=n[0]*g[0]+n[1]*g[1]+n[2]*g[2];
    float distance=Distance(o_p,n_p);
    Point3f force;
    force[0]=g[0]-b*n[0];
    force[1]=g[1]-b*n[1];
    force[2]=g[2]-b*n[2];
    if(force.Norm()==0) return 0;
    float acceleration=(force/m).Norm();
    float n_v=math::Sqrt(pow(v,2)+(2*acceleration*distance));
    return n_v;
}
/**
@def Generate random barycentric coordinates

@return a triple of barycentric coordinates
*/
CMeshO::CoordType RandomBaricentric(){
    CMeshO::CoordType interp;
    static math::MarsenneTwisterRNG rnd;
    interp[1] = rnd.generate01();
    interp[2] = rnd.generate01();

    if(interp[1] + interp[2] > 1.0){
        interp[1] = 1.0 - interp[1];
        interp[2] = 1.0 - interp[2];
    }
    interp[0]=1.0-(interp[1] + interp[2]);
    return interp;
}
/**
@def This funcion calculate the cartesian coordinates of a point given from its barycentric coordinates

@param Point3f bc       - barycentric coordinates of the point
@param FacePointer f    - pointer to the face

@return cartesian coordinates of the point
*/
CMeshO::CoordType fromBarCoords(Point3f bc,CMeshO::FacePointer f){
    CMeshO::CoordType p;
    Point3f p0=f->P(0);
    Point3f p1=f->P(1);
    Point3f p2=f->P(2);
    p=f->P(0)*bc[0]+f->P(1)*bc[1]+f->P(2)*bc[2];
    return p;
}

/**
@def Given a face return the coordinates of its baricenter

@param FacePointer f -

@return The Baricenter of the face
*/
CMeshO::CoordType getBaricenter(CMeshO::FacePointer f){
    Point3f bc;
    bc[0]=0.33f;
    bc[1]=0.33f;
    bc[2]=1-bc[0]-bc[1];
    CMeshO::CoordType pc=fromBarCoords(bc,f);
    return pc;
}


CMeshO::CoordType GetNewVelocity(CMeshO::CoordType i_v,CMeshO::FacePointer face,CMeshO::FacePointer new_face,CMeshO::CoordType force,CMeshO::CoordType g,float m,float t){
    CMeshO::CoordType n_v;
    Point3f n= face->N();
    float b=n[0]*force[0]+n[1]*force[1]+n[2]*force[2];
    Point3f f;
    //Compute force component along the face
    f[0]=force[0]-b*n[0];
    f[1]=force[1]-b*n[1];
    f[2]=force[2]-b*n[2];
    CMeshO::CoordType a=f/m;
    n_v=i_v+a*t;
    return getVelocityComponent(n_v.Norm(),new_face,g);
}

/**
@def Given a Face and a point that lies near an edge of that face return a position slightly more internal to avoid points in an unstable position

@param CoordType   p - point on a face
@param FacePointer f - a pointer to the face

@return a point in the face f in a safe position
*/

CMeshO::CoordType GetSafePosition(CMeshO::CoordType p,CMeshO::FacePointer f){
    CMeshO::CoordType safe_p;
    Point3f bc;
    bc[0]=0.33f;
    bc[1]=0.33f;
    bc[2]=1-bc[0]-bc[1];
    CMeshO::CoordType pc=fromBarCoords(bc,f);
    Ray3<float> ray=Ray3<float>(p,pc);
    ray.Normalize();
    Line3f line;
    Point3f p1=pc-p;
    safe_p=p+p1*0.02f;
    return safe_p;
}


/**
@def Verify if a point lies on a face

@param Point3f p   - Coordinates of the point
@param FacePointer f - Pointer to the face

@return true if point p is on face f, false elsewhere.
*/
bool IsOnFace(Point3f p, CMeshO::FacePointer f){
    //Compute vectors
    Point3f a=f->V(0)->P();
    Point3f b=f->V(2)->P();
    Point3f c=f->V(1)->P();

    Point3f v0 = c-a;
    Point3f v1 = b-a;
    Point3f v2 = p-a;

    // Compute dot products
    float dot00 = v0.dot(v0);
    float dot01 = v0.dot(v1);
    float dot02 = v0.dot(v2);
    float dot11 = v1.dot(v1);
    float dot12 = v1.dot(v2);

    // Compute barycentric coordinates
    float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    if(math::Abs(u)<0) u=0;
    if(math::Abs(v)<0) v=0;
    return (u >= 0) && (v >= 0) && (u + v <=1);
}
/**
@def Simulate the movement of a point, affected by a force "dir" on a face and a gravity "g".

@param CoordType p   - coordinates of the point
@param CoordType v   - velocity of the particle
@param float     m   - mass of the particle
@param FaceType face - pointer to the face
@param CoordType dir - direction of the force
@param float 	   l   - length of the movement
@param float     t   - time step

@return new coordinates of the point
*/
CMeshO::CoordType StepForward(CMeshO::CoordType p,CMeshO::CoordType v,float m,CMeshO::FacePointer &face,CMeshO::CoordType force,float l,float t){
    Point3f new_pos;
    Point3f n= face->N();
    float a=n[0]*force[0]+n[1]*force[1]+n[2]*force[2];
    Point3f f;
    //Compute force component along the face
    f[0]=force[0]-a*n[0];
    f[1]=force[1]-a*n[1];
    f[2]=force[2]-a*n[2];

    new_pos=p+v*t*l+(f/m)*pow(t,2)*0.5*l;

    return new_pos;
}

void DrawDust(MeshModel *base_mesh,MeshModel *cloud_mesh){
    if(tri::HasPerWedgeTexCoord(base_mesh->cm) && base_mesh->cm.textures.size()>0){
        QImage img;
        //QFileInfo text_file=QFileInfo(base_mesh->cm.textures[0].c_str());
        img.load(base_mesh->cm.textures[0].c_str());
        QPainter painter(&img);
        float w=img.width();
        float h=img.height();
        painter.setPen(Qt::black);
        painter.setBrush(Qt::SolidPattern);
        base_mesh->updateDataMask(MeshModel::MM_WEDGTEXCOORD);
        CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph= tri::Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> > (cloud_mesh->cm,std::string("ParticleInfo"));
        CMeshO::VertexIterator vi;
        for(vi=cloud_mesh->cm.vert.begin();vi!=cloud_mesh->cm.vert.end();++vi){
            CMeshO::FacePointer f=ph[vi].face;
            TexCoord2f t0=f->WT(0);
            TexCoord2f t1=f->WT(1);
            TexCoord2f t2=f->WT(2);
            Point2f p0=Point2f(t0.U()*w,h-t0.V()*h);
            Point2f p1=Point2f(t1.U()*w,h-t1.V()*h);
            Point2f p2=Point2f(t2.U()*w,h-t2.V()*h);
            Point3f bc;
            Point2f dbc;
            InterpolationParameters(*f,vi->P(),bc);
            dbc=p0*bc[0]+p1*bc[1]+p2*bc[2];
            painter.drawPoint(dbc[0],dbc[1]);
        }
        QString path=QDir::currentPath()+"/dirt_texture.png";
        img.save(path,"PNG");
        base_mesh->cm.textures.clear();
        base_mesh->cm.textures.push_back(path.toStdString());
    }
}


void ColorizeMesh(MeshModel* m){
    CMeshO::FaceIterator fi;
    float dirtiness;
    for(fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi){
        dirtiness=fi->Q();
        if(dirtiness==0){
            fi->C()=Color4b(255,255,255,0);
        }else{
            if(dirtiness>255) fi->C()=Color4b(0,0,0,0);
            else fi->C()=Color4b(255-dirtiness,255-dirtiness,255-dirtiness,0);
        }
    }
    tri::UpdateColor<CMeshO>::VertexFromFace(m->cm);
}


/**
@def Compute the intersection of the segment from p1 to p2 and the face f

@param CoordType p1 - position of the first point
@param Coordtype p2 - position of the second point
@param Facepointer f - pointer to the face
@param CoordType int_point - intersection point this is a return parameter for the function.
@param FacePointer face - pointer to the new face

@return the intersection edge index if there is an intersection -1 elsewhere
Step
*/
int ComputeIntersection(CMeshO::CoordType p1,CMeshO::CoordType p2,CMeshO::FacePointer &f,CMeshO::FacePointer &new_f,CMeshO::CoordType &int_point){

    CMeshO::CoordType v0=f->V(0)->P();
    CMeshO::CoordType v1=f->V(1)->P();
    CMeshO::CoordType v2=f->V(2)->P();
    float dist[3];
    Point3f int_points[3];
    dist[0]=PSDist(p2,v0,v1,int_points[0]);
    dist[1]=PSDist(p2,v1,v2,int_points[1]);
    dist[2]=PSDist(p2,v2,v0,int_points[2]);
    int edge=-1;
    if(dist[0]<dist[1]){
        if(dist[0]<dist[2]) edge=0;
        else edge=2;
    }else{
        if(dist[1]<dist[2]) edge=1;
        else edge=2;
    }

    CMeshO::VertexType* v;
    if(Distance(int_points[edge],f->V(edge)->P())<Distance(int_points[edge],f->V((edge+1) % 3)->P())) v=f->V(edge);
    else v=f->V((edge+1) % 3);
    vcg::face::Pos<CMeshO::FaceType> p(f,edge,v);
    new_f=f->FFp(edge);
    if(new_f==f) return -1;

    if(Distance(int_points[edge],v->P())<EPSILON){
        p.FlipF();
        CMeshO::FacePointer tmp_f=p.F();

        int n_face=0;
        while(tmp_f!=f){
            p.FlipE();
            p.FlipF();
            tmp_f=p.F();
            n_face++;
        }
        if(n_face!=0){
            int r=(rand()%(n_face-1))+2;
            for(int i=0;i<r;i++){
                p.FlipE();
                p.FlipF();
            }
            new_f=p.F();
        }
    }
    int_point=GetSafePosition(int_points[edge],new_f);
    return edge;
}

/**
@def Compute the Normal Dust Amount Function per face of a Mesh m

@param m MeshModel
@param u CoordType dust direction
@param k float
@param s float

@return nothing
*/
void ComputeNormalDustAmount(MeshModel* m,CMeshO::CoordType u,float k,float s){
    CMeshO::FaceIterator fi;
    float d;
    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        d=k/s+(1+k/s)*pow(fi->N().dot(u),s);
        fi->Q()=d;
    }
}

/**
@def This function compute the Surface Exposure per face of a Mesh m

@param  MeshModel* m - Pointer to the new mesh
@param int r - scaling factor
@param int n_ray - number of rays emitted

@return nothing
*/
void ComputeSurfaceExposure(MeshModel* m,int r,int n_ray){

    CMeshO::PerFaceAttributeHandle<float> eh=vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<float>(m->cm,std::string("exposure"));

    float dh=1.2;
    float exp=0;
    float di=0;
    float xi=0;

    CMeshO::FacePointer face;
    CMeshO::CoordType p_c;
    MetroMeshFaceGrid f_grid;
    f_grid.Set(m->cm.face.begin(),m->cm.face.end());
    MarkerFace markerFunctor;
    markerFunctor.SetMesh(&(m->cm));
    RayTriangleIntersectionFunctor<false> RSectFunct;
    CMeshO::FaceIterator fi;
    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        xi=0;
        eh[fi]=0;

        for(int i=0;i<n_ray;i++){
            //For every f_face  get the central point
            p_c=fromBarCoords(RandomBaricentric(),&*fi);
            //Create a ray with p_c as origin and direction N
            p_c=p_c+NormalizedNormal(*fi)*0.1;
            Ray3<float> ray=Ray3<float>(p_c,fi->N());
            di=0;
            face=0;
            face=f_grid.DoRay<RayTriangleIntersectionFunctor<false>,MarkerFace>(RSectFunct,markerFunctor,ray,1000,di);

            if(di!=0){
                xi=xi+(dh/(dh-di));
		
            }
        }
        exp=1-(xi/n_ray);
        eh[fi]=exp;

    }
}


void ComputeParticlesFallsPosition(MeshModel* base_mesh,MeshModel* cloud_mesh,CMeshO::CoordType dir){
    CMeshO::VertexIterator vi;
    MetroMeshFaceGrid f_grid;
    f_grid.Set(base_mesh->cm.face.begin(),base_mesh->cm.face.end());
    MarkerFace markerFunctor;
    markerFunctor.SetMesh(&(base_mesh->cm));
    RayTriangleIntersectionFunctor<false> RSectFunct;
    CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph= tri::Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(cloud_mesh->cm,"ParticleInfo");
    std::vector<CMeshO::VertexPointer> ToDelVec;
    for(vi=cloud_mesh->cm.vert.begin();vi!=cloud_mesh->cm.vert.end();++vi){
        Particle<CMeshO> info=ph[vi];
        if((*vi).IsS()){
            Point3f p_c=vi->P()+info.face->N().normalized()*0.1;
            Ray3<float> ray=Ray3<float>(p_c,dir);
            float di;
            CMeshO::FacePointer new_f=f_grid.DoRay<RayTriangleIntersectionFunctor<false>,MarkerFace>(RSectFunct,markerFunctor,ray,base_mesh->cm.bbox.Diag(),di);
            if(new_f!=0){
                ph[vi].face=new_f;
                float u;
                float v;
                float t;
                IntersectionRayTriangle<float>(ray,new_f->P(0),new_f->P(1),new_f->P(2),t,u,v);
                Point3f bc(1-u-v,u,v);
                vi->P()=fromBarCoords(bc,new_f);
                vi->ClearS();
                new_f->C()=Color4b::Red;
            }else{
                ToDelVec.push_back(&*vi);
            }
        }
    }
    for(unsigned int i=0;i<ToDelVec.size();i++){
        if(!ToDelVec[i]->IsD()) Allocator<CMeshO>::DeleteVertex(cloud_mesh->cm,*ToDelVec[i]);
    }
}



/**
@def This funcion

@param
@param
@param

@return ?
*/
bool GenerateParticles(MeshModel* m,std::vector<CMeshO::CoordType> &cpv,/*std::vector< Particle<CMeshO> > &dpv,*/int d,float threshold){

    //Handler
    CMeshO::PerFaceAttributeHandle<float> eh=vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<float>(m->cm,std::string("exposure"));

    CMeshO::FaceIterator fi;
    CMeshO::CoordType p;
    cpv.clear();
    //dpv.clear();
    float r=1;
    float a0=0;
    float a=0;
    float a1=0;

    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        int n_dust=0;
        a1=a0+r*eh[fi];
        if(a1<0)
            a=0;
        if(a1>1)
            a=1;
        if(a1>=0 && a1<=1)
            a=a1;


        if(eh[fi]==1) a=1;
        else a=0;

        n_dust=(int)d*fi->Q()*a;

        for(int i=0;i<n_dust;i++){
            p=RandomBaricentric();
            CMeshO::CoordType n_p;
            n_p=fi->P(0)*p[0]+fi->P(1)*p[1]+fi->P(2)*p[2];
            cpv.push_back(n_p);
        }

          fi->Q()=n_dust;

    }


    return true;
}


/**

*/
void associateParticles(MeshModel* b_m,MeshModel* c_m,float &m,float &v,CMeshO::CoordType g){
    MetroMeshFaceGrid   unifGridFace;
    Point3f closestPt;
    CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph= tri::Allocator<CMeshO>::AddPerVertexAttribute<Particle<CMeshO> > (c_m->cm,std::string("ParticleInfo"));
    unifGridFace.Set(b_m->cm.face.begin(),b_m->cm.face.end());
    MarkerFace markerFunctor;
    markerFunctor.SetMesh(&(b_m->cm));
    float dist=1;
    float dist_upper_bound=dist;
    CMeshO::VertexIterator vi;
    vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
    for(vi=c_m->cm.vert.begin();vi!=c_m->cm.vert.end();++vi){
        Particle<CMeshO>* part = new Particle<CMeshO>();
        CMeshO::FacePointer f=unifGridFace.GetClosest(PDistFunct,markerFunctor,vi->P(),dist_upper_bound,dist,closestPt);
        part->face=f;
        part->face->Q()=part->face->Q()+1;
        part->mass=m;
        part->velocity=v;
        part->v=getVelocityComponent(v,f,g);
        ph[vi]=*part;
    }

}

CMeshO::CoordType getVelocityComponent(float v,CMeshO::FacePointer f,CMeshO::CoordType g){
    CMeshO::CoordType cV;
    Point3f n= f->N();
    float a=n[0]*g[0]+n[1]*g[1]+n[2]*g[2];
    Point3f d;
    d[0]=g[0]-a*n[0];
    d[1]=g[1]-a*n[1];
    d[2]=g[2]-a*n[2];
    cV=d/d.Norm();
    cV.Normalize();
    cV[0]=v*d[0];
    cV[1]=v*d[1];
    cV[2]=v*d[2];
    return cV;
}
/**
@def This function initialize the mesh m in order to respect some prerequisites of the filter

@param MeshModel* m - Pointer to the Mesh

@return nothing
*/
void prepareMesh(MeshModel* m){

    m->updateDataMask(MeshModel::MM_FACEFACETOPO);
    m->updateDataMask(MeshModel::MM_FACEMARK);
    m->updateDataMask(MeshModel::MM_FACECOLOR);
    m->updateDataMask(MeshModel::MM_VERTQUALITY);
    m->updateDataMask(MeshModel::MM_FACEQUALITY);
    m->updateDataMask(MeshModel::MM_FACENORMAL);

    tri::UnMarkAll(m->cm);

    //clean Mesh
    tri::Allocator<CMeshO>::CompactFaceVector(m->cm);
    tri::Clean<CMeshO>::RemoveUnreferencedVertex(m->cm);
    tri::Clean<CMeshO>::RemoveDuplicateVertex(m->cm);
    tri::Allocator<CMeshO>::CompactVertexVector(m->cm);

    tri::UpdateFlags<CMeshO>::FaceClear(m->cm);
    //update Mesh
    m->cm.vert.EnableVFAdjacency();
    m->cm.face.EnableVFAdjacency();
    tri::UpdateTopology<CMeshO>::FaceFace(m->cm);
    tri::UpdateTopology<CMeshO>::VertexFace(m->cm);
    tri::UpdateNormal<CMeshO>::PerFaceNormalized(m->cm);

    CMeshO::FaceIterator fi;
    for(fi=m->cm.face.begin();fi!=m->cm.face.end();++fi){
        fi->Q()=0;
    }
}

/**
@def This function move a particle over the mesh
*/
void MoveParticle(Particle<CMeshO> &info,CMeshO::VertexPointer p,float l,int t,Point3f dir,Point3f g,float a){
    if(CheckFallPosition(info.face,g,a)){
        p->SetS();
        return;
    }
    float time=t;
    if(dir.Norm()==0) dir=getRandomDirection();
    Point3f new_pos;
    Point3f current_pos;
    Point3f int_pos;
    CMeshO::FacePointer current_face=info.face;
    CMeshO::FacePointer new_face;
    new_face=current_face;
    current_pos=p->P();
    new_pos=StepForward(current_pos,info.v,info.mass,current_face,g+dir,l,time);
    while(!IsOnFace(new_pos,current_face)){
        int edge=ComputeIntersection(current_pos,new_pos,current_face,new_face,int_pos);
        if(edge!=-1){
            Point3f n = new_face->N();
            if(CheckFallPosition(new_face,g,a))  p->SetS();
            float elapsed_time=GetElapsedTime(current_pos,int_pos,new_pos,time);
            info.v=GetNewVelocity(info.v,current_face,new_face,g+dir,g,info.mass,elapsed_time);
            time=time-elapsed_time;
            current_pos=int_pos;
            current_face->Q()+=elapsed_time*5;
            current_face=new_face;
            new_pos=int_pos;
            if(time>0){
                if(p->IsS()) break;
                new_pos=StepForward(current_pos,info.v,info.mass,current_face,g+dir,l,time);
            }
            current_face->C()=Color4b::Green;//Just Debug!!!!
        }else{
            //We are on a border
            new_pos=int_pos;
            current_face=new_face;
            p->SetS();
            break;
        }
    }
    p->P()=new_pos;
    info.face=current_face;
}




/**
This function compute the repulsion beetwen particles
@param MeshModel* b_m - base mesh
@param MeshModel* c_m - cloud of points
@param int k          - max number of particle to repulse
@param float l        - lenght of the step
@return nothing       - adhesion factor
*/
void ComputeRepulsion(MeshModel* b_m,MeshModel *c_m,int k,float l,Point3f g,float a){
    CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph = Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(c_m->cm,"ParticleInfo");
    MetroMeshVertexGrid v_grid;
    std::vector< Point3<float> > v_points;
    std::vector<CMeshO::VertexPointer> vp;
    std::vector<float> distances;
    v_grid.Set(c_m->cm.vert.begin(),c_m->cm.vert.end(),b_m->cm.bbox);
    CMeshO::VertexIterator vi;
    for(vi=c_m->cm.vert.begin();vi!=c_m->cm.vert.end();++vi){
        vcg::tri::GetKClosestVertex(c_m->cm,v_grid,k,vi->P(),EPSILON,vp,distances,v_points);
        for(unsigned int i=0;i<vp.size();i++){CMeshO::VertexPointer v = vp[i];
            if(v->P()!=vi->P() && !v->IsD() && !vi->IsD()){
                Ray3<float> ray(vi->P(),fromBarCoords(RandomBaricentric(),ph[vp[i]].face));
                ray.Normalize();
                Point3f dir=ray.Direction();
                dir.Normalize();
                MoveParticle(ph[vp[i]],vp[i],0.01,1,dir,g,a);
            }
        }
    }
}
/**
@def This function simulate the movement of the cloud mesh, it requires that every point is associated with a Particle data structure

@param MeshModel cloud  - Mesh of points
@param Point3f   force  - Direction of the force
@param float     l      - Lenght of the  movementstep
@param float     t   - Time Step

@return nothing
*/
void MoveCloudMeshForward(MeshModel *cloud,MeshModel *base,Point3f g,Point3f force,float l,float a,float t,int r_step){

    CMeshO::PerVertexAttributeHandle<Particle<CMeshO> > ph = Allocator<CMeshO>::GetPerVertexAttribute<Particle<CMeshO> >(cloud->cm,"ParticleInfo");
    CMeshO::VertexIterator vi;
    for(vi=cloud->cm.vert.begin();vi!=cloud->cm.vert.end();++vi)
        if(!vi->IsD()) MoveParticle(ph[vi],&*vi,l,t,force,g,a);


    //Handle falls Particle
    ComputeParticlesFallsPosition(base,cloud,g);
    //Compute Particles Repulsion
    for(int i=0;i<r_step;i++)
        ComputeRepulsion(base,cloud,50,l,g,a);
}

