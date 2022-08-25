template <class MeshType>
class Extrude{
{
   public:
//Core functions:
   void SelectedFaces(MeshType & in, ScalarType offset);


// Generate an extruded profile starting from a profile, a path and a set of rotations.
// We assume that the profile is centered around the origin and lie on the XY plane.
// the pathFrameVec encodes the rotations that have to be applied to the profile at each step encoded as eulerian angles.
void FromProfileAndPath(vector<Point3f> &profileVec, vector<Point3f> &pathPointVec, vector<Point3f> &pathFrameVec, vector<float> &scaleVec);

// High level funcs
 void TubeFromPath(vector<Point3f> &pathPointVec, int profileSize);

} 
