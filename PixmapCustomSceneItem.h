#pragma once
#include "CustomSceneItem.h"
#include "ItemTypes.h"
#include "QGraphicsItem"
class PixmapCustomSceneItem :
    public CustomSceneItem
{
public:
    PixmapCustomSceneItem(std::vector<QPixmap> originalPixmap,QString name, int type,std::vector<QGraphicsItem*> items, bool showOnStart, QString attrName, QString attrType);
    ~PixmapCustomSceneItem();
    void addNewItemWithProperties(QGraphicsItem* newItem) override;
    std::vector<QPixmap> mOriginalPixmapList;
    QPixmap currentOriginalPixmapItem();
private:
};

