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


typedef union {
    uint32_t dword_value;
    float float_value;
} Lib3dsDwordFloat;


void
lib3ds_io_setup(Lib3dsIo *io) {
    assert(io);
    io->impl = calloc(sizeof(Lib3dsIoImpl), 1);
}


void
lib3ds_io_cleanup(Lib3dsIo *io) {
    Lib3dsIoImpl *impl;
    assert(io);
    impl = (Lib3dsIoImpl*)io->impl;
    if (impl->tmp_mem) {
        free(impl->tmp_mem);
        impl->tmp_mem = NULL;
    }
    if (impl->tmp_node) {
        lib3ds_node_free(impl->tmp_node);
        impl->tmp_node = NULL;
    }
    free(impl);
}


long
lib3ds_io_seek(Lib3dsIo *io, long offset, Lib3dsIoSeek origin) {
    assert(io);
    if (!io || !io->seek_func) {
        return 0;
    }
    return (*io->seek_func)(io->self, offset, origin);
}


long
lib3ds_io_tell(Lib3dsIo *io) {
    assert(io);
    if (!io || !io->tell_func) {
        return 0;
    }
    return (*io->tell_func)(io->self);
}


size_t
lib3ds_io_read(Lib3dsIo *io, void *buffer, size_t size) {
    assert(io);
    if (!io || !io->read_func) {
        return 0;
    }
    return (*io->read_func)(io->self, buffer, size);
}


size_t
lib3ds_io_write(Lib3dsIo *io, const void *buffer, size_t size) {
    assert(io);
    if (!io || !io->write_func) {
        return 0;
    }
    return (*io->write_func)(io->self, buffer, size);
}


static void 
lib3ds_io_log_str(Lib3dsIo *io, Lib3dsLogLevel level, const char *str) {
    if (!io || !io->log_func)
        return;
    (*io->log_func)(io->self, level, ((Lib3dsIoImpl*)io->impl)->log_indent, str);
}


void 
lib3ds_io_log(Lib3dsIo *io, Lib3dsLogLevel level, const char *format, ...) {
    va_list args;
    /* FIXME */ char str[1024];

    assert(io);
    if (!io || !io->log_func)
        return;

    va_start(args, format);
    /* FIXME: */ vsprintf(str, format, args); 
    lib3ds_io_log_str(io, level, str);

    if (level == LIB3DS_LOG_ERROR) {
        longjmp(((Lib3dsIoImpl*)io->impl)->jmpbuf, 1);
    }
}


void 
lib3ds_io_log_indent(Lib3dsIo *io, int indent) {
    assert(io);
    if (!io)
        return;
    ((Lib3dsIoImpl*)io->impl)->log_indent += indent;
}


void 
lib3ds_io_read_error(Lib3dsIo *io) {
    lib3ds_io_log(io, LIB3DS_LOG_ERROR, "Reading from input stream failed.");
}


void 
lib3ds_io_write_error(Lib3dsIo *io) {
    lib3ds_io_log(io, LIB3DS_LOG_ERROR, "Writing to output stream failed.");
}


/*!
 * Read a byte from a file stream.
 */
uint8_t
lib3ds_io_read_byte(Lib3dsIo *io) {
    uint8_t b;

    assert(io);
    lib3ds_io_read(io, &b, 1);
    return(b);
}


/**
 * Read a word from a file stream in little endian format.
 */
uint16_t
lib3ds_io_read_word(Lib3dsIo *io) {
    uint8_t b[2];
    uint16_t w;

    assert(io);
    lib3ds_io_read(io, b, 2);
    w = ((uint16_t)b[1] << 8) |
        ((uint16_t)b[0]);
    return(w);
}


/*!
 * Read a dword from file a stream in little endian format.
 */
