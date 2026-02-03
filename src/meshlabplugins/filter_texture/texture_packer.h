/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#ifndef _TEXTURE_PACKER_H
#define _TEXTURE_PACKER_H
#include "vcg/space/rect_packer.h"

using namespace vcg;


/**
 * This class defines entities responsible for packing a set of texture images.
 *
 * It is mainly used in the implementation of the filter 'Merge Textures'.
 */
class TexturePacker {

public:
	typedef std::reference_wrapper <const QImage> ReferenceImage;
	typedef std::vector <ReferenceImage> SrcTextures;
	typedef std::vector <QImage> DstTextures;
	typedef std::vector <int> IdVector;

private:
	/**
	 * Data class containing all data needed for copying a
	 * source texture image into the associated destination.
	 */
	class packInfo {
	public:
		ReferenceImage srcReference;	// reference to the original image.

		int dstIdx;		// Index of the destination texture in the associated destination array.

		QPoint dstOff;	// Pixel coordinates in the destination texture
						// where the upper-left of the image resides.

		QSize srcSize;	// Size of the image

		packInfo (const ReferenceImage srcReference, const int dstIdx, const QSize srcSize)
			: srcReference (srcReference), dstIdx(dstIdx), dstOff(0, 0), srcSize(srcSize) { }
	};

	std::vector<packInfo> srcToDestinationMap;	// Each entry stores the information necessary to pack a
												// distinct source texture into its associated destination.

	std::vector<QSize> dstSizes;	// Each entry contains the size of the resulting merged texture.

	/**
	 * Computes the minimal size necessary to merge all requested images.
	 * If possible, it tries to return a square. If the process fails, the returned size will be (0, 0).
	 *
	 * As a side effect, updates the 'dstOff' offset of all entries such that each of them
	 * knows where to start copying the original source image into the associated destination.
	 *
	 * The algorithm depends on the VCGLib function 'PackInt'.
	 *
	 * @param srcIds the set of images to be packed, referred by their indices in the vector 'srcToDestinationMap'.
	 * @return the minimal size (in pixel) necessary to merge all images.
	 */
	QSize computePackSize (const IdVector &srcIds) {
		if (srcIds.empty())	return {0, 0};

		std::vector<Point2i> sizes;
		long long totalArea = 0;
		int maxWidth = 0;
		int maxHeight = 0;
		for (const auto &srcId : srcIds) {
			const auto src = srcToDestinationMap [srcId];
			const auto width = src.srcSize.width ();
			const auto height = src.srcSize.height ();
			const auto size = Point2i (width, height);
			sizes.push_back(size);

			totalArea += static_cast<long long> (width * height);
			maxWidth = std::max (maxWidth, width);
			maxHeight = std::max (maxHeight, height);
		}

		// We use as a heuristic the maximum possible square capable of packing all source textures,
		// computed as the side of the square having as area the sum of all the original images' areas.
		//
		// If for some reason there exists a texture with a bigger size, we pick that instead.
		int heuristicSide = static_cast <int> (std::sqrt (totalArea) );
		heuristicSide = std::max ({heuristicSide, maxWidth, maxHeight});
		const auto maximumSquare = Point2i (heuristicSide, heuristicSide);

		std::vector<Point2i> positions;
		Point2i globalSize;
		bool success = RectPacker <float>::PackInt (sizes, maximumSquare, positions, globalSize);
		if (!success) {
			// If the algorithm was not able to find a solution, we try again with
			// a square having as side the length of the longest square, multiplied
			// by the number of entries.
			const int safeSide = static_cast <int> (std::max (maxWidth, maxHeight) * srcIds.size ());
			const auto safeSquare = Point2i (safeSide, safeSide);
			success = RectPacker <float>::PackInt (sizes, safeSquare, positions, globalSize);
		}

		if (success) {
			for (size_t i = 0; i < srcIds.size(); ++i) {
				const auto srcId = srcIds [i];
				srcToDestinationMap [srcId].dstOff = QPoint (positions[i].X (), positions[i].Y ());
			}
			int finalSide = std::max (globalSize.X (), globalSize.Y ());
			return {finalSide, finalSide};
		}
		return {0, 0};
	}

public:

