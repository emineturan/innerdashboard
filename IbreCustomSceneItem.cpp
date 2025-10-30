#include "IbreCustomSceneItem.h"
#include "QGraphicsItem"


IbreCustomSceneItem::IbreCustomSceneItem  ( QString name , int type , std::vector<QGraphicsItem*> item , bool showOnStart /*= false */,QString attrName,QString attrType )
    : CustomSceneItem ( name , type , item , showOnStart, attrName ,attrType )
{
    mMaxIbreAngle = 0;
    mMinIbreAngle = 0;
    mMaxIbreValue = 0;
    mMinIbreValue = 0;
    mCurrentDiscreteValueIndex = 0;
    mItems.at ( 0 )->setTransformOriginPoint ( mItems.at ( 0 )->boundingRect().width ( ) / 2 , mItems.at ( 0 )->boundingRect ( ).height() / 2 );
}


IbreCustomSceneItem::~IbreCustomSceneItem ( )
{ }
