#ifndef SCENEITEMEDIT_H
#define SCENEITEMEDIT_H

#include <QDialog>
#include <QColor>
#include <QFont>
#include "ui_SceneItemEdit.h"
#include "CustomSceneItem.h"
#include "IbreSceneItemEdit.h"

class InnerDashboardItemEdit;

class SceneItemEdit : public QDialog
{
    Q_OBJECT

public:
    SceneItemEdit(QWidget* parent = 0);
    SceneItemEdit(CustomSceneItem* item, QWidget* parent = 0);
    ~SceneItemEdit();
    virtual void moveEvent(QMoveEvent* event) override;

    CustomSceneItem* mSceneItem;
    IbreSceneItemEdit* mIbreEditWidget;
    InnerDashboardItemEdit* mInnerDashboardEditWidget = nullptr;
    bool mItemIsDeleted;
    std::vector<QString> mNewlyAddedImagePaths;

protected slots:
    // Genel buton slot'larý
    void on_pushButtonShowOnStart_toggled(bool value);
    void on_pushButtonSave_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonOneCikar_clicked();
    void on_pushButtonArkayaGetir_clicked();
    void on_pushButtonAddNewImage_clicked();
    void on_pushButtonShowHide_toggled(bool value);
    void on_comboBoxAttrType_currentIndexChanged(int index);

    // Ibre (Ýðne) item slot'larý
    void ibreEditSetMinClicked();
    void ibreEditSetMaxClicked();
    void ibreEditSetDiscreteClicked();
    void ibreEditCurrentIndexChanged(int);
    void ibreEditDeleteDiscreteValueClicked();
    void ibreEditMinMaxSliderValueChanged();
    void ibreEditCheckBoxToggled(bool check);

    // Diðer handler'lar
    void handleAdditionalAttrCheckBox(int state);
    void handleItemRotation(int rotationValue);

    // *** YENÝ: Text item düzenleme slot'larý ***
    void on_pushButtonSelectColor_clicked();       // Renk seçimi butonu
    void on_fontComboBoxText_currentFontChanged(const QFont& font);  // Font deðiþimi
    void on_spinBoxFontSize_valueChanged(int value);  // Font boyutu deðiþimi

private:
    Ui::SceneItemEdit ui;

    // *** YENÝ: Text item için üye deðiþkenler ***
    QColor mCurrentTextColor;     // Mevcut text rengi (labelColorPreview için)
    CustomSceneItem* mCustomSceneItem = nullptr;
    // *** YENÝ: Text item helper fonksiyonlarý ***
    void loadTextItemProperties();      // Text item özelliklerini yükle
    void saveTextItemProperties();      // Text item özelliklerini kaydet
    void updateColorPreview();          // Renk önizlemesini güncelle
};

#endif // SCENEITEMEDIT_H
