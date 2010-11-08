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
// This file defines the PlainTextEdit widget, which enhances the QPlainTextEdit
// class.

#ifndef QITTY_WIDGETS_PLAIN_TEXT_EDIT_H_
#define QITTY_WIDGETS_PLAIN_TEXT_EDIT_H_

#include <QPlainTextEdit>
#include <QSize>

class QResizeEvent;

namespace qitty_widgets {

class PlainTextEdit : public QPlainTextEdit {
  Q_OBJECT

 public:
  enum Mode {
    kNormalMode = 0,
    // The height will be changed to adapt the text.
    kElasticMode,
  };

  PlainTextEdit(Mode mode = kNormalMode, QWidget *parent = NULL);
  // Accessor functions for properties.
  void set_clear_focus_when_return_pressed(bool b) {
    clear_focus_when_return_pressed_ = b;
  }
  void set_mode(Mode mode) {
    mode_ = mode;
    emit ModeChanged(mode);
  }
  bool is_modified() { return is_modified_; }
  void set_minimum_line_number(int number) { minimum_line_number_ = number; }

 signals:
  // This signal is emitted whenever the focus is out.
  void FocusOut();
  // This signal is emitted whenever the mode is changed.
  void ModeChanged(Mode mode);

 protected:
  void focusInEvent(QFocusEvent *event);
  void focusOutEvent(QFocusEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void resizeEvent(QResizeEvent *event);
  // Overwrites the wheelEvent() to deactivate the wheel.
  // void wheelEvent(QWheelEvent *event) {}

 private slots:
  void RecoverCursorWidth();
  void UpdateAllSelected();
  void UpdateHeight();
  void UpdateModeState(Mode mode);
  void UpdateModified(bool changed);

 private:
  void Init(Mode mode);
  void UpdateMinimumHeight();
  // Accessor functions for properties.
  bool all_selected() { return all_selected_; }
  void set_all_selected(bool b) { all_selected_ = b; }
  bool clear_focus_when_return_pressed() {
    return clear_focus_when_return_pressed_;
  }
  int initial_cursor_width() { return initial_cursor_width_; }
  void set_initial_cursor_width(int width) { initial_cursor_width_ = width; }
  void set_is_modified(bool b) { is_modified_ = b; }
  int minimum_height() { return minimum_height_; }
  void set_minimum_height(int height) { minimum_height_ = height; }
  int minimum_line_number() { return minimum_line_number_; }
  Mode mode() { return mode_; }

  // Sets to true if full text is selected.
  bool all_selected_;
  // Whether to clear focus when the Return or Enter key is pressed.
  bool clear_focus_when_return_pressed_;
  // The initial width of the cursor of this widget.
  int initial_cursor_width_;
  // Whether the text is modified or not, this value is updated by the
  // modificationChanged() signal and the UpdateModified() slot.
  bool is_modified_;
  // The minimum height of this widget. The value will be calculated in the
  // UpdateMinimumHeight() function.
  int minimum_height_;
  // The minimum line number of this widget. This value is used to determine
  // the minimum height of this widget in the kElasticMode.
  int minimum_line_number_;
  // Current mode of the widget.
  Mode mode_;
};

}  // namespace qitty_widgets

#endif  // QITTY_WIDGETS_PLAIN_TEXT_EDIT_H_
