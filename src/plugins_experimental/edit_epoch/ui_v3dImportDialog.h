/********************************************************************************
** Form generated from reading UI file 'v3dImportDialog.ui'
**
** Created: Tue 31. Jan 16:30:15 2012
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_V3DIMPORTDIALOG_H
#define UI_V3DIMPORTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_v3dImportDialog
{
public:
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QLabel *infoLabel;
    QLabel *previewLabel;
    QHBoxLayout *hboxLayout1;
    QLabel *label;
    QSpacerItem *spacerItem;
    QLabel *imgSizeLabel;
    QSpinBox *subsampleSpinBox;
    QHBoxLayout *hboxLayout2;
    QLabel *label_2;
    QSpacerItem *spacerItem1;
    QSpinBox *minCountSpinBox;
    QVBoxLayout *vboxLayout1;
    QSlider *minCountSlider;
    QLabel *rangeLabel;
    QHBoxLayout *hboxLayout3;
    QLabel *label_3;
    QSpacerItem *spacerItem2;
    QDoubleSpinBox *qualitySpinBox;
    QHBoxLayout *hboxLayout4;
    QLabel *label_5;
    QSpacerItem *spacerItem3;
    QDoubleSpinBox *smoothSpinBox;
    QHBoxLayout *hboxLayout5;
    QCheckBox *removeSmallCCCheckBox;
    QDoubleSpinBox *maxCCDiagSpinBox;
    QSpacerItem *spacerItem4;
    QCheckBox *holeCheckBox;
    QSpinBox *holeSpinBox;
    QGridLayout *gridLayout;
    QSlider *dilationSizeSlider;
    QSpinBox *erosionNumPassSpinBox;
    QLabel *lblDilationSize;
    QLabel *lblErosionSizeValue;
    QSpinBox *dilationNumPassSpinBox;
    QLabel *lblDilationSteps;
    QSlider *erosionSizeSlider;
    QLabel *lblDilationSizeValue;
    QLabel *lblErosionSteps;
    QLabel *label_8;
    QLabel *lblErosionSize;
    QCheckBox *dilationCheckBox;
    QCheckBox *erosionCheckBox;
    QHBoxLayout *hboxLayout6;
    QSpacerItem *spacerItem5;
    QCheckBox *fastMergeCheckBox;
    QLabel *label_6;
    QSlider *mergeResolutionSpinBox;
    QLabel *label_7;
    QHBoxLayout *hboxLayout7;
    QPushButton *selectButton;
    QSpacerItem *spacerItem6;
    QSpinBox *subsampleSequenceSpinBox;
    QHBoxLayout *hboxLayout8;
    QLabel *label_10;
    QSpacerItem *spacerItem7;
    QLineEdit *scaleLineEdit;
    QHBoxLayout *hboxLayout9;
    QSpacerItem *spacerItem8;
    QPushButton *okButton;
    QPushButton *plyButton;
    QPushButton *cancelButton;
    QTableWidget *imageTableWidget;

    void setupUi(QDialog *v3dImportDialog)
    {
        if (v3dImportDialog->objectName().isEmpty())
            v3dImportDialog->setObjectName(QString::fromUtf8("v3dImportDialog"));
        v3dImportDialog->resize(792, 606);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(v3dImportDialog->sizePolicy().hasHeightForWidth());
        v3dImportDialog->setSizePolicy(sizePolicy);
        hboxLayout = new QHBoxLayout(v3dImportDialog);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        vboxLayout = new QVBoxLayout();
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        infoLabel = new QLabel(v3dImportDialog);
        infoLabel->setObjectName(QString::fromUtf8("infoLabel"));

        vboxLayout->addWidget(infoLabel);

        previewLabel = new QLabel(v3dImportDialog);
        previewLabel->setObjectName(QString::fromUtf8("previewLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(previewLabel->sizePolicy().hasHeightForWidth());
        previewLabel->setSizePolicy(sizePolicy1);

        vboxLayout->addWidget(previewLabel);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        label = new QLabel(v3dImportDialog);
        label->setObjectName(QString::fromUtf8("label"));

        hboxLayout1->addWidget(label);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem);

        imgSizeLabel = new QLabel(v3dImportDialog);
        imgSizeLabel->setObjectName(QString::fromUtf8("imgSizeLabel"));

        hboxLayout1->addWidget(imgSizeLabel);

        subsampleSpinBox = new QSpinBox(v3dImportDialog);
        subsampleSpinBox->setObjectName(QString::fromUtf8("subsampleSpinBox"));

        hboxLayout1->addWidget(subsampleSpinBox);


        vboxLayout->addLayout(hboxLayout1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        label_2 = new QLabel(v3dImportDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        hboxLayout2->addWidget(label_2);

        spacerItem1 = new QSpacerItem(91, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem1);

        minCountSpinBox = new QSpinBox(v3dImportDialog);
        minCountSpinBox->setObjectName(QString::fromUtf8("minCountSpinBox"));

        hboxLayout2->addWidget(minCountSpinBox);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(0);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        minCountSlider = new QSlider(v3dImportDialog);
        minCountSlider->setObjectName(QString::fromUtf8("minCountSlider"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(minCountSlider->sizePolicy().hasHeightForWidth());
        minCountSlider->setSizePolicy(sizePolicy2);
        minCountSlider->setMaximum(10);
        minCountSlider->setOrientation(Qt::Horizontal);

        vboxLayout1->addWidget(minCountSlider);

        rangeLabel = new QLabel(v3dImportDialog);
        rangeLabel->setObjectName(QString::fromUtf8("rangeLabel"));

        vboxLayout1->addWidget(rangeLabel);


        hboxLayout2->addLayout(vboxLayout1);


        vboxLayout->addLayout(hboxLayout2);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
        label_3 = new QLabel(v3dImportDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        hboxLayout3->addWidget(label_3);

        spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout3->addItem(spacerItem2);

        qualitySpinBox = new QDoubleSpinBox(v3dImportDialog);
        qualitySpinBox->setObjectName(QString::fromUtf8("qualitySpinBox"));
        qualitySpinBox->setDecimals(1);
        qualitySpinBox->setMaximum(90);
        qualitySpinBox->setSingleStep(5);
        qualitySpinBox->setValue(75);

        hboxLayout3->addWidget(qualitySpinBox);


        vboxLayout->addLayout(hboxLayout3);

        hboxLayout4 = new QHBoxLayout();
        hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
        label_5 = new QLabel(v3dImportDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        hboxLayout4->addWidget(label_5);

        spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout4->addItem(spacerItem3);

        smoothSpinBox = new QDoubleSpinBox(v3dImportDialog);
        smoothSpinBox->setObjectName(QString::fromUtf8("smoothSpinBox"));
        smoothSpinBox->setDecimals(0);
        smoothSpinBox->setMaximum(20);
        smoothSpinBox->setValue(3);

        hboxLayout4->addWidget(smoothSpinBox);


        vboxLayout->addLayout(hboxLayout4);

        hboxLayout5 = new QHBoxLayout();
        hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
        removeSmallCCCheckBox = new QCheckBox(v3dImportDialog);
        removeSmallCCCheckBox->setObjectName(QString::fromUtf8("removeSmallCCCheckBox"));
        removeSmallCCCheckBox->setLayoutDirection(Qt::RightToLeft);
        removeSmallCCCheckBox->setChecked(true);

        hboxLayout5->addWidget(removeSmallCCCheckBox);

        maxCCDiagSpinBox = new QDoubleSpinBox(v3dImportDialog);
        maxCCDiagSpinBox->setObjectName(QString::fromUtf8("maxCCDiagSpinBox"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(maxCCDiagSpinBox->sizePolicy().hasHeightForWidth());
        maxCCDiagSpinBox->setSizePolicy(sizePolicy3);
        maxCCDiagSpinBox->setDecimals(0);
        maxCCDiagSpinBox->setMaximum(25);
        maxCCDiagSpinBox->setSingleStep(5);
        maxCCDiagSpinBox->setValue(5);

        hboxLayout5->addWidget(maxCCDiagSpinBox);

        spacerItem4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout5->addItem(spacerItem4);

        holeCheckBox = new QCheckBox(v3dImportDialog);
        holeCheckBox->setObjectName(QString::fromUtf8("holeCheckBox"));
        holeCheckBox->setEnabled(true);
        holeCheckBox->setLayoutDirection(Qt::RightToLeft);

        hboxLayout5->addWidget(holeCheckBox);

        holeSpinBox = new QSpinBox(v3dImportDialog);
        holeSpinBox->setObjectName(QString::fromUtf8("holeSpinBox"));
        holeSpinBox->setSingleStep(5);
        holeSpinBox->setValue(10);

        hboxLayout5->addWidget(holeSpinBox);


        vboxLayout->addLayout(hboxLayout5);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        dilationSizeSlider = new QSlider(v3dImportDialog);
        dilationSizeSlider->setObjectName(QString::fromUtf8("dilationSizeSlider"));
        dilationSizeSlider->setMinimum(1);
        dilationSizeSlider->setMaximum(6);
        dilationSizeSlider->setSingleStep(1);
        dilationSizeSlider->setPageStep(5);
        dilationSizeSlider->setValue(2);
        dilationSizeSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(dilationSizeSlider, 0, 5, 1, 1);

        erosionNumPassSpinBox = new QSpinBox(v3dImportDialog);
        erosionNumPassSpinBox->setObjectName(QString::fromUtf8("erosionNumPassSpinBox"));
        erosionNumPassSpinBox->setMinimum(1);
        erosionNumPassSpinBox->setMaximum(10);
        erosionNumPassSpinBox->setValue(3);

        gridLayout->addWidget(erosionNumPassSpinBox, 1, 3, 1, 1);

        lblDilationSize = new QLabel(v3dImportDialog);
        lblDilationSize->setObjectName(QString::fromUtf8("lblDilationSize"));

        gridLayout->addWidget(lblDilationSize, 0, 4, 1, 1);

        lblErosionSizeValue = new QLabel(v3dImportDialog);
        lblErosionSizeValue->setObjectName(QString::fromUtf8("lblErosionSizeValue"));

        gridLayout->addWidget(lblErosionSizeValue, 1, 6, 1, 1);

        dilationNumPassSpinBox = new QSpinBox(v3dImportDialog);
        dilationNumPassSpinBox->setObjectName(QString::fromUtf8("dilationNumPassSpinBox"));
        dilationNumPassSpinBox->setMinimum(1);
        dilationNumPassSpinBox->setMaximum(5);

        gridLayout->addWidget(dilationNumPassSpinBox, 0, 3, 1, 1);

        lblDilationSteps = new QLabel(v3dImportDialog);
        lblDilationSteps->setObjectName(QString::fromUtf8("lblDilationSteps"));

        gridLayout->addWidget(lblDilationSteps, 0, 2, 1, 1);

        erosionSizeSlider = new QSlider(v3dImportDialog);
        erosionSizeSlider->setObjectName(QString::fromUtf8("erosionSizeSlider"));
        erosionSizeSlider->setMinimum(1);
        erosionSizeSlider->setMaximum(6);
        erosionSizeSlider->setSingleStep(1);
        erosionSizeSlider->setPageStep(5);
        erosionSizeSlider->setValue(2);
        erosionSizeSlider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(erosionSizeSlider, 1, 5, 1, 1);

        lblDilationSizeValue = new QLabel(v3dImportDialog);
        lblDilationSizeValue->setObjectName(QString::fromUtf8("lblDilationSizeValue"));

        gridLayout->addWidget(lblDilationSizeValue, 0, 6, 1, 1);

        lblErosionSteps = new QLabel(v3dImportDialog);
        lblErosionSteps->setObjectName(QString::fromUtf8("lblErosionSteps"));

        gridLayout->addWidget(lblErosionSteps, 1, 2, 1, 1);

        label_8 = new QLabel(v3dImportDialog);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 0, 0, 1, 1);

        lblErosionSize = new QLabel(v3dImportDialog);
        lblErosionSize->setObjectName(QString::fromUtf8("lblErosionSize"));

        gridLayout->addWidget(lblErosionSize, 1, 4, 1, 1);

        dilationCheckBox = new QCheckBox(v3dImportDialog);
        dilationCheckBox->setObjectName(QString::fromUtf8("dilationCheckBox"));
        dilationCheckBox->setChecked(true);

        gridLayout->addWidget(dilationCheckBox, 0, 1, 1, 1);

        erosionCheckBox = new QCheckBox(v3dImportDialog);
        erosionCheckBox->setObjectName(QString::fromUtf8("erosionCheckBox"));
        erosionCheckBox->setChecked(true);

        gridLayout->addWidget(erosionCheckBox, 1, 1, 1, 1);


        vboxLayout->addLayout(gridLayout);

        hboxLayout6 = new QHBoxLayout();
        hboxLayout6->setObjectName(QString::fromUtf8("hboxLayout6"));
        spacerItem5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout6->addItem(spacerItem5);

        fastMergeCheckBox = new QCheckBox(v3dImportDialog);
        fastMergeCheckBox->setObjectName(QString::fromUtf8("fastMergeCheckBox"));
        fastMergeCheckBox->setLayoutDirection(Qt::RightToLeft);

        hboxLayout6->addWidget(fastMergeCheckBox);

        label_6 = new QLabel(v3dImportDialog);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        hboxLayout6->addWidget(label_6);

        mergeResolutionSpinBox = new QSlider(v3dImportDialog);
        mergeResolutionSpinBox->setObjectName(QString::fromUtf8("mergeResolutionSpinBox"));
        mergeResolutionSpinBox->setMinimum(1);
        mergeResolutionSpinBox->setMaximum(5);
        mergeResolutionSpinBox->setValue(3);
        mergeResolutionSpinBox->setOrientation(Qt::Horizontal);

        hboxLayout6->addWidget(mergeResolutionSpinBox);

        label_7 = new QLabel(v3dImportDialog);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        hboxLayout6->addWidget(label_7);


        vboxLayout->addLayout(hboxLayout6);

        hboxLayout7 = new QHBoxLayout();
        hboxLayout7->setObjectName(QString::fromUtf8("hboxLayout7"));
        selectButton = new QPushButton(v3dImportDialog);
        selectButton->setObjectName(QString::fromUtf8("selectButton"));

        hboxLayout7->addWidget(selectButton);

        spacerItem6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout7->addItem(spacerItem6);

        subsampleSequenceSpinBox = new QSpinBox(v3dImportDialog);
        subsampleSequenceSpinBox->setObjectName(QString::fromUtf8("subsampleSequenceSpinBox"));
        subsampleSequenceSpinBox->setValue(3);

        hboxLayout7->addWidget(subsampleSequenceSpinBox);


        vboxLayout->addLayout(hboxLayout7);

        hboxLayout8 = new QHBoxLayout();
        hboxLayout8->setObjectName(QString::fromUtf8("hboxLayout8"));
        label_10 = new QLabel(v3dImportDialog);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        hboxLayout8->addWidget(label_10);

        spacerItem7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout8->addItem(spacerItem7);

        scaleLineEdit = new QLineEdit(v3dImportDialog);
        scaleLineEdit->setObjectName(QString::fromUtf8("scaleLineEdit"));
        scaleLineEdit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        hboxLayout8->addWidget(scaleLineEdit);


        vboxLayout->addLayout(hboxLayout8);

        hboxLayout9 = new QHBoxLayout();
        hboxLayout9->setObjectName(QString::fromUtf8("hboxLayout9"));
        spacerItem8 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout9->addItem(spacerItem8);

        okButton = new QPushButton(v3dImportDialog);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        hboxLayout9->addWidget(okButton);

        plyButton = new QPushButton(v3dImportDialog);
        plyButton->setObjectName(QString::fromUtf8("plyButton"));

        hboxLayout9->addWidget(plyButton);

        cancelButton = new QPushButton(v3dImportDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        hboxLayout9->addWidget(cancelButton);


        vboxLayout->addLayout(hboxLayout9);


        hboxLayout->addLayout(vboxLayout);

        imageTableWidget = new QTableWidget(v3dImportDialog);
        if (imageTableWidget->columnCount() < 3)
            imageTableWidget->setColumnCount(3);
        imageTableWidget->setObjectName(QString::fromUtf8("imageTableWidget"));
        QSizePolicy sizePolicy4(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(imageTableWidget->sizePolicy().hasHeightForWidth());
        imageTableWidget->setSizePolicy(sizePolicy4);
        imageTableWidget->setColumnCount(3);

        hboxLayout->addWidget(imageTableWidget);


        retranslateUi(v3dImportDialog);
        QObject::connect(okButton, SIGNAL(clicked()), v3dImportDialog, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), v3dImportDialog, SLOT(reject()));
        QObject::connect(removeSmallCCCheckBox, SIGNAL(toggled(bool)), maxCCDiagSpinBox, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(v3dImportDialog);
    } // setupUi

    void retranslateUi(QDialog *v3dImportDialog)
    {
        v3dImportDialog->setWindowTitle(QApplication::translate("v3dImportDialog", "V3D Import Settings", 0, QApplication::UnicodeUTF8));
        infoLabel->setText(QApplication::translate("v3dImportDialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        previewLabel->setText(QApplication::translate("v3dImportDialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("v3dImportDialog", "SubSample", 0, QApplication::UnicodeUTF8));
        imgSizeLabel->setText(QApplication::translate("v3dImportDialog", "TextLabel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        subsampleSpinBox->setToolTip(QApplication::translate("v3dImportDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Set the subsample factor:</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">  1 the image is not resized</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">  2 image is halved <span style=\" font-style:italic;\">(one point every 4)</span></p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">  3 image is reduced to one third <span style=\" font-style:italic;\">(one point every 9)</span></p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("v3dImportDialog", "Minimum Count", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        minCountSpinBox->setToolTip(QApplication::translate("v3dImportDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Set the minimum number of match for a sample to be accepted</p><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">3 means that only samples that had been found a correspondence with other 2 images or more are considered</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        rangeLabel->setText(QApplication::translate("v3dImportDialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("v3dImportDialog", "Minimum Angle", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("v3dImportDialog", "Feature Aware Smoothing", 0, QApplication::UnicodeUTF8));
        removeSmallCCCheckBox->setText(QApplication::translate("v3dImportDialog", "Remove pieces less than", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        maxCCDiagSpinBox->setToolTip(QApplication::translate("v3dImportDialog", "When enabled, all the floating pieces smaller than the indicated percentage are deleted. Unit is the diagonal of the bounding box of the object", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        holeCheckBox->setText(QApplication::translate("v3dImportDialog", "Close Holes less than", 0, QApplication::UnicodeUTF8));
        lblDilationSize->setText(QApplication::translate("v3dImportDialog", "Size:", 0, QApplication::UnicodeUTF8));
        lblErosionSizeValue->setText(QApplication::translate("v3dImportDialog", "5 x 5", 0, QApplication::UnicodeUTF8));
        lblDilationSteps->setText(QApplication::translate("v3dImportDialog", "Num. passes:", 0, QApplication::UnicodeUTF8));
        lblDilationSizeValue->setText(QApplication::translate("v3dImportDialog", "5 x 5", 0, QApplication::UnicodeUTF8));
        lblErosionSteps->setText(QApplication::translate("v3dImportDialog", "Num. passes:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("v3dImportDialog", "Depth Filter", 0, QApplication::UnicodeUTF8));
        lblErosionSize->setText(QApplication::translate("v3dImportDialog", "Size:", 0, QApplication::UnicodeUTF8));
        dilationCheckBox->setText(QApplication::translate("v3dImportDialog", "Dilation", 0, QApplication::UnicodeUTF8));
        erosionCheckBox->setText(QApplication::translate("v3dImportDialog", "Erosion", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        fastMergeCheckBox->setToolTip(QApplication::translate("v3dImportDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Perform a fast, fixed resolution merging of all the range maps. If unchecked all the rangemaps are simply put in the same space without merging them.</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        fastMergeCheckBox->setText(QApplication::translate("v3dImportDialog", "Fast merge", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("v3dImportDialog", "Resolution: Min", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("v3dImportDialog", "Max", 0, QApplication::UnicodeUTF8));
        selectButton->setText(QApplication::translate("v3dImportDialog", "Select", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("v3dImportDialog", "Scaling Factor", 0, QApplication::UnicodeUTF8));
        scaleLineEdit->setInputMask(QString());
        scaleLineEdit->setText(QApplication::translate("v3dImportDialog", "1.0", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("v3dImportDialog", "OK", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        plyButton->setToolTip(QApplication::translate("v3dImportDialog", "Process all selected range maps and save them as separated ply", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        plyButton->setText(QApplication::translate("v3dImportDialog", "Export as PLY", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("v3dImportDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class v3dImportDialog: public Ui_v3dImportDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_V3DIMPORTDIALOG_H
