#include "EditGraphicsViewWidget.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QBrush>
#include <QColor>
#include <QPalette>
#include <QResizeEvent>

EditGraphicsViewWidget::EditGraphicsViewWidget(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    // QSS ID kural� i�in isim ver (iste�e ba�l� ama faydal�)
    setObjectName("EditGraphicsViewWidget");

    // Scene'i kur ve view'e ba�la
    mScene = new QGraphicsScene(this);
    ui.graphicsViewMainScreen->setScene(mScene);

    // Varsay�lan: �effaf mod
    setAttribute(Qt::WA_TranslucentBackground, true);

    auto* view = ui.graphicsViewMainScreen;
    view->setAttribute(Qt::WA_TranslucentBackground, true);
    view->viewport()->setAttribute(Qt::WA_TranslucentBackground, true);

    // Boyamay� kapat
    view->setAutoFillBackground(false);
    view->viewport()->setAutoFillBackground(false);

    // Paletleri alfa=0 yap
    {
        QPalette pal = palette();
        pal.setColor(QPalette::Base, Qt::transparent);
        pal.setColor(QPalette::Window, Qt::transparent);
        setPalette(pal);
    }
    {
        QPalette pal = view->palette();
        pal.setColor(QPalette::Base, Qt::transparent);
        pal.setColor(QPalette::Window, Qt::transparent);
        view->setPalette(pal);
    }
    {
        QPalette pal = view->viewport()->palette();
        pal.setColor(QPalette::Base, Qt::transparent);
        pal.setColor(QPalette::Window, Qt::transparent);
        view->viewport()->setPalette(pal);
    }

    // Stylesheet ile peki�tir (global QSS'i ezer)
    view->setStyleSheet("QGraphicsView { background: transparent; }");

    // Scene arka plan� boyamas�n
    mScene->setBackgroundBrush(Qt::NoBrush);

    // Artefaktlar� azalt
    view->setCacheMode(QGraphicsView::CacheNone);

    // �lk sceneRect
    mScene->setSceneRect(view->viewport()->rect());
}

EditGraphicsViewWidget::~EditGraphicsViewWidget() = default;

void EditGraphicsViewWidget::resizeEvent(QResizeEvent * e) {
    QWidget::resizeEvent(e);
    // Viewport de�i�ince sahne dikd�rtgenini uydur
    if (ui.graphicsViewMainScreen && mScene) {
        mScene->setSceneRect(ui.graphicsViewMainScreen->viewport()->rect());
    }
}

void EditGraphicsViewWidget::addToGraphicsScene(QGraphicsItem * item) {
    if (!item) return;
    // Eklerken de uydur
    mScene->setSceneRect(ui.graphicsViewMainScreen->viewport()->rect());
    mScene->addItem(item);
    item->setAcceptTouchEvents(true);
}

QGraphicsView* EditGraphicsViewWidget::getGraphicsView() {
    return ui.graphicsViewMainScreen;
}
