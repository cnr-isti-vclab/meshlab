#include "generic_align.h"

#include <vcg/space/point_matching.h>

#include "utils.h"

using namespace vcg;

GenericAlign::GenericAlign()
{
}


bool
GenericAlign::isCoupleOk(const CVertexO& firstToAl,
                         const CVertexO& firstRef,
                         const CVertexO& secondToAl,
                         const CVertexO& secondRef,
                         float scale)
{

    const float distToAl=Distance(firstToAl.P(), secondToAl.P());
    const float distRef=Distance(firstRef.P(), secondRef.P());
    //std::cout << "DistAl " << distToAl << " DistRef " << distRef << std::endl;
    if(distRef<(distToAl*scale*0.8) || distRef>(distToAl*scale*1.2))
    {
        return false;
    }
    const float angleToAl=Utils::angleNorm<float>(firstToAl.N(), secondToAl.N());
    const float angleRef=Utils::angleNorm<float>(firstRef.N(), secondRef.N());
    //std::cout << "DistAl " << distToAl << " DistRef " << distRef << "angleAl " << angleToAl << " angleRef " << angleRef << std::endl;
    if(abs(angleToAl-angleRef)>20 || angleToAl<20)
    {
        if(angleToAl<20)
            std::cout << "angleAl " << angleToAl << std::endl;
        return false;
    }
    //std::cout << "DistAl " << distToAl << " DistRef " << distRef << "angleAl " << angleToAl << " angleRef " << angleRef << std::endl;

    return true;
}

bool
GenericAlign::isToAlignOk(int ind,
                          MeshModel* toAlign,
                          const std::vector <std::pair<int, int> >& corrs)
{
    bool ok=true;
    for (unsigned int i=0; i<corrs.size(); i++)
    {
        float dist=Distance(toAlign->cm.vert[ind].P(), toAlign->cm.vert[corrs[i].first].P());
        ///// This condition is quite arbitrary...
        float qual=(toAlign->cm.vert[corrs[i].first].Q()/3.0);
        //std::cout << "Dist " << dist << " Qual " << qual << std::endl;
        if(dist<qual)
        {
            ok=false;
            break;
        }
    }

    return ok;
}

////// Given a couple of points, it checks if they are compatible with a list of previous couples, both for distance and difference in normals
bool
GenericAlign::isReferenceOk(int ref,
                            MeshModel* reference,
                            int al,
                            MeshModel* toAlign,
                            const std::vector <std::pair<int, int> > &corrs,
                            float scale)
{

    bool ok=true;
    for (unsigned int i=0; i<corrs.size(); i++)
    {
        float distToAl=Distance(toAlign->cm.vert[al].P(), toAlign->cm.vert[corrs[i].first].P());
        float angleToAl=Utils::angleNorm<float>(toAlign->cm.vert[al].N(), toAlign->cm.vert[corrs[i].first].N());
        float distRef=Distance(reference->cm.vert[ref].P(), reference->cm.vert[corrs[i].second].P());
        float angleRef=Utils::angleNorm<float>(reference->cm.vert[ref].N(), reference->cm.vert[corrs[i].second].N());
        //std::cout << "DistAl " << distToAl << " DistRef " << distRef << "angleAl " << angleToAl << " angleRef " << angleRef << std::endl;
        if(distRef<(distToAl*scale*0.8) || distRef>(distToAl*scale*1.2) || abs(angleToAl-angleRef)>20)
        {
            ok=false;
            break;
        }
    }

    return ok;
}



/////////// This chooses a set of points for which matches could be found. Very naive for now...
std::vector< int >
GenericAlign::selectSeedsDescr(DescriptorBase toAlign,
                               const std::vector<DescrPoint>& descrList,
                               bool shuffle)
{
    // reduction factor applied to the seed distance selection threshold
    const float bboxDistReducFactor = 0.8;

    std::vector< int > seeds;
    double qMax=0.0;
    for(int i = 0; i < toAlign.selection.size(); i++)
    {
        const unsigned int& ind = toAlign.selection[i];

        if(toAlign.model->cm.vert[ind].Q()>qMax)
            qMax=toAlign.model->cm.vert[ind].Q();
    }
    int init; int cand; double bestD=0.0;
    float bboxDist=toAlign.model->cm.bbox.Diag()*bboxDistReducFactor/*/4.0*/;
    if (shuffle)
    {
        float avrgDesc=0.0;
        for(int i = 0; i < descrList.size(); i++)
        {
            avrgDesc+=descrList[i].descrip;
        }
        avrgDesc/=(float)descrList.size();
        std::cout << "AvrgDesc " << avrgDesc << std::endl;

        bool first=false;
        while(!first)
        {
            init=rand() % toAlign.selection.size();
            std::cout << "AvrgDesc " << avrgDesc << " chosen " <<  descrList[init].descrip << std::endl;
            if(descrList[init].descrip>=avrgDesc)
                first=true;
        }
        init=toAlign.selection[init];
    }
    else
    {
        float bestDescr=0.0;
        for(int i = 0; i < descrList.size(); i++)
        {
            if(descrList[i].descrip>bestDescr)
            {
                bestDescr=descrList[i].descrip;
                init=descrList[i].ind;
            }
        }
    }

    //std::cout << "bestD " << bestD << std::endl;
    seeds.push_back(init);
    //seeds.reserve(nbSeeds);
    bestD=0.0;

    // Avoid infinite loop by checking the distance threshold
    while(seeds.size()<_seedNumber && bboxDist > toAlign.minScale)
    {
        cand=-1; bestD=0.0;
        for(int i=0; i<descrList.size(); i++)
        {
            CVertexO point=toAlign.model->cm.vert[descrList[i].ind];
            if(descrList[i].descrip*point.Q()>bestD)
            {
                bool ok=true;
                for(int j=0; j<seeds.size(); j++)
                {
                    if(Distance(point.P(),toAlign.model->cm.vert[seeds[j]].P())<bboxDist)
                    {
                        ok=false;
                        break;
                    }
                }
                if(ok)
                {
                    bestD=descrList[i].descrip*point.Q();
                    cand=descrList[i].ind;
                }
            }
        }
        if(cand==-1)
        {
            bboxDist*=bboxDistReducFactor;
            qMax*=bboxDistReducFactor;
        }
        else
        {
            seeds.push_back(cand);
            //std::cout << "bestD " << bestD << std::endl;
        }

    }

    return seeds;

}

