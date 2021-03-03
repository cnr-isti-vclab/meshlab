/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "ioraster_base.h"

#include <QFileInfo>
#include "exif.h"



IORasterBasePlugin::IORasterBasePlugin() :
	IORasterPlugin(),
	formatList({FileFormat("JPEG", tr("JPEG")),
				FileFormat("JPG", tr("JPG")),
				FileFormat("PNG", tr("PNG")),
				FileFormat("XPM", tr("XPM"))})
{
}

QString IORasterBasePlugin::pluginName() const
{
	return "IORasterBase";
}

QList<FileFormat> IORasterBasePlugin::importFormats() const
{
	return formatList;
}

bool IORasterBasePlugin::open(
		const QString& format,
		const QString& filename,
		RasterModel& rm,
		vcg::CallBackPos*)
{
	bool supportedFormat = false;
	for (const FileFormat& f : formatList){
		if (f.extensions.first().toUpper() == format.toUpper())
			supportedFormat = true;
	}
	
	if (supportedFormat) {
		QFileInfo fi(filename);
		
		if(!fi.exists()) 	{
			QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 does not exist.";
			errorMessage = errorMsgFormat.arg(filename);
			return false;
		}
		if(!fi.isReadable()) 	{
			QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 is not readable.";
			
			return false;
		}
		
		rm.setLabel(filename);
		rm.addPlane(new RasterPlane(filename,RasterPlane::RGBA));
	
		// Read the JPEG file into a buffer
		FILE *fp = fopen(qUtf8Printable(filename), "rb");
		if (!fp) {
			QString errorMsgFormat = "Exif Parsing: Unable to open file:\n\"%1\"\n\nError details: file %1 is not readable.";
			errorMessage = errorMsgFormat.arg(filename);
			return false;
		}
		fseek(fp, 0, SEEK_END);
		unsigned long fsize = ftell(fp);
		rewind(fp);
		unsigned char *buf = new unsigned char[fsize];
		if (fread(buf, 1, fsize, fp) != fsize) {
			QString errorMsgFormat = "Exif Parsing: Unable to read the content of the opened file:\n\"%1\"\n\nError details: file %1 is not readable.";
			errorMessage = errorMsgFormat.arg(filename);
			delete[] buf;
			fclose(fp);
			return false;
		}
		fclose(fp);
	
		// Parse EXIF
		easyexif::EXIFInfo ImageInfo;
		int code = ImageInfo.parseFrom(buf, fsize);
		delete[] buf;
		if (!code) {
			log(GLLogStream::FILTER, "Warning unable to parse exif for file  %s", qPrintable(filename));
		}
	
		if (code && ImageInfo.FocalLengthIn35mm==0.0f)
		{
			rm.shot.Intrinsics.ViewportPx = vcg::Point2i(rm.currentPlane->image.width(), rm.currentPlane->image.height());
			rm.shot.Intrinsics.CenterPx   = Point2m(float(rm.currentPlane->image.width()/2.0), float(rm.currentPlane->image.width()/2.0));
			rm.shot.Intrinsics.PixelSizeMm[0]=36.0f/(float)rm.currentPlane->image.width();
			rm.shot.Intrinsics.PixelSizeMm[1]=rm.shot.Intrinsics.PixelSizeMm[0];
			rm.shot.Intrinsics.FocalMm = 50.0f;
		}
		else
		{
			rm.shot.Intrinsics.ViewportPx = vcg::Point2i(ImageInfo.ImageWidth, ImageInfo.ImageHeight);
			rm.shot.Intrinsics.CenterPx   = Point2m(float(ImageInfo.ImageWidth/2.0), float(ImageInfo.ImageHeight/2.0));
			float ratioFocal=ImageInfo.FocalLength/ImageInfo.FocalLengthIn35mm;
			rm.shot.Intrinsics.PixelSizeMm[0]=(36.0f*ratioFocal)/(float)ImageInfo.ImageWidth;
			rm.shot.Intrinsics.PixelSizeMm[1]=(24.0f*ratioFocal)/(float)ImageInfo.ImageHeight;
			rm.shot.Intrinsics.FocalMm = ImageInfo.FocalLength;
		}
		// End of EXIF reading
		return true;
	}
	else {
		assert(0); //Unknown format
		return false;
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(IORasterBasePlugin)
