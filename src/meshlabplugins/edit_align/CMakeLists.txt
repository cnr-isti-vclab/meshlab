# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have newuoa
if(TARGET external-newuoa)

	set(SOURCES
		align/align_parameter.cpp
		align/AlignGlobal.cpp
		align/OccupancyGrid.cpp
		alignDialog.cpp
		AlignPairDialog.cpp
		AlignPairWidget.cpp
		edit_align_factory.cpp
		edit_align.cpp
		meshtree.cpp
		../../meshlab/rich_parameter_gui/richparameterwidgets.cpp
		../../meshlab/rich_parameter_gui/richparameterlistframe.cpp
		../../meshlab/rich_parameter_gui/richparameterlistdialog.cpp
		${VCGDIR}/wrap/gui/trackball.cpp
		${VCGDIR}/wrap/gui/trackmode.cpp
		${VCGDIR}/wrap/ply/plylib.cpp)

	set(HEADERS
		align/align_parameter.h
		align/AlignGlobal.h
		align/OccupancyGrid.h
		alignDialog.h
		AlignPairDialog.h
		AlignPairWidget.h
		edit_align_factory.h
		edit_align.h
		meshtree.h
		../../meshlab/rich_parameter_gui/richparameterwidgets.h
		../../meshlab/rich_parameter_gui/richparameterlistframe.h
		../../meshlab/rich_parameter_gui/richparameterlistdialog.h
		${VCGDIR}/wrap/gui/trackball.h
		${VCGDIR}/wrap/gui/trackmode.h
		${VCGDIR}/wrap/gl/trimesh.h)

	set(RESOURCES edit_align.qrc)

	set(UI alignDialog.ui)

	add_meshlab_plugin(edit_align ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

	target_link_libraries(edit_align PRIVATE external-newuoa OpenGL::GLU)
	if(OpenMP_CXX_FOUND)
		target_link_libraries(edit_align PRIVATE OpenMP::OpenMP_CXX)
	endif()

else()
	message(
		STATUS "Skipping edit_align - missing newuoa in external directory.")
endif()
