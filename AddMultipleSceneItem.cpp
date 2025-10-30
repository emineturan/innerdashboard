#include "AddMultipleSceneItem.h"
#include "qfiledialog.h"
#include "qinputdialog.h"
#include "qfontdialog.h"
#include "qcolordialog.h"

AddMultipleSceneItem::AddMultipleSceneItem(int type, QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    ui.labelPrecision->setVisible(false);
    ui.lineEditPrecision->setVisible(false);
    ui.comboBoxAttrType->addItem("Bool");
    ui.comboBoxAttrType->addItem("Int");
    ui.comboBoxAttrType->addItem("Float");
    ui.comboBoxAttrType->addItem("String");
    ui.comboBoxAttrType->setCurrentIndex(0);

    //disable additional attr segment
    ui.lineEditAdditionalAttr->setEnabled(false);;
    ui.checkBoxAdditionalAttrIsCommand->setEnabled(false);
    connect(ui.checkBoxAdditionalAttr, SIGNAL(stateChanged(int)), this, SLOT(handleAdditionalAttrCheckBox(int)));

    //these are only shown in text item
    ui.comboBoxTextAlign->setVisible(false);
    ui.checkBoxLineBreak->setVisible(false);
    ui.labelMaxUzunluk->setVisible(false);
    ui.lineEditMaxUzunluk->setVisible(false);


    mType = type;

    if (mType == GRAPHICS_PIXMAP_LIGHT_ITEM || mType == GRAPHICS_PIXMAP_MULTIPLE_LIGHT_ITEM)
    {
        //take on off infos
        QString selectedImage1 = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("Isik kapaliykenki resmi secin."),
            "",
            tr("Image Files (*.png *.jpg)"));
        if (selectedImage1.size() > 0)
        {
            mNewlyAddedImagePaths.push_back(selectedImage1);
            QLabel* newLabel = new QLabel();
            QPixmap newPixMap(selectedImage1);
            /***********************/
            newLabel->setPixmap(newPixMap);
            /***********************/
            ui.gridLayoutAddedImages->addWidget(newLabel, mNewlyAddedImagePaths.size() - 1, 0);
            QLabel* statusLabel = new QLabel(QStringLiteral("Isik Kapali Resmi (index 0 icin)"));
            ui.gridLayoutAddedImages->addWidget(statusLabel, mNewlyAddedImagePaths.size() - 1, 1);
        }
        QString selectedImage2 = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("Isik acikkenki resmi secin."),
            "",
            tr("Image Files (*.png *.jpg)"));
        if (selectedImage2.size() > 0)
        {
            mNewlyAddedImagePaths.push_back(selectedImage2);
            QLabel* newLabel = new QLabel();
            QPixmap newPixMap(selectedImage2);
            /***********************/
            newLabel->setPixmap(newPixMap);
            /***********************/
            ui.gridLayoutAddedImages->addWidget(newLabel, mNewlyAddedImagePaths.size() - 1, 0);
            QLabel* statusLabel = new QLabel(QStringLiteral("Isik Acik Resmi (index 1 icin)"));
            ui.gridLayoutAddedImages->addWidget(statusLabel, mNewlyAddedImagePaths.size() - 1, 1);
        }
    }
    
    else if (mType == GRAPHICS_PIXMAP_IBRE_ITEM)
    {
        QString selectedImage1 = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("Ibre Resmi Secin"),
            "",
            tr("Image Files (*.png *.jpg)"));
        if (selectedImage1.size() > 0)
        {
            mNewlyAddedImagePaths.push_back(selectedImage1);
            QLabel* newLabel = new QLabel();
            QPixmap newPixMap(selectedImage1);
            /***********************/
            newLabel->setPixmap(newPixMap);
            /***********************/
            ui.gridLayoutAddedImages->addWidget(newLabel, mNewlyAddedImagePaths.size() - 1, 0);
            QLabel* statusLabel = new QLabel(QStringLiteral("Ibre Resmi (index 0 icin)"));
            ui.gridLayoutAddedImages->addWidget(statusLabel, mNewlyAddedImagePaths.size() - 1, 1);
            ui.pushButtonAddNewImage->setDisabled(true);
        }
    }
    
    else if (mType == GRAPHICS_PIXMAP_BACKGROUND_ITEM)
    {
        QString selectedImage1 = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("Arkaplan Resmi Secin"),
            "",
            tr("Image Files (*.png *.jpg)"));
        if (selectedImage1.size() > 0)
        {
            mNewlyAddedImagePaths.push_back(selectedImage1);
            QLabel* newLabel = new QLabel();
            QPixmap newPixMap(selectedImage1);
            /***********************/
            newLabel->setPixmap(newPixMap.scaled(QSize(newPixMap.width()/2,newPixMap.height()/2)));
            /***********************/
            ui.gridLayoutAddedImages->addWidget(newLabel, mNewlyAddedImagePaths.size() - 1, 0);
            QLabel* statusLabel = new QLabel(QStringLiteral("Arka plan Resmi (index 0 icin)"));
            ui.gridLayoutAddedImages->addWidget(statusLabel, mNewlyAddedImagePaths.size() - 1, 1);
            ui.pushButtonAddNewImage->setDisabled(true);
            ui.comboBoxAttrType->setDisabled(true);
            ui.lineEditAttrName->setDisabled(true);
            ui.checkBoxShowOnStart->setChecked(true);
            ui.checkBoxShowOnStart->setDisabled(true);
        }
    }
    
    else if (mType == GRAPHICS_PIXMAP_ITEM)
    {
        QString selectedImage1 = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("Tek Resmi Secin"),
            "",
            tr("Image Files (*.png *.jpg)"));
        if (selectedImage1.size() > 0)
        {
            mNewlyAddedImagePaths.push_back(selectedImage1);
            QLabel* newLabel = new QLabel();
            QPixmap newPixMap(selectedImage1);
            /***********************/
            newLabel->setPixmap(newPixMap);
            /***********************/
            ui.gridLayoutAddedImages->addWidget(newLabel, mNewlyAddedImagePaths.size() - 1, 0);
            QLabel* statusLabel = new QLabel(QStringLiteral("Tek Resim (index 0 icin)"));
            ui.gridLayoutAddedImages->addWidget(statusLabel, mNewlyAddedImagePaths.size() - 1, 1);
            ui.pushButtonAddNewImage->setDisabled(true);
        }
    }
    
    //else if (mType == GRAPHICS_PIXMAP_BUTTON_INPUT_ITEM || mType)
    //{
    //    //take on off infos
    //    QString selectedImage1 = QFileDialog::getOpenFileName(
    //        this,
    //        QStringLiteral("Buton basili degilkenki resmi secin."),
    //        "",
    //        tr("Image Files (*.png *.jpg)"));
    //    if (selectedImage1.size() > 0)
    //    {
    //        mNewlyAddedImagePaths.push_back(selectedImage1);
    //        QLabel* newLabel = new QLabel();
    //        QPixmap newPixMap(selectedImage1);
    //        /***********************/
    //        newLabel->setPixmap(newPixMap);
    //        /***********************/
    //        ui.gridLayoutAddedImages->addWidget(newLabel, mNewlyAddedImagePaths.size() - 1, 0);
    //        QLabel* statusLabel = new QLabel(QStringLiteral("Buton basili degil Resmi (index 0 icin)"));
    //        ui.gridLayoutAddedImages->addWidget(statusLabel, mNewlyAddedImagePaths.size() - 1, 1);
    //    }
    //    QString selectedImage2 = QFileDialog::getOpenFileName(
    //        this,
    //        QStringLiteral("Buton basili iken resmi secin."),
    //        "",
    //        tr("Image Files (*.png *.jpg)"));
    //    if (selectedImage2.size() > 0)
    //    {
    //        mNewlyAddedImagePaths.push_back(selectedImage2);
    //        QLabel* newLabel = new QLabel();
    //        QPixmap newPixMap(selectedImage2);
    //        /***********************/
    //        newLabel->setPixmap(newPixMap);
    //        /***********************/
    //        ui.gridLayoutAddedImages->addWidget(newLabel, mNewlyAddedImagePaths.size() - 1, 0);
    //        QLabel* statusLabel = new QLabel(QStringLiteral("Buton Basili Resmi (index 1 icin)"));
    //        ui.gridLayoutAddedImages->addWidget(statusLabel, mNewlyAddedImagePaths.size() - 1, 1);
    //    }
    //    ui.pushButtonAddNewImage->setDisabled(true);
    //}
    
    else if (mType == GRAPHICS_TEXT_ITEM)
    {
        bool ok = false;
        QString text = QInputDialog::getText(this, tr("Yaziyi girin"),
                                             tr("Girilecek yazi :"), QLineEdit::Normal,
                                             "yazi", &ok);
        if (ok && !text.isEmpty())
        {
            bool ok;
            QFont font = QFontDialog::getFont(
                &ok, QFont("Helvetica [Cronyx]", 10), this);
            QColor color = QColorDialog::getColor(Qt::black, this, "Renk Sec", QColorDialog::DontUseNativeDialog);
            mTextInfos.push_back(text);
            mColorInfos.push_back(color);
            mFontInfos.push_back(font);
            QLabel* newLabel = new QLabel(text);
            newLabel->setFont(font);
            newLabel->setStyleSheet("QLabel {color : " + color.name() + "; }");
            ui.gridLayoutAddedImages->addWidget(newLabel, mTextInfos.size() - 1, 0);
            QLabel* statusLabel = new QLabel(QStringLiteral("index 0 icin text nesnesi "));
            ui.gridLayoutAddedImages->addWidget(statusLabel, mTextInfos.size() - 1, 1);

            ui.comboBoxTextAlign->setVisible(true);
            ui.checkBoxLineBreak->setVisible(true);
            ui.labelMaxUzunluk->setVisible(true);
            ui.lineEditMaxUzunluk->setVisible(true);
            ui.lineEditMaxUzunluk->setText(QString::number(text.length()));
        }
    }

    if (mType != GRAPHICS_TEXT_ITEM)
    {
        ui.pushButtonAddNewImage->setText(QStringLiteral("Yeni Resim Ekle"));
    }
    else
    {
        ui.pushButtonAddNewImage->setText(QStringLiteral("Yeni Yazi Ekle"));
    }

}

