#include "BarSceneItemEdit.h"
#include "qcolordialog.h"

BarSceneItemEdit::BarSceneItemEdit(BarCustomSceneItem* barItem,QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    mSceneItem = barItem;
    ui.comboBoxBarType->setCurrentIndex(barItem->mBarType);
    ui.lineEditMinBarVal->setText(QString::number(barItem->mMinVal,'f',2));
    ui.lineEditMaxBarVal->setText(QString::number(barItem->mMaxVal,'f',2));
    // Arc için kalınlık aralığını ayarla (2 katı limitler)
    if (barItem->mBarType == ARC_BAR || barItem->mBarType == ARC_BAR_REVERSE)
    {
        ui.horizontalSliderBarKalinligi->setMinimum(1);
        ui.horizontalSliderBarKalinligi->setMaximum(100); // 50'nin 2 katı
    }
    else
    {
        ui.horizontalSliderBarKalinligi->setMinimum(1);
        ui.horizontalSliderBarKalinligi->setMaximum(200); // 100'ün 2 katı
    }
    ui.horizontalSliderBarKalinligi->setValue(barItem->mLineThickness);
    ui.lineEditKalinlik->setText(QString::number(barItem->mLineThickness, 'f', 0));
    
    // Arc bar için açı, normal bar için uzunluk
    if (barItem->mBarType == ARC_BAR || barItem->mBarType == ARC_BAR_REVERSE)
    {
        // Arc için slider aralığını ayarla
        ui.horizontalSliderBarLength->setMinimum(10);
        ui.horizontalSliderBarLength->setMaximum(700); // 2 katı limit
        // Arc span angle değerini kontrol et ve koru
        int safeSpanAngle = barItem->mArcSpanAngle;
        if (safeSpanAngle < 10) 
        {
            safeSpanAngle = 90; // Default görünür değer
            barItem->mArcSpanAngle = safeSpanAngle; // Değeri kaydet
        }
        if (safeSpanAngle > 700) 
        {
            safeSpanAngle = 700; // Maksimum değer
            barItem->mArcSpanAngle = safeSpanAngle; // Değeri kaydet
        }
        
        ui.horizontalSliderBarLength->setValue(safeSpanAngle);
        ui.lineEditBarLength->setText(QString::number(safeSpanAngle, 'f', 0));
        
        // Bar uzunluğu label'ını değiştir
        ui.label_7->setText("Bar Uzunluğu (Arc Açısı):");
    }
    else
    {
        // Normal bar için slider aralığını geri yükle
        ui.horizontalSliderBarLength->setMinimum(2);
        ui.horizontalSliderBarLength->setMaximum(1024);
        ui.horizontalSliderBarLength->setValue(barItem->mLineLength);
        ui.lineEditBarLength->setText(QString::number(barItem->mLineLength, 'f', 0));
        
        // Bar uzunluğu label'ını geri yükle
        ui.label_7->setText("Bar uzunluğu:");
    }
    ui.textEditCss->setText(mSceneItem->mBarWidgetCss);
    
    // Arc bar kontrollerini başlat
    ui.horizontalSliderArcRadius->setValue(barItem->mArcRadius);
    ui.lineEditArcRadius->setText(QString::number(barItem->mArcRadius, 'f', 0));
    ui.horizontalSliderBarRadius->setValue(barItem->mBarRadius);
    ui.lineEditBarRadius->setText(QString::number(barItem->mBarRadius, 'f', 0));
    ui.horizontalSliderArcBend->setValue(barItem->mArcBend);
    ui.lineEditArcBend->setText(QString::number(barItem->mArcBend, 'f', 0));
    ui.horizontalSliderArcStartAngle->setValue(barItem->mArcStartAngle);
    ui.lineEditArcStartAngle->setText(QString::number(barItem->mArcStartAngle, 'f', 0));
    
    // Progress bar min/max değerlerini ayarla
    if (barItem->mProgressBar)
    {
        barItem->mProgressBar->setMinimum((int)barItem->mMinVal);
        barItem->mProgressBar->setMaximum((int)barItem->mMaxVal);
    }
    
    // Başlangıçta kontrolleri bar tipine göre ayarla
    if (barItem->mBarType == ARC_BAR || barItem->mBarType == ARC_BAR_REVERSE)
    {
        // Arc kontrolleri göster
        ui.horizontalSliderArcRadius->setVisible(true);
        ui.lineEditArcRadius->setVisible(true);
        ui.horizontalSliderBarRadius->setVisible(true);
        ui.lineEditBarRadius->setVisible(true);
        ui.horizontalSliderArcBend->setVisible(true);
        ui.lineEditArcBend->setVisible(true);
        ui.horizontalSliderArcStartAngle->setVisible(true);
        ui.lineEditArcStartAngle->setVisible(true);
        
        // Bar uzunluğu kontrollerini göster (Arc için açı kontrolü olarak)
        ui.horizontalSliderBarLength->setVisible(true);
        ui.lineEditBarLength->setVisible(true);
    }
    else
    {
        // Arc kontrolleri gizle
        ui.horizontalSliderArcRadius->setVisible(false);
        ui.lineEditArcRadius->setVisible(false);
        ui.horizontalSliderBarRadius->setVisible(false);
        ui.lineEditBarRadius->setVisible(false);
        ui.horizontalSliderArcBend->setVisible(false);
        ui.lineEditArcBend->setVisible(false);
        ui.horizontalSliderArcStartAngle->setVisible(false);
        ui.lineEditArcStartAngle->setVisible(false);
        
        // Normal bar kontrolleri göster
        ui.horizontalSliderBarLength->setVisible(true);
        ui.lineEditBarLength->setVisible(true);
    }
}