uint32_t
lib3ds_io_read_dword(Lib3dsIo *io) {
    uint8_t b[4];
    uint32_t d;

    assert(io);
    lib3ds_io_read(io, b, 4);
    d = ((uint32_t)b[3] << 24) |
        ((uint32_t)b[2] << 16) |
        ((uint32_t)b[1] << 8) |
        ((uint32_t)b[0]);
    return(d);
}


/*!
 * Read a signed byte from a file stream.
 */
int8_t
lib3ds_io_read_intb(Lib3dsIo *io) {
    int8_t b;

    assert(io);
    lib3ds_io_read(io, &b, 1);
    return(b);
}


/*!
 * Read a signed word from a file stream in little endian format.
 */
int16_t
lib3ds_io_read_intw(Lib3dsIo *io) {
    uint8_t b[2];
    uint16_t w;

    assert(io);
    lib3ds_io_read(io, b, 2);
    w = ((uint16_t)b[1] << 8) |
        ((uint16_t)b[0]);
    return((int16_t)w);
}


/*!
 * Read a signed dword a from file stream in little endian format.
 */
int32_t
lib3ds_io_read_intd(Lib3dsIo *io) {
    uint8_t b[4];
    uint32_t d;

    assert(io);
    lib3ds_io_read(io, b, 4);
    d = ((uint32_t)b[3] << 24) |
        ((uint32_t)b[2] << 16) |
        ((uint32_t)b[1] << 8) |
        ((uint32_t)b[0]);
    return((int32_t)d);
}


/*!
 * Read a float from a file stream in little endian format.
 */
float
lib3ds_io_read_float(Lib3dsIo *io) {
    uint8_t b[4];
    Lib3dsDwordFloat d;

    assert(io);
    lib3ds_io_read(io, b, 4);
    d.dword_value = ((uint32_t)b[3] << 24) |
                    ((uint32_t)b[2] << 16) |
                    ((uint32_t)b[1] << 8) |
                    ((uint32_t)b[0]);
    return d.float_value;
}


/*!
 * Read a vector from a file stream in little endian format.
 *
 * \param io IO input handle.
 * \param v  The vector to store the data.
 */
void
lib3ds_io_read_vector(Lib3dsIo *io, float v[3]) {
    assert(io);
    v[0] = lib3ds_io_read_float(io);
    v[1] = lib3ds_io_read_float(io);
    v[2] = lib3ds_io_read_float(io);
}


void
lib3ds_io_read_rgb(Lib3dsIo *io, float rgb[3]) {
    assert(io);
    rgb[0] = lib3ds_io_read_float(io);
    rgb[1] = lib3ds_io_read_float(io);
    rgb[2] = lib3ds_io_read_float(io);
}


/*!
 * Read a zero-terminated string from a file stream.
 *
 * \param io      IO input handle.
 * \param s       The buffer to store the read string.
 * \param buflen  Buffer length.
 *
 * \return        True on success, False otherwise.
 */
void
lib3ds_io_read_string(Lib3dsIo *io, char *s, int buflen) {
    char c;
    int k = 0;

    assert(io);
    for (;;) {
        if (lib3ds_io_read(io, &c, 1) != 1) {
            lib3ds_io_read_error(io);
        }
        *s++ = c;
        if (!c) {
            break;
        }
        ++k;
        if (k >= buflen) {
            lib3ds_io_log(io, LIB3DS_LOG_ERROR, "Invalid string in input stream.");
        }
    }
}


/*!
 * Writes a byte into a file stream.
 */
void
lib3ds_io_write_byte(Lib3dsIo *io, uint8_t b) {
    assert(io);
    if (lib3ds_io_write(io, &b, 1) != 1) {
        lib3ds_io_write_error(io);
    }
}


/*!
 * Writes a word into a little endian file stream.
 */
void
lib3ds_io_write_word(Lib3dsIo *io, uint16_t w) {
    uint8_t b[2];

    assert(io);
    b[1] = ((uint16_t)w & 0xFF00) >> 8;
    b[0] = ((uint16_t)w & 0x00FF);
    if (lib3ds_io_write(io, b, 2) != 2) {
        lib3ds_io_write_error(io);
    }
}


