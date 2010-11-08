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
// This file implements the detail of the LineEdit class.

#include "qitty/line_edit.h"

#include <QtGui>

namespace qitty_widgets {

// Constructs a line edit with no text.
LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent) {
  Init();
}

// Constructs a line edit containing the text `contents`.
LineEdit::LineEdit(const QString &contents, QWidget *parent) :
    QLineEdit(contents, parent) {
  Init();
}

// Clears focus if the clear_focus_when_return_pressed_ property is set to true.
void LineEdit::ClearFocusWhenReturnPressed() {
  if (clear_focus_when_return_pressed())
    clearFocus();
}

// Reimplements the QLineEdit::focusInEvent() function.
void LineEdit::focusInEvent(QFocusEvent *event) {
  // Clears the text if the hint is set.
  if (!hint().isNull())
    setText("");

  QLineEdit::focusInEvent(event);
}

// Reimplements the QLineEdit::focusOutEvent() function.
void LineEdit::focusOutEvent(QFocusEvent *event) {
  // Emits the FocusOut() signal.
  emit FocusOut();

  // Sets the text to the hint if the hint is set.
  if (!hint().isNull())
    setText(hint());

  QLineEdit::focusOutEvent(event);
}

// Initializes the LineEdit object. This function should be called in all
// constructors.
void LineEdit::Init() {
  set_clear_focus_when_return_pressed(false);
  set_is_modified(false);
  // Updates the is_modified_ property using the modificationChanged() signal.
  connect(this, SIGNAL(textEdited(const QString)),
          this, SLOT(SetModifiedToTrue()));
  // Clears focus when the Return or the Enter key pressed if the
  // clear_focus_when_return_pressed_ is set to true.
  connect(this, SIGNAL(returnPressed()),
          this, SLOT(ClearFocusWhenReturnPressed()));
}

// Sets the hint, also sets the text if the widget doesn't have focus.
void LineEdit::SetHint(const QString &hint) {
  set_hint(hint);
  if (!hasFocus())
    setText(hint);
}

// [slot] Sets the is_modified_ property to true.
void LineEdit::SetModifiedToTrue() {
  set_is_modified(true);
}

} // namespace qitty_widgets
