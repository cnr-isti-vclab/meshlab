/* ------------------------------------------------------------------------- */

/* 
 * Search for name in the extensions string. Use of strstr()
 * is not sufficient because extension names can be prefixes of
 * other extension names. Could use strtok() but the constant
 * string returned by glGetString might be in read-only memory.
 */
GLboolean glewGetExtension (const char* name)
{    
  GLubyte* p;
  GLubyte* end;
  GLuint len = _glewStrLen((const GLubyte*)name);
  p = (GLubyte*)glGetString(GL_EXTENSIONS);
  if (0 == p) return GL_FALSE;
  end = p + _glewStrLen(p);
  while (p < end)
  {
    GLuint n = _glewStrCLen(p, ' ');
    if (len == n && _glewStrSame((const GLubyte*)name, p, n)) return GL_TRUE;
    p += n+1;
  }
  return GL_FALSE;
}

/* ------------------------------------------------------------------------- */

#ifndef GLEW_MX
static
#endif
GLenum glewContextInit (GLEW_CONTEXT_ARG_DEF_LIST)
{
  const GLubyte* s;
  GLuint dot, major, minor;
  /* query opengl version */
  s = glGetString(GL_VERSION);
  dot = _glewStrCLen(s, '.');
  major = dot-1;
  minor = dot+1;
  if (dot == 0 || s[minor] == '\0')
    return GLEW_ERROR_NO_GL_VERSION;
  if (s[major] == '1' && s[minor] == '0')
  {
	return GLEW_ERROR_GL_VERSION_10_ONLY;
  }
  else
  {
    CONST_CAST(GLEW_VERSION_1_1) = GL_TRUE;
	if (s[major] >= '2')
	{
      CONST_CAST(GLEW_VERSION_1_2) = GL_TRUE;
      CONST_CAST(GLEW_VERSION_1_3) = GL_TRUE;
      CONST_CAST(GLEW_VERSION_1_4) = GL_TRUE;
	  CONST_CAST(GLEW_VERSION_1_5) = GL_TRUE;
	  CONST_CAST(GLEW_VERSION_2_0) = GL_TRUE;
	  if (s[minor] >= '1')
	  {
	    CONST_CAST(GLEW_VERSION_2_1) = GL_TRUE;
      }
	}
	else
	{
	  if (s[minor] >= '5')
	  {
		CONST_CAST(GLEW_VERSION_1_2) = GL_TRUE;
		CONST_CAST(GLEW_VERSION_1_3) = GL_TRUE;
		CONST_CAST(GLEW_VERSION_1_4) = GL_TRUE;
		CONST_CAST(GLEW_VERSION_1_5) = GL_TRUE;
		CONST_CAST(GLEW_VERSION_2_0) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_2_1) = GL_FALSE;
	  }
	  if (s[minor] == '4')
	  {
		CONST_CAST(GLEW_VERSION_1_2) = GL_TRUE;
		CONST_CAST(GLEW_VERSION_1_3) = GL_TRUE;
		CONST_CAST(GLEW_VERSION_1_4) = GL_TRUE;
		CONST_CAST(GLEW_VERSION_1_5) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_2_0) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_2_1) = GL_FALSE;
	  }
	  if (s[minor] == '3')
	  {
		CONST_CAST(GLEW_VERSION_1_2) = GL_TRUE;
		CONST_CAST(GLEW_VERSION_1_3) = GL_TRUE;
		CONST_CAST(GLEW_VERSION_1_4) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_1_5) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_2_0) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_2_1) = GL_FALSE;
	  }
	  if (s[minor] == '2')
	  {
		CONST_CAST(GLEW_VERSION_1_2) = GL_TRUE;
		CONST_CAST(GLEW_VERSION_1_3) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_1_4) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_1_5) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_2_0) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_2_1) = GL_FALSE;
	  }
	  if (s[minor] < '2')
	  {
		CONST_CAST(GLEW_VERSION_1_2) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_1_3) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_1_4) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_1_5) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_2_0) = GL_FALSE;
		CONST_CAST(GLEW_VERSION_2_1) = GL_FALSE;
	  }
	}
  }
  /* initialize extensions */
