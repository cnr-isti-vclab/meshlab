#ifndef EDIT_VS_WIDGET_H
#define EDIT_VS_WIDGET_H

/* Qt includes */
#include    <QWidget>
#include    <QGridLayout>
#include    <QGroupBox>
#include    <QButtonGroup>
#include    <QRadioButton>
#include    <QLabel>
#include    <QLineEdit>
#include    <QIntValidator>
#include    <QDoubleValidator>
#include    <QSlider>
#include    <QCheckBox>
#include    <QPushButton>

class EditVsWidget: public QWidget
{
    Q_OBJECT

public:

    EditVsWidget                ( QWidget* parent = 0, Qt::WindowFlags flags = 0 );
    virtual ~EditVsWidget       ( void );

    void    initializeWidgets   ( bool useCustomPovs, int povsNumber,
                                  float xAxis, float yAxis, float zAxis, int gap,
                                  int uRes, int uGap,
                                  int fRes, int bigJump, int borderConeGap, int smallJump, int normalsAngle,
                                  bool unifyClouds );

signals:

    void    customPovsMode      ( bool on );
    void    povsNumberChanged   ( int povsNumber );
    void    xAxisChanged        ( float newComp );
    void    yAxisChanged        ( float newComp );
    void    zAxisChanged        ( float newComp );
    void    povConeGapChanged   ( int newGap );
    void    uResChanged         ( int newSide );
    void    uConeGapChanged     ( int newGap );
    void    fResChanged         ( int newSide );
    void    bigJumpChanged      ( float newJump );
    void    borderFGapChanged   ( int newGap );
    void    smallJumpChanged    ( float newJump );
    void    normalsAngleChanged ( int newAngle );
    void    unifyPointClouds    ( bool unify );
    void    go                  ( void );

private:

    /* QWidgets-related stuff */
    QGridLayout*    mainLayout;

    QGroupBox*      povsBox;
    QGridLayout*    povsBoxLayout;
    QButtonGroup*   povsGroup;
    QRadioButton*   genPovsRadioBtn;
    QRadioButton*   savedPovsRadioBtn;
    QLabel*         povsNumberLabel;
    QLineEdit*      povsNumberLine;

    QGroupBox*      coneAxisBox;
    QGridLayout*    coneAxisBoxLayout;
    QLabel*         xCompLabel;
    QLineEdit*      xCompLine;
    QLabel*         yCompLabel;
    QLineEdit*      yCompLine;
    QLabel*         zCompLabel;
    QLineEdit*      zCompLine;

    QLabel*         gapLabel;
    QSlider*        gapSlider;
    QLabel*         gapValue;

    QGroupBox*      uniformBox;
    QGridLayout*    uniformBoxLayout;
    QLabel*         uResLabel;
    QSlider*        uResSlider;
    QLabel*         uResValue;
    QLabel*         uConeLabel;
    QSlider*        uConeSlider;
    QLabel*         uConeValue;

    QGroupBox*      featureBox;
    QGridLayout*    featureBoxLayout;
    QLabel*         fResLabel;
    QSlider*        fResSlider;
    QLabel*         fResValue;
    QLabel*         borderJumpLabel;
    QSlider*        borderJumpSlider;
    QLabel*         borderJumpValue;
    QLabel*         borderFacingLabel;
    QSlider*        borderFacingSlider;
    QLabel*         borderFacingValue;
    QLabel*         patchJumpLabel;
    QSlider*        patchJumpSlider;
    QLabel*         patchJumpValue;
    QLabel*         normalsAngleLabel;
    QSlider*        normalsAngleSlider;
    QLabel*         normalsAngleValue;

    QCheckBox*      unifyCheck;
    QPushButton*    goBtn;

    /* other Qt fields */
    QIntValidator*      povsValidator;
    QDoubleValidator*   axisValidator;

    /* private functions */
    void    createWidgets           ( void );
    void    destroyWidgets          ( void );
    void    prepareWidgets          ( void );
    void    makeEventConnections    ( void );

private slots:

    void    useSavedPovsRadioToggled    ( bool toggled );
    void    povsNumberTextChanged       ( QString newText );
    void    xCompTextChanged            ( QString newText );
    void    yCompTextChanged            ( QString newText );
    void    zCompTextChanged            ( QString newText );
    void    gapSliderChanged            ( int val );
    void    uResSliderChanged           ( int val );
    void    uGapSliderChanged           ( int val );
    void    fResSliderChanged           ( int val );
    void    borderJumpSliderChanged     ( int val );
    void    borderFacingSliderChanged   ( int val );
    void    patchJumpSliderChanged      ( int val );
    void    normalsSliderChanged        ( int val );
    void    unifyCheckToggled           ( bool toggled );
    void    goBtnClicked                ( void );

};

#endif