/*!
 * Writes a dword into a little endian file stream.
 */
void
lib3ds_io_write_dword(Lib3dsIo *io, uint32_t d) {
    uint8_t b[4];

    assert(io);
    b[3] = (uint8_t)(((uint32_t)d & 0xFF000000) >> 24);
    b[2] = (uint8_t)(((uint32_t)d & 0x00FF0000) >> 16);
    b[1] = (uint8_t)(((uint32_t)d & 0x0000FF00) >> 8);
    b[0] = (uint8_t)(((uint32_t)d & 0x000000FF));
    if (lib3ds_io_write(io, b, 4) != 4) {
        lib3ds_io_write_error(io);
    }
}


/*!
 * Writes a signed byte in a file stream.
 */
void
lib3ds_io_write_intb(Lib3dsIo *io, int8_t b) {
    assert(io);
    if (lib3ds_io_write(io, &b, 1) != 1) {
        lib3ds_io_write_error(io);
    }
}


/*!
 * Writes a signed word into a little endian file stream.
 */
void
lib3ds_io_write_intw(Lib3dsIo *io, int16_t w) {
    uint8_t b[2];

    assert(io);
    b[1] = ((uint16_t)w & 0xFF00) >> 8;
    b[0] = ((uint16_t)w & 0x00FF);
    if (lib3ds_io_write(io, b, 2) != 2) {
        lib3ds_io_write_error(io);
    }
}


/*!
 * Writes a signed dword into a little endian file stream.
 */
void
lib3ds_io_write_intd(Lib3dsIo *io, int32_t d) {
    uint8_t b[4];

    assert(io);
    b[3] = (uint8_t)(((uint32_t)d & 0xFF000000) >> 24);
    b[2] = (uint8_t)(((uint32_t)d & 0x00FF0000) >> 16);
    b[1] = (uint8_t)(((uint32_t)d & 0x0000FF00) >> 8);
    b[0] = (uint8_t)(((uint32_t)d & 0x000000FF));
    if (lib3ds_io_write(io, b, 4) != 4) {
        lib3ds_io_write_error(io);
    }
}


/*!
 * Writes a float into a little endian file stream.
 */
void
lib3ds_io_write_float(Lib3dsIo *io, float l) {
    uint8_t b[4];
    Lib3dsDwordFloat d;

    assert(io);
    d.float_value = l;
    b[3] = (uint8_t)(((uint32_t)d.dword_value & 0xFF000000) >> 24);
    b[2] = (uint8_t)(((uint32_t)d.dword_value & 0x00FF0000) >> 16);
    b[1] = (uint8_t)(((uint32_t)d.dword_value & 0x0000FF00) >> 8);
    b[0] = (uint8_t)(((uint32_t)d.dword_value & 0x000000FF));
    if (lib3ds_io_write(io, b, 4) != 4) {
        lib3ds_io_write_error(io);
    }
}


/*!
 * Writes a vector into a file stream in little endian format.
 */
void
lib3ds_io_write_vector(Lib3dsIo *io, float v[3]) {
    int i;
    for (i = 0; i < 3; ++i) {
        lib3ds_io_write_float(io, v[i]);
    }
}


void
lib3ds_io_write_rgb(Lib3dsIo *io, float rgb[3]) {
    int i;
    for (i = 0; i < 3; ++i) {
        lib3ds_io_write_float(io, rgb[i]);
    }
}


/*!
 * Writes a zero-terminated string into a file stream.
 */
void
lib3ds_io_write_string(Lib3dsIo *io, const char *s) {
    size_t len;
    assert(io && s);
    len = strlen(s);
    if (lib3ds_io_write(io, s, len + 1) != len +1) {
        lib3ds_io_write_error(io);
    }
}
