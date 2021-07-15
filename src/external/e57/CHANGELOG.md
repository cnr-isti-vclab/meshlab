# libE57Format

- v2.2.0 (in development)
  - Add and update the E57Simple API from the old reference library ([#41](https://github.com/asmaloney/libE57Format/pull/41))	(Thanks Jiri!)
  - Enable building E57Format as a shared library ([#40](https://github.com/asmaloney/libE57Format/pull/40)) (Thanks Amodio!)
  - Remove all usage of dynamic_cast<> ([#39](https://github.com/asmaloney/libE57Format/pull/39))	(Thanks Jiri!)
  - Added a [clang-format](https://clang.llvm.org/docs/ClangFormat.html) file, a cmake target for it ("format"), and reformatted the code
  - Fix building with E57_MAX_VERBOSE defined ([#44](https://github.com/asmaloney/libE57Format/pull/44))
  - `E57_V1_0_URI` was changed from a `#define` to a `constexpr`, so if you use it, it will need to be updated with a namespace: `e57::E57_V1_0_URI`
  - Split classes out from E57FormatImpl.[h,cpp]
  - {win} Fix MSVC warnings  ([#34](https://github.com/asmaloney/libE57Format/pull/34))) ([#36](https://github.com/asmaloney/libE57Format/pull/36))
  - {cmake} Review and update cmake files
  - {doc} Moved some documentation to new repo ([libE57Format-docs](https://github.com/asmaloney/libE57Format-docs)) and generate the [docs](https://asmaloney.github.io/libE57Format-docs/)
  - {doc} Add info about using [SPDX License Identifiers](https://spdx.org/ids)
  - {ci} Add GitHub Actions to build macOS, Linux, and Windows ([#35](https://github.com/asmaloney/libE57Format/pull/35))

- v2.1.0 (01 Apr 2020)
  - handle UTF8 file names on Windows (based on [#26](https://github.com/asmaloney/libE57Format/issues/26))
  - now supports _char*_ input ([#22](https://github.com/asmaloney/libE57Format/pull/22))
  - internal code cleanups
  - {cmake} remove unused ICU requirement for Linux
  - {cmake} mark xerces-c as required
  - {cmake} added fallback configuration for RelWithDebInfo and MinSizeRel [#29](https://github.com/asmaloney/libE57Format/pull/29)
  - {cmake} added proper install configuration [#28](https://github.com/asmaloney/libE57Format/pull/28)
  - {cmake} various cleanups

- v2.0.1 (15 Jan 2019)
  - writing files was broken and would produce the following error:
    Error: bad API function argument provided by user (E57_ERROR_BAD_API_ARGUMENT) (ImageFileImpl.cpp line 109)

- v2.0 (06 Jan 2019)
  - forked from E57RefImpl
  - removed all but the main sources for reading and writing E57 files
  - now requires C++11
  - now requires cmake 3.1+
  - no longer uses BOOST
  - multiple fixes for compilation on macOS
  - fix a couple of fallthrough bugs which would result in undefined behaviour
  - turn off `E57_MAX_DEBUG` by default
  - improve file read times
  - add a checksum policy (see _ReadChecksumPolicy_ in *E57Format.h*) so the library user can decide how frequently to check them
  - remove "big endian" byte swap code (not sure it was working and no way to test)
  - lots and lots of code cleanups
    - refactored the code into multiple files
    - removed unused macros and code
    - remove non-useful comments
    - add proper initialization of class and struct members
    - modernize using c++11
  - [Windows] add cmake option ()`USING_STATIC_XERCES`) to tell the build if you are using a static Xerces lib

# E57RefImpl

- 2013-04-03 roland_schwarz
  - e57unpack now also can handle 2d images
- 2013-01-30 roland_schwarz
  - added missing library identifier string to cmake build
- 2011-10-04 roland_schwarz
  - update to use boost version 1.46.1
  - streamlined cmake files for better static linking
- 2011-03-14 roland_schwarz
  - E57RefImplConfig.cmake added
  - e57validate tool added to cmake
- 2011-03-10: roland_schwarz
  - Added E57RefImplConfig.cmake for user project configuration.
  - The build instructions can be found inside the CMakeLists.txt file.
- 2010-10-16 roland_schwarz
  - e57unpack tool added
  - riegl_1 example files added
