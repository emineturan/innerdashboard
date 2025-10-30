#ifndef CUSTOMRECTSCENEITEMEDIT_H
#define CUSTOMRECTSCENEITEMEDIT_H

#include <QWidget>
#include "ui_CustomRectSceneItemEdit.h"
#include <CustomRectSceneItem.h>

class CustomRectSceneItemEdit : public QWidget
{
    Q_OBJECT

public:
	CustomRectSceneItemEdit(CustomRectSceneItem* barItem,QWidget *parent = 0);
    ~CustomRectSceneItemEdit();
    protected slots:
		void on_pushButtonApplyChanges_clicked();
		void on_pushButtonFillColor_clicked();
		void on_pushButtonGeneralBorderColor_clicked();
		void on_pushButtonTopEdgeColor_clicked();
		void on_pushButtonRightEdgeColor_clicked();
		void on_pushButtonBottomEdgeColor_clicked();
		void on_pushButtonLeftEdgeColor_clicked();
private:
    Ui::CustomRectSceneItemEdit ui;
	CustomRectSceneItem* mSceneItem;
	CustomQGraphicsRectItem* mCustomGraphicsRectItem;
};

#endif // CUSTOMRECTSCENEITEMEDIT_H
