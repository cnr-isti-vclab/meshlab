/*
    Copyright (C) 1996-2008 by Jan Eric Kyprianidis <www.kyprianidis.com>
    All rights reserved.

    This program is free  software: you can redistribute it and/or modify 
    it under the terms of the GNU Lesser General Public License as published 
    by the Free Software Foundation, either version 2.1 of the License, or 
    (at your option) any later version.

    Thisprogram  is  distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
    GNU Lesser General Public License for more details.

    You should  have received a copy of the GNU Lesser General Public License
    along with  this program; If not, see <http://www.gnu.org/licenses/>. 
*/
#include "lib3ds_impl.h"


void
lib3ds_viewport_read(Lib3dsViewport *viewport, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;

    memset(viewport, 0, sizeof(*viewport));
    lib3ds_chunk_read_start(&c, 0, io);
    switch (c.chunk) {
        case CHK_VIEWPORT_LAYOUT: {
            int cur = 0;
            viewport->layout_style = lib3ds_io_read_word(io);
            viewport->layout_active = lib3ds_io_read_intw(io);
            lib3ds_io_read_intw(io);
            viewport->layout_swap = lib3ds_io_read_intw(io);
            lib3ds_io_read_intw(io);
            viewport->layout_swap_prior = lib3ds_io_read_intw(io);
            viewport->layout_swap_view = lib3ds_io_read_intw(io);
            lib3ds_chunk_read_tell(&c, io);
            while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
                switch (chunk) {
                    case CHK_VIEWPORT_SIZE: {
                        viewport->layout_position[0] = lib3ds_io_read_word(io);
                        viewport->layout_position[1] = lib3ds_io_read_word(io);
                        viewport->layout_size[0] = lib3ds_io_read_word(io);
                        viewport->layout_size[1] = lib3ds_io_read_word(io);
                        break;
                    }

                    case CHK_VIEWPORT_DATA_3: {
                        if (cur < LIB3DS_LAYOUT_MAX_VIEWS) {
                            lib3ds_io_read_intw(io);
                            viewport->layout_views[cur].axis_lock = lib3ds_io_read_word(io);
                            viewport->layout_views[cur].position[0] = lib3ds_io_read_intw(io);
                            viewport->layout_views[cur].position[1] = lib3ds_io_read_intw(io);
                            viewport->layout_views[cur].size[0] = lib3ds_io_read_intw(io);
                            viewport->layout_views[cur].size[1] = lib3ds_io_read_intw(io);
                            viewport->layout_views[cur].type = lib3ds_io_read_word(io);
                            viewport->layout_views[cur].zoom = lib3ds_io_read_float(io);
                            lib3ds_io_read_vector(io, viewport->layout_views[cur].center);
                            viewport->layout_views[cur].horiz_angle = lib3ds_io_read_float(io);
                            viewport->layout_views[cur].vert_angle = lib3ds_io_read_float(io);
                            lib3ds_io_read(io, viewport->layout_views[cur].camera, 11);
                            ++cur;
                        }
                        break;
                    }

                    case CHK_VIEWPORT_DATA:
                        /* 3DS R2 & R3 chunk unsupported */
                        break;

                    default:
                        lib3ds_chunk_unknown(chunk, io);
                }
            }
            break;
        }

        case CHK_DEFAULT_VIEW: {
            while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
                switch (chunk) {
                    case CHK_VIEW_TOP: {
                        viewport->default_type = LIB3DS_VIEW_TOP;
                        lib3ds_io_read_vector(io, viewport->default_position);
                        viewport->default_width = lib3ds_io_read_float(io);
                        break;
                    }

                    case CHK_VIEW_BOTTOM: {
                        viewport->default_type = LIB3DS_VIEW_BOTTOM;
                        lib3ds_io_read_vector(io, viewport->default_position);
                        viewport->default_width = lib3ds_io_read_float(io);
                        break;
                    }

                    case CHK_VIEW_LEFT: {
                        viewport->default_type = LIB3DS_VIEW_LEFT;
                        lib3ds_io_read_vector(io, viewport->default_position);
                        viewport->default_width = lib3ds_io_read_float(io);
                        break;
                    }

                    case CHK_VIEW_RIGHT: {
                        viewport->default_type = LIB3DS_VIEW_RIGHT;
                        lib3ds_io_read_vector(io, viewport->default_position);
                        viewport->default_width = lib3ds_io_read_float(io);
                        break;
                    }

                    case CHK_VIEW_FRONT: {
                        viewport->default_type = LIB3DS_VIEW_FRONT;
                        lib3ds_io_read_vector(io, viewport->default_position);
                        viewport->default_width = lib3ds_io_read_float(io);
                        break;
                    }

                    case CHK_VIEW_BACK: {
                        viewport->default_type = LIB3DS_VIEW_BACK;
                        lib3ds_io_read_vector(io, viewport->default_position);
                        viewport->default_width = lib3ds_io_read_float(io);
                        break;
                    }

                    case CHK_VIEW_USER: {
                        viewport->default_type = LIB3DS_VIEW_USER;
                        lib3ds_io_read_vector(io, viewport->default_position);
                        viewport->default_width = lib3ds_io_read_float(io);
                        viewport->default_horiz_angle = lib3ds_io_read_float(io);
                        viewport->default_vert_angle = lib3ds_io_read_float(io);
                        viewport->default_roll_angle = lib3ds_io_read_float(io);
                        break;
                    }

                    case CHK_VIEW_CAMERA: {
                        viewport->default_type = LIB3DS_VIEW_CAMERA;
                        lib3ds_io_read(io, viewport->default_camera, 11);
                        break;
                    }

                    default:
                        lib3ds_chunk_unknown(chunk, io);
                }
            }
            break;
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


void
lib3ds_viewport_write(Lib3dsViewport *viewport, Lib3dsIo *io) {
    if (viewport->layout_nviews) {
        Lib3dsChunk c;
        int i;

        c.chunk = CHK_VIEWPORT_LAYOUT;
        lib3ds_chunk_write_start(&c, io);

        lib3ds_io_write_word(io, viewport->layout_style);
        lib3ds_io_write_intw(io, viewport->layout_active);
        lib3ds_io_write_intw(io, 0);
        lib3ds_io_write_intw(io, viewport->layout_swap);
        lib3ds_io_write_intw(io, 0);
        lib3ds_io_write_intw(io, viewport->layout_swap_prior);
        lib3ds_io_write_intw(io, viewport->layout_swap_view);

        {
            Lib3dsChunk c;
            c.chunk = CHK_VIEWPORT_SIZE;
            c.size = 14;
            lib3ds_chunk_write(&c, io);
            lib3ds_io_write_intw(io, viewport->layout_position[0]);
            lib3ds_io_write_intw(io, viewport->layout_position[1]);
            lib3ds_io_write_intw(io, viewport->layout_size[0]);
            lib3ds_io_write_intw(io, viewport->layout_size[1]);
        }

        for (i = 0; i < viewport->layout_nviews; ++i) {
            Lib3dsChunk c;
            c.chunk = CHK_VIEWPORT_DATA_3;
            c.size = 55;
            lib3ds_chunk_write(&c, io);

            lib3ds_io_write_intw(io, 0);
            lib3ds_io_write_word(io, viewport->layout_views[i].axis_lock);
            lib3ds_io_write_intw(io, viewport->layout_views[i].position[0]);
            lib3ds_io_write_intw(io, viewport->layout_views[i].position[1]);
            lib3ds_io_write_intw(io, viewport->layout_views[i].size[0]);
            lib3ds_io_write_intw(io, viewport->layout_views[i].size[1]);
            lib3ds_io_write_word(io, viewport->layout_views[i].type);
            lib3ds_io_write_float(io, viewport->layout_views[i].zoom);
            lib3ds_io_write_vector(io, viewport->layout_views[i].center);
            lib3ds_io_write_float(io, viewport->layout_views[i].horiz_angle);
            lib3ds_io_write_float(io, viewport->layout_views[i].vert_angle);
            lib3ds_io_write(io, viewport->layout_views[i].camera, 11);
        }

        lib3ds_chunk_write_end(&c, io);
    }

    if (viewport->default_type) {
        Lib3dsChunk c;

        c.chunk = CHK_DEFAULT_VIEW;
        lib3ds_chunk_write_start(&c, io);

        switch (viewport->default_type) {
            case LIB3DS_VIEW_TOP: {
                Lib3dsChunk c;
                c.chunk = CHK_VIEW_TOP;
                c.size = 22;
                lib3ds_chunk_write(&c, io);
                lib3ds_io_write_vector(io, viewport->default_position);
                lib3ds_io_write_float(io, viewport->default_width);
                break;
            }

            case LIB3DS_VIEW_BOTTOM: {
                Lib3dsChunk c;
                c.chunk = CHK_VIEW_BOTTOM;
                c.size = 22;
                lib3ds_chunk_write(&c, io);
                lib3ds_io_write_vector(io, viewport->default_position);
                lib3ds_io_write_float(io, viewport->default_width);
                break;
            }

            case LIB3DS_VIEW_LEFT: {
                Lib3dsChunk c;
                c.chunk = CHK_VIEW_LEFT;
                c.size = 22;
                lib3ds_chunk_write(&c, io);
                lib3ds_io_write_vector(io, viewport->default_position);
                lib3ds_io_write_float(io, viewport->default_width);
                break;
            }

            case LIB3DS_VIEW_RIGHT: {
                Lib3dsChunk c;
                c.chunk = CHK_VIEW_RIGHT;
                c.size = 22;
                lib3ds_chunk_write(&c, io);
                lib3ds_io_write_vector(io, viewport->default_position);
                lib3ds_io_write_float(io, viewport->default_width);
                break;
            }

            case LIB3DS_VIEW_FRONT: {
                Lib3dsChunk c;
                c.chunk = CHK_VIEW_FRONT;
                c.size = 22;
                lib3ds_chunk_write(&c, io);
                lib3ds_io_write_vector(io, viewport->default_position);
                lib3ds_io_write_float(io, viewport->default_width);
                break;
            }

            case LIB3DS_VIEW_BACK: {
                Lib3dsChunk c;
                c.chunk = CHK_VIEW_BACK;
                c.size = 22;
                lib3ds_chunk_write(&c, io);
                lib3ds_io_write_vector(io, viewport->default_position);
                lib3ds_io_write_float(io, viewport->default_width);
                break;
            }

            case LIB3DS_VIEW_USER: {
                Lib3dsChunk c;
                c.chunk = CHK_VIEW_USER;
                c.size = 34;
                lib3ds_chunk_write(&c, io);
                lib3ds_io_write_vector(io, viewport->default_position);
                lib3ds_io_write_float(io, viewport->default_width);
                lib3ds_io_write_float(io, viewport->default_horiz_angle);
                lib3ds_io_write_float(io, viewport->default_vert_angle);
                lib3ds_io_write_float(io, viewport->default_roll_angle);
                break;
            }

            case LIB3DS_VIEW_CAMERA: {
                Lib3dsChunk c;
                c.chunk = CHK_VIEW_CAMERA;
                c.size = 17;
                lib3ds_chunk_write(&c, io);
                lib3ds_io_write(io, viewport->default_camera, 11);
                break;
            }
        }

        lib3ds_chunk_write_end(&c, io);
    }
}