/////////// This chooses a set of points for which matches could be found. Very naive for now...
std::vector< int >
GenericAlign::selectSeeds(MeshModel* toAlign,
                          MeshModel* /*reference*/ ,
                          int start)
{
    int ind;
    std::vector< int > seeds;
    double maxQ=0.0; double newMaxQ=0.0;

    //maxQ=toAlign->cm.vert[start].Q();
    for (int i = 0; i != toAlign->cm.vn; i++) {
        if (toAlign->cm.vert[i].IsS() && toAlign->cm.vert[i].Q()>maxQ)
        {
            maxQ=toAlign->cm.vert[i].Q();
            ind=i;
        }
    }
    if (start!=-1)
    {
        ind=start;
    }

    std::cout << "MaxQ: " << maxQ << "Ind: " << ind << std::endl;
    seeds.push_back(ind);
    int cand=0;

    while (seeds.size()<15 && cand!=-1)
    {
        //std::cout << "Cycling... " << std::endl;
        cand=-1;
        double halfMax=maxQ/3.0;
        //std::cout << " Half MaxQ: " << halfMax << std::endl;
        for (int i = 0; i != toAlign->cm.vn; i++) {
            //newMaxQ=0.0;

            if (toAlign->cm.vert[i].IsS() && toAlign->cm.vert[i].Q()>=halfMax && toAlign->cm.vert[i].Q()>=newMaxQ)
            {

                //std::cout << " Ok, count: " << count << std::endl;
                //std::cout << "Checking... " << std::endl;

                bool ok=true;
                for (unsigned int j = 0; j < seeds.size(); j++) {
                    float halfDist=toAlign->cm.vert[seeds[j]].Q()/6.0;
                    //std::cout << " MinDist: " << halfDist << std::endl;
                    float dist=Distance(toAlign->cm.vert[i].P(),toAlign->cm.vert[seeds[j]].P());
                    //std::cout << " Dist: " << dist << std::endl;
                    if(dist<halfDist)
                    {
                        ok=false;
                        //break;
                    }
                }
                if(ok)
                {
                    //std::cout << "Chosen cand: " << i << std::endl;
                    cand=i;
                    //std::cout << " NewMaxQ: " << toAlign->cm.vert[i].Q() << std::endl;
                    newMaxQ=toAlign->cm.vert[i].Q();
                }
            }
        }
        //std::cout << "Final cand: " << cand << std::endl;
        if (cand!=-1)
            seeds.push_back(cand);
        maxQ=newMaxQ;
        //std::cout << " new MaxQ: " << toAlign->cm.vert[cand].Q() << std::endl;
        //std::cout << " new MaxQ: " << maxQ << std::endl;
    }


    return seeds;
}


