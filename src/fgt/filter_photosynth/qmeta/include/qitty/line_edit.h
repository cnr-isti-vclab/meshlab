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
//
// This file defines the LineEdit class which enhances the QLineEdit class.

#ifndef QITTY_WIDGETS_LINE_EDIT_H_
#define QITTY_WIDGETS_LINE_EDIT_H_

#include <QLineEdit>

namespace qitty_widgets {

class LineEdit : public QLineEdit {
  Q_OBJECT

 public:
  LineEdit(QWidget *parent = NULL);
  LineEdit(const QString &contents, QWidget *parent = NULL);
  void SetHint(const QString &hint);
  // Accessor functions for properties.
  void set_clear_focus_when_return_pressed(bool b) {
    clear_focus_when_return_pressed_ = b;
  }
  bool is_modified() { return is_modified_; }

 signals:
  // This signal is emitted whenever the focus is out.
  void FocusOut();

 protected:
  void focusInEvent(QFocusEvent *event);
  void focusOutEvent(QFocusEvent *event);

 private slots:
  void ClearFocusWhenReturnPressed();
  void SetModifiedToTrue();

 private:
  void Init();
  // Accessor functions for properties.
  bool clear_focus_when_return_pressed() {
    return clear_focus_when_return_pressed_;
  }
  QString hint() { return hint_; }
  void set_hint(QString hint) { hint_ = hint; }
  void set_is_modified(bool is_modified) { is_modified_ = is_modified; }

  // Whether to clear focus when the Return or Enter key is pressed.
  bool clear_focus_when_return_pressed_;
  // Whether the text is modified or not, this value is updated by the
  // textEdited() signal and the SetModifiedToTrue() slot.
  bool is_modified_;
  // The hint displayed as text if focus is out.
  QString hint_;
};

}  // namespace qitty_widgets

#endif  // QITTY_WIDGETS_LINE_EDIT_H_
