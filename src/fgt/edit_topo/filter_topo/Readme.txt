To compile this filter, these files are needed:

 - "edit_topomeshbuilder.h" 

	This file contains the core functions used 
	by the retopology filter, and is located 
	in the "edit_topo" folder.

 - "filterparameter.cpp"

	Usually located in 
	"/meshlab/filterparameter.cpp"


So, in order to compile with qmake, it may be 
necessary to change the path of those files 
in the Qt project file ("filter_topo.pro").