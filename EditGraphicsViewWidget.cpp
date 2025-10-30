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

    // QSS ID kuralý için isim ver (isteðe baðlý ama faydalý)
    setObjectName("EditGraphicsViewWidget");

    // Scene'i kur ve view'e baðla
    mScene = new QGraphicsScene(this);
    ui.graphicsViewMainScreen->setScene(mScene);

    // Varsayýlan: Þeffaf mod
    setAttribute(Qt::WA_TranslucentBackground, true);

    auto* view = ui.graphicsViewMainScreen;
    view->setAttribute(Qt::WA_TranslucentBackground, true);
    view->viewport()->setAttribute(Qt::WA_TranslucentBackground, true);

    // Boyamayý kapat
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

    // Stylesheet ile pekiþtir (global QSS'i ezer)
    view->setStyleSheet("QGraphicsView { background: transparent; }");

    // Scene arka planý boyamasýn
    mScene->setBackgroundBrush(Qt::NoBrush);

    // Artefaktlarý azalt
    view->setCacheMode(QGraphicsView::CacheNone);

    // Ýlk sceneRect
    mScene->setSceneRect(view->viewport()->rect());
}

EditGraphicsViewWidget::~EditGraphicsViewWidget() = default;

void EditGraphicsViewWidget::resizeEvent(QResizeEvent * e) {
    QWidget::resizeEvent(e);
    // Viewport deðiþince sahne dikdörtgenini uydur
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
