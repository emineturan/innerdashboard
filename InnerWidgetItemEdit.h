#pragma once

#include <QWidget>
#include "ui_InnerWidgetItemEdit.h"
#include <InnerWidgetItem.h>

class InnerWidgetItemEdit : public QWidget
{
	Q_OBJECT

public:
	InnerWidgetItemEdit(InnerWidgetItem* barItem,QWidget *parent = Q_NULLPTR);
	~InnerWidgetItemEdit();
protected slots:
	void on_pushButtonApplyChanges_clicked();
private:
	Ui::InnerWidgetItemEdit ui;
	InnerWidgetItem* mSceneItem;
	CustomInnerWidgetRectItem* mCustomInnerWidgetRectItem;
};
