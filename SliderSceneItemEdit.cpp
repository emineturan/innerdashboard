#include "SliderSceneItemEdit.h"
#include "qcolordialog.h"

SliderSceneItemEdit::SliderSceneItemEdit(SliderCustomSceneItem* SliderItem,QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    mSceneItem = SliderItem;
    ui.comboBoxSliderType->setCurrentIndex(SliderItem->mSliderType);
    ui.lineEditMinSliderVal->setText(QString::number(SliderItem->mMinVal,'f',2));
    ui.lineEditMaxSliderVal->setText(QString::number(SliderItem->mMaxVal,'f',2));
    ui.horizontalSliderSliderKalinligi->setValue(SliderItem->mLineThickness);
    ui.lineEditKalinlik->setText(QString::number(SliderItem->mLineThickness, 'f', 0));
    ui.horizontalSliderSliderLength->setValue(SliderItem->mLineLength);
    ui.lineEditSliderLength->setText(QString::number(SliderItem->mLineLength, 'f', 0));
    ui.textEditCss->setText(mSceneItem->mSliderWidgetCss);
}

SliderSceneItemEdit::~SliderSceneItemEdit()
{

}

void SliderSceneItemEdit::on_comboBoxSliderType_currentIndexChanged(int val)
{
    mSceneItem->changeSliderType((SliderType)val);
}

void SliderSceneItemEdit::on_lineEditMinSliderVal_editingFinished()
{
    bool conversionOk = false;
    float num = ui.lineEditMinSliderVal->text().toFloat(&conversionOk);
    if (conversionOk)
    {
        mSceneItem->mMinVal = num;
    }
}

void SliderSceneItemEdit::on_lineEditMaxSliderVal_editingFinished()
{
    bool conversionOk = false;
    float num = ui.lineEditMaxSliderVal->text().toFloat(&conversionOk);
    if (conversionOk)
    {
        mSceneItem->mMaxVal = num;
    }
}

void SliderSceneItemEdit::on_horizontalSliderSliderKalinligi_sliderMoved(int value)
{
    mSceneItem->changeSliderWidth(value);
    ui.lineEditKalinlik->setText(QString::number(mSceneItem->mLineThickness, 'f', 0));
}

void SliderSceneItemEdit::on_horizontalSliderSliderLength_sliderMoved(int value)
{
    mSceneItem->changeSliderHeight(value);
    ui.lineEditSliderLength->setText(QString::number(mSceneItem->mLineLength, 'f', 0));
}

void SliderSceneItemEdit::on_lineEditKalinlik_editingFinished()
{
    bool conversionOk = false;
    int newValFromLineEdit = ui.lineEditKalinlik->text().toFloat(&conversionOk);
    if (conversionOk)
    {
        mSceneItem->changeSliderWidth(newValFromLineEdit);
        ui.horizontalSliderSliderKalinligi->setValue(newValFromLineEdit);
    }
    else
    {
        ui.lineEditKalinlik->setText(QString::number(mSceneItem->mLineThickness, 'f', 0));
    } 
}

void SliderSceneItemEdit::on_lineEditSliderLength_editingFinished()
{
    bool conversionOk = false;
    int newValFromLineEdit = ui.lineEditSliderLength->text().toFloat(&conversionOk);
    if (conversionOk)
    {
        mSceneItem->changeSliderHeight(newValFromLineEdit);
        ui.horizontalSliderSliderLength->setValue(newValFromLineEdit);
    }
    else
    {
        ui.lineEditSliderLength->setText(QString::number(mSceneItem->mLineLength, 'f', 0));
    }
}

void SliderSceneItemEdit::on_horizontalSliderMinMaxDeneme_sliderMoved(int value)
{
    mSceneItem->changeSliderPercentage(value+1);
    ui.labelPercentage->setText(QString::number(value + 1));
}

void SliderSceneItemEdit::on_textEditCss_textChanged()
{
    mSceneItem->changeSliderCss(ui.textEditCss->toPlainText());
}
