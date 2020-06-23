#ifndef _OPENGR_IO_PLY_H
#define _OPENGR_IO_PLY_H

#include <cstdarg>

typedef enum {
    BINARY_BIG_ENDIAN_1,
    BINARY_LITTLE_ENDIAN_1,
    ASCII_1
} PLYFormat;

static const unsigned int MAX_COMMENT_SIZE = 256;


inline unsigned int
readHeader ( const char *filename,
             unsigned int & numOfVertices,
             unsigned int & numOfFaces,
             PLYFormat & format,
             unsigned int & numOfVertexProperties,
             bool         & haveColor )
{
    using namespace std;

    // use binary mode to preserve end line symbols on linux and windows
    ifstream in (filename, std::ios_base::in | std::ios_base::binary);
    if (!in){
        cerr << "(PLY) error opening file" << endl;
        return 0;
    }

    numOfVertexProperties = 0;
    numOfVertices = 0;
    numOfFaces = 0;
    haveColor = false;

    string current, currentelement;
    in >> current;
    if (current != "ply"){
        cerr << "(PLY) not a PLY file" << endl;
        return 0;
    }
    in >> current;
    int lid = 0;
    while (current != "end_header") {
        if (current == "format") {
            in >> current;
            if (current == "binary_big_endian") {
                in >> current;
                if (current == "1.0")
                    format = BINARY_BIG_ENDIAN_1;
                else{
                    cerr << "(PLY) error parsing header - bad binary big endian version" << endl;
                    return 0;
                }
            } else if (current == "binary_little_endian") {
                in >> current;
                if (current == "1.0")
                    format = BINARY_LITTLE_ENDIAN_1;
                else{
                    cerr << "(PLY) error parsing header - bad binary little endian version" << endl;
                    return 0;
                }
            } else if (current == "ascii") {
                in >> current;
                if (current == "1.0")
                    format = ASCII_1;
                else{
                    cerr << "(PLY) error parsing header - bad ascii version" << endl;
                    return 0;
                }
            } else {
                cerr << "(PLY) error parsing header (format)" << endl;
                return 0;
            }
        } else if (current == "element") {
            in >> current;
            if (current == "vertex"){
                currentelement = current;
                in >> numOfVertices;
            }
            else if (current == "face"){
                currentelement = current;
                in >> numOfFaces;
            }
            else{
                cerr << "(PLY) ignoring unknown element " << current << endl;
                currentelement = "";
            }
        } else if (currentelement != "" && current == "property") {
            in >> current;
            if (current == "float" || current == "double") {
                numOfVertexProperties++;
                in >> current;
            }
            else if (current == "uchar") { // color
                numOfVertexProperties++;
                haveColor = true;
                in >> current;
            }
            else if (current == "list") {
                in >> current;
                in >> current;
                in >> current;
            } else {
                cerr << "(PLY) error parsing header (property)" << endl;
                return 0;
            }
        } else if ( (current == "comment") || (current.find("obj_info") != std::string::npos) ) {
            char comment[MAX_COMMENT_SIZE];
            in.getline (comment, MAX_COMMENT_SIZE);
        } else {
            ;
            //cerr << "(PLY) ignoring line: " << current << endl;
            //return 0;
        }
        in >> current;
        lid++;
    }

    unsigned int headerSize = in.tellg ();
    in.close ();
    return headerSize+1;
}


template <class T>
void
bigLittleEndianSwap (T * v, unsigned int numOfElements)
{
    char * tmp = (char*)v;
    for (unsigned int j = 0; j < numOfElements; j++){
        unsigned int offset = 4*j;
        char c = tmp[offset];
        tmp[offset] =  tmp[offset+3];
        tmp[offset+3] = c;
        c = tmp[offset+1];
        tmp[offset+1] = tmp[offset+2];
        tmp[offset+2] = c;
    }
}

inline bool
almostsafefread ( void * ptr, size_t size, size_t count, FILE * stream ){
    size_t result = fread (ptr,size,count,stream);
    return (result == count);
}

template<int targetCount, typename... Args>
inline bool
almostsafefscanf ( FILE * stream, const char * format, Args&&... args ){
    int count = fscanf (stream,format, std::forward<Args>(args)...);
    return (count == targetCount);
}

