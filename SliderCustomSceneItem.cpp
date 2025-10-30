#include "SliderCustomSceneItem.h"
#include "QGraphicsItem"
#include "qgraphicsproxywidget.h"
#include <QDebug>              
#include <QLoggingCategory>
#include "Logging.h"




SliderCustomSceneItem::SliderCustomSceneItem(QString name, int type, std::vector<QGraphicsItem*> item, bool showOnStart /*= false*/, QString attrName /*= ""*/, QString attrType /*= ""*/)
    : CustomSceneItem(name, type, item, showOnStart, attrName, attrType)
{
    mSliderType = VERTICALSlider;
    mMaxVal = 0;
    mMinVal = 100;
    mLineThickness = 10;
    mLineLength = 30;
    mSlider = NULL;
}

SliderCustomSceneItem::~SliderCustomSceneItem()
{

}

void SliderCustomSceneItem::changeSliderType(SliderType newSliderType)
{
    if (mSlider == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    mSliderType = newSliderType;
    if (mSliderType == VERTICALSlider)
    {
        mSlider->setOrientation(Qt::Vertical);
        mSlider->setInvertedAppearance(false);
    }
    else if (mSliderType== HORIZONTALSlider)
    {
        mSlider->setOrientation(Qt::Horizontal);
        mSlider->setInvertedAppearance(false);
    }
    else if (mSliderType == VERTICALSlider_REVERSE)
    {
        mSlider->setOrientation(Qt::Vertical);
        mSlider->setInvertedAppearance(true);

    }
    else if (mSliderType == HORIZONTALSlider_REVERSE)
    {
        mSlider->setOrientation(Qt::Horizontal);
        mSlider->setInvertedAppearance(true);
    }
}

void SliderCustomSceneItem::changeSliderWidth(int newWidth)
{
    if (mSlider == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    mLineThickness = newWidth;
    mSlider->setFixedWidth(newWidth);
}

void SliderCustomSceneItem::changeSliderHeight(int newHeight)
{
    if (mSlider == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    mLineLength = newHeight;
    mSlider->setFixedHeight(newHeight);
}

void SliderCustomSceneItem::changeSliderPercentage(int param1)
{
    if (mSlider == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    mSlider->setValue(param1);
}

void SliderCustomSceneItem::changeSliderCss(QString cssStr)
{
    if (mSlider == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    mSliderWidgetCss = cssStr;
    mSlider->setStyleSheet(mSliderWidgetCss);
}


void CustomSliderProxyWidgetItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void CustomSliderProxyWidgetItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void CustomSliderProxyWidgetItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void CustomSliderProxyWidgetItem::createDefaultSlider(SliderCustomSceneItem* customItem)
{
    QSlider* progressSlider = new QSlider();
    progressSlider->setOrientation(Qt::Vertical);
    progressSlider->setInvertedAppearance(false);
    customItem->mSliderType = VERTICALSlider;
    progressSlider->setFixedWidth(20);
    customItem->mLineThickness = 20;
    progressSlider->setFixedHeight(100);
    customItem->mLineLength = 100;
    progressSlider->setValue(100);
    customItem->mSlider = progressSlider;
    this->setWidget(progressSlider);
}

void CustomSliderProxyWidgetItem::createSliderWithGivenItem(SliderCustomSceneItem* customItem)
{
    QPointF scenePosBefore = this->scenePos(); //scene pos changes when widget is added
    QSlider* progressSlider = new QSlider();
    customItem->mSlider = progressSlider;
    this->setWidget(progressSlider);
    customItem->changeSliderType(customItem->mSliderType);
    customItem->changeSliderCss(customItem->mSliderWidgetCss);
    customItem->changeSliderWidth(customItem->mLineThickness);
    customItem->changeSliderHeight(customItem->mLineLength);
    progressSlider->setValue(100);
    this->setPos(scenePosBefore);
}

