
// classe di facile uso per allineare 
namespace vcg 
{
class AlignWrapper
{
public:
  class MeshHandle
  {
    public:
    std::string filename;
    Matrix44d Tr;
    AlignPair::A2Mesh *M;
    void WriteRaw();
    void ReadRaw();
    bool ImportFromPly(const char *plyname);
  };

  bool OpenAln(const char *alnfilename);
  bool SetMesh(std::vector<MeshHandle> &M);

  bool ProcessAll();
  bool InitOccupancyGrid();
  bool ChooseArcs();
  bool ProcessArcs();
  bool CalcArc();
  bool GlobalAlign();

  static int ParseAln(char  *alnfile, std::vector<std::string> &names, std::vector<Matrix44d> &LTr, bool RigidFlag=false);
  static void FileMatrixWrite(FILE *fp, const Matrix44d &Tr);

};

}