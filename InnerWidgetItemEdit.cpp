#include "InnerWidgetItemEdit.h"

InnerWidgetItemEdit::InnerWidgetItemEdit(InnerWidgetItem* rectItem,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	mSceneItem = rectItem;
	if (mSceneItem->mItems.size() > 0)
	{
		mCustomInnerWidgetRectItem = (CustomInnerWidgetRectItem*)mSceneItem->mItems.at(0);
	}
	else
	{
		mCustomInnerWidgetRectItem = NULL;
	}
	if (mCustomInnerWidgetRectItem)
	{
		ui.lineEditInnerWidgetName->setText(mCustomInnerWidgetRectItem->mInnerWidgetName);
		ui.lineEditWidgetWidth->setText(QString::number(mCustomInnerWidgetRectItem->mWidth));
		ui.lineEditWidgetHeight->setText(QString::number(mCustomInnerWidgetRectItem->mHeight));
		ui.lineEditWindowId->setText(QString::number(mCustomInnerWidgetRectItem->mIGWindowID));

		ui.lineEditOverrideSizeWidthAttr->setText(mCustomInnerWidgetRectItem->mOverrideSizeWidthAttrName);
		ui.lineEditOverrideSizeHeightAttr->setText(mCustomInnerWidgetRectItem->mOverrideSizeHeightAttrName);
		ui.lineEditOverridePosXAttr->setText(mCustomInnerWidgetRectItem->mOverridePosXAttrName);
		ui.lineEditOverridePosYAttr->setText(mCustomInnerWidgetRectItem->mOverridePosYAttrName);

	}
}

InnerWidgetItemEdit::~InnerWidgetItemEdit()
{
}

void InnerWidgetItemEdit::on_pushButtonApplyChanges_clicked()
{
	if (mSceneItem && mCustomInnerWidgetRectItem)
	{
		QString innerWidgetName = ui.lineEditInnerWidgetName->text();
		if (innerWidgetName.size() > 0)
		{
			mCustomInnerWidgetRectItem->mInnerWidgetName = innerWidgetName;
			mSceneItem->mConfigurationsMap["InnerWidgetName"] = mCustomInnerWidgetRectItem->mInnerWidgetName.toStdString();
		}
		QString overrideSizeWidthAttrName = ui.lineEditOverrideSizeWidthAttr->text();
		if (overrideSizeWidthAttrName.size() > 0)
		{
			mCustomInnerWidgetRectItem->mOverrideSizeWidthAttrName = overrideSizeWidthAttrName;
			mSceneItem->mConfigurationsMap["OverrideWidthAttrName"] = overrideSizeWidthAttrName.toStdString();
		}
		QString overrideSizeHeightAttrName = ui.lineEditOverrideSizeHeightAttr->text();
		if (overrideSizeHeightAttrName.size() > 0)
		{
			mCustomInnerWidgetRectItem->mOverrideSizeHeightAttrName = overrideSizeHeightAttrName;
			mSceneItem->mConfigurationsMap["OverrideHeightAttrName"] = overrideSizeHeightAttrName.toStdString();
		}
		QString overrideXPosAttrName = ui.lineEditOverridePosXAttr->text();
		if (overrideXPosAttrName.size() > 0)
		{
			mCustomInnerWidgetRectItem->mOverridePosXAttrName = overrideXPosAttrName;
			mSceneItem->mConfigurationsMap["OverrideXPosAttrName"] = overrideXPosAttrName.toStdString();
		}
		QString overrideYPosAttrName = ui.lineEditOverridePosYAttr->text();
		if (overrideYPosAttrName.size() > 0)
		{
			mCustomInnerWidgetRectItem->mOverridePosYAttrName = overrideYPosAttrName;
			mSceneItem->mConfigurationsMap["OverrideYPosAttrName"] = overrideYPosAttrName.toStdString();
		}
		int width = ui.lineEditWidgetWidth->text().toInt();
		int height = ui.lineEditWidgetHeight->text().toInt();
		mCustomInnerWidgetRectItem->setWidthHeight(width, height);
		mCustomInnerWidgetRectItem->mIGWindowID = ui.lineEditWindowId->text().toInt();
		mCustomInnerWidgetRectItem->update();
	}
}