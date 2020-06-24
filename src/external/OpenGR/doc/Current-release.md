# Release Notes {#currentrelease}
## V1.1.x
This release introduces new code structure allowing use as an actual library.

 - Build system
   - Super4PCS now generates a CMake package, so you just need to call `find_package(Super4PCS)` to work with the library.
   - Save binaries compiled by AppVeyor integration system as Artifacts.
 - Demos, scripts and assets
   - assets are now added to the install folder, including downloaded datasets.
   - demo scripts have been updated to work directly after installation. Windows batch scripts have been added.
 - Dependencies
   - Chealpix dependency is marked as deprecated: it slows down the approach and reduce it's robustness. Consequently, Chealpix is still built in CI but not tested.
 - Devel
   - Update include directive relatively to super4pcs root dir. (including fix by @xinkang when enabling timings).
   - Add silent mode in matcher to avoid unwanted logging on standard output. This is implemented by adding a visitor in the matcher main loop, and set it as a logger in the Super4pcs main app.
   - Change directories structure with separated library, tests and demos folders.


## V1.0.x-alpha
Pre-release introducing the new version of the Super4PCS library.
No much changes on the API, but many changes on the code internal structure, tests, etc.

Major changes:
 * Dependencies:
   * remove ANN dependency,
   * remove dependency to Chealpix. Can be controlled by the option `SUPER4PCS_USE_CHEALPIX`. This options will be available for some time for experiment purposes. During this time, Chealpix and Cfitsio files will still be shipped with Super4PCS source code.
   * OpenCV is required only to load meshes textures. This dependency can be removed by switching option `IO_USE_OPENCV` to `FALSE`,
 * remove duplicate code between 4PCS and Super4PCS,
 * clean code structure,
 * demo dataset + run script added.
 * CMake project can now be opened directly with Visual Studio 2017.

## V0.2.x-alpha
Pre-release including improved compilation system, supporting both windows and linux platforms.
Binaries are available for x64 Windows platforms (tested with windows 7 and 10, requires Microsoft Visual C++ Redistributable for Visual Studio 2015).

Major changes:
 * Fix segfault reported in issue #11
 * fix IO bug #9
 * add windows support #4. As a side effect, dependencies are now packaged with the library.
 * improved Cmake configuration: c++11 flags check, add option to set/unset timing recording

Compatibility:
 * add compatibility with OpenCV3.0 and greater
 * fix compilation errors with gcc 4.9.1

Minor and devel changes :
 * exploration is now stopped when LCP = 1
 * fix several minor bugs, causing instabilities and reduced performances
 * use functors in several place, to improve code reusability and prepare next milestones
 * removed several compilation warnings

## V0.1-alpha
Alpha version of the Super4PCS first release, introducing:

* compilation environment using CMake.
* unit-tests based on CTest: to process both sub-routines and the whole Super4PCS algorithm using an online dataset with ground truth (Standford).
* improved performances and bugfixes.

