# Tests {#tests}

Tests are currently under active development. We use the following tools in this project:
* [Travis](https://travis-ci.org/STORM-IRIT/OpenGR) : continuous integration for linux and MacOS
* [AppVeyor](https://ci.appveyor.com/project/nmellado/opengr/) : continuous integration for windows

## Continuous integration
Continuous Integration status:

[stsimg-linux]: https://api.travis-ci.org/STORM-IRIT/OpenGR.svg?branch=master
[stsimg-windw]: https://ci.appveyor.com/api/projects/status/wpilmlfk8obuod8b/branch/master?svg=true

| Linux  \& MacOS | Windows         |
| :----:          | :-----:         |
| ![stsimg-linux] | ![stsimg-windw] |


## Compiling and running tests
Tests are implemented with CTest. To run the tests, call:
```{bash}
make buildtests && make test
```


Tests currently available:
* `pair_extraction`: generate random point clouds in 2, 3 and 4D, and query the pair generation structure with various radius.
* `quad_extraction`: generate random point clouds in 3D and query the quad generation structure with various radius. Routines from 4PCS and Super4PCS are then compared. This test is still in development mode and would need some improvements.
* `matching`: test the whole Super4PCS pipeline by registering range maps from the standford repository. You need an internet connection to build this test, since the datasets are downloaded at this time. Here range maps are registered sequentially, and the test is passed only if the estimated transformation is close enough to the GT provided in the dataset.
