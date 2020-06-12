//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
//
//  IFXOSFileIterator.cpp
//
//  This file describes the class which allows to specify path and
//  mask for files and iterate through them.
//
//  Unix version: note about the above : this is not a general file iterator
//  at all, it just looks for dynamic objects in a directory and its subdirs.
// 
//  Note: must check standard on filesystem file names: is it always
//  UTF-8 ? In which case we could replace our stupid conversion
//  functions with proper utf-8 enc/decoding
//
//***************************************************************************

#include "IFXOSFileIterator.h"
#include <unistd.h>
#include <wchar.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

#ifdef _DEBUG
#define DPRINT(X) fprintf X
#else
#define DPRINT(X)
#endif

#define IFXOSFI_SUBDIR  L"Plugins"
#define IFXOSFI_EXT L".so"

static const char* s_pDefaultCorePath = U3DCorePath;
static const char* s_pCorePathEnvVar = "U3D_LIBDIR";

// Convert name from wchar_t to char the brute way and 
// hope for the best...
static void wtoc(char *d, const wchar_t *s)
{
	size_t l = wcslen(s);
	size_t i;
	for (i = 0 ; i < l; i++)
 		d[i] = (char)s[i];
	d[i] = 0;
}

static void ctow(wchar_t *d, const char *s)
{
	size_t l = strlen(s);
	size_t i;
	for (i = 0 ; i < l; i++)
		d[i] = (wchar_t)s[i];
	d[i] = 0;
}

/************************************************************************
*
* Default constructor. It will initialize File Iterator with "*.dll" file
* mask and "plugins" sub-directory.
*
************************************************************************/
IFXOSFileIterator::IFXOSFileIterator()
{
    IFXRESULT res;
    res = OpenIterator(IFXOSFI_SUBDIR);

    m_filesExtension.Assign( IFXOSFI_EXT );

    if(IFXSUCCESS(res)) 
		m_isOk = TRUE;
	else 
		m_isOk = FALSE;
}

/************************************************************************
*
* Constructor. It will initialize File Iterator with specified file mask
* (must be in "*.XXX" or "XXX" format, where XXX is the desired file
* extension) and specified sub-directory. If pointer to input data
* element is NULL then it will be initialized with default value ("*.dll"
* for file mask and "plugins" sub-directory).
*
************************************************************************/
IFXOSFileIterator::IFXOSFileIterator(
					const IFXCHAR *filesPath, 
				    const IFXCHAR *filesExtension)
{
    IFXRESULT res;

    if(NULL != filesPath) 
	{
		res = OpenIterator(filesPath);
    } 
	else	
	{
		res = OpenIterator(IFXOSFI_SUBDIR);
    }

    if(NULL != filesExtension) 
	{
		m_filesExtension.Assign(filesExtension);
    } 
	else 
	{
		m_filesExtension.Assign(IFXOSFI_EXT);
    }

    if(IFXSUCCESS(res))	
	{
		m_isOk = TRUE;
    } 
	else 
	{
		m_isOk = FALSE;
    }
}

/************************************************************************
*
* Return TRUE if initialization was ok, otherwise - FALSE
*
************************************************************************/
BOOL IFXOSFileIterator::IsOk(void) const
{
    return m_isOk;
}

/************************************************************************
*
* Destructor
*
************************************************************************/
IFXOSFileIterator::~IFXOSFileIterator()
{
    CloseIterator();
}

/************************************************************************
*
* Open File Iterator. Should be called from constructor
*
************************************************************************/
IFXRESULT IFXOSFileIterator::OpenIterator(const IFXCHAR *filesPath)
{
    IFXRESULT result = IFX_OK;
	
    m_depth = 0;
    m_count = 0;
    m_plugins.Clear();

    char pluginLocation[IFXOSFI_MAXPATHLEN];

    // The Windows version looks for the path to IFXCore.dll  by calling
    // GetModuleHandle() and GetModuleFilename(). But we can't do that
    // 
    // The input filePaths is like "plugins", or an equivalent. This
    // is likely to be a subdirectory of the main 'lib' directory in
    // the IFX installation Not knowing the structure of the install,
    // we use an environment variable for the lib directory: U3D_LIBDIR
    const char *lib = getenv(s_pCorePathEnvVar);
    if (lib == 0)
		lib = s_pDefaultCorePath;
		
    strncpy(pluginLocation, lib, IFXOSFI_MAXPATHLEN);
    pluginLocation[IFXOSFI_MAXPATHLEN-1] = 0;

    if (strlen(pluginLocation) + wcslen(filesPath) + 3 >= IFXOSFI_MAXPATHLEN -1) 
	{
		result = IFX_E_UNDEFINED;
    }

	if (IFXSUCCESS(result))
	{
	    // Translate to wchar
    	IFXCHAR wpl[IFXOSFI_MAXPATHLEN];
	    ctow(wpl, pluginLocation);
		
    	// Concatenate subdir
	    wcscat(wpl, L"/");
    	wcscat(wpl, filesPath);
	    wcscat(wpl, L"/");
    	m_pluginLocation.Assign(wpl);
	}
	
    return result;
}

/************************************************************************
*
* Close File Iterator. Should be called from destructor
*
************************************************************************/
void IFXOSFileIterator::CloseIterator()
{
    // free all resources
}

/************************************************************************
*
* Process all sub-directories and all files.
*
************************************************************************/

IFXRESULT IFXOSFileIterator::GetPlugins(  IFXString& rPlugins, U32& rPluginCount )
{
	IFXString empty(L"");

	// start iteration thru this directory and all subdirectories
	// (except ones that exceed nesting limitation defined by IFXOSFI_MAXDEPTH)
	IFXRESULT res = GetPlugins( &empty );

	if( IFXSUCCESS( res ) )
	{
		// output plugin names
		rPlugins.Assign( &m_plugins );

		// output plugins count
		rPluginCount = m_count;
	}
	else
	{
		// if error then clean the output string
		m_plugins.Clear();
	}

	// return reference to string with plugins' names
	return res;
}

