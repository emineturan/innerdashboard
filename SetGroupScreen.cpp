#include "SetGroupScreen.h"
#include "qgraphicsitem.h"
#include "ItemTypes.h"
#include "DashboardGeneratorGui.h"
#include <QMouseEvent>

SetGroupScreen::SetGroupScreen( QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.comboBoxSelectGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCurrentSelectionChanged(int)));
}
 
SetGroupScreen::~SetGroupScreen()
{

}

void SetGroupScreen::updateScreen()
{
    if (mCurrentSceneElement != NULL)
    {
        QString currentElementGroupName = mCurrentSceneElement->mGroupIndex < 0 ? "No GROUP" :
            ui.comboBoxSelectGroup->itemText(mCurrentSceneElement->mGroupIndex);
        ui.labelElementName->setText(mCurrentSceneElement->mItemName + " group name " + currentElementGroupName);
    }
    ui.comboBoxSelectGroup->clear();
    ui.comboBoxSelectGroup->addItem("No GROUP");
    for (int i = 0; i < mGroupNames.size(); i++)
    {
        ui.comboBoxSelectGroup->addItem(mGroupNames.at(i));
    }

    ui.comboBoxSelectGroup->setCurrentIndex(0);
    ui.lineEditEditGroupName->setText(ui.comboBoxSelectGroup->currentText());


}
int SetGroupScreen::getCurrentSelectedGroupIndex()
{
    return ui.comboBoxSelectGroup->currentIndex();
}
void SetGroupScreen::on_pushButtonEditGroupName_clicked()
{
    if (ui.comboBoxSelectGroup->currentIndex()>0)
    {
        emit groupNameEdited(ui.comboBoxSelectGroup->currentIndex()-1, ui.lineEditEditGroupName->text());
    }
}
void SetGroupScreen::on_pushButtonSetGroup_clicked()
{   
    emit groupSet(mCurrentSceneElement, ui.comboBoxSelectGroup->currentIndex()-1);
}
void SetGroupScreen::on_pushButtonAddNewGroup_clicked()
{
    if (ui.lineEditAddNewGroup->text().size() > 0)
    {
        emit newGroupAdded(ui.lineEditAddNewGroup->text());
    }
}
void SetGroupScreen::handleCurrentSelectionChanged(int index)
{
    if (index >= 0)
    {
        ui.lineEditEditGroupName->setText(ui.comboBoxSelectGroup->currentText());
    }
}

