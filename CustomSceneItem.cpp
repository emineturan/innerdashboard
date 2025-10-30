#include "CustomSceneItem.h"
#include "qgraphicsview.h"
#include "qgraphicsitem.h"
#include "ItemTypes.h"


CustomSceneItem::~CustomSceneItem()
{

}

CustomSceneItem::CustomSceneItem(QString name, int type, std::vector<QGraphicsItem*> items, bool showOnStart, QString attrName, QString attrType)
{
    mIsItemShown = true;
    mAttirubuteName = attrName;
    mAttributeType = attrType;
    mCurrentActiveItem = 0;
    mItemName = name;
    mTypeId = type;
    mItems = items;
    mShowOnStart = showOnStart;
    //We do not want to move background item
    if (mTypeId != GRAPHICS_PIXMAP_BACKGROUND_ITEM)
    {
        for (int i = 0; i < mItems.size(); i++)
        {
            mItems.at(i)->setFlag(QGraphicsItem::ItemIsMovable);
            mItems.at(i)->setFlag(QGraphicsItem::ItemIsSelectable);
            //mItems.at(i)->setFlag ( QGraphicsItem::ItemIsFocusable);
        }
    }
    else
    {
        showOnStart = true;
    }
    for (int i = 0; i < mItems.size(); i++)
    {
        if (i == mCurrentActiveItem)
        {
            mItems.at(i)->show();
        }
        else
        {
            mItems.at(i)->hide();
        }
    }
}

void CustomSceneItem::updateItemsAccordingToActiveItem()
{
    if (mCurrentActiveItem >= mItems.size())
    {
        return;
    }
    QPointF activeItemPos = mItems.at(mCurrentActiveItem)->scenePos();
    if (mIsItemShown)
    {
        for (int i = 0; i < mItems.size(); i++)
        {
            if (i == mCurrentActiveItem)
            {
                mItems.at(i)->show();
            }
            else
            {
                mItems.at(i)->hide();
            }

            if (mRotationValue < 1 && mTypeId != GRAPHICS_PIXMAP_IBRE_ITEM)
            {
                mItems.at(i)->setPos(activeItemPos);
            }
        }
    }
}

void CustomSceneItem::addNewItemWithProperties(QGraphicsItem* newItem)
{
    mItems.push_back(newItem);
    if (mTypeId != GRAPHICS_PIXMAP_BACKGROUND_ITEM)
    {
        mItems.at(mItems.size() - 1)->setFlag(QGraphicsItem::ItemIsMovable);
        mItems.at(mItems.size() - 1)->setFlag(QGraphicsItem::ItemIsSelectable);
    }

    mItems.at(mItems.size() - 1)->hide();
}

void CustomSceneItem::updateShowStatus(bool showStatus)
{
    mIsItemShown = showStatus;
    if (!mIsItemShown)
    {
        for (int i = 0; i < mItems.size(); i++)
        {
            mItems.at(i)->setVisible(false);
        }
    }
    else
    {
        updateItemsAccordingToActiveItem();
    }
}

QPointF CustomSceneItem::getActiveItemPos() {
    return mItems.at(mCurrentActiveItem)->scenePos();
}