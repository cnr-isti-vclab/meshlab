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

/**
	@file	IFXOSFileIterator.h

		    Header file for IFXOSFileIterator class.
    
	@note	Do not put any OS specific includes or types in this header!
*/

#ifndef IFXOSFILEITERATOR_H
#define IFXOSFILEITERATOR_H

#include "IFXResult.h"
#include "IFXString.h"

#define IFXOSFI_MAXPATHLEN 512
#define IFXOSFI_MAXDEPTH   5    ///< Max depth of sub-directories nesting

const IFXCHAR IFXOSFI_DELIM[] = L":";

/**
	This class provides functionality to iterate through files in specified
	directory

	If you want to specify subdirectory to search thru or file extension then you
	should use second construstor (with input parameters) otherwise you can use
	default strings: "plugins" subdirectory and ".dll" mask. If you want to
	specify another directory or mask after iterator creation then you should
	destroy present iterator and create the new one.

	The result of iteration will be stored in m_plugins. Plugins' names with
	relative pathes will be recorded here. For example, if your plugins directory
	is "D:\Name1\Name2\Name3\plugins\" then you can get the following IFXString:
	"plugin1.abc:plugin2.abc:dir1\plugin3.abc:dir2\dir3\plugin4.abc:". This
	string means that you have following plugins:

	D:\Name1\Name2\Name3\plugins\plugin1.abc
	D:\Name1\Name2\Name3\plugins\plugin2.abc
	D:\Name1\Name2\Name3\plugins\dir1\plugin3.abc
	D:\Name1\Name2\Name3\plugins\dir2\dir3\plugin4.abc

	You can getfull path to plugins directory using GetPluginsLocation() method
	and string with names and number of plugins using GetPlugins() method.
*/
class IFXOSFileIterator
{
public:
    IFXOSFileIterator( void );
    IFXOSFileIterator( const IFXCHAR* pFilesPath, const IFXCHAR* pFilesExtension );
   ~IFXOSFileIterator( void );

    IFXRESULT GetPlugins( IFXString& rPlugins, U32& rPluginCount );
    void GetPluginsLocation( IFXString& rLocation );
    BOOL IsOk( void ) const;

private:
    IFXRESULT GetPlugins( IFXString* pSubPath );
    IFXRESULT OpenIterator( const IFXCHAR* pFilesPath );
    void CloseIterator( void );
    IFXRESULT ProcessDir( IFXString* pSubPath );
    int IsDir( IFXString* pName );

    U32         m_depth;                ///< Depth of nesting of current sub-directory
    U32         m_count;                ///< Total number of files found
    IFXString	m_pluginLocation;       ///< Plugins directory path
    IFXString   m_filesExtension;       ///< Plugins extension
    IFXString   m_plugins;              ///< List of relative paths to plugins delimited with commas
    BOOL        m_isOk;                 ///< True if initialization was ok; otherwise - false
};

#endif
