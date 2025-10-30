#include "CustomRectSceneItem.h"
#include "QGraphicsItem"
#include "qgraphicsproxywidget.h"


CustomRectSceneItem::CustomRectSceneItem(QString name, int type, std::vector<QGraphicsItem*> item, bool showOnStart, QString attrName, QString attrType)
    : CustomSceneItem(name, type, item, showOnStart, attrName, attrType)
{
	mCustomQGraphicsRectItem = (CustomQGraphicsRectItem*)item.at(0);
}

CustomRectSceneItem::~CustomRectSceneItem()
{

}



CustomQGraphicsRectItem::CustomQGraphicsRectItem() : 
QGraphicsItem()
{

}

QRectF CustomQGraphicsRectItem::boundingRect() const
{
	QRectF boundingRectProperty;
	boundingRectProperty.setX(0);
	boundingRectProperty.setX(0);
	boundingRectProperty.setWidth(mWidth);
	boundingRectProperty.setHeight(mHeight);
	return boundingRectProperty;
}

void CustomQGraphicsRectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	if (mIsFilled)
	{
		QBrush fillBrush(mFillColor);
		painter->setBrush(fillBrush);
		painter->fillRect(0, 0, mWidth, mHeight, fillBrush);
	}
	for (int i = 0; i < 4; i++)
	{
		QColor edgeColor = mGeneralEdgeColor;
		int edgeThickness = mGeneralThickness;
		if (mEdgeProperties[i].isCustom)
		{
			edgeColor = mEdgeProperties[i].color;
			edgeThickness = mEdgeProperties[i].thickness;
		}
		QBrush brush;
		brush.setColor(edgeColor);
		QPen pen(edgeColor);
		pen.setWidth(edgeThickness);
		//pen.setBrush(brush);
		painter->setPen(pen);

		if (i==0)//top edge
		{
			painter->drawLine(QLine(0,0,mWidth,0));
		}
		else if (i==1)//right edge
		{
			painter->drawLine(QLine(mWidth, 0, mWidth, mHeight));
		}
		else if (i==2)//bottom edge
		{
			painter->drawLine(QLine(mWidth, mHeight, 0, mHeight));
		}
		else//left edge
		{
			painter->drawLine(QLine(0, mHeight, 0, 0));
		}
	}
}

void CustomQGraphicsRectItem::setWidthHeight(int width, int height)
{
	if (width != mWidth || height != mHeight)
	{
		mWidth = width;
		mHeight = height;
		prepareGeometryChange();
	}
}
