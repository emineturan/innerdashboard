#include "InnerDashboardItem.h"
#include "InnerDashboardItemEdit.h"
#include "QGraphicsItem"
#include <QTimer>

//using namespace InnerDashboardItemEdit;

InnerDashboardItem::InnerDashboardItem(QString name, int type, std::vector<QGraphicsItem*> item, bool showOnStart /*= false*/, QString attrName /*= ""*/, QString attrType /*= ""*/)
    : CustomSceneItem(name, type, item, showOnStart, attrName, attrType)
{
	mCustomInnerDashboardRectItem = (CustomInnerDashboardRectItem*)item.at(0);
}

InnerDashboardItem::~InnerDashboardItem()
{

}



CustomInnerDashboardRectItem::CustomInnerDashboardRectItem() : 
QGraphicsItem()
{

}

QRectF CustomInnerDashboardRectItem::boundingRect() const
{
	QRectF boundingRectProperty;
	boundingRectProperty.setX(0);
	boundingRectProperty.setX(0);
	boundingRectProperty.setWidth(mWidth);
	boundingRectProperty.setHeight(mHeight);
	return boundingRectProperty;
}

void CustomInnerDashboardRectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setBrush(QBrush(mFillColor));
	painter->fillRect(0, 0, mWidth, mHeight, QBrush(mFillColor));

	painter->setBrush(QBrush(mTextColor));
	QFont font = painter->font();
	font.setPixelSize(48);
	painter->setFont(font);
	painter->setPen(QPen(mTextColor));
	painter->drawText(QPoint(10, 10), mInnerDashboardName);

	for (int i = 0; i < 4; i++)
	{
		QPen pen(mGeneralEdgeColor);
		pen.setWidth(mGeneralThickness);
		painter->setPen(pen);

		if (i == 0)
			painter->drawLine(QLine(0, 0, mWidth, 0));
		else if (i == 1)
			painter->drawLine(QLine(mWidth, 0, mWidth, mHeight));
		else if (i == 2)
			painter->drawLine(QLine(mWidth, mHeight, 0, mHeight));
		else
			painter->drawLine(QLine(0, mHeight, 0, 0));
	}
}

void CustomInnerDashboardRectItem::setWidthHeight(int width, int height)
{
	if (width != mWidth || height != mHeight)
	{
		mWidth = width;
		mHeight = height;
		prepareGeometryChange();
	}
}

void InnerDashboardItem::initializeBasicView() {

	if (!mCustomInnerDashboardRectItem) return;
	mCustomInnerDashboardRectItem->setWidthHeight(600, 350);
	mCustomInnerDashboardRectItem->mFillColor = QColor(20, 120, 60);
	mCustomInnerDashboardRectItem->mTextColor = Qt::white;
	mCustomInnerDashboardRectItem->mInnerDashboardName = mItemName;

	if (!mTitle) {
		mTitle = new QGraphicsSimpleTextItem(QString("Inner: %1").arg(mItemName), mCustomInnerDashboardRectItem);
		mTitle->setBrush(QBrush(Qt::white));
		mTitle->setPos(16, 60);
	}

	mCustomInnerDashboardRectItem->setFlag(QGraphicsItem::ItemIsMovable, true);
	mCustomInnerDashboardRectItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
	mCustomInnerDashboardRectItem->update();

}


void InnerDashboardItem::loadPhased() {
	if (mTitle) mTitle->setVisible(false);
	QTimer::singleShot(100, [this] { if (mTitle) mTitle->setVisible(true); });
}

