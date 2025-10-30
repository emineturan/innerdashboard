#include "InnerDashboardItemEdit.h"
#include "InnerDashboardItem.h"
#include "qdebug.h"
InnerDashboardItemEdit::InnerDashboardItemEdit(InnerDashboardItem* rectItem,QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	mSceneItem = rectItem;
	mInnerDashboardItem = rectItem;
	if (mSceneItem->mItems.size() > 0)
	{
		mCustomInnerDashboardRectItem = (CustomInnerDashboardRectItem*)mSceneItem->mItems.at(0);
	}
	else
	{
		mCustomInnerDashboardRectItem = NULL;
	}
	if (mCustomInnerDashboardRectItem)
	{
		ui.lineEditInnerDashboardName->setText(mCustomInnerDashboardRectItem->mInnerDashboardName);
	}
}

InnerDashboardItemEdit::~InnerDashboardItemEdit()
{
}

void InnerDashboardItemEdit::handleRadioButtonIsSetSizeAsPercentage(bool check)
{
	if (!check) {
		ui.lineEditInnerDashboardSizeAsPercentage->setReadOnly(true);
		mSceneItem->mIsSetSizeAsPercentage = false;
		ui.lineEditInnerDashboardSizeAsPercentage->setText("0");
	}
	else {
		ui.lineEditInnerDashboardSizeAsPercentage->setReadOnly(false);
		mSceneItem->mIsSetSizeAsPercentage = true;
	}
}

void InnerDashboardItemEdit::on_pushButtonApplyChanges_clicked()
{
	QString name = ui.lineEditInnerDashboardName->text();

	if (mInnerDashboardItem && mInnerDashboardItem->mCustomInnerDashboardRectItem) {
		mInnerDashboardItem->mCustomInnerDashboardRectItem->mInnerDashboardName = name;
		mInnerDashboardItem->mItemName = name;

		// Sahneye ekle (ilk kez ekleniyor)
		if (mInnerDashboardItem->mCustomInnerDashboardRectItem->scene() == nullptr) {
			DashboardGeneratorGui* mainWindow = qobject_cast<DashboardGeneratorGui*>(parentWidget());
			if (mainWindow) {
				mainWindow->addItemToScene(mInnerDashboardItem->mCustomInnerDashboardRectItem);
			}
		}

		// Görünümü kur
		mInnerDashboardItem->initializeBasicView();
		mInnerDashboardItem->loadPhased();


		if (DashboardGeneratorGui* main = qobject_cast<DashboardGeneratorGui*>(parentWidget())) {
			main->loadInnerDashboardAll(mInnerDashboardItem);
		}
	}
	this->close();
}

