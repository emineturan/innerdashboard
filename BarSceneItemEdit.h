#ifndef BARSCENEITEMEDIT_H
#define BARSCENEITEMEDIT_H

#include <QWidget>
#include "ui_BarSceneItemEdit.h"
#include "BarCustomSceneItem.h"

class BarSceneItemEdit : public QWidget
{
    Q_OBJECT

public:
    BarSceneItemEdit(BarCustomSceneItem* barItem,QWidget *parent = 0);
    ~BarSceneItemEdit();

    BarCustomSceneItem* mSceneItem;
    protected slots:
    void on_comboBoxBarType_currentIndexChanged(int val);
    void on_lineEditMinBarVal_editingFinished();
    void on_lineEditMaxBarVal_editingFinished();
    void on_horizontalSliderBarKalinligi_sliderMoved(int value);
    void on_horizontalSliderBarLength_sliderMoved(int value);
    void on_lineEditKalinlik_editingFinished();
    void on_lineEditBarLength_editingFinished();
    void on_pushButtonSelectColor_pressed();
    void on_horizontalSliderMinMaxDeneme_sliderMoved(int value);
    void on_textEditCss_textChanged();
    void on_horizontalSliderArcRadius_sliderMoved(int value);
    void on_lineEditArcRadius_editingFinished();
    void on_horizontalSliderBarRadius_sliderMoved(int value);
    void on_lineEditBarRadius_editingFinished();
    void on_horizontalSliderArcBend_sliderMoved(int value);
    void on_lineEditArcBend_editingFinished();
    void on_horizontalSliderArcStartAngle_sliderMoved(int value);
    void on_lineEditArcStartAngle_editingFinished();
private:
    Ui::BarSceneItemEdit ui;
};

#endif // BARSCENEITEMEDIT_H
