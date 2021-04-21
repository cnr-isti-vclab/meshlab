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

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

struct Timer {
    using hrc = std::chrono::high_resolution_clock;

    hrc::time_point start;
    hrc::time_point last;

    Timer() : start(hrc::now()) { last = start; }

    double TimeElapsed() { last = hrc::now(); return std::chrono::duration<double>(last - start).count(); }

    double TimeSinceLastCheck() { auto t = last; last = hrc::now(); return std::chrono::duration<double>(last - t).count(); }

    void Reset() { start = last = hrc::now(); }

};

#endif // TIMER_H

