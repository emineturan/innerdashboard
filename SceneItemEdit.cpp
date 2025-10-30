#include "SceneItemEdit.h"
#include "ItemTypes.h"
#include "CustomSceneItem.h"
#include "IbreCustomSceneItem.h"
#include "BarCustomSceneItem.h"
#include "QGraphicsItem"
#include "qfiledialog.h"
#include "BarSceneItemEdit.h"
#include <CustomRectSceneItemEdit.h>
#include <SliderSceneItemEdit.h>
#include <InnerDashboardItemEdit.h>
#include "Inputs/PixmapInputSceneItem.h"
#include "Inputs/ButtonInputDialog.h"
#include <InnerWidgetItemEdit.h>
#include <QSettings>
// *** YENİ: Text editing için include'lar ***
#include <QColorDialog>
#include <QDebug>

SceneItemEdit::SceneItemEdit(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    QSettings settings;

    // Eğer InnerDashboardItem ise, InnerDashboardItemEdit'i layout'a ekle
    if (mCustomSceneItem->mTypeId == 7) {
        InnerDashboardItem* innerItem = (InnerDashboardItem*)mCustomSceneItem;
        InnerDashboardItemEdit* innerEdit = new InnerDashboardItemEdit(innerItem, this);

        // SceneItemEdit'in layout'una ekle (örnek: groupBox veya verticalLayout)
        // Senin UI yapına göre düzenle:
        ui.gridLayout->addWidget(innerEdit);  // veya başka bir layout
    }

    if (settings.contains("SceneItemEdit"))
    {
        QByteArray result = settings.value("SceneItemEdit").toByteArray();
        this->window()->restoreGeometry(result);
    }
}

