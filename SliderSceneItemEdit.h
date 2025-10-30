#ifndef SLIDERSCENEITEMEDIT_H
#define SLIDERSCENEITEMEDIT_H

#include <QWidget>
#include "ui_SliderSceneItemEdit.h"
#include "SliderCustomSceneItem.h"

class SliderSceneItemEdit : public QWidget
{
    Q_OBJECT

public:
    SliderSceneItemEdit(SliderCustomSceneItem* SliderItem,QWidget *parent = 0);
    ~SliderSceneItemEdit();

    SliderCustomSceneItem* mSceneItem;
    protected slots:
    void on_comboBoxSliderType_currentIndexChanged(int val);
    void on_lineEditMinSliderVal_editingFinished();
    void on_lineEditMaxSliderVal_editingFinished();
    void on_horizontalSliderSliderKalinligi_sliderMoved(int value);
    void on_horizontalSliderSliderLength_sliderMoved(int value);
    void on_lineEditKalinlik_editingFinished();
    void on_lineEditSliderLength_editingFinished();
    void on_horizontalSliderMinMaxDeneme_sliderMoved(int value);
    void on_textEditCss_textChanged();
private:
    Ui::SliderSceneItemEdit ui;
};

#endif // SliderSCENEITEMEDIT_H
