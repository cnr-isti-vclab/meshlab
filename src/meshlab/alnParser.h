/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef __OM_ALNPARSER_
#define __OM_ALNPARSER_

#include <vector>
#include <string>
#include <vcg/math/matrix44.h>

struct RangeMap
{
	RangeMap()
	{
		quality = 1.0f;
	}

	std::string			filename;
	vcg::Matrix44f	trasformation;
	float						quality;
};

class ALNParser
{
public:
	enum ALNParserCodes {NoError, CantOpen, UnexpectedEOF, ExpectingComment};

	static const char* ErrorMsg(int message_code)
	{
		static const char* error_msg[4] = {"No errors", "Can't open file", "Premature End of file", "I was expecting a comment"};

		if(message_code>3 || message_code<0) 
			return "Unknown error";
		else 
			return error_msg[message_code];
	};

	static int BuildALN(std::vector<RangeMap> &rangemaps, std::vector< std::string > &files)
	{
		rangemaps.clear();
		rangemaps.resize( files.size() );
		std::vector< RangeMap >::iterator		 rm  = rangemaps.begin();
		std::vector< std::string >::iterator it  = files.begin();
		std::vector< std::string >::iterator end = files.end();
		for ( ; it!=end; it++, rm++)
		{
			(*rm).filename = (*it);
			(*rm).quality  = 1.0f;
			(*rm).trasformation.SetIdentity();
		}
		files.clear();
		return NoError;
	};

	static int ParseALN(std::vector<RangeMap> &rangemaps, const char *ALNname)
	{
		rangemaps.clear();

		FILE *stream=fopen(ALNname, "rt");
		if(stream==NULL)
			return CantOpen; 

		int mesh_number;
		fscanf(stream, "%i\n", &mesh_number);

		char buffer[1024];
		for (int m=0; m<mesh_number; m++)
		{
			RangeMap rm;

			fgets(buffer, 1024, stream);
			*strchr(buffer, '\n')=0;
			if(strchr(buffer,'\r')) 	*strchr(buffer,'\r')=0;
			rm.filename = buffer;

			fgets(buffer, 1024, stream);
			if(buffer[0]!='#') 
				return ExpectingComment;

			*strchr(buffer,'\n')=0;
			if(strchr(buffer,'\r')) 	*strchr(buffer,'\r')=0;

			
			char *occurrence = strchr(buffer, 'W');
			if(occurrence!=NULL && occurrence[1]==':') 
				rm.quality = (float) atof(occurrence+2);
			assert(rm.quality>0);

			fscanf(stream,"%f %f %f %f \n",&(rm.trasformation[0][0]),&(rm.trasformation[0][1]),&(rm.trasformation[0][2]),&(rm.trasformation[0][3]));
			fscanf(stream,"%f %f %f %f \n",&(rm.trasformation[1][0]),&(rm.trasformation[1][1]),&(rm.trasformation[1][2]),&(rm.trasformation[1][3]));
			fscanf(stream,"%f %f %f %f \n",&(rm.trasformation[2][0]),&(rm.trasformation[2][1]),&(rm.trasformation[2][2]),&(rm.trasformation[2][3]));
			fscanf(stream,"%f %f %f %f \n",&(rm.trasformation[3][0]),&(rm.trasformation[3][1]),&(rm.trasformation[3][2]),&(rm.trasformation[3][3]));

			rangemaps.push_back(rm);
		}

		return NoError;
	} // end of ParseALN

static bool SaveALN(const char *alnfile, std::vector<std::string> &names)
{
	std::vector<vcg::Matrix44f> Tr(names.size());
	for(int i=0; i < static_cast<int>(Tr.size()); ++i) Tr[i].SetIdentity();
	return SaveALN(alnfile,names, Tr);
}
template <class matrixfloat>
static bool SaveALN(const char *alnfile, std::vector<std::string> &names, std::vector<vcg::Matrix44<matrixfloat> > &Tr)
{
 // printf("Saving aln file %s\n",alnfile);
  FILE *fp=fopen(alnfile,"w");
  if(!fp)
  {
    printf("unable to open file %s\n",alnfile);
    return false;
  }

  fprintf(fp,"%i\n",(int)names.size());
  for(int i=0;i < static_cast<int>(names.size());++i)
  {
    fprintf(fp,"%s\n",names[i].c_str());

    fprintf(fp,"#\n");
    fprintf(fp,"%lf %lf %lf %lf \n",(Tr[i][0][0]),(Tr[i][0][1]),(Tr[i][0][2]),(Tr[i][0][3]));
    fprintf(fp,"%lf %lf %lf %lf \n",(Tr[i][1][0]),(Tr[i][1][1]),(Tr[i][1][2]),(Tr[i][1][3]));
    fprintf(fp,"%lf %lf %lf %lf \n",(Tr[i][2][0]),(Tr[i][2][1]),(Tr[i][2][2]),(Tr[i][2][3]));
    fprintf(fp,"%lf %lf %lf %lf \n",(Tr[i][3][0]),(Tr[i][3][1]),(Tr[i][3][2]),(Tr[i][3][3]));
  }
  fprintf(fp,"0\n");

  fclose(fp);
  return true;
}

};

#endif //__OM_ALNPARSER_