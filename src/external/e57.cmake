# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, 2021 Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_LIBE57_FORMAT "Allow use of bundled libE57Format source" ON)
option(ALLOW_SYSTEM_LIBE57_FORMAT "Allow use of system-provided libE57Format" ON)

set(E57Format_DIR ${EXTERNAL_DIR}/e57)

if(ALLOW_SYSTEM_LIBE57_FORMAT AND TARGET E57Format::E57Format)

    message(STATUS "- libE57Format - using system-provided library")
    add_library(external-libE57Format INTERFACE)
    target_link_libraries(external-libE57Format INTERFACE E57Format::E57Format)

elseif(ALLOW_BUNDLED_LIBE57_FORMAT)

    message(STATUS "- libE57Format - using bundled source")

    add_library(
            external-libE57Format STATIC
            ${E57Format_DIR}/src/BlobNodeImpl.h
            ${E57Format_DIR}/src/BlobNodeImpl.cpp
            ${E57Format_DIR}/src/CheckedFile.h
            ${E57Format_DIR}/src/CheckedFile.cpp
            ${E57Format_DIR}/src/Common.h
            ${E57Format_DIR}/src/Common.cpp
            ${E57Format_DIR}/src/CompressedVectorNodeImpl.h
            ${E57Format_DIR}/src/CompressedVectorNodeImpl.cpp
            ${E57Format_DIR}/src/CompressedVectorReaderImpl.h
            ${E57Format_DIR}/src/CompressedVectorReaderImpl.cpp
            ${E57Format_DIR}/src/CompressedVectorWriterImpl.h
            ${E57Format_DIR}/src/CompressedVectorWriterImpl.cpp
            ${E57Format_DIR}/src/DecodeChannel.h
            ${E57Format_DIR}/src/DecodeChannel.cpp
            ${E57Format_DIR}/src/Decoder.h
            ${E57Format_DIR}/src/Decoder.cpp
            ${E57Format_DIR}/src/Encoder.h
            ${E57Format_DIR}/src/Encoder.cpp
            ${E57Format_DIR}/src/FloatNodeImpl.h
            ${E57Format_DIR}/src/FloatNodeImpl.cpp
            ${E57Format_DIR}/src/IntegerNodeImpl.h
            ${E57Format_DIR}/src/IntegerNodeImpl.cpp
            ${E57Format_DIR}/src/NodeImpl.h
            ${E57Format_DIR}/src/NodeImpl.cpp
            ${E57Format_DIR}/src/Packet.h
            ${E57Format_DIR}/src/Packet.cpp
            ${E57Format_DIR}/src/ImageFileImpl.cpp
            ${E57Format_DIR}/src/ImageFileImpl.h
            ${E57Format_DIR}/src/ReaderImpl.cpp
            ${E57Format_DIR}/src/ReaderImpl.h
            ${E57Format_DIR}/src/ScaledIntegerNodeImpl.h
            ${E57Format_DIR}/src/ScaledIntegerNodeImpl.cpp
            ${E57Format_DIR}/src/SectionHeaders.h
            ${E57Format_DIR}/src/SectionHeaders.cpp
            ${E57Format_DIR}/src/SourceDestBufferImpl.h
            ${E57Format_DIR}/src/SourceDestBufferImpl.cpp
            ${E57Format_DIR}/src/StringNodeImpl.h
            ${E57Format_DIR}/src/StringNodeImpl.cpp
            ${E57Format_DIR}/src/StructureNodeImpl.h
            ${E57Format_DIR}/src/StructureNodeImpl.cpp
            ${E57Format_DIR}/src/VectorNodeImpl.h
            ${E57Format_DIR}/src/VectorNodeImpl.cpp
            ${E57Format_DIR}/src/WriterImpl.cpp
            ${E57Format_DIR}/src/WriterImpl.h
            ${E57Format_DIR}/src/E57Exception.cpp
            ${E57Format_DIR}/src/E57Format.cpp
            ${E57Format_DIR}/src/E57SimpleData.cpp
            ${E57Format_DIR}/src/E57SimpleReader.cpp
            ${E57Format_DIR}/src/E57SimpleWriter.cpp
            ${E57Format_DIR}/src/E57Version.h
            ${E57Format_DIR}/src/E57XmlParser.cpp
            ${E57Format_DIR}/src/E57XmlParser.h
    )
    target_include_directories(external-libE57Format SYSTEM PUBLIC "${E57Format_DIR}/include")
    target_compile_definitions(external-libE57Format PUBLIC LIBE57_FORMAT_STATIC)
    set_property(TARGET external-libE57Format PROPERTY FOLDER External)
    target_link_libraries(external-libE57Format PRIVATE external-disable-warnings)
endif()
