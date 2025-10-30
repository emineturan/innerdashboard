#include "PixmapCustomSceneItem.h"
#include "QGraphicsItem"
PixmapCustomSceneItem::PixmapCustomSceneItem(std::vector<QPixmap> originalPixmap,QString name, int type, std::vector<QGraphicsItem*> items, bool showOnStart = false, QString attrName = "", QString attrType = "")
	:CustomSceneItem(name, type,items, showOnStart, attrName, attrType)
{
	mOriginalPixmapList = originalPixmap;
}

PixmapCustomSceneItem::~PixmapCustomSceneItem() {

}

void PixmapCustomSceneItem::addNewItemWithProperties(QGraphicsItem* newItem)
{
	CustomSceneItem::addNewItemWithProperties(newItem);
	QGraphicsPixmapItem* pixmapItem = (QGraphicsPixmapItem*) newItem;
	mOriginalPixmapList.push_back(pixmapItem->pixmap());
}

QPixmap PixmapCustomSceneItem::currentOriginalPixmapItem() {
	return mOriginalPixmapList.at(mCurrentActiveItem);
}