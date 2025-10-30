#pragma once
#include "CustomSceneItem.h"
#include "qcolor.h"
#include <QGraphicsItem>
class CustomInnerWidgetRectItem;
class InnerWidgetItem :
    public CustomSceneItem
{
    public:
	InnerWidgetItem(QString name, int type, std::vector<QGraphicsItem*> item, bool showOnStart = false, QString attrName = "", QString attrType = "");
    ~InnerWidgetItem();
	float mInnerDashboardNewSizeAsPercentage = 0;
	bool mIsSetSizeAsPercentage = false;
	CustomInnerWidgetRectItem* mCustomInnerWidgetRectItem;
};
class CustomInnerWidgetRectItem : public QGraphicsItem
{   
    public:
		CustomInnerWidgetRectItem();
		QRectF boundingRect() const override;
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
		void setWidthHeight(int width,int height);
		QString mInnerWidgetName;
		int mWidth = 100;
		int mHeight = 100;
		QColor mGeneralEdgeColor = QColor(Qt::black);
		QColor mFillColor = QColor(Qt::green);
		int mGeneralThickness = 3;
		QColor mTextColor = QColor(Qt::red);
		int mIGWindowID = -1;
		//override size and position 
		QString mOverrideSizeWidthAttrName;
		QString mOverrideSizeHeightAttrName;
		QString mOverridePosXAttrName;
		QString mOverridePosYAttrName;
};