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

		FILE *stream=fopen(ALNname, "r");
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
			rm.filename = buffer;

			fgets(buffer, 1024, stream);
			*strchr(buffer,'\n')=0;
			if(buffer[0]!='#') 
				return ExpectingComment;

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
};

#endif //__OM_ALNPARSER_