	/**
	 * Creates an instance mapping from the source space into a destination space having exactly
	 * 'dstCount' target images.
	 *
	 * @param srcTexts: the set of textures we want to compact.
	 * @param dstCount: the number of final target images which will pack the source space.
	 */
	TexturePacker (const SrcTextures &srcTexts, const int dstCount) {
		if (srcTexts.empty() || dstCount <= 0) return;

		const size_t texturesPerDst = (srcTexts.size() + dstCount - 1) / dstCount;
		int currentId = 0;

		IdVector commonDst;					// Store source ids together by the destination id.
		std::vector <IdVector> packedSrc;	// Used for computing the target sizes and offsets.
		for (size_t i = 0; i < srcTexts.size(); ++i) {
			const QImage &src = srcTexts [i].get ();

			auto dstId = currentId;
			auto srcSize = src.size ();
			srcToDestinationMap.emplace_back (srcTexts[i], dstId, srcSize);
			commonDst.push_back ( static_cast<int> (i) );

			// Check if we should move to the next destination texture
			if ((i + 1) % texturesPerDst == 0  && (currentId + 1) < dstCount) {
				currentId++;
				packedSrc.push_back (commonDst);
				commonDst = IdVector ();
			}
		}
		packedSrc.push_back (commonDst);	// Add the last computed set of common original ids.

		// Compute the target sizes and offsets.
		for (auto &srcsToPack : packedSrc) {
			dstSizes.push_back (computePackSize (srcsToPack));
		}
	}

	/**
	 * Returns the set of merged images obtained by packing the source
	 * textures according to the mapping defined by the instance.
	 */
	DstTextures packTextures () const {
		// We are assuming that all source textures share the same format.
		const auto dstFormat = srcToDestinationMap [0].srcReference.get ().format ();

		DstTextures dstTexts;
		for (const QSize &size : dstSizes) {
			QImage img(size, dstFormat);
			img.fill(0);
			dstTexts.push_back(img);
		}

		for (const auto &item : srcToDestinationMap ) {
			const QImage& src = item.srcReference.get ();
			QImage& dst = dstTexts [item.dstIdx];
			QPainter painter (&dst);
			painter.setCompositionMode (QPainter::CompositionMode_Source);
			painter.drawImage (item.dstOff, src);
			painter.end ();
		}
		return dstTexts;
	}
	/**
	 * Updates the texture coordinates of the passed mesh to those in the merged textures.
	 *
	 * @param mesh: the source mesh, whose texture coordinate refers to the original texture space.
	 */
	void updateTextureCoordinates (MeshModel &mesh) const {
		for (auto &face : mesh.cm.face) {
			if (face.IsD ())	continue;

			const int oldId = face.WT (0).N ();
			if (oldId < 0 || oldId >= static_cast <int> (srcToDestinationMap.size ()))	continue;

			const auto &info = srcToDestinationMap [oldId];
			const auto &dSize = dstSizes [info.dstIdx];

			for (int k = 0; k < face.VN(); ++k) {
				face.WT(k).N() = static_cast <short> (info.dstIdx);

				// Remap U
				face.WT(k).U() = face.WT(k).U() * static_cast <float> (info.srcSize.width ()) /
																				static_cast <float> (dSize.width ());

				// Remap V: note that QImage stores coordinates top-down, meshlab bottom-up.
				const float v_offset_from_bottom =	static_cast <float> (dSize.height ())  -
													static_cast <float> (info.dstOff.y ()) -
													static_cast <float> (info.srcSize.height ());

				const float v_concrete = face.WT(k).V() * static_cast<float> (info.srcSize.height ()) +
																								v_offset_from_bottom;
				// Apply normalization.
				face.WT(k).V() = v_concrete / static_cast <float> (dSize.height ());
			}
		}
	}
};


#endif