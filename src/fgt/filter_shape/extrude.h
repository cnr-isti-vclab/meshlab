template <class MeshType>
class Extrude{
{
   public:
//Core functions:
   void SelectedFaces(MeshType & in, ScalarType offset);

   void FromProfileAndPath(vector<Point3f> &profileVec, vector<Point3f> &pathPointVec, vector<Point3f> &pathFrameVec);

// High level funcs
   void TubeFromPath(vector<Point3f> &profileVec, int profileSize);

} 