SceneItemEdit::SceneItemEdit(CustomSceneItem* item, QWidget* parent /*= 0 */) :
    QDialog(parent),
    mSceneItem(item),
    mItemIsDeleted(false)
{
    qDebug() << "===== SceneItemEdit constructor START =====";

    ui.setupUi(this);

    qDebug() << "===== setupUi DONE =====";

    QSettings settings;
    if (settings.contains("SceneItemEdit"))
    {
        QByteArray result = settings.value("SceneItemEdit").toByteArray();
        this->window()->restoreGeometry(result);
    }

    ui.labelPrecision->setVisible(false);
    ui.lineEditPrecision->setVisible(false);

    // *** YENİ: Text düzenleme kontrollerini başta gizle ***
    ui.lineEditTextContent->setVisible(false);
    ui.fontComboBoxText->setVisible(false);
    ui.spinBoxFontSize->setVisible(false);
    ui.pushButtonSelectColor->setVisible(false);
    ui.labelColorPreview->setVisible(false);

    //these are only shown in text item
    ui.comboBoxTextAlign->setVisible(false);
    ui.checkBoxLineBreak->setVisible(false);
    ui.labelMaxUzunluk->setVisible(false);
    ui.lineEditMaxUzunluk->setVisible(false);
    ui.labelRotation->setVisible(false);
    ui.horizontalSliderRotationValue->setVisible(false);
    ui.lineEditRotationValue->setVisible(false);

    qDebug() << "Visibility settings DONE";

    if (mSceneItem->mItems.size() > 0)
    {
        ui.lbl_zValue->setText(QString::number(mSceneItem->mItems[0]->zValue()));
    }

    if (mSceneItem->mTypeId == GRAPHICS_TEXT_ITEM)
    {
        qDebug() << "Setting up TEXT ITEM...";

        ui.lineEditPrecision->setText(QString::fromStdString(mSceneItem->mConfigurationsMap.find("precision") != mSceneItem->mConfigurationsMap.end() ? mSceneItem->mConfigurationsMap["precision"] : ""));
        ui.comboBoxTextAlign->setVisible(true);
        ui.checkBoxLineBreak->setVisible(true);
        ui.labelMaxUzunluk->setVisible(true);
        ui.lineEditMaxUzunluk->setVisible(true);

        // *** YENİ: Text düzenleme kontrollerini göster ***
        ui.lineEditTextContent->setVisible(true);
        ui.fontComboBoxText->setVisible(true);
        ui.spinBoxFontSize->setVisible(true);
        ui.pushButtonSelectColor->setVisible(true);
        ui.labelColorPreview->setVisible(true);

        bool convertionOk;
        ui.comboBoxTextAlign->setCurrentIndex(mSceneItem->mConfigurationsMap.find("alignment") != mSceneItem->mConfigurationsMap.end() ? QString::fromStdString(mSceneItem->mConfigurationsMap["alignment"]).toInt(&convertionOk) : 0);
        ui.checkBoxLineBreak->setChecked(mSceneItem->mConfigurationsMap.find("lineBreak") != mSceneItem->mConfigurationsMap.end() ? QString::fromStdString(mSceneItem->mConfigurationsMap["lineBreak"]).toInt(&convertionOk) : 0);
        ui.lineEditMaxUzunluk->setText(mSceneItem->mConfigurationsMap.find("maxLength") != mSceneItem->mConfigurationsMap.end() ? QString::fromStdString(mSceneItem->mConfigurationsMap["maxLength"]) : "-1");

        // *** YENİ: Text item özelliklerini yükle (GEÇİCİ YORUM) ***
        try {
            loadTextItemProperties();
            qDebug() << "loadTextItemProperties SUCCESS";
        }
        catch (...) {
            qDebug() << "ERROR: loadTextItemProperties FAILED!";
        }

        qDebug() << "TEXT ITEM setup DONE";
    }

    ui.checkBoxIsOutput->setChecked(mSceneItem->mConfigurationsMap.find("mainAttributeIsOutput") != mSceneItem->mConfigurationsMap.end() && mSceneItem->mConfigurationsMap["mainAttributeIsOutput"].compare("true") == 0);

    if (mSceneItem->mConfigurationsMap.find("enableDisableAttrName") != mSceneItem->mConfigurationsMap.end() && mSceneItem->mConfigurationsMap["enableDisableAttrName"].length() > 0)
    {
        ui.checkBoxAdditionalAttr->setChecked(true);
        ui.lineEditAdditionalAttr->setText(QString::fromStdString(mSceneItem->mConfigurationsMap["enableDisableAttrName"]));
        ui.checkBoxAdditionalAttrIsCommand->setChecked(mSceneItem->mConfigurationsMap.find("enableDisableAttrIsCommandId") != mSceneItem->mConfigurationsMap.end() &&
            mSceneItem->mConfigurationsMap["enableDisableAttrIsCommandId"].compare("true") == 0);
    }
    else
    {
        ui.lineEditAdditionalAttr->setEnabled(false);
        ui.checkBoxAdditionalAttrIsCommand->setEnabled(false);
    }

    qDebug() << "Connecting signals...";

    connect(ui.checkBoxAdditionalAttr, SIGNAL(stateChanged(int)), this, SLOT(handleAdditionalAttrCheckBox(int)));
    connect(ui.horizontalSliderRotationValue, SIGNAL(valueChanged(int)), this, SLOT(handleItemRotation(int)));

    // *** YENİ: Renk seçimi butonu için signal bağlantısı ***
    connect(ui.pushButtonSelectColor, SIGNAL(clicked()), this, SLOT(on_pushButtonSelectColor_clicked()));
    // *** YENİ: Font değişikliklerini dinle ***
    connect(ui.fontComboBoxText, SIGNAL(currentFontChanged(QFont)), this, SLOT(on_fontComboBoxText_currentFontChanged(QFont)));
    connect(ui.spinBoxFontSize, SIGNAL(valueChanged(int)), this, SLOT(on_spinBoxFontSize_valueChanged(int)));

    qDebug() << "Signals connected";

    ui.comboBoxAttrType->addItem("Bool");
    ui.comboBoxAttrType->addItem("Int");
    ui.comboBoxAttrType->addItem("Float");
    ui.comboBoxAttrType->addItem("String");
    ui.comboBoxAttrType->setCurrentText(mSceneItem->mAttributeType);

    qDebug() << "Creating Ibre widget...";

    mIbreEditWidget = new IbreSceneItemEdit();
    mIbreEditWidget->hide();
    mIbreEditWidget->ui.groupBoxDiscrete->setVisible(false);
    mIbreEditWidget->ui.checkBoxDiscrete->setChecked(false);
    connect(mIbreEditWidget->ui.pushButtonSetMin, SIGNAL(clicked()), this, SLOT(ibreEditSetMinClicked()));
    connect(mIbreEditWidget->ui.pushButtonSetMax, SIGNAL(clicked()), this, SLOT(ibreEditSetMaxClicked()));
    connect(mIbreEditWidget->ui.pushButtonDeleteDiscreteValue, SIGNAL(clicked()), this, SLOT(ibreEditDeleteDiscreteValueClicked()));
    connect(mIbreEditWidget->ui.pushButtonSetDiscreteValue, SIGNAL(clicked()), this, SLOT(ibreEditSetDiscreteClicked()));
    connect(mIbreEditWidget->ui.checkBoxDiscrete, SIGNAL(toggled(bool)), this, SLOT(ibreEditCheckBoxToggled(bool)));
    connect(mIbreEditWidget->ui.comboBoxDiscreteValue, SIGNAL(currentIndexChanged(int)), this, SLOT(ibreEditCurrentIndexChanged(int)));
    connect(mIbreEditWidget->ui.horizontalSliderMinMax, SIGNAL(valueChanged(int)), this, SLOT(ibreEditMinMaxSliderValueChanged()));

    qDebug() << "Ibre widget created";

    // *** PIXMAP ITEMS - gridLayoutAddedImages YORUM SATIRI ***
    if (mSceneItem->mTypeId == GRAPHICS_PIXMAP_ITEM || mSceneItem->mTypeId == GRAPHICS_PIXMAP_BACKGROUND_ITEM || mSceneItem->mTypeId == GRAPHICS_PIXMAP_LIGHT_ITEM || mSceneItem->mTypeId == GRAPHICS_PIXMAP_IBRE_ITEM)
    {
        qDebug() << "Setting up PIXMAP items...";
        // Labels oluşturuluyor ama eklenmiyor (gridLayoutAddedImages yok)
        for (int i = 0; i < mSceneItem->mItems.size(); i++)
        {
            QLabel* newLabel = new QLabel();
            newLabel->setPixmap(((QGraphicsPixmapItem*)mSceneItem->mItems.at(i))->pixmap());
            QLabel* statusLabel = new QLabel(QString("index " + QString::number(i) + " icin"));
            // Not: Bu label'lar kullanılmıyor çünkü gridLayoutAddedImages widget'ı Qt Designer'da yok
        }
        qDebug() << "PIXMAP items setup DONE (labels not added)";
    }

    if (mSceneItem->mTypeId == GRAPHICS_PIXMAP_IBRE_ITEM)
    {
        qDebug() << "Setting up IBRE item...";
        IbreCustomSceneItem* ibreItem = (IbreCustomSceneItem*)(mSceneItem);
        ui.horizontalLayoutSpecialArea->addWidget(mIbreEditWidget);
        mIbreEditWidget->ui.lineEditMaxValue->setText(QString::number(ibreItem->mMaxIbreValue));
        mIbreEditWidget->ui.lineEditMinValue->setText(QString::number(ibreItem->mMinIbreValue));
        if (ibreItem->mDiscreteIbreValueList.size() > 2) {
            mIbreEditWidget->ui.groupBoxDiscrete->setVisible(true);
            mIbreEditWidget->ui.checkBoxDiscrete->setChecked(true);
        }

        for (int i = 0; i < ibreItem->mDiscreteIbreValueList.size(); i++) {
            mIbreEditWidget->ui.comboBoxDiscreteValue->addItem("Value : " + QString::number(ibreItem->mDiscreteIbreValueList.at(i)) + "  Angle : " + QString::number(ibreItem->mDiscreteIbreAngleList.at(i)));
        }
        mIbreEditWidget->show();
        qDebug() << "IBRE item setup DONE";
    }
    else if (mSceneItem->mTypeId == GRAPHICS_BAR_ITEM)
    {
        qDebug() << "Setting up BAR item...";
        BarSceneItemEdit* barEdit = new BarSceneItemEdit((BarCustomSceneItem*)mSceneItem);
        ui.horizontalLayoutSpecialArea->addWidget(barEdit);
        ui.pushButtonAddNewImage->hide();
    }
    else if (mSceneItem->mTypeId == GRAPHICS_RECT_ITEM)
    {
        qDebug() << "Setting up RECT item...";
        CustomRectSceneItemEdit* rectEdit = new CustomRectSceneItemEdit((CustomRectSceneItem*)mSceneItem);
        ui.horizontalLayoutSpecialArea->addWidget(rectEdit);
        ui.pushButtonAddNewImage->hide();
    }
    else if (mSceneItem->mTypeId == GRAPHICS_SLIDER_ITEM)
    {
        qDebug() << "Setting up SLIDER item...";
        SliderSceneItemEdit* sceneEdit = new SliderSceneItemEdit((SliderCustomSceneItem*)mSceneItem);
        ui.horizontalLayoutSpecialArea->addWidget(sceneEdit);
        ui.pushButtonAddNewImage->hide();
    }
    else if (mSceneItem->mTypeId == INNER_DASHBOARD_ITEM)
    {
        qDebug() << "Setting up INNER DASHBOARD item...";
        InnerDashboardItemEdit* sceneEdit = new InnerDashboardItemEdit((InnerDashboardItem*)mSceneItem);
        InnerDashboardItem* innerDashboardItem = (InnerDashboardItem*)mSceneItem;
        ui.horizontalLayoutSpecialArea->addWidget(sceneEdit);
        ui.pushButtonAddNewImage->hide();
        sceneEdit->ui.lineEditInnerDashboardSizeAsPercentage->setText(QString::number(innerDashboardItem->mInnerDashboardNewSizeAsPercentage));
    }
    else if (mSceneItem->mTypeId == INNER_WIDGET_ITEM)
    {
        qDebug() << "Setting up INNER WIDGET item...";
        InnerWidgetItemEdit* sceneEdit = new InnerWidgetItemEdit((InnerWidgetItem*)mSceneItem);
        ui.horizontalLayoutSpecialArea->addWidget(sceneEdit);
        ui.pushButtonAddNewImage->hide();
    }

    qDebug() << "Setting button states...";

    if (mSceneItem->mShowOnStart)
    {
        ui.pushButtonShowOnStart->setText(QStringLiteral("Dashboard Acildiginda Gosterilecek - Degistir"));
        ui.pushButtonShowOnStart->setChecked(true);
    }
    else
    {
        ui.pushButtonShowOnStart->setText(QStringLiteral("Dashboard Acildiginda Gosterilmeyecek - Degistir"));
        ui.pushButtonShowOnStart->setChecked(false);
    }

    ui.lineEditItemName->setText(mSceneItem->mItemName);
    ui.lineEditAttrName->setText(mSceneItem->mAttirubuteName);

    if (!mSceneItem->mIsItemShown)
    {
        ui.pushButtonShowHide->setText(QStringLiteral("Nesne Gorunmez, Gorunur Yap"));
        ui.pushButtonShowHide->setChecked(true);
    }

    if (mSceneItem->mTypeId == GRAPHICS_SLIDER_ITEM || mSceneItem->mTypeId == GRAPHICS_RECT_ITEM || mSceneItem->mTypeId == GRAPHICS_BAR_ITEM || mSceneItem->mTypeId == GRAPHICS_PIXMAP_ITEM || mSceneItem->mTypeId == GRAPHICS_TEXT_ITEM || mSceneItem->mTypeId == GRAPHICS_PIXMAP_LIGHT_ITEM)
    {
        ui.labelRotation->setVisible(true);
        ui.horizontalSliderRotationValue->setVisible(true);
        ui.lineEditRotationValue->setVisible(true);

        ui.lineEditRotationValue->setText(QString::number(mSceneItem->mRotationValue));
        ui.horizontalSliderRotationValue->setValue(mSceneItem->mRotationValue);
    }

    qDebug() << "===== Constructor END =====";
}