template<typename Scalar>
bool
readBinary1Body (const std::string & filename,
                 unsigned int headerSize,
                 unsigned int numOfVertices,
                 unsigned int numOfFaces,
                 unsigned int numOfVertexProperties,
                 bool         haveColor,
                 bool bigEndian,
                 std::vector<gr::Point3D<Scalar> >& vertex,
                 std::vector<typename gr::Point3D<Scalar>::VectorType>& normal,
                 std::vector<tripple>& face )
{
    using namespace std;
    using namespace gr;

    FILE * in = fopen (filename.c_str (), "r");
    if (!in){
        cerr << "(PLY) error opening file" << endl;
        return false;
    }

    char c;
    for (unsigned int i = 0; i < headerSize; i++) {
        almostsafefread (&c, 1, 1, in);
    }

    // *****************
    // Reading geometry.
    // *****************
    typename Point3D<Scalar>::VectorType n;
    typename Point3D<Scalar>::VectorType rgb;
    float * v = new float[numOfVertexProperties];
    unsigned char rgb_buff [4];

    for (unsigned int i = 0; i < numOfVertices && !feof (in); i++) {
        if (numOfVertexProperties==10){
            almostsafefread (v, 4, 6, in);
            almostsafefread (rgb_buff, sizeof(unsigned char), 4, in);
        }else if (numOfVertexProperties==9){
            almostsafefread (v, 4, 6, in);
            almostsafefread (rgb_buff, sizeof(unsigned char), 3, in);
        }else if (numOfVertexProperties==6 && haveColor){
            almostsafefread (v, 4, 3, in);
            almostsafefread (rgb_buff, sizeof(unsigned char), 3, in);
        }else if (numOfVertexProperties==7 ){
            almostsafefread (v, 4, 3, in);
            almostsafefread (rgb_buff, sizeof(unsigned char), 4, in);
        }
        else
            almostsafefread (v, 4, numOfVertexProperties, in);
        if (bigEndian == true)
            bigLittleEndianSwap (v, numOfVertexProperties);
        vertex.emplace_back( v[0],v[1],v[2] );

        if (numOfVertexProperties == 6){
            if (haveColor){
                rgb << rgb_buff[0], rgb_buff[1], rgb_buff[2];
                vertex.back().set_rgb(rgb);
            }else{
                n << v[3], v[4], v[5];
                normal.push_back (n);
                vertex.back().set_normal(n);
            }
        }else if (numOfVertexProperties == 7){
            rgb << rgb_buff[0], rgb_buff[1], rgb_buff[2];
            vertex.back().set_rgb(rgb);
        }else if (numOfVertexProperties == 9 || numOfVertexProperties == 10){
            n << v[3], v[4], v[5];
            rgb << rgb_buff[0], rgb_buff[1], rgb_buff[2];
            normal.push_back (n);
            vertex.back().set_normal(n);
            vertex.back().set_rgb(rgb);
        }
    }
    delete [] v;

    if (numOfFaces != 0){
        if (feof (in)){
            cerr << "(PLY) incomplete file" << endl;
            return false;
        }

        // *****************
        // Reading topology.
        // *****************
        for (unsigned int i = 0; i < numOfFaces && !feof (in); i++) {
            unsigned int f[4];
            char polygonSize;
            almostsafefread (&polygonSize, 1, 1, in);
            almostsafefread (f, 4, 3, in);
            if (bigEndian == true)
                bigLittleEndianSwap (f, 3);
            face.emplace_back(f[0],f[1],f[2]);
        }
    }

    return true;
}