/************************************************************************
*
* Return pointer to IFXString that stores path to basic plugins directory.
*
************************************************************************/

void IFXOSFileIterator::GetPluginsLocation( IFXString& rLocation )
{
	rLocation.Assign( &m_pluginLocation );
}

/************************************************************************
*
* Process all plugins in subdirectory specified by input IFXString and
* its subdirectories (if their nesting depths are under the limit defined
* by IFXOSFI_MAXDEPTH). All found plugins (with relative paths) will be
* stored in m_plugins variable delimited by IFXOSFI_DELIM1 char.
*
************************************************************************/

IFXRESULT IFXOSFileIterator::GetPlugins(IFXString *subPath)
{
    IFXString localPath(m_pluginLocation);
    localPath.Concatenate(subPath);

    char dirname[IFXOSFI_MAXPATHLEN];
    wtoc(dirname, localPath.Raw());
    DPRINT((stderr, "IFXOSFileIterator::GetPlugins: %s\n", dirname));

    // Handle file entries. This is not exactly optimal because we end
    // up reading the directory twice, but it mimicks the Windows
    // version
    ProcessDir(subPath);
	
    if (m_depth >= IFXOSFI_MAXDEPTH)
		return IFX_OK;


    DIR* d = opendir(dirname);
    if (d == 0) 
	{
		DPRINT((stderr, "IFXOSFileIterator::GetPlugins: error opening %s ", dirname));
		perror("");
		return IFX_E_NOT_DONE;
    }

    struct dirent *ent;
    while ((ent = readdir(d)) != 0) 
	{
		// Don't process ., .., and all hidden files
		if (ent->d_name[0] == '.') 
		    continue;

		char fn[IFXOSFI_MAXPATHLEN];
		snprintf(fn, IFXOSFI_MAXPATHLEN, "%s/%s", dirname, ent->d_name);
		struct stat st;
		int statret = stat(fn, &st);
		if (statret == -1) 
		{
	    	DPRINT((stderr, "IFXOSFileIterator::GetPlugins: can stat %s ", fn));
		    continue;
		}
		
		if (S_ISDIR(st.st_mode)) 
		{
		    wchar_t wname[IFXOSFI_MAXPATHLEN];
	    	ctow(wname, ent->d_name);
		    IFXString tempPath(subPath);
	    	tempPath.Concatenate(wname);
		    m_depth++;
		    GetPlugins(&tempPath);
	    	m_depth--;
		}
    }

    if (d)
		closedir(d);
		
    return IFX_OK;
}

/************************************************************************
*
* Find all files with required extension in the directory specified by
* input IFXString value. If such files were found then record their names
* (with relative paths) to m_plugins.
*
************************************************************************/

IFXRESULT IFXOSFileIterator::ProcessDir(IFXString *subPath)
{
    IFXString localPath(m_pluginLocation);
    localPath.Concatenate(subPath);

    char dirname[IFXOSFI_MAXPATHLEN];
    wtoc(dirname, localPath.Raw());

    DIR* d = opendir(dirname);
    if (d == 0) 
	{
		DPRINT((stderr, "IFXOSFileIterator::ProcessDir: error opening %s ", dirname));
		perror("");
		return IFX_E_NOT_DONE;
    }

    struct dirent *ent;
    while ((ent = readdir(d)) != 0) 
	{
		// Don't process ., .., and all hidden files
		if (ent->d_name[0] == '.') 
		    continue;

		char fn[IFXOSFI_MAXPATHLEN];
		snprintf(fn, IFXOSFI_MAXPATHLEN, "%s/%s", dirname, ent->d_name);
		struct stat st;
		int statret = stat(fn, &st);
		if (statret == -1) 
		{
	    	DPRINT((stderr, "IFXOSFileIterator::ProcessDir: can stat %s \n", fn));
		    continue;
		}
		
		if (S_ISREG(st.st_mode)) 
		{
		    // Check extension match
		    char ext[IFXOSFI_MAXPATHLEN];
		    wtoc(ext, m_filesExtension.Raw());
		    size_t le = strlen(ext);
	    	size_t ln = strlen(ent->d_name);

		    if (ln <= le || strcmp(ent->d_name+ln-le, ext))
				continue;

			// Add to list
		    DPRINT((stderr, "IFXOSFileIterator::ProcessDir: adding %s\n",ent->d_name));
		    wchar_t wname[IFXOSFI_MAXPATHLEN];
		    ctow(wname, ent->d_name);
	    	m_plugins.Concatenate(subPath );
		    m_plugins.Concatenate(wname);
		    m_plugins.Concatenate(IFXOSFI_DELIM);
	    	m_count++;
		}
    }

    if (d)
		closedir(d);
		
    return IFX_OK;
}

/************************************************************************
*
* If you provide this function with name (with full path) to an object then
* it will answer you is it a file or a directory.
*
* Return values:
* 1 (or any value greater than zero)    - if object is a directory,
* 0                                     - if object isn't a directory
* -1                                    - if error occured
*
************************************************************************/
int IFXOSFileIterator::IsDir(IFXString *name)
{
    size_t l = wcslen(name->Raw());
    char *cname = (char *)malloc(l+1);
	
    if (!cname) 
	{
		return -1;
    }
	
    wtoc(cname, name->Raw());
    struct stat st;
	
    if (stat(cname, &st) < 0) 
	{
		free(cname);
		return -1;
    }
	
    free(cname);
    if (S_ISDIR(st.st_mode)) 
	{
		return 1;
    }
	
    return 0;
}
