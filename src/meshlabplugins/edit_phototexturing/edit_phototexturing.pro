include (../../shared.pri)

HEADERS       = photoTexturing.h

SOURCES += 	edit_phototex_factory.cpp \
			photoTexturing.cpp \
			PhotoTexturingWidget.cpp \
			src/PhotoTexturer.cpp \
			src/UVFaceTexture.cpp \
			src/Camera.cpp \
			src/CameraCalibration.cpp \
			src/TextureFilter.cpp \
			src/TextureMerger.cpp \
			src/Tsai/TsaiCameraCalibration.cpp \
			src/Tsai/matrix/matrix.c \
			src/Tsai/minpack/lmdif.c \ 
			src/Tsai/minpack/dpmpar.c \
			src/Tsai/minpack/enorm.c \
			src/Tsai/minpack/fdjac2.c \
			src/Tsai/minpack/lmpar.c \
			src/Tsai/minpack/qrfac.c \
			src/Tsai/minpack/qrsolv.c \
			src/Tsai/cal_tran.c \
			src/Tsai/cal_util.c \
			src/Tsai/cal_main.c \
			src/Tsai/cal_eval.c \ 
			#src/Poly/PolyCameraCalibration.cpp \
			#src/Poly/Polynom3_444.cpp \
			src/QuadTree/QuadTreeNode.cpp \
			$$VCGDIR/wrap/gui/trackball.cpp\
			$$VCGDIR/wrap/gui/trackmode.cpp \
			$$GLEWDIR/src/glew.c \
			../../meshlab/filterparameter.cpp \
			../../meshlab/stdpardialog.cpp 


HEADERS += 	edit_phototex_factory.h \
			src/PhotoTexturer.h \
			src/Camera.h \
			src/CameraCalibration.h\
			PhotoTexturingWidget.h \
			src/Tsai/matrix/matrix.h \ 
			src/PhotoTexturer.h \
			src/Tsai/cal_main.h \
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
