#include "AddedElement.h"
#include "qgraphicsitem.h"
#include "ItemTypes.h"
#include "DashboardGeneratorGui.h"
#include <QMouseEvent>
AddedElement::AddedElement(CustomSceneItem* customItem, QWidget *parent)
    : QWidget(parent),
    mCustomItem(customItem),
    mSelected(false)
{
    ui.setupUi(this);

    this->ui.groupBox->setTitle(mCustomItem->mItemName);
    this->ui.labelAttrName->installEventFilter(this);
    this->ui.labelElementName->installEventFilter(this);
    this->ui.labelShowOnStart->installEventFilter(this);
    this->ui.graphicsViewElementInfo->installEventFilter(this);
    this->ui.labelVisibleStatus->installEventFilter(this);

    ui.labelElementName->setText("Name   : " + mCustomItem->mItemName);
    if (mCustomItem->mIsItemShown)
    {
        this->ui.labelVisibleStatus->setText("Visible: On");
        setStyleSheet("");
    }    
    else
    {
        this->ui.labelVisibleStatus->setText("Visible: Off");
        setStyleSheet("background-color:#ff9933;");
    }


    QGraphicsScene *scene = new QGraphicsScene(ui.graphicsViewElementInfo);
    scene->setSceneRect(ui.graphicsViewElementInfo->rect());
    ui.graphicsViewElementInfo->setScene(scene);

    QGraphicsItem* itemToAdd;
    if (mCustomItem->mTypeId == GRAPHICS_PIXMAP_ITEM || mCustomItem->mTypeId == GRAPHICS_PIXMAP_BACKGROUND_ITEM || mCustomItem->mTypeId == GRAPHICS_PIXMAP_LIGHT_ITEM || mCustomItem->mTypeId == GRAPHICS_PIXMAP_IBRE_ITEM /****/ || mCustomItem->mTypeId == GRAPHICS_PIXMAP_BUTTON_INPUT_ITEM )
    {
        itemToAdd = new QGraphicsPixmapItem(((QGraphicsPixmapItem*)mCustomItem->mItems.at(mCustomItem->mCurrentActiveItem))->pixmap());
    }
    else if (mCustomItem->mTypeId == GRAPHICS_TEXT_ITEM)
    {
        itemToAdd = new QGraphicsTextItem(((QGraphicsTextItem*)mCustomItem->mItems.at(mCustomItem->mCurrentActiveItem))->toPlainText());
    }
    else if (mCustomItem->mTypeId == GRAPHICS_BAR_ITEM || mCustomItem->mTypeId == GRAPHICS_SLIDER_ITEM)
    {
        itemToAdd = new QGraphicsLineItem(0,0,100,100);
    }
	else if (mCustomItem->mTypeId == GRAPHICS_RECT_ITEM)
	{
		itemToAdd = new QGraphicsRectItem(0, 0, 50, 50);
	}
    else if (mCustomItem->mTypeId == INNER_DASHBOARD_ITEM)
    {
        itemToAdd = new QGraphicsRectItem(0, 0, 50, 50);
    }
    else if (mCustomItem->mTypeId == INNER_WIDGET_ITEM)
    {
        itemToAdd = new QGraphicsRectItem(0, 0, 50, 50);
    }

    ui.labelShowOnStart->setText(mCustomItem->mShowOnStart ? QStringLiteral("Baslangic: On") : QStringLiteral("Baslangic: Off"));
    ui.labelAttrName->setText("attr-id: " + mCustomItem->mAttirubuteName);
    ui.graphicsViewElementInfo->scene()->addItem(itemToAdd);
    itemToAdd->setPos(0, 0);

    QAction* setParentAction = new QAction(this);
    connect(setParentAction, SIGNAL(triggered()), this, SLOT(setParentActionTriggered()));
    setParentAction->setText(QStringLiteral("Set Group"));
    mPopUpMenu.addAction(setParentAction);
}
 
AddedElement::~AddedElement()
{

}
void AddedElement::setParentActionTriggered()
{
    signalSetParentTriggered(this);
}
void AddedElement::mouseDoubleClickEvent(QMouseEvent * event)
{
    mouseDoubleClickedOnItem(mCustomItem);
}

void AddedElement::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::MouseButton::RightButton)
    {
        mPopUpMenu.popup(QPoint(event->screenPos().x(), event->screenPos().y()));
    }
    else if (event->button() == Qt::MouseButton::LeftButton)
    {
        mCustomItem->mItems.at(mCustomItem->mCurrentActiveItem)->scene()->clearSelection();
        mCustomItem->mItems.at(mCustomItem->mCurrentActiveItem)->setSelected(true);
        mSelected = true;
        signalSelectedAddedElementPart(this);
    }
}

void AddedElement::handleSelectedStatus()
{
    if (mSelected)
    {
        setStyleSheet("background-color:yellow;");
    }
    else
    {
        if (!mCustomItem->mIsItemShown)
        {
            setStyleSheet("background-color:#ff9933;");
        }
        else
        {
            setStyleSheet("");
        }

    }
}

bool AddedElement::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)
    {
        event->accept();
        mousePressEvent((QMouseEvent *)event);
    }
    else if (event->type() == QEvent::MouseButtonDblClick)
    {
        event->accept();
        mouseDoubleClickEvent((QMouseEvent *)event);
    }
    return false;
}

void AddedElement::updateParts()
{
    ui.labelElementName->setText("Name   : " + mCustomItem->mItemName);
    mCustomItem->updateShowStatus(mCustomItem->mIsItemShown);
    if (mCustomItem->mIsItemShown)
    {
        this->ui.labelVisibleStatus->setText("Visible: On");
        setStyleSheet("");
    }
    else
    {
        this->ui.labelVisibleStatus->setText("Visible: Off");
        setStyleSheet("background-color:#ff9933;");
    }
    ui.labelShowOnStart->setText(mCustomItem->mShowOnStart ? QStringLiteral("Baslangic: On") : QStringLiteral("Baslangic: Off"));
    ui.labelAttrName->setText("attr-id: " + mCustomItem->mAttirubuteName);
}