BarSceneItemEdit::~BarSceneItemEdit()
{

}

void BarSceneItemEdit::on_comboBoxBarType_currentIndexChanged(int val)
{
    mSceneItem->changeBarType((BarType)val);
    
    // Radial progress bar tipi seçildiğinde radial bar'ı oluştur
    if (val == ARC_BAR || val == ARC_BAR_REVERSE)
    {
        // changeBarType zaten radial bar'ı oluşturuyor, burada sadece UI kontrollerini ayarlayalım
        if (mSceneItem->mRadialBar)
        {
            mSceneItem->mRadialBar->setVisible(true);
            mSceneItem->mRadialBar->updateProgress();
        }
        
        // Arc kontrolleri göster
        ui.horizontalSliderArcRadius->setVisible(true);
        ui.lineEditArcRadius->setVisible(true);
        ui.horizontalSliderBarRadius->setVisible(true);
        ui.lineEditBarRadius->setVisible(true);
        ui.horizontalSliderArcStartAngle->setVisible(true);
        ui.lineEditArcStartAngle->setVisible(true);
        
        // Bar uzunluğu kontrolü göster (Arc için açı kontrolü olarak)
        ui.horizontalSliderBarLength->setVisible(true);
        ui.lineEditBarLength->setVisible(true);
        
        // Arc için slider aralıklarını ayarla (2 katı limitler)
        ui.horizontalSliderBarLength->setMinimum(10);
        ui.horizontalSliderBarLength->setMaximum(700); // 350'nin 2 katı
        ui.horizontalSliderBarKalinligi->setMinimum(1);
        ui.horizontalSliderBarKalinligi->setMaximum(100); // 50'nin 2 katı
        
        // Mevcut değerleri güvenli aralıkta güncelle
        int currentSpanAngle = mSceneItem->mArcSpanAngle;
        if (currentSpanAngle < 10) currentSpanAngle = 90; // Default görünür değer
        if (currentSpanAngle > 700) currentSpanAngle = 700;
        
        ui.horizontalSliderBarLength->setValue(currentSpanAngle);
        ui.lineEditBarLength->setText(QString::number(currentSpanAngle, 'f', 0));
        
        // Label'ı arc için değiştir
        ui.label_7->setText("Yay Uzunluğu (10-700°):");
        
        // Arc span angle'ı da güncelle
        mSceneItem->mArcSpanAngle = currentSpanAngle;
    }
    else
    {
        // Normal bar tipine geçerken radial bar'ı gizle
        if (mSceneItem->mRadialBar)
        {
            mSceneItem->mRadialBar->setVisible(false);
        }
        
        // Progress bar'ı geri yükle
        if (mSceneItem->mProgressBar)
        {
            mSceneItem->mProgressBar->setVisible(true);
            mSceneItem->mProgressBar->show();
        }
        
        // Arc kontrolleri gizle
        ui.horizontalSliderArcRadius->setVisible(false);
        ui.lineEditArcRadius->setVisible(false);
        ui.horizontalSliderBarRadius->setVisible(false);
        ui.lineEditBarRadius->setVisible(false);
        ui.horizontalSliderArcStartAngle->setVisible(false);
        ui.lineEditArcStartAngle->setVisible(false);
        
        // Normal bar kontrolleri göster
        ui.horizontalSliderBarLength->setVisible(true);
        ui.lineEditBarLength->setVisible(true);
    }
}

