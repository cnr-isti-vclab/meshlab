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
// This file implements the detail of the PlainTextEdit class.

#include "qitty/plain_text_edit.h"

#include <QtGui>

#include "qitty/style_sheet.h"

namespace qitty_widgets {

// Constructs an PlainTextEdit with `mode` and `parent`.
PlainTextEdit::PlainTextEdit(Mode mode, QWidget *parent) :
    QPlainTextEdit(parent) {
  Init(mode);
}

// Reimplements the QPlainTextEdit::focusInEvent() function.
void PlainTextEdit::focusInEvent(QFocusEvent *event) {
  // Clears the cursor. The width of the cursor will be recovered in the
  // keyPressEvent() and the mousePressEvent() functions.
  setCursorWidth(0);

  // Selects all text. This step is trying to simulates the effect of the
  // QLineEdit widget whenever the focus moves in to the widget by the Tab key.
  selectAll();

  QPlainTextEdit::focusInEvent(event);
}

// Reimplements the QPlainTextEdit::focusOutEvent() function.
void PlainTextEdit::focusOutEvent(QFocusEvent *event) {
  // Clears the selection.
  QTextCursor text_cursor = textCursor();
  text_cursor.clearSelection();
  setTextCursor(text_cursor);

  // Emits the FocusOut() signal.
  emit FocusOut();

  QPlainTextEdit::focusOutEvent(event);
}

// Initializes the PlainTextEdit object. This function should be called
// in all constructors.
void PlainTextEdit::Init(Mode mode) {
  setObjectName("plain_text_edit");
  qitty_utils::SetStyleSheet(this, ":/qitty/css/plain_text_edit.css");
  // Initializes properties.
  set_all_selected(false);
  set_clear_focus_when_return_pressed(false);
  set_initial_cursor_width(cursorWidth());
  set_minimum_line_number(3);
  // Updates the state of the all_selected_ property whenever selection changed.
  connect(this, SIGNAL(selectionChanged()), this, SLOT(UpdateAllSelected()));
  // Updates the is_modified_ property using the modificationChanged() signal.
  connect(this, SIGNAL(modificationChanged(bool)),
          this, SLOT(UpdateModified(bool)));
  // Updates the state whenever the mode is changed and keeps the specified
  // `mode`
  connect(this, SIGNAL(ModeChanged(Mode)), this, SLOT(UpdateModeState(Mode)));
  set_mode(mode);
}

// Reimplements the QPlainTextEdit::keyPressEvent() function.
void PlainTextEdit::keyPressEvent(QKeyEvent *event) {
  // Clears focus if the clear_focus_when_return_pressed_ property is set to
  // true and the Return or the Enter key is pressed.
  if (clear_focus_when_return_pressed() &&
      (event->key() == 16777220 || event->key() == 16777221)) {
    clearFocus();
    return;
  }

  // Recovers the width of the cursor. This step is needed because the cursor
  // is cleared in the focusInEvent() function.
  RecoverCursorWidth();

  // If full text is selected, moves the cursor to the start if left or up
  // arrows are pressed, moves to the end if right or bottom arrows are pressed.
  if (all_selected()) {
    if (event->key() == 16777234 || event->key() == 16777235)
      moveCursor(QTextCursor::Start);
    else if (event->key() == 16777236 || event->key() == 16777237)
      moveCursor(QTextCursor::End);
  }

  QPlainTextEdit::keyPressEvent(event);
}

// Reimplements the QPlainTextEdit::mousePressEvent() function.
void PlainTextEdit::mousePressEvent(QMouseEvent *event) {
  // Recovers the width of the cursor. This step is needed because the cursor
  // is cleared in the focusInEvent() function.
  RecoverCursorWidth();

  // Clears the selection if full text is selected.
  if (all_selected()) {
    QTextCursor text_cursor = textCursor();
    text_cursor.clearSelection();
    setTextCursor(text_cursor);
  }

  QPlainTextEdit::mousePressEvent(event);
}

// Sets the width of the cursor back to the orignal one.
void PlainTextEdit::RecoverCursorWidth() {
  setCursorWidth(initial_cursor_width());
}

// Reimplements the QPlainTextEdit::resizeEvent() function.
void PlainTextEdit::resizeEvent(QResizeEvent *event) {
  QPlainTextEdit::resizeEvent(event);

  // Updates the minimum_height_ property and the hieght of this widget if
  // this widget is in the kElasticMode.
  if (mode() == kElasticMode) {
    UpdateMinimumHeight();
    UpdateHeight();
  }
}

// Sets the all_selected_ property to true if full text is selected.
void PlainTextEdit::UpdateAllSelected() {
  QTextCursor text_cursor = textCursor();
  if (text_cursor.selectedText() == toPlainText())
    set_all_selected(true);
  else
    set_all_selected(false);
}

// Calculates the proper height according to the plain text. If the height is
// shorter than the minimum height, sets the height to the minimum height.
// Note that this function is used only in the kElasticMode.
void PlainTextEdit::UpdateHeight() {
  // Calculates the proper height according to current plain text.
  QFontMetrics font_metrics = fontMetrics();
  QRect bounding_rect = font_metrics.boundingRect(0, 0, width() - 6, 0,
                                                  Qt::TextWordWrap,
                                                  toPlainText());
  // Determines the actual height by comparing with the minimum height.
  int height = minimum_height();
  if (bounding_rect.height() > height)
    height = bounding_rect.height();
  // Sets the height of this widget. Adds 6 for padding.
  setFixedHeight(height + 6);
}

// Calculates the minimum height according to the minimum line number.
// Note that this function is used only in the kElasticMode.
void PlainTextEdit::UpdateMinimumHeight() {
  // Creates an empty text to contain minimum lines.
  QString text;
  for (int i = 1; i < minimum_line_number(); ++i)
    text.append(QChar::LineSeparator);
  // Calculates the height according to the text.
  QFontMetrics font_metrics = fontMetrics();
  QRect bounding_rect = font_metrics.boundingRect(0, 0, width(), 0,
                                                  Qt::TextWordWrap, text);
  set_minimum_height(bounding_rect.height());
}

// [slot] Updates the state according to the specified `mode`.
void PlainTextEdit::UpdateModeState(Mode mode) {
  if (mode == kElasticMode) {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTabChangesFocus(true);
    // // Updates the height of this widget whenever text changed.
    connect(this, SIGNAL(textChanged()), this, SLOT(UpdateHeight()));
  } else {
    disconnect(this, SIGNAL(textChanged()), this, SLOT(UpdateHeight()));
  }
}

// [slot] Updates the is_modified_ property by the specified `changed`.
void PlainTextEdit::UpdateModified(bool changed) {
  set_is_modified(changed);
}

}  // namespace qitty_widgets