// *** YENİ: Text item özelliklerini yükle ***
void SceneItemEdit::loadTextItemProperties()
{
    qDebug() << "loadTextItemProperties START";

    if (mSceneItem->mItems.size() == 0)
    {
        qDebug() << "ERROR: No items in mSceneItem";
        return;
    }

    qDebug() << "Item count:" << mSceneItem->mItems.size();

    QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(mSceneItem->mItems.at(0));
    if (!textItem)
    {
        qDebug() << "WARNING: Item is not QGraphicsTextItem, trying QGraphicsSimpleTextItem...";

        QGraphicsSimpleTextItem* simpleTextItem = dynamic_cast<QGraphicsSimpleTextItem*>(mSceneItem->mItems.at(0));
        if (simpleTextItem)
        {
            qDebug() << "Item is QGraphicsSimpleTextItem";
            ui.lineEditTextContent->setText(simpleTextItem->text());
            ui.fontComboBoxText->setCurrentFont(simpleTextItem->font());

            QFont currentFont = simpleTextItem->font();
            int fontSize = currentFont.pointSize();
            if (fontSize <= 0) fontSize = 12;
            ui.spinBoxFontSize->setValue(fontSize);

            mCurrentTextColor = simpleTextItem->brush().color();
            updateColorPreview();
            qDebug() << "loadTextItemProperties DONE (SimpleText)";
            return;
        }

        qDebug() << "WARNING: Item type unknown, skipping property load";
        return;
    }

    qDebug() << "Loading QGraphicsTextItem properties...";

    // Text içeriğini yükle
    ui.lineEditTextContent->setText(textItem->toPlainText());

    // Font yükle
    QFont currentFont = textItem->font();
    ui.fontComboBoxText->setCurrentFont(currentFont);

    int fontSize = currentFont.pointSize();
    if (fontSize <= 0) fontSize = 12;
    ui.spinBoxFontSize->setValue(fontSize);

    // Renk yükle
    mCurrentTextColor = textItem->defaultTextColor();
    updateColorPreview();

    qDebug() << "loadTextItemProperties DONE";
}

