/*===========================================================================*\
 *                                                                           *
 *                               CoMISo                                      *
 *      Copyright (C) 2008-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.rwth-graphics.de                            *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of CoMISo.                                             *
 *                                                                           *
 *  CoMISo is free software: you can redistribute it and/or modify           *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  CoMISo is distributed in the hope that it will be useful,                *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with CoMISo.  If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                           *
\*===========================================================================*/ 

#ifndef COMISODLLEXPORT
	#ifdef WIN32
		#ifdef COMISODLL
			#ifdef USECOMISO
				#define COMISODLLEXPORT __declspec(dllimport)
				#define COMISODLLEXPORTONLY 
			#else
				#define COMISODLLEXPORT __declspec(dllexport)
				#define COMISODLLEXPORTONLY __declspec(dllexport)
			#endif
		#else		
			#define COMISODLLEXPORT	
			#define COMISODLLEXPORTONLY
		#endif
	#else
		#define COMISODLLEXPORT
		#define COMISODLLEXPORTONLY
	#endif
#endif

#undef min
#undef max


