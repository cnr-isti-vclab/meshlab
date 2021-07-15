// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2020 Jérémie Dumas <jeremie.dumas@ens-lyon.org>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_FILE_UTILS_H
#define IGL_FILE_UTILS_H

#include "igl_inline.h"

#include <streambuf>
#include <istream>
#include <string>
#include <vector>

namespace igl {

IGL_INLINE void read_file_binary(FILE *fp,
                                 std::vector<uint8_t> &fileBufferBytes);

struct file_memory_buffer : public std::streambuf {
  char *p_start{nullptr};
  char *p_end{nullptr};
  size_t size;

  file_memory_buffer(char const *first_elem, size_t size)
      : p_start(const_cast<char *>(first_elem)), p_end(p_start + size),
        size(size) {
    setg(p_start, p_start, p_end);
  }

  pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                   std::ios_base::openmode which) override {
    if (dir == std::ios_base::cur) {
      gbump(static_cast<int>(off));
    } else {
      setg(p_start, (dir == std::ios_base::beg ? p_start : p_end) + off, p_end);
    }
    return gptr() - p_start;
  }

  pos_type seekpos(pos_type pos, std::ios_base::openmode which) override {
    return seekoff(pos, std::ios_base::beg, which);
  }
};

struct file_memory_stream : virtual file_memory_buffer, public std::istream {
  file_memory_stream(char const *first_elem, size_t size)
      : file_memory_buffer(first_elem, size), std::istream(
                                                  static_cast<std::streambuf *>(
                                                      this)) {}
};

} // namespace igl

#ifndef IGL_STATIC_LIBRARY
#include "file_utils.cpp"
#endif

#endif
