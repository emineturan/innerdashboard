#pragma once
#include "CustomSceneItem.h"
#include "qcolor.h"
#include <QGraphicsItem>
class CustomQGraphicsRectItem;
class CustomRectSceneItem :
    public CustomSceneItem
{
    public:
	CustomRectSceneItem(QString name, int type, std::vector<QGraphicsItem*> item, bool showOnStart = false, QString attrName = "", QString attrType = "");
    ~CustomRectSceneItem();
	CustomQGraphicsRectItem* mCustomQGraphicsRectItem;
};
struct RectItemEdgeProperty
{
	QColor color;
	int thickness;
	bool isDotted;
	bool isCustom = false;
};
class CustomQGraphicsRectItem : public QGraphicsItem
{   
    public:
		CustomQGraphicsRectItem();
		QRectF boundingRect() const override;
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
		void setWidthHeight(int width,int height);
		int mWidth = 50;
		int mHeight = 50;
		int mGeneralThickness = 1;
		QColor mGeneralEdgeColor;
		bool mIsFilled = false;
		QColor mFillColor;
		RectItemEdgeProperty mEdgeProperties[4];
};