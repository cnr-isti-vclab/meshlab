#ifndef CELL_ATTRIBUTES_H
#define CELL_ATTRIBUTES_H

#include "cell.h"



/* the correspondences between names of the chains in Cell::perVertex_attributes and Cell::perFace_attributes
   with the access memebers of BertexType and FaceType are fixed and determined in this file
   */

template <class VertexType>
    void Cell::ExportVertexAttribute(std::string attr_name, const unsigned int & i, VertexType & v ){
    if( attr_name == std::string("Normal3f"))   {  GetVertexAttribute(std::string("Normal3f"),i,&v.N());   return;}
    if( attr_name == std::string("Color4b"))    {  GetVertexAttribute(std::string("Color4b"),i,&v.C());    return;}
    sprintf(lgn->Buf(),"Vertex Attribute %s not implemented\n",attr_name.c_str());
    lgn->Push();
}

template <class FaceType>
    void Cell::ExportFaceAttribute(std::string attr_name, const unsigned int & i, FaceType & f ){
    if( attr_name == std::string("Normal3f"))   { GetFaceAttribute(std::string("Normal3f"),i,&f.N());   return;}
    if( attr_name == std::string("Color4b"))    { GetFaceAttribute(std::string("Color4b"),i,&f.C());    return;}
    sprintf(lgn->Buf(),"Face Attribute %s not implemented\n",attr_name.c_str());
    lgn->Push();
}


template <class VertexType>
    void Cell::ImportVertexAttribute(std::string attr_name, const unsigned int & i, VertexType & v ){
    if( attr_name == std::string("Normal3f"))   {SetVertexAttribute(std::string("Normal3f"),i,&v.N()); return;}
    if( attr_name == std::string("Color4b"))    {SetVertexAttribute(std::string("Color4b"),i,&v.C()); return;}
    sprintf(lgn->Buf(),"Vertex Attribute %s not implemented\n",attr_name.c_str());
    lgn->Push();
}

template <class FaceType>
    void Cell::ImportFaceAttribute(std::string attr_name, const unsigned int & i, FaceType & f ){
    if( attr_name == std::string("Normal3f"))   { SetFaceAttribute(std::string("Normal3f"),i,&f.N());  return;}
    if( attr_name == std::string("Color4b"))    { SetFaceAttribute(std::string("Color4b") ,i,&f.C());  return;}
    sprintf(lgn->Buf(),"Face Attribute %s not implemented\n",attr_name.c_str());
    lgn->Push();
}


/*

  */

struct AttributeMapper{

    std::vector<std::string> vert_attrs;
    std::vector<std::string> face_attrs;


    /* Import / Export functions */
    template <class VertexType>
    void ImportVertex(Cell*c,VertexType & v, const unsigned int & pos){
        std::vector<std::string>::iterator ai;
        for(ai = vert_attrs.begin(); ai != vert_attrs.end(); ++ai)
            c->ImportVertexAttribute((*ai),pos,v);
    }

    template <class FaceType>
    void ImportFace(Cell*c,FaceType & v, const unsigned int & pos){
        std::vector<std::string>::iterator ai;
        for(ai = face_attrs.begin(); ai != face_attrs.end(); ++ai)
            c->ImportFaceAttribute((*ai),pos,v);
    }

    template <class VertexType>
    void ExportVertex(Cell*c,VertexType & v, const unsigned int & pos){
        std::vector<std::string>::iterator ai;
        for(ai = vert_attrs.begin(); ai != vert_attrs.end(); ++ai)
            c->ExportVertexAttribute((*ai),pos,v);
    }

    template <class FaceType>
    void ExportFace(Cell*c,FaceType & v, const unsigned int & pos){
        std::vector<std::string>::iterator ai;
        for(ai = face_attrs.begin(); ai != face_attrs.end(); ++ai)
            c->ExportFaceAttribute((*ai),pos,v);
    }


};


#endif // CELL_ATTRIBUTES_H