// *** YENİ: Renk önizlemesini güncelle ***
void SceneItemEdit::updateColorPreview()
{
    QString styleSheet = QString("QLabel { background-color: %1; border: 2px solid black; }")
        .arg(mCurrentTextColor.name());
    ui.labelColorPreview->setStyleSheet(styleSheet);
    ui.labelColorPreview->setText(mCurrentTextColor.name());
}

// *** YENİ: Renk seçimi butonu ***
void SceneItemEdit::on_pushButtonSelectColor_clicked()
{
    qDebug() << "Color selection button clicked";

    QColor selectedColor = QColorDialog::getColor(
        mCurrentTextColor,
        this,
        QString::fromUtf8("Yazı Rengini Seç"),
        QColorDialog::DontUseNativeDialog
    );

    if (selectedColor.isValid())
    {
        mCurrentTextColor = selectedColor;
        updateColorPreview();
        qDebug() << "New color selected:" << selectedColor.name();
    }
}

// *** YENİ: Font değiştiğinde ***
void SceneItemEdit::on_fontComboBoxText_currentFontChanged(const QFont& font)
{
    qDebug() << "Font changed to:" << font.family();
}

// *** YENİ: Font boyutu değiştiğinde ***
void SceneItemEdit::on_spinBoxFontSize_valueChanged(int value)
{
    qDebug() << "Font size changed to:" << value;
}

