#ifndef _SHADER_BASIC_
#define _SHADER_BASIC_



struct Shader{

	static int  SetFromString( const GLchar *stringV,   const GLchar *stringF, GLuint &fs, GLuint & vs, GLuint &pr){
		fs= glCreateShader(GL_FRAGMENT_SHADER);
		vs= glCreateShader(GL_VERTEX_SHADER);


		if(stringV!=NULL){
			glShaderSource(vs, 1, &stringV,NULL);
			glCompileShader(vs);
			int errV;
			glGetShaderiv(vs,GL_COMPILE_STATUS,&errV);
			if(errV!=GL_TRUE) return -2;
		}else return -1;
		if(stringF!=NULL){
			glShaderSource(fs, 1, &stringF,NULL);
			glCompileShader(fs);
			int errF;
			glGetShaderiv(fs,GL_COMPILE_STATUS,&errF);
			if(errF!=GL_TRUE) return -4;
		}else return -3;
		pr = glCreateProgram();
		glAttachShader(pr,vs);
		glAttachShader(pr,fs);
		glLinkProgram(pr);
		return 0;
	}


	static void Validate( const int & s ){
		int res;
		glValidateProgram(s);
		glGetProgramiv(s,GL_VALIDATE_STATUS,&res);
                qDebug("validation of program %d:%d \n",s,res);

		glGetProgramiv(s,GL_LINK_STATUS,&res);
                qDebug("linking of program %d:%d \n",s,res);

		glGetProgramiv(s,GL_ACTIVE_ATTRIBUTES,&res);
                qDebug("active attribute of program %d:%d \n",s,res);

		glGetProgramiv(s,GL_ACTIVE_UNIFORMS,&res);
                qDebug("active uniform  of program %d:%d \n",s,res);

		glGetProgramiv(s,GL_ACTIVE_UNIFORM_MAX_LENGTH,&res);
                qDebug("active uniform Max Length of program %d:%d \n",s,res);
}
};

struct FBO{
static void Check(int fboStatus)
{
        if ( fboStatus != GL_FRAMEBUFFER_COMPLETE_EXT)
        {
                if (fboStatus == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)
                {
                        qDebug("FBO Incomplete: Attachment");
                }
                else if (fboStatus == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT)
                {
                        qDebug("FBO Incomplete: Missing Attachment");
                }
                else if (fboStatus == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)
                {
                        qDebug("FBO Incomplete: Dimensions");
                }
                else if (fboStatus == GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)
                {
                        qDebug("FBO Incomplete: Formats");
                }
                else if (fboStatus == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT)
                {
                        qDebug("FBO Incomplete: Draw Buffer");
                }
                else if (fboStatus == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT)
                {
                        qDebug("FBO Incomplete: Read Buffer");
                }
                else if (fboStatus == GL_FRAMEBUFFER_UNSUPPORTED_EXT)
                {
                        qDebug("FBO Unsupported");
                }
                else
                {
                        printf("Undefined FBO error");
                        exit(-4);
                }
        }
 }
};
#endif