void BarSceneItemEdit::on_lineEditMinBarVal_editingFinished()
{
    bool conversionOk = false;
    float num = ui.lineEditMinBarVal->text().toFloat(&conversionOk);
    if (conversionOk)
    {
        mSceneItem->mMinVal = num;
        // Progress bar'ın min/max değerlerini güncelle
        if (mSceneItem->mProgressBar)
        {
            mSceneItem->mProgressBar->setMinimum((int)mSceneItem->mMinVal);
            mSceneItem->mProgressBar->setMaximum((int)mSceneItem->mMaxVal);
        }
        // Radial bar'ı da güncelle
        if (mSceneItem->mRadialBar)
        {
            mSceneItem->mRadialBar->updateProgress();
        }
    }
}

void BarSceneItemEdit::on_lineEditMaxBarVal_editingFinished()
{
    bool conversionOk = false;
    float num = ui.lineEditMaxBarVal->text().toFloat(&conversionOk);
    if (conversionOk)
    {
        mSceneItem->mMaxVal = num;
        // Progress bar'ın min/max değerlerini güncelle
        if (mSceneItem->mProgressBar)
        {
            mSceneItem->mProgressBar->setMinimum((int)mSceneItem->mMinVal);
            mSceneItem->mProgressBar->setMaximum((int)mSceneItem->mMaxVal);
        }
        // Radial bar'ı da güncelle
        if (mSceneItem->mRadialBar)
        {
            mSceneItem->mRadialBar->updateProgress();
        }
    }
}

void BarSceneItemEdit::on_horizontalSliderBarKalinligi_sliderMoved(int value)
{
    mSceneItem->changeBarWidth(value);
    ui.lineEditKalinlik->setText(QString::number(mSceneItem->mLineThickness, 'f', 0));
}

void BarSceneItemEdit::on_horizontalSliderBarLength_sliderMoved(int value)
{
    // Arc bar için açı kontrolü, normal bar için uzunluk kontrolü
    if (mSceneItem->mBarType == ARC_BAR || mSceneItem->mBarType == ARC_BAR_REVERSE)
    {
        mSceneItem->changeArcSpanAngle(value);
        ui.lineEditBarLength->setText(QString::number(mSceneItem->mArcSpanAngle, 'f', 0));
        // Radial bar'ı güncelle
        if (mSceneItem->mRadialBar)
        {
            mSceneItem->mRadialBar->updateProgress();
        }
    }
    else
    {
        mSceneItem->changeBarHeight(value);
        ui.lineEditBarLength->setText(QString::number(mSceneItem->mLineLength, 'f', 0));
    }
}

void BarSceneItemEdit::on_lineEditKalinlik_editingFinished()
{
    bool conversionOk = false;
    int newValFromLineEdit = ui.lineEditKalinlik->text().toFloat(&conversionOk);
    if (conversionOk)
    {
        mSceneItem->changeBarWidth(newValFromLineEdit);
        ui.horizontalSliderBarKalinligi->setValue(newValFromLineEdit);
    }
    else
    {
        ui.lineEditKalinlik->setText(QString::number(mSceneItem->mLineThickness, 'f', 0));
    } 
}

void BarSceneItemEdit::on_lineEditBarLength_editingFinished()
{
    bool conversionOk = false;
    int newValFromLineEdit = ui.lineEditBarLength->text().toFloat(&conversionOk);
    if (conversionOk)
    {
        mSceneItem->changeBarHeight(newValFromLineEdit);
        ui.horizontalSliderBarLength->setValue(newValFromLineEdit);
    }
    else
    {
        ui.lineEditBarLength->setText(QString::number(mSceneItem->mLineLength, 'f', 0));
    }
}

