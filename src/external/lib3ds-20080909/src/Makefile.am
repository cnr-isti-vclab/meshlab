lib_LTLIBRARIES = lib3ds.la

lib3ds_la_LDFLAGS = \
  -version-info $(LIB3DS_MINOR_VERSION):$(LIB3DS_MICRO_VERSION):0 \
  -release $(LIB3DS_MAJOR_VERSION)

lib3ds_la_LIBADD = -lm

lib3ds_la_SOURCES = \
  lib3ds_impl.h \
  lib3ds_atmosphere.c \
  lib3ds_background.c \
  lib3ds_camera.c \
  lib3ds_chunk.c \
  lib3ds_chunktable.c \
  lib3ds_file.c \
  lib3ds_io.c \
  lib3ds_light.c \
  lib3ds_material.c \
  lib3ds_math.c \
  lib3ds_matrix.c \
  lib3ds_mesh.c \
  lib3ds_node.c \
  lib3ds_quat.c \
  lib3ds_shadow.c \
  lib3ds_track.c \
  lib3ds_util.c \
  lib3ds_vector.c \
  lib3ds_viewport.c

lib3dsdir=$(includedir)
lib3ds_HEADERS = \
  lib3ds.h

EXTRA_DIST = \
  lib3ds.vcproj \
  lib3ds.rc 
