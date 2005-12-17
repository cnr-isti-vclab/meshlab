#ifndef REFINE_DIALOG_H
#define REFINE_DIALOG_H

// for options on refine and decimator
#include <QDialog>
#include "ui_refine.h"

class RefineDialog : public QDialog, Ui::Dialog {

Q_OBJECT

public:
  RefineDialog() : QDialog()
  {
    setupUi( this );
  }


};

#endif //REFINE_DIALOG_H
