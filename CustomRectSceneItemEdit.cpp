#include "CustomRectSceneItemEdit.h"
#include "qcolordialog.h"

CustomRectSceneItemEdit::CustomRectSceneItemEdit(CustomRectSceneItem* rectItem,QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	mSceneItem = rectItem;
	if (mSceneItem->mItems.size() > 0)
	{
		mCustomGraphicsRectItem = (CustomQGraphicsRectItem*)mSceneItem->mItems.at(0);
	}
	else
	{
		mCustomGraphicsRectItem = NULL;
	}
	if (mCustomGraphicsRectItem)
	{
		ui.lineEditRectWidth->setText(QString::number(mCustomGraphicsRectItem->mWidth));
		ui.lineEditRectHeigth->setText(QString::number(mCustomGraphicsRectItem->mHeight));
		ui.checkBoxIsFilled->setChecked(mCustomGraphicsRectItem->mIsFilled);
		ui.lineEditGeneralThickness->setText(QString::number(mCustomGraphicsRectItem->mGeneralThickness));
		for (int i = 0; i < 4 ; i++)
		{
			if (mCustomGraphicsRectItem->mEdgeProperties[i].isCustom)
			{
				if (i==0)
				{
					ui.checkBoxCustomTopEdge->setChecked(true);
					ui.lineEditTopEdgeThickness->setText(QString::number(mCustomGraphicsRectItem->mEdgeProperties[i].thickness));
				}
				else if (i==1)
				{
					ui.checkBoxCustomRightEdge->setChecked(true);
					ui.lineEditRightEdgeThickness->setText(QString::number(mCustomGraphicsRectItem->mEdgeProperties[i].thickness));
				}
				else if (i==2)
				{
					ui.checkBoxCustomButtomEdge->setChecked(true);
					ui.lineEditBottomEdgeThickness->setText(QString::number(mCustomGraphicsRectItem->mEdgeProperties[i].thickness));
				}
				else {
					ui.checkBoxCustomLeftEdge->setChecked(true);
					ui.lineEditLeftEdgeThickness->setText(QString::number(mCustomGraphicsRectItem->mEdgeProperties[i].thickness));
				}
			}
		}
	}
}

CustomRectSceneItemEdit::~CustomRectSceneItemEdit()
{

}

void CustomRectSceneItemEdit::on_pushButtonFillColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(mCustomGraphicsRectItem->mFillColor), this, "Rect Item Icin Doldurma Rengi Sec", QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	mCustomGraphicsRectItem->mFillColor = color;
	mCustomGraphicsRectItem->update();
}

void CustomRectSceneItemEdit::on_pushButtonGeneralBorderColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(mCustomGraphicsRectItem->mGeneralEdgeColor), this, "Rect Item Icin Genel Kenar Rengi Sec", QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	mCustomGraphicsRectItem->mGeneralEdgeColor = color;
	mCustomGraphicsRectItem->update();
}

void CustomRectSceneItemEdit::on_pushButtonTopEdgeColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(mCustomGraphicsRectItem->mEdgeProperties[0].color), this, "Rect Item Yukari Kenar Rengi Sec", QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	mCustomGraphicsRectItem->mEdgeProperties[0].color = color;
	mCustomGraphicsRectItem->update();
}

void CustomRectSceneItemEdit::on_pushButtonRightEdgeColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(mCustomGraphicsRectItem->mEdgeProperties[1].color), this, "Rect Item Sag Kenar Rengi Sec", QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	mCustomGraphicsRectItem->mEdgeProperties[1].color = color;
	mCustomGraphicsRectItem->update();
}

void CustomRectSceneItemEdit::on_pushButtonBottomEdgeColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(mCustomGraphicsRectItem->mEdgeProperties[2].color), this, "Rect Item Asagi Kenar Rengi Sec", QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	mCustomGraphicsRectItem->mEdgeProperties[2].color = color;
	mCustomGraphicsRectItem->update();
}

void CustomRectSceneItemEdit::on_pushButtonLeftEdgeColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(mCustomGraphicsRectItem->mEdgeProperties[3].color), this, "Rect Item Sol Kenar Rengi Sec", QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	mCustomGraphicsRectItem->mEdgeProperties[3].color = color;
	mCustomGraphicsRectItem->update();
}

void CustomRectSceneItemEdit::on_pushButtonApplyChanges_clicked()
{
	if (mCustomGraphicsRectItem)
	{
		int width,height;
		bool conversionSuccessful1 = false;
		bool conversionSuccessful2 = false;
		width = ui.lineEditRectWidth->text().toInt(&conversionSuccessful1);
		height = ui.lineEditRectHeigth->text().toInt(&conversionSuccessful2);
		if (conversionSuccessful1 && conversionSuccessful2)
		{
			mCustomGraphicsRectItem->setWidthHeight(width,height);
		}
		mCustomGraphicsRectItem->mIsFilled = ui.checkBoxIsFilled->isChecked();
		int generalThickness = ui.lineEditGeneralThickness->text().toInt(&conversionSuccessful1);
		if (conversionSuccessful1)
		{
			mCustomGraphicsRectItem->mGeneralThickness = generalThickness;
		}
		for (int i = 0; i < 4; i++)
		{
			if (i == 0) {
				mCustomGraphicsRectItem->mEdgeProperties[i].isCustom = ui.checkBoxCustomTopEdge->isChecked();
				if (mCustomGraphicsRectItem->mEdgeProperties[i].isCustom)
				{
					int thickness = ui.lineEditTopEdgeThickness->text().toInt(&conversionSuccessful1);
					if (conversionSuccessful1)
					{
						mCustomGraphicsRectItem->mEdgeProperties[i].thickness = thickness;
					}
				}
			}
			else if (i==1)
			{
				mCustomGraphicsRectItem->mEdgeProperties[i].isCustom = ui.checkBoxCustomRightEdge->isChecked();
				if (mCustomGraphicsRectItem->mEdgeProperties[i].isCustom)
				{
					int thickness = ui.lineEditRightEdgeThickness->text().toInt(&conversionSuccessful1);
					if (conversionSuccessful1)
					{
						mCustomGraphicsRectItem->mEdgeProperties[i].thickness = thickness;
					}
				}
			}
			else if (i==2)
			{
				mCustomGraphicsRectItem->mEdgeProperties[i].isCustom = ui.checkBoxCustomButtomEdge->isChecked();
				if (mCustomGraphicsRectItem->mEdgeProperties[i].isCustom)
				{
					int thickness = ui.lineEditBottomEdgeThickness->text().toInt(&conversionSuccessful1);
					if (conversionSuccessful1)
					{
						mCustomGraphicsRectItem->mEdgeProperties[i].thickness = thickness;
					}
				}
			}
			else
			{
				mCustomGraphicsRectItem->mEdgeProperties[i].isCustom = ui.checkBoxCustomLeftEdge->isChecked();
				if (mCustomGraphicsRectItem->mEdgeProperties[i].isCustom)
				{
					int thickness = ui.lineEditLeftEdgeThickness->text().toInt(&conversionSuccessful1);
					if (conversionSuccessful1)
					{
						mCustomGraphicsRectItem->mEdgeProperties[i].thickness = thickness;
					}
				}
			}
		}
		mCustomGraphicsRectItem->update();
	}
}