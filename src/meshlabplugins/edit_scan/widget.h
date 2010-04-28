#ifndef WIDGET_H
#define WIDGET_H

#include <QDockWidget>
#include <QMainWindow>
#include "ui_widget.h"
#include <cassert>
namespace Ui {
    class Widget;
}

// Defines the bounds (more than 1 object use them)
enum {MIN_SAMPFREQ   = 1,   MAX_SAMPFREQ   = 30,    DEF_SAMPFREQ   = 5};
enum {MIN_SCANWIDTH  = 1,   MAX_SCANWIDTH  = 100,   DEF_SCANWIDTH  = 30};
enum {MIN_NUMSAMPLE  = 2,   MAX_NUMSAMPLE  = 100,   DEF_NUMSAMPLE  = 10};

class Widget : public QDockWidget {
    Q_OBJECT
public:
    Widget(QWidget *parent=0) :  QDockWidget(parent), ui(new Ui::Widget){
        assert( parent!=0 );
        ui->setupUi(this);
        setFloating(false);
        setAllowedAreas(Qt::RightDockWidgetArea);
        ((QMainWindow*) parent)->addDockWidget( Qt::RightDockWidgetArea, this );
        show();

        // Init the UI
        init_ui();

        // Inherit the control variables
        sampfreq  = ui->dial_freq->value();
        scanwidth = ui->dial_width->value();
        numsample = ui->dial_numsamples->value();

    }
    ~Widget(){
        delete ui;
    }
    // Accessers
    int getSampfreq(){ return sampfreq; }
    int getScanwidth(){ return scanwidth; }
    int getNumsample(){ return numsample; }
    bool getDrawLineFlag() {return  ui->drawLineCheckBox->isChecked(); }

signals:
    void laser_parameter_updated();
    void scan_requested();
    void save_requested();

private:
    Ui::Widget *ui;

    int    sampfreq;  // lines/sec [1..30], def 5
    int   scanwidth;  // %-bbox diagonal [0..100], def 20
    int   numsample;  // [2..100], def 10

    void init_ui(){
        // Frequency
        ui->dial_freq->setMinimum(MIN_SAMPFREQ);
        ui->dial_freq->setMaximum(MAX_SAMPFREQ);
        ui->dial_freq->setValue(DEF_SAMPFREQ);
        ui->sbox_freq->setMinimum(MIN_SAMPFREQ);
        ui->sbox_freq->setMaximum(MAX_SAMPFREQ);
        ui->dial_freq->setValue(DEF_SAMPFREQ);
        // numsamples
        ui->dial_numsamples->setMinimum(MIN_NUMSAMPLE);
        ui->dial_numsamples->setMaximum(MAX_NUMSAMPLE);
        ui->dial_numsamples->setValue(DEF_NUMSAMPLE);
        ui->sbox_numsamples->setMinimum(MIN_NUMSAMPLE);
        ui->sbox_numsamples->setMaximum(MAX_NUMSAMPLE);
        ui->sbox_numsamples->setValue(DEF_NUMSAMPLE);

        // scanwidth
        ui->dial_width->setMinimum(MIN_SCANWIDTH);
        ui->dial_width->setMaximum(MAX_SCANWIDTH);
        ui->dial_width->setValue(DEF_SCANWIDTH);
        ui->sbox_width->setMinimum(MIN_SCANWIDTH);
        ui->sbox_width->setMaximum(MAX_SCANWIDTH);
        ui->sbox_width->setValue(DEF_SCANWIDTH);
    }

private slots:
    void on_sbox_numsamples_valueChanged(int val){
        numsample = qBound((int)MIN_NUMSAMPLE,  val, (int)MAX_NUMSAMPLE);
        emit laser_parameter_updated();
    }
    void on_sbox_width_valueChanged(int val){
        scanwidth = qBound((int)MIN_SCANWIDTH, val, (int)MAX_SCANWIDTH);
        emit laser_parameter_updated();
    }
    void on_sbox_freq_valueChanged(int val){
        sampfreq  = qBound((int)MIN_SAMPFREQ, val, (int)MAX_SAMPFREQ);
        emit laser_parameter_updated();
    }
    void on_scanbutton_released(){
        emit scan_requested();
    }
    void on_saveButton_released(){
        emit save_requested();
    }
};
#endif // WIDGET_H
