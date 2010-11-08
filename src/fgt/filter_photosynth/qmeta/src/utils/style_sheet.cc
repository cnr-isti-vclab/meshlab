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

#include "qitty/style_sheet.h"

#include <QFile>
#include <QIODevice>
#include <QString>
#include <QWidget>

namespace qitty_utils {

// Sets the style sheet of the specified widget from the specified file path.
void SetStyleSheet(QWidget *widget, const QString &path) {
  QFile file(path);
  file.open(QIODevice::ReadOnly);
  widget->setStyleSheet(QString(file.readAll().constData()));
  file.close();
}

}  // namespace qitty_utils
