#include "InnerWidgetItem.h"
#include "QGraphicsItem"

InnerWidgetItem::InnerWidgetItem(QString name, int type, std::vector<QGraphicsItem*> item, bool showOnStart /*= false*/, QString attrName /*= ""*/, QString attrType /*= ""*/)
    : CustomSceneItem(name, type, item, showOnStart, attrName, attrType)
{
	mCustomInnerWidgetRectItem = (CustomInnerWidgetRectItem*)item.at(0);
}

InnerWidgetItem::~InnerWidgetItem()
{

}

CustomInnerWidgetRectItem::CustomInnerWidgetRectItem() :
	QGraphicsItem()
{

}

QRectF CustomInnerWidgetRectItem::boundingRect() const
{
	QRectF boundingRectProperty;
	boundingRectProperty.setX(0);
	boundingRectProperty.setX(0);
	boundingRectProperty.setWidth(mWidth);
	boundingRectProperty.setHeight(mHeight);
	return boundingRectProperty;
}

void CustomInnerWidgetRectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	QBrush fillBrush(mFillColor);
	painter->setBrush(fillBrush);
	painter->fillRect(0, 0, mWidth, mHeight, fillBrush);
	QBrush textBrush(mTextColor);
	painter->setBrush(textBrush);
	QFont font = painter->font();
	font.setPixelSize(48);
	painter->setFont(font);
	painter->drawText(QPoint(10,10),mInnerWidgetName);
	for (int i = 0; i < 4; i++)
	{
		QColor edgeColor = mGeneralEdgeColor;
		int edgeThickness = mGeneralThickness;

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

void CustomInnerWidgetRectItem::setWidthHeight(int width, int height)
{
	if (width != mWidth || height != mHeight)
	{
		mWidth = width;
		mHeight = height;
		prepareGeometryChange();
	}
}
