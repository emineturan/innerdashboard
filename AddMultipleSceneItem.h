#ifndef ADDMULTIPLESCENEITEM_H
#define ADDMULTIPLESCENEITEM_H

#include <QDialog>
#include "ui_AddMultipleSceneItem.h"
#include "qlabel.h"
#include "ItemTypes.h"

class AddMultipleSceneItem: public QDialog
{
    Q_OBJECT

    public:
    AddMultipleSceneItem(int type = GRAPHICS_PIXMAP_LIGHT_ITEM, QWidget *parent = 0);
    ~AddMultipleSceneItem();
    //std::vector<QLabel*> mLabels;
    std::vector<QString> mNewlyAddedImagePaths;
    //for text items
    std::vector<QFont> mFontInfos;
    std::vector<QColor> mColorInfos;
    std::vector<QString> mTextInfos;

    std::map<std::string, std::string> mConfigurations;

    QString mName;
    bool mShowOnStart;
    QString mAttrName;
    QString mAttrType;
    int mType;
    protected slots:
    void on_pushButtonAddNewImage_clicked();
    void on_pushButtonSave_clicked();
    void on_comboBoxAttrType_currentIndexChanged(int index);
    private:
    Ui::AddMultipleSceneItem ui;
    protected slots:
    void handleAdditionalAttrCheckBox(int state);
};

#endif // ADDMULTIPLESCENEITEM_H
