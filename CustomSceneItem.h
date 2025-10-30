#ifndef CUSTOMSCENEITEM_H
#define CUSTOMSCENEITEM_H

#include <QString>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPointF>
#include <QVector2D>
#include <vector>
#include <map>
#include <string>

class CustomSceneItem
{

    public:
    /***/
    QPointF getActiveItemPos();
    /***/
    CustomSceneItem(QString name, int type, std::vector<QGraphicsItem*> items, bool showOnStart = false, QString attrName = "", QString attrType = "");
    ~CustomSceneItem();
    virtual void addNewItemWithProperties(QGraphicsItem* newItem);
    QString mItemName;
    int mTypeId;
    std::vector<QGraphicsItem*> mItems;
    int mCurrentActiveItem;
    bool mShowOnStart;
    QString mAttirubuteName;
    QString mAdditionalAttrName;
    QString mAttributeType;
    bool mIsItemShown;
    QVector2D mScaleFactor = QVector2D(1.0f,1.0f);
    std::map<std::string, std::string> mConfigurationsMap;
    void updateShowStatus(bool showStatus);
    void updateItemsAccordingToActiveItem();
    int mGroupIndex = -1;
    int mRotationValue = 0;
    private:


};

#endif // CUSTOMSCENEITEM_H
