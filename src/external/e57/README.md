# libE57Format

[![Build Status](https://travis-ci.org/asmaloney/libE57Format.svg?branch=master)](https://travis-ci.org/asmaloney/libE57Format)

A library to provide read & write support for the E57 file format.

This is a fork of [E57RefImpl](https://sourceforge.net/projects/e57-3d-imgfmt/) v1.1.332. It is primarily for use in the [CloudCompare](https://github.com/CloudCompare/CloudCompare) project, but it should work for anyone else who wants to use it.

The original source is from [E57RefImpl 1.1.332](https://sourceforge.net/projects/e57-3d-imgfmt/files/E57Refimpl-src/) and then everything was stripped out except the main implementation for reading and writing E57.

This version also removes the dependency on [Boost](http://www.boost.org/) and requires C++11.

Many, many other changes were made prior to the first release of this fork. See the [CHANGELOG](CHANGELOG.md) and git history for details.

## Why Fork?

The E57RefImpl code hasn't been touched in years and I wanted to make changes to compile this library with macOS. Forking it gives me a bit more freedom to update the code and make changes as required.

I changed the name of the project so that it is not confused with the **E57RefImpl** project.

I have also changed the main include file's name from `E57Foundation.h` to `E57Format.h` to make sure there is no inclusion confusion.

Versions of **libE57Format** start at 2.0.

## Documentation

The doxygen-generated documentation may be [found here](https://asmaloney.github.io/libE57Format-docs/). These docs are generated and saved in the [libE57Format-docs](https://github.com/asmaloney/libE57Format-docs) repo.

## E57Simple API

Since the original fork, [Jiri Hörner](https://github.com/ptc-jhoerner) has added the E57Simple API from the old reference implementation and updated it.

## Tools

[Ryan Baumann](https://github.com/ryanfb) has updated the `e57unpack` and `e57validate` tools to work with **libE57Format**. You can find them in the [e57tools](https://github.com/ryanfb/e57tools) repo.

## Contributing

I am happy to review any contributions - [pull requests](https://github.com/asmaloney/libE57Format/pulls), [documentation](https://github.com/asmaloney/libE57Format-docs), etc.. Please keep any pull requests as short as possible. Each pull request should be atomic and only address one issue.

This project uses [clang-format](https://clang.llvm.org/docs/ClangFormat.html) to format the code. There is a cmake target (*format*) - which runs *clang-format* on the source files. After changes have been made, and before you submit your pull request, please run the following:

```sh
cmake --build . --target format
```

## License

This project as a whole is licensed under the [**BSL-1.0**](https://opensource.org/licenses/BSL-1.0) license - see the [LICENSE](LICENSE.md) file for details.

Individual source files may contain the following tag instead of the full license text:

	SPDX-License-Identifier: BSL-1.0

Some CMake files are licensed under the **MIT** license - see the [LICENSE-MIT](LICENSE-MIT.txt) file for details.

These files contain the following tag instead of the full license text:

	SPDX-License-Identifier: MIT

Using SPDX enables machine processing of license information based on the [SPDX License Identifiers](https://spdx.org/ids) and makes it easier for developers to see at a glance which license they are dealing with.
