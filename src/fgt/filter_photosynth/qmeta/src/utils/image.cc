// Copyright 2010, Ollix
// All rights reserved.
//
// This file is part of Qitty.
//
// Qitty is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or any later version.
//
// Qitty is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with Qitty. If not, see <http://www.gnu.org/licenses/>.

// ---
// Author: olliwang@ollix.com (Olli Wang)
//
// Qitty - a toolkit to enhance the Qt library.

#include "qitty/image.h"

#include <QImage>
#include <QImageReader>
#include <QString>

namespace qitty_utils {

// Returns an image with the given length and tries to load the image from the
// file with the given file_name.
QImage SacledImage(const QString &file_name, int length) {
  QImageReader image_reader(file_name);
  if (!image_reader.canRead()) {
    QImage image;
    return image;
  }

  int image_width = image_reader.size().width();
  int image_height = image_reader.size().height();
  if (image_width > image_height) {
    image_height = static_cast<double>(length) / image_width * image_height;
    image_width = length;
  } else if (image_width < image_height) {
    image_width = static_cast<double>(length) / image_height * image_width;
    image_height = length;
  } else {
    image_width = length;
    image_height = length;
  }
  image_reader.setScaledSize(QSize(image_width, image_height));
  return image_reader.read();
}

}  // namespace qitty_utils
