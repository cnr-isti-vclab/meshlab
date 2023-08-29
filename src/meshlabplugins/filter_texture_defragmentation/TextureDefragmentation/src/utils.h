/*******************************************************************************
    Copyright (c) 2021, Andrea Maggiordomo, Paolo Cignoni and Marco Tarini

    This file is part of TextureDefrag, a reference implementation for
    the paper ``Texture Defragmentation for Photo-Reconstructed 3D Models''
    by Andrea Maggiordomo, Paolo Cignoni and Marco Tarini.

    TextureDefrag is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TextureDefrag is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TextureDefrag. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <iostream>

#define ensure(expr) \
    ((expr) \
     ? (void) (0) \
     : ensure_fail(#expr, __FILE__, __LINE__))

[[ noreturn ]]
inline void ensure_fail(const char *expr, const char *filename, unsigned int line)
{
    std::cerr << filename << " (line " << line << "): Failed check `" << expr << "'" << std::endl;
    std::abort();
}

#endif // UTILS_H

