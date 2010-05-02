These files have been imported from the original distribution of mini_sdf available
at the address:


- CalculateSDF.h/.cpp:
- tnt/*: NIST template numerical toolkit



class CalculateSDFThread : public QThread{
    Q_OBJECT;

private:
    QString            m_name;
    CMeshO*            m_mesh;
    float*          m_results;
    const float*    m_origins;
    int                m_size;
    int            m_numCones;
    float    m_coneSeperation;
    int          m_raysInCone;
    bool    m_gaussianWeights;

public:
    /// constructor which accepts the target vertices/facets
    CalculateSDFThread( CMeshO* mesh, const float* origins, const int size, float* results, int numCones, float coneSeperation, int raysInCone, bool gaussianWeights, const QString& name = QString::null);

    /// Start the multi-threaded computation
    virtual void run();
};

enum NormalizeTypes {
    None,
    MinMax,
    Log
};

CalculateSDFThread::CalculateSDFThread( CMeshO* mesh, const float* origins,const int size, float* results, int numCones, float coneSeperation, int raysInCone, bool gaussianWeights,const QString& name) :
m_name(name), m_mesh(mesh), m_results(results), m_origins(origins), m_size(size), m_numCones(numCones), m_coneSeperation(coneSeperation), m_raysInCone(raysInCone), m_gaussianWeights(gaussianWeights){

// TODO check if there was something in here

}

void CalculateSDFThread::run(){
    //setPriority(QThread::LowPriority);
    QTime timer; timer.start();
    float degree = m_coneSeperation * DEG2RAD;
    int i;
    for (i = 0; i < m_size; i++){
        Point3f p(m_origins[i*6], m_origins[i*6+1], m_origins[i*6+2]);
        Point3f n(m_origins[i*6+3], m_origins[i*6+4], m_origins[i*6+5]);
        m_results[i] = compute_volume(*m_mesh, p, n, m_numCones, degree, m_raysInCone, m_gaussianWeights, NULL);
    }
    int totalTime = timer.elapsed();
    qDebug("Total time %f seconds (%f msecs each point)\n", (float)totalTime/1000, (float)totalTime/m_size);
}

bool CalculateSDF::go( CMeshO* mesh, const bool onVertices, const bool multiThreaded, const int numCones, const float coneSeperation, const int raysInCone,
                       const bool gaussianWeights, const bool smoothing, const bool smoothingAnisotropic, const int smoothingIterations,
                       const bool showDialog){
    QTime timer;
    timer.start();

    int originsSize = (onVertices?mesh->vn:mesh->fn);
    float* origins = new float[originsSize * 6];
    preprocess(mesh, onVertices, origins);
    int numOfThreads = (multiThreaded? SDF_THREADS_NUM : 1);
    int timerPreprocess = timer.elapsed();

    //////////////////////////////////////////////////////////////////////////
    // Prepare search structure on mesh
    //////////////////////////////////////////////////////////////////////////
    computeRayIntersect();
    int timerComputeSearchStructure = timer.elapsed();

    //////////////////////////////////////////////////////////////////////////
    // Initialize the threads
    //////////////////////////////////////////////////////////////////////////
    float* results = new float[originsSize];
    float* resultsPtr = results;
    float* dataPtr = origins;
    int dataSize = originsSize / numOfThreads;
    std::vector<CalculateSDFThread*> threads;
    for (int i=0; i<numOfThreads; i++){
        if (i == numOfThreads-1 && originsSize % dataSize != 0)
            dataSize += originsSize % dataSize;

        CalculateSDFThread* cst =
            new CalculateSDFThread(mesh, dataPtr, dataSize, resultsPtr, numCones, coneSeperation, raysInCone, gaussianWeights, QString::number(i));

        connect(cst, SIGNAL(advancedIn(int)), this, SLOT(addToProgress(int)));

        dataPtr += dataSize * 6;
        resultsPtr += dataSize;
        threads.push_back(cst);
    }

    // Run the threads and wait for them to finish
    if (multiThreaded) {
        for (unsigned int i=0; i<threads.size(); i++) {
            threads[i]->start();
        }

        for (unsigned int i=0; i<threads.size(); i++) {
            threads[i]->wait();
        }
    } else {
        threads[0]->run();
    }

    int timerAllThreadsFinished = timer.elapsed();

    //////////////////////////////////////////////////////////////////////////
    // Put the results back in the mesh
    //////////////////////////////////////////////////////////////////////////
    //only apply smoothing if on vertices
    //if (!postprocess(mesh, onVertices, results, normalize, smoothing && onVertices))
    //	return false;

    //perform smoothing on the mesh itself
    if (!onVertices)
        makeFacesNVolume(smoothing, smoothingAnisotropic, smoothingIterations );
        // mesh->makeFacesNVolume(smoothing, smoothingAnisotropic, smoothingIterations);

    int timerPostprocess = timer.elapsed();

    //////////////////////////////////////////////////////////////////////////
    // Clean up
    //////////////////////////////////////////////////////////////////////////
    for (unsigned int i=0; i<threads.size(); i++) {
        delete threads[i];
    }
    threads.clear();
    delete[] origins;
    delete[] results;

    int timerTotal = timer.elapsed();

    if (showDialog) {
        qDebug("Calculated SDF total time %dms. preprocess %dms, compute %dms, threads %dms, postprocess %dms",
            timerTotal, timerPreprocess, timerComputeSearchStructure, timerAllThreadsFinished, timerPostprocess);

        // ATA2: Progressbar stuff....
        // QString s;
        // QString workingon = (onVertices?"vertices" : "facets");
        // QString mt = (multiThreaded?"multithreaded" : "not multithreaded");
        // s.sprintf("Mesh size v=%d f=%d\nWorking on %s, Working %s\nTotal time %f seconds", mesh->vn, mesh->fn, workingon.toAscii(), mt.toAscii(), (float)timerTotal / 1000);
        // QMessageBox::information(g_main, "SDF calculation complete", s);
    }
    return true;
}


bool CalculateSDF::go( CMeshO* mesh, const bool onVertices, const bool multiThreaded, const int numCones, const float coneSeperation, const int raysInCone,
                       const bool gaussianWeights, const bool smoothing, const bool smoothingAnisotropic, const int smoothingIterations,
                       const bool showDialog){
    QTime timer;
    timer.start();

    int originsSize = (onVertices?mesh->vn:mesh->fn);
    float* origins = new float[originsSize * 6];
    preprocess(mesh, onVertices, origins);
    int numOfThreads = (multiThreaded? SDF_THREADS_NUM : 1);
    int timerPreprocess = timer.elapsed();

    //////////////////////////////////////////////////////////////////////////
    // Prepare search structure on mesh
    //////////////////////////////////////////////////////////////////////////
    computeRayIntersect();
    int timerComputeSearchStructure = timer.elapsed();

    //////////////////////////////////////////////////////////////////////////
    // Initialize the threads
    //////////////////////////////////////////////////////////////////////////
    float* results = new float[originsSize];
    float* resultsPtr = results;
    float* dataPtr = origins;
    int dataSize = originsSize / numOfThreads;
    std::vector<CalculateSDFThread*> threads;
    for (int i=0; i<numOfThreads; i++){
        if (i == numOfThreads-1 && originsSize % dataSize != 0)
            dataSize += originsSize % dataSize;

        CalculateSDFThread* cst =
            new CalculateSDFThread(mesh, dataPtr, dataSize, resultsPtr, numCones, coneSeperation, raysInCone, gaussianWeights, QString::number(i));

        connect(cst, SIGNAL(advancedIn(int)), this, SLOT(addToProgress(int)));

        dataPtr += dataSize * 6;
        resultsPtr += dataSize;
        threads.push_back(cst);
    }

    // Run the threads and wait for them to finish
    if (multiThreaded) {
        for (unsigned int i=0; i<threads.size(); i++) {
            threads[i]->start();
        }

        for (unsigned int i=0; i<threads.size(); i++) {
            threads[i]->wait();
        }
    } else {
        threads[0]->run();
    }

    int timerAllThreadsFinished = timer.elapsed();

    //////////////////////////////////////////////////////////////////////////
    // Put the results back in the mesh
    //////////////////////////////////////////////////////////////////////////
    //only apply smoothing if on vertices
    //if (!postprocess(mesh, onVertices, results, normalize, smoothing && onVertices))
    //	return false;

    //perform smoothing on the mesh itself
    if (!onVertices)
        makeFacesNVolume(smoothing, smoothingAnisotropic, smoothingIterations );
        // mesh->makeFacesNVolume(smoothing, smoothingAnisotropic, smoothingIterations);

    int timerPostprocess = timer.elapsed();

    //////////////////////////////////////////////////////////////////////////
    // Clean up
    //////////////////////////////////////////////////////////////////////////
    for (unsigned int i=0; i<threads.size(); i++) {
        delete threads[i];
    }
    threads.clear();
    delete[] origins;
    delete[] results;

    int timerTotal = timer.elapsed();

    if (showDialog) {
        qDebug("Calculated SDF total time %dms. preprocess %dms, compute %dms, threads %dms, postprocess %dms",
            timerTotal, timerPreprocess, timerComputeSearchStructure, timerAllThreadsFinished, timerPostprocess);

        // ATA2: Progressbar stuff....
        // QString s;
        // QString workingon = (onVertices?"vertices" : "facets");
        // QString mt = (multiThreaded?"multithreaded" : "not multithreaded");
        // s.sprintf("Mesh size v=%d f=%d\nWorking on %s, Working %s\nTotal time %f seconds", mesh->vn, mesh->fn, workingon.toAscii(), mt.toAscii(), (float)timerTotal / 1000);
        // QMessageBox::information(g_main, "SDF calculation complete", s);
    }
    return true;
}


void CalculateSDF::preprocess(CMeshO* mesh, const bool onVertices, float* origins){
    m_rayIntersect = new RayIntersect( *mesh );

    if (onVertices) {
        int ind = 0;
        CMeshO::VertexIterator vit = mesh->vert.begin();
        CMeshO::VertexIterator vit_end = mesh->vert.end();
        for (; vit != vit_end; vit++) {
            origins[ind*6  ] = vit->P()[0];
            origins[ind*6+1] = vit->P()[1];
            origins[ind*6+2] = vit->P()[2];
            origins[ind*6+3] = vit->N()[0];
            origins[ind*6+4] = vit->N()[1];
            origins[ind*6+5] = vit->N()[2];
            ind++;
        }
    } else {
        int ind = 0;
        CMeshO::FaceIterator fit = mesh->face.begin();
        CMeshO::FaceIterator fit_end = mesh->face.end();
        for (; fit != fit_end; fit++) {
            Point3f p = Barycenter( *fit );
            origins[ind*6  ] = p[0];
            origins[ind*6+1] = p[1];
            origins[ind*6+2] = p[2];
            origins[ind*6+3] = fit->N()[0];
            origins[ind*6+4] = fit->N()[1];
            origins[ind*6+5] = fit->N()[2];
            ind++;
        }
    }
}
