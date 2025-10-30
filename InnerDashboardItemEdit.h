#ifndef INNERDASHBOARDITEMEDIT_H
#define INNERDASHBOARDITEMEDIT_H

#include <QWidget>
#include "ui_InnerDashboardItemEdit.h"
#include <InnerDashboardItem.h>
#include "DashboardGeneratorGui.h"
class DashboardGeneratorGui;
class InnerDashboardItemEdit : public QWidget
{
    Q_OBJECT

public:
	InnerDashboardItemEdit(InnerDashboardItem* barItem,QWidget *parent = 0);
    ~InnerDashboardItemEdit();
	Ui::InnerDashboardItemEdit ui;
    protected slots:
		void on_pushButtonApplyChanges_clicked();
		void handleRadioButtonIsSetSizeAsPercentage(bool);
private:
    
    InnerDashboardItem* mSceneItem = nullptr;
    InnerDashboardItem* mInnerDashboardItem = nullptr;
    CustomInnerDashboardRectItem* mCustomInnerDashboardRectItem = nullptr;


};

#endif // CUSTOMRECTSCENEITEMEDIT_H
