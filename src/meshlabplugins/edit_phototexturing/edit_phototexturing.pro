include (../../shared.pri)

HEADERS       = photoTexturing.h

SOURCES += 	edit_phototex_factory.cpp \
			photoTexturing.cpp \
			PhotoTexturingWidget.cpp \
			src/PhotoTexturer.cpp \
			src/UVFaceTexture.cpp \
			src/Camera.cpp \
			src/CameraCalibration.cpp \
			src/TextureMerger.cpp \
			src/Tsai/TsaiCameraCalibration.cpp \
			../../external/TSAI30B3/matrix/matrix.c \
			../../external/TSAI30B3//minpack/lmdif.c \ 
			../../external/TSAI30B3//minpack/dpmpar.c \
			../../external/TSAI30B3//minpack/enorm.c \
			../../external/TSAI30B3//minpack/fdjac2.c \
			../../external/TSAI30B3//minpack/lmpar.c \
			../../external/TSAI30B3//minpack/qrfac.c \
			../../external/TSAI30B3//minpack/qrsolv.c \
			../../external/TSAI30B3//cal_tran.c \
			../../external/TSAI30B3//cal_util.c \
			../../external/TSAI30B3//cal_main.c \
			../../external/TSAI30B3//cal_eval.c \ 
			src/SmartBlendTextureMerger.cpp \
			src/WinnerTakesAllTextureMerger.cpp \
			src/QuadTree/QuadTreeNode.cpp \
			$$VCGDIR/wrap/gui/trackball.cpp\
			$$VCGDIR/wrap/gui/trackmode.cpp \
			../../meshlab/stdpardialog.cpp 


HEADERS += 	edit_phototex_factory.h \
			src/PhotoTexturer.h \
			src/Camera.h \
			src/CameraCalibration.h\
			PhotoTexturingWidget.h \
			../../external/TSAI30B3/matrix/matrix.h \ 
			src/PhotoTexturer.h \
			../../external/TSAI30B3/cal_main.h \
			src/SmartBlendTextureMerger.h \
			src/WinnerTakesAllTextureMerger.h \
			src/QuadTree/QuadTreeNode.h \
			src/QuadTree/QuadTreeLeaf.h \
			$$VCGDIR/wrap/gl/trimesh.h \
			../../meshlab/meshlabeditdockwidget.h \ 
			../../meshlab/stdpardialog.h 
			
			
FORMS        += ui/photoTexturingWidget.ui
TARGET        = photo_texturing

QT           += opengl
QT           += xml
RESOURCES     = edit_phototexturing.qrc
