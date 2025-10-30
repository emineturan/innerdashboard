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
    // Genel buton slot'lar�
    void on_pushButtonShowOnStart_toggled(bool value);
    void on_pushButtonSave_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonOneCikar_clicked();
    void on_pushButtonArkayaGetir_clicked();
    void on_pushButtonAddNewImage_clicked();
    void on_pushButtonShowHide_toggled(bool value);
    void on_comboBoxAttrType_currentIndexChanged(int index);

    // Ibre (��ne) item slot'lar�
    void ibreEditSetMinClicked();
    void ibreEditSetMaxClicked();
    void ibreEditSetDiscreteClicked();
    void ibreEditCurrentIndexChanged(int);
    void ibreEditDeleteDiscreteValueClicked();
    void ibreEditMinMaxSliderValueChanged();
    void ibreEditCheckBoxToggled(bool check);

    // Di�er handler'lar
    void handleAdditionalAttrCheckBox(int state);
    void handleItemRotation(int rotationValue);

    // *** YEN�: Text item d�zenleme slot'lar� ***
    void on_pushButtonSelectColor_clicked();       // Renk se�imi butonu
    void on_fontComboBoxText_currentFontChanged(const QFont& font);  // Font de�i�imi
    void on_spinBoxFontSize_valueChanged(int value);  // Font boyutu de�i�imi

private:
    Ui::SceneItemEdit ui;

    // *** YEN�: Text item i�in �ye de�i�kenler ***
    QColor mCurrentTextColor;     // Mevcut text rengi (labelColorPreview i�in)
    CustomSceneItem* mCustomSceneItem = nullptr;
    // *** YEN�: Text item helper fonksiyonlar� ***
    void loadTextItemProperties();      // Text item �zelliklerini y�kle
    void saveTextItemProperties();      // Text item �zelliklerini kaydet
    void updateColorPreview();          // Renk �nizlemesini g�ncelle
};

#endif // SCENEITEMEDIT_H
