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


/*#define LIB3DS_CHUNK_DEBUG*/
/*#define LIB3DS_CHUNK_WARNING*/


/*!
 * Reads a 3d-Studio chunk header from a little endian file stream.
 *
 * \param c  The chunk to store the data.
 * \param io The file stream.
 *
 * \return   True on success, False otherwise.
 */
void
lib3ds_chunk_read(Lib3dsChunk *c, Lib3dsIo *io) {
    assert(c);
    assert(io);
    c->cur = lib3ds_io_tell(io);
    c->chunk = lib3ds_io_read_word(io);
    c->size = lib3ds_io_read_dword(io);
    c->end = c->cur + c->size;
    c->cur += 6;
    if (c->size < 6) {
        lib3ds_io_log(io, LIB3DS_LOG_ERROR, "Invalid chunk header.");
    }

}


void
lib3ds_chunk_read_start(Lib3dsChunk *c, uint16_t chunk, Lib3dsIo *io) {
    assert(c);
    assert(io);
    lib3ds_chunk_read(c, io);
    if ((chunk != 0) && (c->chunk != chunk)) {
        lib3ds_io_log(io, LIB3DS_LOG_ERROR, "Unexpected chunk found.");
    }
    ((Lib3dsIoImpl*)io->impl)->log_indent++;
}


void
lib3ds_chunk_read_tell(Lib3dsChunk *c, Lib3dsIo *io) {
    c->cur = lib3ds_io_tell(io);
}


uint16_t
lib3ds_chunk_read_next(Lib3dsChunk *c, Lib3dsIo *io) {
    Lib3dsChunk d;

    if (c->cur >= c->end) {
        assert(c->cur == c->end);
        return 0;
    }

    lib3ds_io_seek(io, (long)c->cur, LIB3DS_SEEK_SET);
    d.chunk = lib3ds_io_read_word(io);
    d.size = lib3ds_io_read_dword(io);
    c->cur += d.size;

    if (io->log_func) {
        lib3ds_io_log(io, LIB3DS_LOG_INFO, "%s (0x%X) size=%lu", lib3ds_chunk_name(d.chunk), d.chunk, d.size);
    }

    return d.chunk;
}


void
lib3ds_chunk_read_reset(Lib3dsChunk *c, Lib3dsIo *io) {
    lib3ds_io_seek(io, -6, LIB3DS_SEEK_CUR);
}


void
lib3ds_chunk_read_end(Lib3dsChunk *c, Lib3dsIo *io) {
    ((Lib3dsIoImpl*)io->impl)->log_indent--;
    lib3ds_io_seek(io, c->end, LIB3DS_SEEK_SET);
}


/*!
 * Writes a 3d-Studio chunk header into a little endian file stream.
 *
 * \param c  The chunk to be written.
 * \param io The file stream.
 *
 * \return   True on success, False otherwise.
 */
void
lib3ds_chunk_write(Lib3dsChunk *c, Lib3dsIo *io) {
    assert(c);
    lib3ds_io_write_word(io, c->chunk);
    lib3ds_io_write_dword(io, c->size);
}


void
lib3ds_chunk_write_start(Lib3dsChunk *c, Lib3dsIo *io) {
    assert(c);
    c->size = 0;
    c->cur = lib3ds_io_tell(io);
    lib3ds_io_write_word(io, c->chunk);
    lib3ds_io_write_dword(io, c->size);
}


void
lib3ds_chunk_write_end(Lib3dsChunk *c, Lib3dsIo *io) {
    assert(c);
    c->size = lib3ds_io_tell(io) - c->cur;
    lib3ds_io_seek(io, c->cur + 2, LIB3DS_SEEK_SET);
    lib3ds_io_write_dword(io, c->size);
    c->cur += c->size;
    lib3ds_io_seek(io, c->cur, LIB3DS_SEEK_SET);
}


void
lib3ds_chunk_unknown(uint16_t chunk, Lib3dsIo *io) {
    if (io->log_func) {
        lib3ds_io_log(io, LIB3DS_LOG_WARN, "Unknown Chunk: %s (0x%X)", lib3ds_chunk_name(chunk), chunk);
    }
}


