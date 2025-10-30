#ifndef SetGroupScreen_H
#define SetGroupScreen_H

#include <QWidget>
#include "ui_SetGroupScreen.h"
#include "qgraphicsview.h"
#include <QMenu>
#include<AddedElement.h>
class SetGroupScreen: public QWidget
{
    Q_OBJECT

    public:
    SetGroupScreen(QWidget *parent = 0);
    ~SetGroupScreen();
    Ui::SetGroupScreen ui;
    CustomSceneItem* mCurrentSceneElement = NULL;
    QStringList mGroupNames;
    void updateScreen();
    int getCurrentSelectedGroupIndex();
protected slots:
    void handleCurrentSelectionChanged(int index);
    void on_pushButtonEditGroupName_clicked();
    void on_pushButtonSetGroup_clicked();
    void on_pushButtonAddNewGroup_clicked();
signals:
    void groupNameEdited(int index, QString newName);
    void groupSet(CustomSceneItem* element,int groupIndex);
    void newGroupAdded(QString gorupName);
};

#endif // SetGroupScreen_H
