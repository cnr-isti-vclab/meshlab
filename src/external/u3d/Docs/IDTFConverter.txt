IDTFConverter Readme
====================

Supported functionality
-----------------------

- Nodes: Group, View, Light, Model

- Nodes hierarchy: parent-child, multiple parents

- Resources: Light, View, Mesh, Line Set, Point Set, Shader, Material, Texture, Motion

- Modifiers: Shading, Animation, Bone Weight, CLOD, Subdivision, Glyph

- Meta-Data for Nodes, Resources, Modifiers, Scene

- File Reference

Usage
-----

You can use following command line parameters with IDTFConverter

Debugging:

-debuglevel or -dl <number 0 or 1>
	0 - no debug dump (default)
	1 - dump debug information to the file

Export options:

-pquality or -pq <number 0 to 1000>: mesh's position quality (1000 by default)

-tcquality or -tcq <number 0 to 1000>: mesh's texture coordinate quality (1000 by default)

-nquality or -nq <number 0 to 1000>: mesh's normal quality (1000 by default)

-dcquality or -dcq <number 0 to 1000>: mesh's diffuse color quality (1000 by default)

-scquality or -scq <number 0 to 1000>: mesh's specular color quality (1000 by default)

-gquality or -gq <number 0 to 1000>: default geometry quality (1000 by default)
									  used if some of above quality factors are not set

-tquality or -tq <number 0 to 100>: texture quality (100 by default)

-aquality or -aq <number 0 to 1000>: animation quality (1000 by default)

-removezerofaces or -rzf <number 0 or 1>: disable or enable zero area faces removal 
											(DISABLED by default)

-zerofacetolerance or -zft <positive float number>: zero area face tolerance 
													(100.0*FLT_MAX by default)

-excludenormals or -en <number 0 or 1>: disable or enable normals exclusion

-texturelimit or -tl <number>: limit textures to <number> by <number> in size (0 = None)

-exportoptions or -eo
    0     - do not export scene
    1     - export animation
    2     - export geometry
    4     - export lights
    8     - export materials
    16    - export node hierarchy
    32    - export shaders
    64    - export textures
    65535 - export everything (default)
    
Note: description of different mesh's quality factors can be found in 
	  the IFXAuthorGeomCompiler.h.

I/O:

-input or -i: input file name
-output or -o: output file name
-pfile or -pf: read user options from a parameter file, overrides command line params.

Unsupported functionality
-------------------------

- bones for line and point set

Known issues
------------

