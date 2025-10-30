#ifndef ADDEDELEMENT_H
#define ADDEDELEMENT_H

#include <QWidget>
#include "ui_AddedElement.h"
#include "qgraphicsview.h"
#include "CustomSceneItem.h"
#include <QMenu>
class AddedElement: public QWidget
{
    Q_OBJECT

    public:
    AddedElement(CustomSceneItem* customItem, QWidget *parent = 0);
    ~AddedElement();
    CustomSceneItem* mCustomItem;
    Ui::AddedElement ui;
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    bool mSelected;
    void handleSelectedStatus();
    bool eventFilter(QObject *target, QEvent *event);
    void updateParts();
    QMenu mPopUpMenu;
signals:
    void mouseDoubleClickedOnItem(CustomSceneItem* item);
    void signalSelectedAddedElementPart(AddedElement* addedElement);
    void signalSetParentTriggered(AddedElement* addedElement);
protected slots:
    void setParentActionTriggered();
    private:
};

#endif // ADDEDELEMENT_H
