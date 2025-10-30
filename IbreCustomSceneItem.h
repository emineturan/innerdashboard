#pragma once
#include "CustomSceneItem.h"
class IbreCustomSceneItem:
    public CustomSceneItem
{
    public:
    IbreCustomSceneItem(QString name, int type, std::vector<QGraphicsItem*> item, bool showOnStart = false, QString attrName = "", QString attrType = "");
    ~IbreCustomSceneItem();
    float mMaxIbreValue;
    float mMinIbreValue;
    float mMinIbreAngle;
    float mMaxIbreAngle;
    int mCurrentDiscreteValueIndex;
    std::vector<float> mDiscreteIbreValueList;
    std::vector<float> mDiscreteIbreAngleList;
    bool mIsDiscreteIbre = false;
};

