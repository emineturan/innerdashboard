#ifndef DASHBOARDGENERATORGUI_H
#define DASHBOARDGENERATORGUI_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLayout>
#include "ui_DashboardGeneratorGui.h"
#include "AddedElement.h"
#include "CustomSceneItem.h"
#include "PixmapCustomSceneItem.h"
#include "ItemTypes.h"
#include <QDomDocument>  // DEÐÝÞTÝRÝLDÝ: "qdom.h" yerine
#include <QDomElement>   // EKLENDÝ
#include <QDomNode>      // EKLENDÝ
#include <QFile>         // EKLENDÝ
#include <QTextStream>   // EKLENDÝ
#include "qmenu.h"
#include <SetGroupScreen.h>
#include "EditGraphicsViewWidget.h"
#include <qgraphicsitem.h>
#include "Inputs/ButtonInputDialog.h"
#include "Inputs/PixmapInputSceneItem.h"
#include "Inputs/RotatableStateOfInput.h"
#include <QCloseEvent>

class InnerDashboardItem;
class DashboardGeneratorGui : public QWidget
{
    Q_OBJECT

public:
    /*********************/
    enum class resizeMode
    {
        RIGHT,
        BOTTOM,
        BOTTOM_RIGHT,
        NONE,
    } resizeMode;
    EditGraphicsViewWidget* mEditGraphicViewWidget;

    bool mIsResizable;
    bool mCursorIsVisible = true;
    bool isMousePosOnCursor;
    bool isPixmapItem(CustomSceneItem* sceneItem);
    bool isMousePressed = false;

    int mCurrentSelectedCustomSceneItemWidth;
    int mCurrentSelectedCustomSceneItemHeight;

    QPointF mCursorPos;
    QPointF tempParam1;
    QSize mCurrentPixmapSize;
    std::vector<QGraphicsPixmapItem*> mResizeCursors;

    enum resizeMode _resizeMode;
    void resizeEvent(QPointF param1);
    void handleMousePressFromScene(QPointF param1);
    void createResizeCursors();
    void handleMouseMoveFromScene();
    void handleMousePosOnCursor(QPointF param1);
    void handleMouseReleaseFromScene();
    void setCursorPosOnScene(CustomSceneItem* item);
    void setResizeCursorsPositions();
    CustomSceneItem* mCurrentSelectedCustomSceneItem;
    QGraphicsPixmapItem* mRightCursor;
    QGraphicsPixmapItem* mBottomTopCursor;
    QGraphicsPixmapItem* mCornerCursor;

    /*********************/
    DashboardGeneratorGui(QWidget* parent = 0);
    ~DashboardGeneratorGui();
    void createSceneItemsWithGivenImagePaths(QStringList imagePaths, bool isBackground = false);
    void addItemToScene(QGraphicsItem* item);
    std::vector<CustomSceneItem*> mSceneItems;
    int mCurrentNameIndex;
    void createSceneTextItem(QString text, QFont font, QColor color);
    void updateAddedElementsPart(bool newElement = true, bool groupIndexChanged = false);
    std::vector<AddedElement*> mAddedElementsVector;
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    CustomSceneItem* findClickedItemWithGraphicsItem(QGraphicsItem* itemClicked, int& index);
    void handleMouseDoubleClickFromSceneWithItem(CustomSceneItem* item);
    QMenu mPopUpMenu;
    bool mDisableEnableAllFlag;
    QMap<CustomSceneItem*, bool> mSceneItemEditWindowStates;
    QStringList mGroupNames;
    SetGroupScreen* mSetGroupScreenWindow;
    int mOldGroupSelectionIndex = -1;
    bool eventFilter(QObject* target, QEvent* event);
    CustomSceneItem* createSceneItemWithCommonProperties(QDomNode node);
    void setIbrePropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem);
    QString mProjectDir;
    bool checkNameExists(QString name, int exclusiveIndex);
    // SÝLÝNDÝ: const XMLCh* toXmlCh(QString str);  // Artýk gerek yok, QString kullanacaðýz
    QString createUniqueItemName(QString itemName, const QString& defaultNamePrefix = "imageItem");
    bool saveDashboard(QString dashboardName);
    bool saveDashboard(QString directory, QString dashboardName);
    bool showForgetDashboardDialog();
    void setLayoutWidgetsEnabled(const QLayout* layout, bool value);
    void setProjectDirectory(QString value);

protected slots:
    void on_pushButtonNewDashboard_clicked();
    void on_pushButtonLoadDashboard_clicked();
   // void on_pushButtonAddBackground_clicked();
    void on_pushButtonAddDashboard_clicked();
    void on_pushButtonAddNormalImage_clicked();
    void on_pushButtonAddLight_clicked();
    void on_pushButtonAddIbre_clicked();
    //void on_pushButtonAddTextItem_clicked();
    void on_pushButtonAddBar_clicked();
    void on_pushButtonToggleShowGroup_clicked();
    void on_pushButtonSlider_clicked();
    void on_pushButtonAddRectItem_clicked();
    /**/
    void on_pushButtonAddInput_clicked();
    void on_pushButtonAddWidget_clicked();
    /**/
    void handleMouseDoubleClickFromScene(QPointF param1);
    void handleMouseDoubleClickedOnItem(CustomSceneItem* item);
    void handleSelectedAddedElementPart(AddedElement* addedElement);
    void handleSignalSetParentTriggered(AddedElement* addedElement);
    void on_pushButtonSave_clicked();
    void on_pushButtonSaveAs_clicked();
    void handleWheelEventFromScene(QPointF param1, int param2);
    void resetDashboard();
    void disableEnableAllItems();
    void setBarPropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem);
    void setSliderPropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem);
    void setRectPropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem);
    void setInnerDashboardPropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem);
    void setInnerWidgetPropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem);
    void handleGroupNameEdited(int index, QString newName);
    void handleGroupSet(CustomSceneItem* element, int index);
    void handleNewGroupAdded(QString groupName);
    void handleCurrentGroupSelectionChanged(int index);
   
 

  

protected:
    void closeEvent(QCloseEvent* event) override;
   
   

private:
    Ui::DashboardGeneratorGui ui;
    void loadInnerDashboardAll(InnerDashboardItem* item);
    
};

#endif // DASHBOARDGENERATORGUI_H