// *** YENİ: Text item özelliklerini kaydet ***
void SceneItemEdit::saveTextItemProperties()
{
    if (mSceneItem->mItems.size() == 0)
        return;

    QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(mSceneItem->mItems.at(0));
    if (!textItem)
    {
        qDebug() << "WARNING: Cannot save, item is not QGraphicsTextItem";
        return;
    }

    qDebug() << "Saving text item properties...";

    // Text içeriğini kaydet
    QString newText = ui.lineEditTextContent->text();
    if (newText != textItem->toPlainText())
    {
        textItem->setPlainText(newText);
        qDebug() << "Text saved:" << newText;
    }

    // Font kaydet
    QFont newFont = ui.fontComboBoxText->currentFont();
    newFont.setPointSize(ui.spinBoxFontSize->value());
    textItem->setFont(newFont);
    qDebug() << "Font saved:" << newFont.family() << "Size:" << newFont.pointSize();

    // Renk kaydet
    textItem->setDefaultTextColor(mCurrentTextColor);
    qDebug() << "Color saved:" << mCurrentTextColor.name();
}

void SceneItemEdit::on_comboBoxAttrType_currentIndexChanged(int index)
{
    if (mSceneItem->mTypeId == GRAPHICS_TEXT_ITEM)
    {
        if (index == 1 || index == 2)
        {
            ui.labelPrecision->setVisible(true);
            ui.lineEditPrecision->setVisible(true);
        }
        else
        {
            ui.labelPrecision->setVisible(false);
            ui.lineEditPrecision->setVisible(false);
        }
    }
}

