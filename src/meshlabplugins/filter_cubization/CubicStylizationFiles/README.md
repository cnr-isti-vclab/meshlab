# Cubic Stylization filter

Cubic stylization is a 3D stylization tool. Unlike image stylization (2D to 2D) and non-photorealistic rendering (3D to 2D), cubic stylization is a 3D to 3D stylization algorithm which takes a manifold triangle mesh as the input and outputs a cubified triangle mesh. 

This is a MeshLab filter based on "[Cubic Stylization](https://www.dgp.toronto.edu/projects/cubic-stylization/)" by [Hsueh-Ti Derek Liu](https://www.dgp.toronto.edu/~hsuehtil/) and [Alec Jacobson](https://www.cs.toronto.edu/~jacobson/). 

The folder `/src` contains the original C++ implementation of Cubic stylization performed by [Hsueh-Ti Derek Liu](https://www.dgp.toronto.edu/~hsuehtil/) and [Alec Jacobson](https://www.cs.toronto.edu/~jacobson/).
Their code is licensed under [MPL2](https://www.mozilla.org/en-US/MPL/2.0/).

## Bibtex
```
@article{Liu:CubicStyle:2019,
  title = {Cubic Stylization},
  author = {Hsueh-Ti Derek Liu and Alec Jacobson},
  year = {2019},
  journal = {ACM Transactions on Graphics}, 
}
```
