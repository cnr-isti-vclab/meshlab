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
//  WIN32 version
//
//***************************************************************************

#include "IFXOSFileIterator.h"
#include <windows.h>

#define IFXOSFI_SUBDIR  L"plugins"
#define IFXOSFI_EXT     L"*.dll"
#define IFXOSFI_EXTALL  L"*"
#define IFXOSFI_CURRDIR L"."
#define IFXOSFI_UPPRDIR L".."


/************************************************************************
*
* Default constructor. It will initialize File Iterator with "*.dll" file
* mask and "plugins" sub-directory.
*
************************************************************************/
IFXOSFileIterator::IFXOSFileIterator()
{
	IFXRESULT res;
	res = this->OpenIterator( IFXOSFI_SUBDIR );

	m_filesExtension.Assign( IFXOSFI_EXT );

	if( IFXSUCCESS( res ) )
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
* Constructor. It will initialize File Iterator with specified file mask
* (must be in "*.XXX" or "XXX" format, where XXX is the desired file
* extension) and specified sub-directory. If pointer to input data
* element is NULL then it will be initialized with default value ("*.dll"
* for file mask and "plugins" sub-directory).
*
************************************************************************/
IFXOSFileIterator::IFXOSFileIterator( const IFXCHAR *filesPath, const IFXCHAR *filesExtension )
{
	IFXRESULT res;

	if( NULL != filesPath )
	{
		res = this->OpenIterator( filesPath );
	}
	else
	{
		res = this->OpenIterator( IFXOSFI_SUBDIR );
	}

	if( NULL != filesExtension )
	{
		if( filesExtension[0] != '*' && filesExtension[1] != '.' )
		{
			m_filesExtension.Assign( L"*." );
			m_filesExtension.Concatenate( filesExtension );
		}
		else
		{
			m_filesExtension.Assign( filesExtension );
		}
	}
	else
	{
		m_filesExtension.Assign( IFXOSFI_EXT );
	}

	if( IFXSUCCESS( res ) )
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
	this->CloseIterator();
}

/************************************************************************
*
* Open File Iterator. Should be called from constructor
*
************************************************************************/
IFXRESULT IFXOSFileIterator::OpenIterator( const IFXCHAR *filesPath )
{
	IFXRESULT res = IFX_OK;
	m_depth = 0;
	m_count = 0;
	m_plugins.Clear();

	IFXCHAR pluginLocation[ IFXOSFI_MAXPATHLEN ];

	HMODULE mod = GetModuleHandle(L"IFXCore.dll");
	if( !mod )
	{
		res = IFX_E_NOT_DONE;
	}
	else
	{
		if( !GetModuleFileName( mod, (LPTSTR)pluginLocation, IFXOSFI_MAXPATHLEN ) )
		{
			res = IFX_E_NOT_DONE;
		}
	}

	if( IFXSUCCESS( res ) )
	{
		// cut off the file name and get 'pure' directory name
		/// @todo: this following line is not safe
		*wcsrchr(pluginLocation, '\\') = 0;
		wcscat(pluginLocation, L"\\");
		wcscat(pluginLocation, filesPath);
		wcscat(pluginLocation, L"\\");

		m_pluginLocation.Assign( pluginLocation );
	}

	return res;
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
* Return IFXString that stores path to basic plugins directory.
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
* stored in m_plugins variable delimited by IFXOSFI_DELIM char.
*
************************************************************************/

IFXRESULT IFXOSFileIterator::GetPlugins( IFXString *subPath )
{
	IFXRESULT result = IFX_OK;
	WIN32_FIND_DATA data;
	BOOL res = FALSE;
	HANDLE hdl;
	IFXString tempPath;

	// find and store all files in this dir
	ProcessDir( subPath );

	// now process subdirs
	IFXString localPath( m_pluginLocation );
	localPath.Concatenate( subPath );
	localPath.Concatenate( IFXOSFI_EXTALL );

	hdl = FindFirstFile( localPath.Raw(), &data );

	// if there are no any file/directory then skip next block
	if( INVALID_HANDLE_VALUE != hdl )
	{
		// keep searching while there are any files/directories
		do
		{
			// create full path to the found object
			tempPath.Assign( &m_pluginLocation );
			tempPath.Concatenate( subPath );
			tempPath.Concatenate( data.cFileName );

			// we already found and stored all files we wanted, so check if found object is
			// a) a directory,
			// b) its nesting doesn't exceed the limitation (IFXOSFI_MAXDEPTH),
			// c) its name isn't a "." or ".."
			if( IsDir( &tempPath ) > 0 && m_depth < IFXOSFI_MAXDEPTH &&
				wcscmp( data.cFileName, IFXOSFI_CURRDIR ) && wcscmp( data.cFileName, IFXOSFI_UPPRDIR ) )
			{
				// we have found a directory and we want to look in it, so
				// create its relative path:
				tempPath.Assign( subPath );
				tempPath.Concatenate( data.cFileName );
				tempPath.Concatenate( L"\\" );
				// increment the depth (nesting)
				m_depth++;
				// step inside
				GetPlugins( &tempPath );
				// decrement the depth (nesting)
				m_depth--;
			}

			// find next file/directory
			res = FindNextFile( hdl, &data );

		} while( res );

		// close handle
		FindClose( hdl );
	}

	return result;
}

/************************************************************************
*
* Find all files with required extension in the directory specified by
* input IFXString value. If such files were found then record their names
* (with relative paths) to m_plugins.
*
************************************************************************/

IFXRESULT IFXOSFileIterator::ProcessDir( IFXString *subPath )
{
	HANDLE hdl;
	BOOL isDone = FALSE;
	WIN32_FIND_DATA data;
	IFXRESULT res = IFX_OK;
	IFXString tempPath;

	IFXString localPath( m_pluginLocation );
	localPath.Concatenate( subPath );
	localPath.Concatenate( m_filesExtension );

	hdl = FindFirstFile( localPath.Raw(), &data );

	if( INVALID_HANDLE_VALUE == hdl )
	{
		isDone = TRUE; // no files were found
	}

	if( !isDone )
	{
		// do it while there are any files left
		do
		{
			// create full path to the found object
			tempPath.Assign( &m_pluginLocation );
			tempPath.Concatenate( subPath );
			tempPath.Concatenate( data.cFileName );

			// if it is not a dir then record file name
			if( !IsDir( &tempPath ) )
			{
				m_plugins.Concatenate( subPath );
				m_plugins.Concatenate( data.cFileName );
				m_plugins.Concatenate( IFXOSFI_DELIM );
				// increment plugins count
				m_count++;
			}

		} while( FindNextFile( hdl, &data ) );

		// close handle
		FindClose( hdl );
	}

	return res;
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

int IFXOSFileIterator::IsDir( IFXString *name )
{
	int res = 0;
	BOOL res1;
	WIN32_FILE_ATTRIBUTE_DATA attr;

	// get object's attributes to check if it's a directory
	res1 = GetFileAttributesEx( name->Raw(), GetFileExInfoStandard, (LPVOID)&attr );

	if( ( FILE_ATTRIBUTE_DIRECTORY & attr.dwFileAttributes ) && res1 )
	{
		res = 1;
	}

	if( !res1 )
	{
		res = -1;
	}

	return res;
}


// Congratulations! You have reached the end of file :)