void SceneItemEdit::on_pushButtonAddNewImage_clicked()
{
    if (mSceneItem->mTypeId != GRAPHICS_TEXT_ITEM)
    {
        QString selectedImage1 = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("Resim secin."),
            "",
            tr("Image Files (*.png *.jpg)"));
        if (selectedImage1.size() > 0)
        {
            mNewlyAddedImagePaths.push_back(selectedImage1);
            // Not: gridLayoutAddedImages yok, bu yüzden preview eklenmiyor
        }
    }
}

void SceneItemEdit::on_pushButtonShowOnStart_toggled(bool value)
{
    if (value)
    {
        ui.pushButtonShowOnStart->setText(QStringLiteral("Dashboard Acildiginda Gosterilecek - Degistir"));
    }
    else
    {
        ui.pushButtonShowOnStart->setText(QStringLiteral("Dashboard Acildiginda Gosterilmeyecek - Degistir"));
    }
}

void SceneItemEdit::on_pushButtonSave_clicked()
{
    qDebug() << "Save button clicked";

    // *** YENİ: Text item ise önce text özelliklerini kaydet ***
    if (mSceneItem->mTypeId == GRAPHICS_TEXT_ITEM)
    {
        saveTextItemProperties();
    }

    mSceneItem->mShowOnStart = ui.pushButtonShowOnStart->isChecked();
    mSceneItem->mItemName = ui.lineEditItemName->text();
    mSceneItem->mAttirubuteName = ui.lineEditAttrName->text();
    mSceneItem->mAttributeType = ui.comboBoxAttrType->currentText();
    mSceneItem->mRotationValue = ui.lineEditRotationValue->text().toInt();

    if (ui.lineEditPrecision->isVisible() && ui.lineEditPrecision->text().size() > 0 && mSceneItem->mTypeId == GRAPHICS_TEXT_ITEM)
    {
        mSceneItem->mConfigurationsMap["precision"] = ui.lineEditPrecision->text().toStdString();
    }
    else
    {
        if (mSceneItem->mConfigurationsMap.find("precision") != mSceneItem->mConfigurationsMap.end())
        {
            mSceneItem->mConfigurationsMap.erase("precision");
        }
    }

    if (mSceneItem->mTypeId == GRAPHICS_TEXT_ITEM)
    {
        mSceneItem->mConfigurationsMap["alignment"] = QString::number(ui.comboBoxTextAlign->currentIndex()).toStdString();
        mSceneItem->mConfigurationsMap["lineBreak"] = QString::number(ui.checkBoxLineBreak->isChecked()).toStdString();
        mSceneItem->mConfigurationsMap["maxLength"] = ui.lineEditMaxUzunluk->text().toStdString();
    }

    if (ui.checkBoxIsOutput->isChecked())
    {
        mSceneItem->mConfigurationsMap["mainAttributeIsOutput"] = "true";
    }
    else
    {
        if (mSceneItem->mConfigurationsMap.find("mainAttributeIsOutput") != mSceneItem->mConfigurationsMap.end())
        {
            mSceneItem->mConfigurationsMap.erase("mainAttributeIsOutput");
        }
    }

    if (ui.checkBoxAdditionalAttr->isChecked())
    {
        mSceneItem->mConfigurationsMap["enableDisableAttrName"] = ui.lineEditAdditionalAttr->text().toStdString();
        if (ui.checkBoxAdditionalAttrIsCommand->isChecked())
        {
            mSceneItem->mConfigurationsMap["enableDisableAttrIsCommandId"] = "true";
        }
        else
        {
            if (mSceneItem->mConfigurationsMap.find("enableDisableAttrIsCommandId") != mSceneItem->mConfigurationsMap.end())
            {
                mSceneItem->mConfigurationsMap.erase("enableDisableAttrIsCommandId");
            }
        }
    }
    else
    {
        if (mSceneItem->mConfigurationsMap.find("enableDisableAttrIsCommandId") != mSceneItem->mConfigurationsMap.end())
        {
            mSceneItem->mConfigurationsMap.erase("enableDisableAttrIsCommandId");
        }
        if (mSceneItem->mConfigurationsMap.find("enableDisableAttrName") != mSceneItem->mConfigurationsMap.end())
        {
            mSceneItem->mConfigurationsMap.erase("enableDisableAttrName");
        }
    }

    qDebug() << "Save completed, closing dialog";
    done(QDialog::Accepted);
}

