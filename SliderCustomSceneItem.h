#pragma once
#include "CustomSceneItem.h"
#include "qcolor.h"
#include "qgraphicsproxywidget.h"
#include "qslider.h"
enum SliderType {
    VERTICALSlider = 0,
    HORIZONTALSlider = 1,
    VERTICALSlider_REVERSE = 2,
    HORIZONTALSlider_REVERSE = 3
};
class SliderCustomSceneItem:
    public CustomSceneItem
{
    public:
    SliderCustomSceneItem(QString name, int type, std::vector<QGraphicsItem*> item, bool showOnStart = false, QString attrName = "", QString attrType = "");
    ~SliderCustomSceneItem();
    void changeSliderType(SliderType newSliderType);
    void changeSliderWidth(int newWidth);
    void changeSliderHeight(int newHeight);
    void changeSliderPercentage(int param1);
    void changeSliderCss(QString cssStr);
    SliderType mSliderType;
    int mLineThickness;
    float mLineLength;
    float mMaxVal;
    float mMinVal;
    QString mSliderWidgetCss;
    QSlider* mSlider;
};
class CustomSliderProxyWidgetItem: public QGraphicsProxyWidget
{   
    public:
    void createDefaultSlider(SliderCustomSceneItem* customItem);
    void createSliderWithGivenItem(SliderCustomSceneItem* customItem);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    
};