template<typename Scalar>
bool
readASCII1Body (const std::string & filename,
                unsigned int headerSize,
                unsigned int numOfVertices,
                unsigned int numOfFaces,
                unsigned int numOfVertexProperties,
                bool         haveColor,
                std::vector<gr::Point3D<Scalar> >& vertex,
                std::vector<typename gr::Point3D<Scalar>::VectorType>& normal,
                std::vector<tripple>& face )
{
    using namespace std;
    using namespace gr;

    FILE * in = fopen (filename.c_str (), "r");
    if (!in){
        cerr << "(PLY) error opening file" << endl;
        return false;
    }

    char c;
    for (unsigned int i = 0; i < headerSize; i++) {
        almostsafefread (&c, 1, 1, in);
    }

    // *****************
    // Reading geometry.
    // *****************
    typename Point3D<Scalar>::VectorType n;
    typename Point3D<Scalar>::VectorType rgb;
    unsigned int rgb_buff [4];
    for (unsigned int i = 0; i < numOfVertices && !feof (in); i++) {
        std::vector<float> v(numOfVertexProperties);

        if (numOfVertexProperties==10){
            for (unsigned int j = 0;  j < 6;  j++)
                almostsafefscanf<1> (in, "%f", &v[j]);
            for (unsigned int j = 0;  j < 4;  j++)
                almostsafefscanf<1> (in, "%i", &rgb_buff[j]);
        }
        else if (numOfVertexProperties==9){
            for (unsigned int j = 0;  j < 6;  j++)
                almostsafefscanf<1> (in, "%f", &v[j]);
            for (unsigned int j = 0;  j < 3;  j++)
                almostsafefscanf<1> (in, "%i", &rgb_buff[j]);
        }else if (numOfVertexProperties==6 && haveColor){
            for (unsigned int j = 0;  j < 3;  j++)
                almostsafefscanf<1> (in, "%f", &v[j]);
            for (unsigned int j = 0;  j < 3;  j++)
                almostsafefscanf<1> (in, "%i", &rgb_buff[j]);
        }else if (numOfVertexProperties==7){
            for (unsigned int j = 0;  j < 3;  j++)
                almostsafefscanf<1> (in, "%f", &v[j]);
            for (unsigned int j = 0;  j < 4;  j++)
                almostsafefscanf<1> (in, "%i", &rgb_buff[j]);
        }
        else
            for (unsigned int j = 0;  j < numOfVertexProperties;  j++)
                almostsafefscanf<1> (in, "%f", &v[j]);

        vertex.emplace_back( v[0],v[1],v[2] );

        if (numOfVertexProperties == 6){
            if (haveColor){
                rgb << rgb_buff[0], rgb_buff[1], rgb_buff[2];
                vertex.back().set_rgb(rgb);

            }else{
                n << v[3], v[4], v[5];
                normal.push_back (n);
                vertex.back().set_normal(n);
            }
        }else if (numOfVertexProperties == 7){
            rgb << rgb_buff[0], rgb_buff[1], rgb_buff[2];
            vertex.back().set_rgb(rgb);
        }else if (numOfVertexProperties == 9 || numOfVertexProperties == 10){
            n << v[3], v[4], v[5];
            rgb << rgb_buff[0], rgb_buff[1], rgb_buff[2];
            normal.push_back (n);
            vertex.back().set_normal(n);
            vertex.back().set_rgb(rgb);
        }
    }

    if (numOfFaces != 0){
        if (feof (in)){
            cerr << "(PLY) incomplete file" << endl;
            return false;
        }

        // *****************
        // Reading topology.
        // *****************
        for (unsigned int i = 0; i < numOfFaces && !feof (in); i++) {
            int f[3];
            int polygonSize;
            almostsafefscanf<4> (in, "%d %d %d %d", &polygonSize, &f[0], &f[1], &f[2]);
            face.emplace_back(f[0],f[1],f[2]);
        }
    }

    return true;
}

template<typename Scalar>
bool
IOManager::ReadPly(const char *filename,
                   vector<Point3D<Scalar> > &v,
                   vector<typename Point3D<Scalar>::VectorType> &normals){

  vector<tripple> face;
  unsigned int numOfVertexProperties, numOfVertices, numOfFaces;
  PLYFormat format;
  bool haveColor;
  unsigned int headerSize = readHeader (filename,
                                        numOfVertices,
                                        numOfFaces,
                                        format,
                                        numOfVertexProperties,
                                        haveColor);
  if (haveColor)
    cout << "haveColor" << endl;
  if (headerSize != 0){
    if (format == BINARY_BIG_ENDIAN_1)
      return readBinary1Body<Scalar> (filename, headerSize, numOfVertices,
                       numOfFaces,
                       numOfVertexProperties, haveColor, true, v, normals, face );
    else if (format == BINARY_LITTLE_ENDIAN_1)
      return readBinary1Body<Scalar> (filename, headerSize, numOfVertices,
                       numOfFaces,
                       numOfVertexProperties, haveColor, false, v, normals, face );
    else if (format == ASCII_1)
      return readASCII1Body<Scalar> ( filename, headerSize, numOfVertices,
                       numOfFaces,
                       numOfVertexProperties, haveColor, v, normals, face );
    else{
      cerr << "(PLY) no support for this PLY format" << endl;
      return false;
    }
  }

  return false;
}

#endif //IO_PLY_H