void SceneItemEdit::on_pushButtonDelete_clicked()
{
    mItemIsDeleted = true;
    done(QDialog::Accepted);
}

SceneItemEdit::~SceneItemEdit()
{
}

void SceneItemEdit::moveEvent(QMoveEvent* event)
{
    QSettings settings;
    settings.setValue("SceneItemEdit", this->window()->saveGeometry());
}

void SceneItemEdit::ibreEditSetMinClicked()
{
    IbreCustomSceneItem* ibreItem = (IbreCustomSceneItem*)(mSceneItem);
    bool ok = false;
    float minVal = mIbreEditWidget->ui.lineEditMinValue->text().toFloat(&ok);
    if (!ok)
    {
        minVal = 0;
    }
    ibreItem->mMinIbreValue = minVal;
    ibreItem->mMinIbreAngle = ibreItem->mItems.at(0)->rotation();

    ibreItem->mDiscreteIbreValueList.push_back(minVal);
    ibreItem->mDiscreteIbreAngleList.push_back(ibreItem->mItems.at(0)->rotation());
    mIbreEditWidget->ui.comboBoxDiscreteValue->addItem("Value : " + QString::number(minVal) + "  Angle : " + QString::number(ibreItem->mItems.at(0)->rotation()));
}

void SceneItemEdit::ibreEditSetMaxClicked()
{
    IbreCustomSceneItem* ibreItem = (IbreCustomSceneItem*)(mSceneItem);
    bool ok = false;
    float maxVal = mIbreEditWidget->ui.lineEditMaxValue->text().toFloat(&ok);
    if (!ok)
    {
        maxVal = 0;
    }
    ibreItem->mMaxIbreValue = maxVal;
    ibreItem->mMaxIbreAngle = ibreItem->mItems.at(0)->rotation();

    ibreItem->mDiscreteIbreValueList.push_back(maxVal);
    ibreItem->mDiscreteIbreAngleList.push_back(ibreItem->mItems.at(0)->rotation());
    mIbreEditWidget->ui.comboBoxDiscreteValue->addItem("Value : " + QString::number(maxVal) + "  Angle : " + QString::number(ibreItem->mItems.at(0)->rotation()));
}

void SceneItemEdit::ibreEditSetDiscreteClicked()
{
    IbreCustomSceneItem* ibreItem = (IbreCustomSceneItem*)(mSceneItem);
    bool ok = false;
    float discreteVal = mIbreEditWidget->ui.lineEditDiscreteValue->text().toFloat(&ok);
    if (!ok)
    {
        discreteVal = 0;
    }
    ibreItem->mDiscreteIbreValueList.push_back(discreteVal);
    ibreItem->mDiscreteIbreAngleList.push_back(ibreItem->mItems.at(0)->rotation());
    mIbreEditWidget->ui.comboBoxDiscreteValue->addItem("Value : " + QString::number(discreteVal) + "  Angle : " + QString::number(ibreItem->mItems.at(0)->rotation()));
}

void SceneItemEdit::ibreEditCurrentIndexChanged(int index)
{
    if (index >= 0) {
        IbreCustomSceneItem* ibreItem = (IbreCustomSceneItem*)(mSceneItem);
        ibreItem->mCurrentDiscreteValueIndex = index;
    }
}

