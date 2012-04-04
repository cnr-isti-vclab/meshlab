#Please define the EXIF_DIR variable which locates the Exif directory in your system.
#eg. EXIF_DIR = ../code/lib/Exif

!contains(DEFINES, EXIF_DIR){
  DEFINES += EXIF_DIR
  
  INCLUDEPATH += $$EXIF_DIR/
  SOURCES += $$EXIF_DIR/exif.c
  SOURCES += $$EXIF_DIR/gpsinfo.c
  SOURCES += $$EXIF_DIR/iptc.c
  SOURCES += $$EXIF_DIR/jhead.c
  SOURCES += $$EXIF_DIR/jpgfile.c
  SOURCES += $$EXIF_DIR/makernote.c
  win32:SOURCES += $$EXIF_DIR/myglob.c
  SOURCES += $$EXIF_DIR/paths.c
}