void AddMultipleSceneItem::on_comboBoxAttrType_currentIndexChanged(int index)
{
    if (mType == GRAPHICS_TEXT_ITEM)
    {
        //if selected types are float or int, then we can show precision section
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

AddMultipleSceneItem::~AddMultipleSceneItem()
{

}

void AddMultipleSceneItem::on_pushButtonAddNewImage_clicked()
{
    if (mType != GRAPHICS_TEXT_ITEM)
    {
        QString selectedImage1 = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("Resim secin."),
            "",
            tr("Image Files (*.png *.jpg)"));
        if (selectedImage1.size() > 0)
        {
            mNewlyAddedImagePaths.push_back(selectedImage1);
            QLabel* newLabel = new QLabel();
            QPixmap newPixMap(selectedImage1);
            /***********************/
            newLabel->setPixmap(newPixMap);
            /***********************/
            ui.gridLayoutAddedImages->addWidget(newLabel, mNewlyAddedImagePaths.size() - 1, 0);
            QLabel* statusLabel = new QLabel("index " + QString::number(mNewlyAddedImagePaths.size() - 1) + " icin");
            ui.gridLayoutAddedImages->addWidget(statusLabel, mNewlyAddedImagePaths.size() - 1, 1);
        }
    }
    else
    {
        bool ok = false;
        QString text = QInputDialog::getText(this, tr("Yaziyi girin"),
                                             tr("Girilecek yazi :"), QLineEdit::Normal,
                                             "", &ok);
        if (ok && !text.isEmpty())
        {
            bool ok;
            QFont font = QFontDialog::getFont(
                &ok, QFont("Helvetica [Cronyx]", 10), this);
            QColor color = QColorDialog::getColor(Qt::black, this, "Renk Sec", QColorDialog::DontUseNativeDialog);
            mTextInfos.push_back(text);
            mColorInfos.push_back(color);
            mFontInfos.push_back(font);
            QLabel* newLabel = new QLabel(text);
            newLabel->setFont(font);
            newLabel->setStyleSheet("QLabel {color : " + color.name() + "; }");
            ui.gridLayoutAddedImages->addWidget(newLabel, mTextInfos.size() - 1, 0);
            QLabel* statusLabel = new QLabel("index " + QString::number(mTextInfos.size() - 1) + " icin");
            ui.gridLayoutAddedImages->addWidget(statusLabel, mTextInfos.size() - 1, 1);
        }
    }
}

void AddMultipleSceneItem::on_pushButtonSave_clicked()
{
    mName = ui.lineEditName->text();
    mShowOnStart = ui.checkBoxShowOnStart->isChecked();
    mAttrName = ui.lineEditAttrName->text();
    mAttrType = ui.comboBoxAttrType->currentText();

    if (ui.lineEditPrecision->isVisible() && ui.lineEditPrecision->text().size() > 0 && mType == GRAPHICS_TEXT_ITEM)
    {
        mConfigurations["precision"] = ui.lineEditPrecision->text().toStdString();
    }
    if (ui.checkBoxIsOutput->isChecked())
    {
        mConfigurations["mainAttributeIsOutput"] = "true";
    }
    if (ui.checkBoxAdditionalAttr->isChecked())
    {
        mConfigurations["enableDisableAttrName"] = ui.lineEditAdditionalAttr->text().toStdString();
        if (ui.checkBoxAdditionalAttrIsCommand->isChecked())
        {
            mConfigurations["enableDisableAttrIsCommandId"] = "true";
        }
    }
    if (mType == GRAPHICS_TEXT_ITEM)
    {
        mConfigurations["alignment"] = QString::number(ui.comboBoxTextAlign->currentIndex()).toStdString();
        mConfigurations["lineBreak"] = QString::number(ui.checkBoxLineBreak->isChecked()).toStdString();
        mConfigurations["maxLength"] = ui.lineEditMaxUzunluk->text().toStdString();
    }

    done(QDialog::Accepted);
}

void AddMultipleSceneItem::handleAdditionalAttrCheckBox(int state)
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