void SceneItemEdit::ibreEditDeleteDiscreteValueClicked()
{
    IbreCustomSceneItem* ibreItem = (IbreCustomSceneItem*)(mSceneItem);
    if (ibreItem->mDiscreteIbreValueList.size() > 0) {
        ibreItem->mDiscreteIbreValueList.erase(ibreItem->mDiscreteIbreValueList.begin() + ibreItem->mCurrentDiscreteValueIndex);
        ibreItem->mDiscreteIbreAngleList.erase(ibreItem->mDiscreteIbreAngleList.begin() + ibreItem->mCurrentDiscreteValueIndex);
        mIbreEditWidget->ui.comboBoxDiscreteValue->removeItem(ibreItem->mCurrentDiscreteValueIndex);
    }
}

void SceneItemEdit::ibreEditMinMaxSliderValueChanged()
{
    IbreCustomSceneItem* ibreItem = (IbreCustomSceneItem*)(mSceneItem);
    int sliderVal = this->mIbreEditWidget->ui.horizontalSliderMinMax->value();

    float currentAngle = ibreItem->mMinIbreAngle + ((ibreItem->mMaxIbreAngle - ibreItem->mMinIbreAngle) * (((float)sliderVal + 1) / 100));
    ibreItem->mItems.at(0)->setRotation(currentAngle);
}

void SceneItemEdit::ibreEditCheckBoxToggled(bool check)
{
    if (check) {
        mIbreEditWidget->ui.groupBoxDiscrete->setVisible(true);
    }
    else {
        mIbreEditWidget->ui.groupBoxDiscrete->setVisible(false);
    }
}

void SceneItemEdit::on_pushButtonOneCikar_clicked()
{
    for (int i = 0; i < mSceneItem->mItems.size(); i++)
    {
        mSceneItem->mItems.at(i)->setZValue(mSceneItem->mItems.at(i)->zValue() + 1);
    }

    if (mSceneItem->mItems.size() > 0)
    {
        ui.lbl_zValue->setText(QString::number(mSceneItem->mItems[0]->zValue()));
    }
}

void SceneItemEdit::on_pushButtonArkayaGetir_clicked()
{
    for (int i = 0; i < mSceneItem->mItems.size(); i++)
    {
        mSceneItem->mItems.at(i)->setZValue(mSceneItem->mItems.at(i)->zValue() - 1);
    }

    if (mSceneItem->mItems.size() > 0)
    {
        ui.lbl_zValue->setText(QString::number(mSceneItem->mItems[0]->zValue()));
    }
}

void SceneItemEdit::on_pushButtonShowHide_toggled(bool value)
{
    mSceneItem->updateShowStatus(!value);
    if (value)
    {
        ui.pushButtonShowHide->setText(QStringLiteral("Nesne Gorunmez, Gorunur Yap"));
    }
    else
    {
        ui.pushButtonShowHide->setText(QStringLiteral("Nesne Gorunmez, Gorunmez Yap"));
    }
}

void SceneItemEdit::handleAdditionalAttrCheckBox(int state)
{
    if (state == 0)
    {
        ui.lineEditAdditionalAttr->setEnabled(false);
        ui.checkBoxAdditionalAttrIsCommand->setEnabled(false);
    }
    else
    {
        ui.lineEditAdditionalAttr->setEnabled(true);
        ui.checkBoxAdditionalAttrIsCommand->setEnabled(true);
    }
}

void SceneItemEdit::handleItemRotation(int rotationValue)
{
    ui.lineEditRotationValue->setText(QString::number(rotationValue));

    mSceneItem->mItems.at(mSceneItem->mCurrentActiveItem)->setTransformOriginPoint(mSceneItem->mItems.at(0)->boundingRect().width() / 2, mSceneItem->mItems.at(0)->boundingRect().height() / 2);
    mSceneItem->mItems.at(mSceneItem->mCurrentActiveItem)->setRotation(rotationValue);

    BarCustomSceneItem* barItem = dynamic_cast<BarCustomSceneItem*>(mSceneItem);
    if (barItem && (barItem->mBarType == ARC_BAR || barItem->mBarType == ARC_BAR_REVERSE))
    {
        if (barItem->mRadialBar)
        {
            barItem->mRadialBar->setRotation(rotationValue);
            barItem->mRadialBar->update();
        }
    }
}