/////// Given a set of three couples of points, it checks if the transformation is ok, both for final distances and difference in normals
bool GenericAlign::checkTriplets(const DescriptorBase &toAlign,
                                 const DescriptorBase &reference,
                                 std::vector<std::pair<int, int> >corrs,
                                 const std::vector<Cand> &thirdPoints,
                                 float scale,
                                 float error)
{
    float bestError=10000000;
    Matrix44f res;

#pragma omp parallel for
    for(int r=0; r<thirdPoints.size(); r++)
    {
        res.SetIdentity(); Matrix33f rot; rot.SetIdentity();

        std::vector<Point3f> ref; std::vector<Point3f> refN;
        std::vector<Point3f> toAl; std::vector<Point3f> toAlN;
        corrs[2].second=thirdPoints[r].ind;
        for (int i=0; i<3; i++)
        {
            vcg::Point3f r,rN;
            r=reference.model->cm.vert[corrs[i].second].P();
            rN=reference.model->cm.vert[corrs[i].second].N();
            ref.push_back(r);
            refN.push_back(rN);

            vcg::Point3f a, aN;
            a=toAlign.model->cm.vert[corrs[i].first].P();
            aN=toAlign.model->cm.vert[corrs[i].first].N();
            toAl.push_back(a);
            toAlN.push_back(aN);

        }
        ////// This computes the roto-translation + scaling
        ComputeSimilarityMatchMatrix(ref, toAl, res);
        bool okNorm=true;
        /// This is not so nice to see, but it works...
        rot[0][0]=res[0][0];rot[0][1]=res[0][1];rot[0][2]=res[0][2];rot[1][0]=res[1][0];rot[1][1]=res[1][1];rot[1][2]=res[1][2];rot[2][0]=res[2][0];rot[2][1]=res[2][1];rot[2][2]=res[2][2];
        for (int i=0; i<3; i++)
        {
            vcg::Point3f normToAlign=rot*toAlN[i];
            float angle=Utils::angleNorm<float>(refN[i],normToAlign);
            if (angle>20)
            {
                okNorm=false;
                break;
            }

        }
        //std::cout << res[0][0] << " " << res[0][1] << " " << res[0][2] << " " << res[0][3] << std::endl;
        if (okNorm)
        {
            float totError=0; float error;
            for (int i=0; i<3; i++)
            {
                vcg::Point4f rotToAlign=res*vcg::Point4f(toAl[i].X(),toAl[i].Y(),toAl[i].Z(),1);
                error=Distance(ref[i],vcg::Point3f(rotToAlign[0],rotToAlign[1],rotToAlign[2]));
                totError+=error;
            }
#pragma omp critical
            {
                //std::cout << "New scale " << getScaleFromTransf(res) << std::endl;
                if (totError<bestError && isScaleOk(scale,getScaleFromTransf(res),toAlign.multiplier))
                {
                    std::cout << "Error after alignment " << totError/3.0f << std::endl;
                    bestError=totError/3.0f;

                }
            }
        }
    }
    if(bestError<error)
    {
        toAlign.model->cm.Tr=res;
        return true;
    }

    return false;
}

/////// Given a set of four couples of points, it checks if the transformation is ok, both for final distances and difference in normals
bool
GenericAlign::checkQuadriplets(const DescriptorBase &toAlign,
                               const DescriptorBase &reference,
                               std::vector<std::pair<int, int> >corrs,
                               const std::vector<Cand> &fourthPoints,
                               float scale,
                               float error)
{
    float bestError=10000000;
    Matrix44f res;

#pragma omp parallel for
    for(int r=0; r<fourthPoints.size(); r++)
    {
         res.SetIdentity(); Matrix33f rot; rot.SetIdentity();

        std::vector<Point3f> ref; std::vector<Point3f> refN;
        std::vector<Point3f> toAl; std::vector<Point3f> toAlN;
        corrs[3].second=fourthPoints[r].ind;
        for (int i=0; i<4; i++)
        {
            vcg::Point3f r,rN;
            r=reference.model->cm.vert[corrs[i].second].P();
            rN=reference.model->cm.vert[corrs[i].second].N();
            ref.push_back(r);
            refN.push_back(rN);

            vcg::Point3f a, aN;
            a=toAlign.model->cm.vert[corrs[i].first].P();
            aN=toAlign.model->cm.vert[corrs[i].first].N();
            toAl.push_back(a);
            toAlN.push_back(aN);

        }
        ////// This computes the roto-translation + scaling
        ComputeSimilarityMatchMatrix(ref, toAl, res);
        bool okNorm=true;
        /// This is not so nice to see, but it works...
        rot[0][0]=res[0][0];rot[0][1]=res[0][1];rot[0][2]=res[0][2];rot[1][0]=res[1][0];rot[1][1]=res[1][1];rot[1][2]=res[1][2];rot[2][0]=res[2][0];rot[2][1]=res[2][1];rot[2][2]=res[2][2];
        for (int i=0; i<4; i++)
        {
            vcg::Point3f normToAlign=rot*toAlN[i];
            float angle=Utils::angleNorm<float>(refN[i],normToAlign);
            if (angle>20)
            {
                okNorm=false;
                break;
            }

        }
        //std::cout << res[0][0] << " " << res[0][1] << " " << res[0][2] << " " << res[0][3] << std::endl;
        if (okNorm)
        {
            float totError=0; float error;
            for (int i=0; i<4; i++)
            {
                vcg::Point4f rotToAlign=res*vcg::Point4f(toAl[i].X(),toAl[i].Y(),toAl[i].Z(),1);
                error=Distance(ref[i],vcg::Point3f(rotToAlign[0],rotToAlign[1],rotToAlign[2]));
                totError+=error;
            }
#pragma omp critical
            {
                if (totError<bestError && isScaleOk(scale,getScaleFromTransf(res),toAlign.multiplier))
                {
                    std::cout << "Error after alignment " << totError/4.0f << std::endl;
                    bestError=totError/4.0f;

                }
            }
        }
    }
    if(bestError<error)
    {
        toAlign.model->cm.Tr=res;
        return true;
    }

    return false;
}


