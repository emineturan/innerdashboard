#pragma once
#include "CustomSceneItem.h"
#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QTimer>
#include <QColor>

class CustomInnerDashboardRectItem : public QGraphicsItem
{
public:
    CustomInnerDashboardRectItem();
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void setWidthHeight(int width, int height);

    QString mInnerDashboardName;
    int mWidth = 100;
    int mHeight = 100;
    QColor mGeneralEdgeColor = QColor(Qt::black);
    QColor mFillColor = QColor(Qt::green);
    int mGeneralThickness = 3;
    QColor mTextColor = QColor(Qt::red);
};

class InnerDashboardItem : public CustomSceneItem
{
public:
    InnerDashboardItem(QString name, int type, std::vector<QGraphicsItem*> item,
        bool showOnStart = false, QString attrName = "", QString attrType = "");
    ~InnerDashboardItem();

    void initializeBasicView();
    void loadPhased();

    float mInnerDashboardNewSizeAsPercentage = 0;
    bool mIsSetSizeAsPercentage = false;
    CustomInnerDashboardRectItem* mCustomInnerDashboardRectItem = nullptr;

private:
    QGraphicsSimpleTextItem* mTitle = nullptr;
};