void BarSceneItemEdit::on_pushButtonSelectColor_pressed()
{
    QColor color = QColorDialog::getColor(QColor(mSceneItem->mBarColor), this, "Bar Icin Renk Sec", QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
    mSceneItem->changeBarColor(color);
}

void BarSceneItemEdit::on_horizontalSliderMinMaxDeneme_sliderMoved(int value)
{
    mSceneItem->changeBarPercentage(value+1);
    ui.labelPercentage->setText(QString::number(value + 1));
    
    // Radial bar'ı da güncelle
    if (mSceneItem->mRadialBar)
    {
        mSceneItem->mRadialBar->updateProgress();
    }
}

void BarSceneItemEdit::on_textEditCss_textChanged()
{
    mSceneItem->changeBarCss(ui.textEditCss->toPlainText());
}

void BarSceneItemEdit::on_horizontalSliderArcRadius_sliderMoved(int value)
{
    mSceneItem->changeArcRadius(value);
    ui.lineEditArcRadius->setText(QString::number(value, 'f', 0));
}


void BarSceneItemEdit::on_lineEditArcRadius_editingFinished()
{
    bool conversionOk = false;
    int newValFromLineEdit = ui.lineEditArcRadius->text().toInt(&conversionOk);
    if (conversionOk && newValFromLineEdit >= 10 && newValFromLineEdit <= 200)
    {
        mSceneItem->changeArcRadius(newValFromLineEdit);
        ui.horizontalSliderArcRadius->setValue(newValFromLineEdit);
    }
    else
    {
        ui.lineEditArcRadius->setText(QString::number(mSceneItem->mArcRadius, 'f', 0));
    }
}


void BarSceneItemEdit::on_horizontalSliderBarRadius_sliderMoved(int value)
{
    mSceneItem->changeBarRadius(value);
    ui.lineEditBarRadius->setText(QString::number(value, 'f', 0));
}

void BarSceneItemEdit::on_lineEditBarRadius_editingFinished()
{
    bool conversionOk = false;
    int newValFromLineEdit = ui.lineEditBarRadius->text().toInt(&conversionOk);
    if (conversionOk && newValFromLineEdit >= -100 && newValFromLineEdit <= 100)
    {
        mSceneItem->changeBarRadius(newValFromLineEdit);
        ui.horizontalSliderBarRadius->setValue(newValFromLineEdit);
    }
    else
    {
        ui.lineEditBarRadius->setText(QString::number(mSceneItem->mBarRadius, 'f', 0));
    }
}

void BarSceneItemEdit::on_horizontalSliderArcStartAngle_sliderMoved(int value)
{
    mSceneItem->changeArcStartAngle(value);
    ui.lineEditArcStartAngle->setText(QString::number(value, 'f', 0));
}

void BarSceneItemEdit::on_lineEditArcStartAngle_editingFinished()
{
    bool conversionOk;
    int newValFromLineEdit = ui.lineEditArcStartAngle->text().toInt(&conversionOk);
    if (conversionOk)
    {
        mSceneItem->changeArcStartAngle(newValFromLineEdit);
        ui.horizontalSliderArcStartAngle->setValue(newValFromLineEdit);
    }
    else
    {
        ui.lineEditArcStartAngle->setText(QString::number(mSceneItem->mArcStartAngle, 'f', 0));
    }
}

void BarSceneItemEdit::on_horizontalSliderArcBend_sliderMoved(int value)
{
    mSceneItem->changeArcBend(value);
    ui.lineEditArcBend->setText(QString::number(value, 'f', 0));
}

void BarSceneItemEdit::on_lineEditArcBend_editingFinished()
{
    bool conversionOk = false;
    int newValFromLineEdit = ui.lineEditArcBend->text().toInt(&conversionOk);
    if (conversionOk && newValFromLineEdit >= -100 && newValFromLineEdit <= 100)
    {
        mSceneItem->changeArcBend(newValFromLineEdit);
        ui.horizontalSliderArcBend->setValue(newValFromLineEdit);
    }
    else
    {
        ui.lineEditArcBend->setText(QString::number(mSceneItem->mArcBend, 'f', 0));
    }
}