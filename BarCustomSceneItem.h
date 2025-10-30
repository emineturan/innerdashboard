#ifndef BARCUSTOMSCENEITEM_H
#define BARCUSTOMSCENEITEM_H

#pragma once

#include <QColor>
#include <QGraphicsProxyWidget>
#include <QProgressBar>
#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOption>
#include <QGraphicsSceneEvent>
#include <QWidget>
#include <QRectF>
#include "CustomSceneItem.h"

// Forward declarations
class CustomRadialProgressBar;
class CustomBarProxyWidgetItem;

// Qt forward declarations  
class QProgressBar;

enum BarType {
    VERTICALBAR = 0,
    HORIZONTALBAR = 1,
    VERTICALBAR_REVERSE = 2,
    HORIZONTALBAR_REVERSE = 3,
    ARC_BAR = 4,
    ARC_BAR_REVERSE = 5
};

class BarCustomSceneItem: public CustomSceneItem
{
public:
    BarCustomSceneItem(QString name, int type, std::vector<QGraphicsItem*> item, bool showOnStart = false, QString attrName = "", QString attrType = "");
    ~BarCustomSceneItem();
    void changeBarType(BarType newBarType);
    void changeBarWidth(int newWidth);
    void changeBarHeight(int newHeight);
    void changeBarColor(QColor color);
    void changeBarPercentage(int param1);
    void changeBarCss(QString cssStr);
    void changeArcRadius(int radius);
    void changeArcStartAngle(int angle);
    void changeArcSpanAngle(int angle);
    void updateArcBarStyle(bool reverse = false);
    void changeBarRadius(int radius);
    void changeArcBend(int bend);
    void changeArcBendDirection(int direction);
    void changeArcRotation(int rotation);
    
    BarType mBarType;
    QColor mBarColor;
    int mLineThickness;
    float mLineLength;
    float mMaxVal;
    float mMinVal;
    QString mBarWidgetCss;
    int mArcRadius;
    int mArcStartAngle;
    int mArcSpanAngle;
    int mBarRadius;
    int mArcBend;
    int mArcBendDirection;
    int mArcRotation;
    QProgressBar* mProgressBar;
    CustomRadialProgressBar* mRadialBar;
    float mCurrentBarValue;
};

class CustomBarProxyWidgetItem: public QGraphicsProxyWidget
{   
public:
    void createDefaultProgressBar(BarCustomSceneItem* customItem);
    void createProgressBarWithGivenItem(BarCustomSceneItem* customItem);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

class CustomRadialProgressBar: public QGraphicsItem
{
public:
    CustomRadialProgressBar(BarCustomSceneItem* barItem);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void updateProgress();
    
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    
private:
    BarCustomSceneItem* mBarItem;
    int mRadius;
    int mThickness;
    QColor mColor;
    int mValue;
    int mMinValue;
    int mMaxValue;
    int mSpanAngle;
    
    void drawBentArc(QPainter* painter, int x, int y, int width, int height, 
                     int startAngle, int spanAngle, int thickness, int bendFactor);
    void drawBentArcWithBreak(QPainter* painter, int x, int y, int width, int height, 
                              int startAngle, int spanAngle, int thickness, int bendFactor);
};

#endif // BARCUSTOMSCENEITEM_H



