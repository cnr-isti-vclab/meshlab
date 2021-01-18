# MeshLab Server 

MeshLab Server is a tool that can perform mesh processing in batch mode. It allows to automate all the operations and filters that can be applied with  MeshLab, without a GUI.

In order to be used, MeshLab server usually requires at least three arguments:

* An input file, that can be a MeshLab project (`*.mlp`), or one of the various mesh formats supported by MeshLab;
* An output file, that can be a MeshLab project (`*.mlp`), or one of the various mesh formats supported by MeshLab;
* A script file (`*.mlx`) containing a set of filters with their parameters that will be applied to the input file.

## MeshLab Scripts (`*.mlx`)

A MeshLab script (`*.mlx`) is an XML file that can be read by MeshLab Server, and can contains an ordered set of filters that can be applied to a given input mesh. 

MeshLab scripts can be generated using MeshLab: after loading a mesh, just apply your desired filters and then go to Filters -> Show current filter script. A dialog appears, allowing to see the list of filters applied with the given parameters, that can be also edited. At the end you can save your script that can be then used by MeshLab Server.

