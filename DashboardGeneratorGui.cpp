/************************************************************************/
/* this code includes unnecessary repeats                 */
/************************************************************************/

#include "DashboardGeneratorGui.h"
#include "qfiledialog.h"
#include <QDebug>
#include <QLoggingCategory>

#include "qgraphicsitem.h"
#include <QInputDialog>
#include <QFontDialog>
#include "qcolordialog.h"
#include "qgraphicsview.h"
#include "qevent.h"
#include <QTransform>
#include "qgraphicssceneevent.h"
#include "SceneItemEdit.h"
#include "CustomSceneItem.h"

#include "AddMultipleSceneItem.h"
#include "IbreCustomSceneItem.h"
#include "qmessagebox.h"
#include "qbytearray.h"
#include "qfile.h"
#include "qtextdocument.h"
#include "qprogressdialog.h"
#include "BarCustomSceneItem.h"
#include "qcolor.h"
#include <QGraphicsProxyWidget>
#include "qprogressbar.h"
#include <CustomRectSceneItem.h>
#include <SliderCustomSceneItem.h>
#include <InnerDashboardItem.h>
#include <InnerDashboardItemEdit.h>
#include <qstringliteral.h>
#include <InnerWidgetItem.h>
#include <QCloseEvent>

#include <QCoreApplication>
#include <qdebug.h>
#include <qpainter.h>
#include <QSettings>
#include <QDomDocument>
#include <QDomElement>  
#include <QDomNode>
#include <QDomNodeList>
#include <QFile>
#include <QTextStream>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <QBrush>
#include <QPen>

Q_LOGGING_CATEGORY(dashboardGen, "dashboard.generator")








//Turkce karakter kullanilabilecek degiskenlerde toXmlCh methodu kullanilsin
// tekli direk isim yazdigimiz kullanimlarda X kullanalim														 

DashboardGeneratorGui::DashboardGeneratorGui(QWidget* parent)
    : QWidget(parent)
    , mCurrentSelectedCustomSceneItem(NULL)
    , mCurrentNameIndex(0)
{
    // Required to use 'QSettings'
    QCoreApplication::setOrganizationName("Simsoft");
    QCoreApplication::setApplicationName("GenericDashboardGenerator");

    ui.setupUi(this);

    /****************************/
    mEditGraphicViewWidget = new EditGraphicsViewWidget;
    mEditGraphicViewWidget->setGeometry(0, 0, 1920, 1080);
    mEditGraphicViewWidget->setWindowFlag(Qt::FramelessWindowHint, true);
    mEditGraphicViewWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    mEditGraphicViewWidget->show();
    mEditGraphicViewWidget->getGraphicsView()->scene()->installEventFilter(this);


    /****************************/
    


    //this->setMouseTracking ( true );
    //this->ui.graphicsViewMainScreen->installEventFilter ( this );
    //this->ui.graphicsViewMainScreen->scene()->installEventFilter(this);
    ui.toolBox->setMinimumWidth(320);
    //pop up menu
    mDisableEnableAllFlag = false;
    QAction* disableEnableAllAction = new QAction(this);
    connect(disableEnableAllAction, SIGNAL(triggered()), this, SLOT(disableEnableAllItems()));
    disableEnableAllAction->setText(QStringLiteral("Butun Nesneleri Goster/Gizle"));
    mPopUpMenu.addAction(disableEnableAllAction);
    mSetGroupScreenWindow = new SetGroupScreen(NULL);
    connect(mSetGroupScreenWindow, SIGNAL(groupNameEdited(int, QString)), this, SLOT(handleGroupNameEdited(int, QString)));
    connect(mSetGroupScreenWindow, SIGNAL(groupSet(CustomSceneItem*, int)), this, SLOT(handleGroupSet(CustomSceneItem*, int)));
    connect(mSetGroupScreenWindow, SIGNAL(newGroupAdded(QString)), this, SLOT(handleNewGroupAdded(QString)));
    mSetGroupScreenWindow->hide();
    connect(ui.comboBoxGroupSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCurrentGroupSelectionChanged(int)));

    this->ui.pushButtonNewDashboard->setEnabled(false);
    this->ui.pushButtonSave->setEnabled(false);
    this->setLayoutWidgetsEnabled(this->ui.verticalLayout_4, false);
    this->setLayoutWidgetsEnabled(this->ui.verticalLayout_5, false);

    mIsResizable = false;
    createResizeCursors();
    isMousePosOnCursor = true;
}

DashboardGeneratorGui::~DashboardGeneratorGui()
{
    // Clean up 'mEditGraphicViewWidget'
    if (this->mEditGraphicViewWidget != nullptr)
    {
        this->mEditGraphicViewWidget->close();
        delete this->mEditGraphicViewWidget;
        this->mEditGraphicViewWidget = nullptr;
    }

    // Clean up 'mSetGroupScreenWindow'
    if (this->mSetGroupScreenWindow != nullptr)
    {
        this->mSetGroupScreenWindow->close();
        delete this->mSetGroupScreenWindow;
        this->mSetGroupScreenWindow = nullptr;
    }
}

void DashboardGeneratorGui::closeEvent(QCloseEvent* event)
{
    QMessageBox msgBox;
    msgBox.setText(QStringLiteral("Yaptiginiz degisiklikleri kaydetmediyseniz, degisikler kaybolacak"));
    msgBox.setInformativeText(QStringLiteral("Kapatmak istiyor musunuz?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowIcon(this->windowIcon());
    int ret = msgBox.exec();
    if (ret != QMessageBox::Yes)
    {
        event->ignore(); // Prevent shutdown
    }
    else
    {
        // Clean up resources before closing
        if (this->mEditGraphicViewWidget != nullptr)
        {
            this->mEditGraphicViewWidget->close();
        }

        if (this->mSetGroupScreenWindow != nullptr)
        {
            this->mSetGroupScreenWindow->close();
        }

        event->accept(); // Continue the shutdown process
    }
}

void DashboardGeneratorGui::on_pushButtonLoadDashboard_clicked()
{
    if (!this->mProjectDir.isEmpty() && !this->showForgetDashboardDialog())
    {
        return;
    }

    // Get last load dashboard path
    QSettings settings;
    auto lastLoadDashboardPath = settings.value("GenericDashboardGenerator/LastLoadDashboardPath", "").toString();

    auto selectedFile = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Yuklenecek Dashboard XML dosyasini secin."),
        lastLoadDashboardPath,
        tr("XML files (*.xml)"));
    if (selectedFile.size() <= 0)
    {
        return;
    }

    auto file = new QFile(selectedFile);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCCritical(dashboardGen) << "CURRENT FILE CANNOT BE REACHED FOR LOADING DASHBOARD GENERATION!";

        return;
    }

    settings.setValue("GenericDashboardGenerator/LastLoadDashboardPath", selectedFile); // Save last load path

    QByteArray xml;
    xml = file->readAll();
    QFileInfo fileInfo(*file);
    QDir directoryInfo = fileInfo.absoluteDir();
    directoryInfo.cdUp();
    this->setProjectDirectory(directoryInfo.absolutePath());
    file->close();

    QDomDocument* generatorDocument = new QDomDocument;
    if (!generatorDocument->setContent(xml, true))
    {
        qCCritical(dashboardGen) << "CURRENT FILE CANNOT BE REACHED FOR LOADING DASHBOARD GENERATION!";

        return;
    }

    // Set window title to selected xml
    this->setWindowTitle(selectedFile.right(selectedFile.length() - selectedFile.lastIndexOf("/") - 1));
    QDomElement generatorElement = generatorDocument->documentElement();
    //taking common properties of dashboard
    ui.lineEditDashboardName->setText(generatorElement.attribute("name"));
    ui.lineEditDashboardBrightnessAttribute->setText(generatorElement.attribute("brightnessAttributeName"));
    /**********ADDED FOR FIXED SIZE******************/
    mEditGraphicViewWidget->getGraphicsView()->setFixedSize(QSize(generatorElement.attribute("width").toInt(), generatorElement.attribute("height").toInt()));
    mEditGraphicViewWidget->getGraphicsView()->scene()->setSceneRect(mEditGraphicViewWidget->getGraphicsView()->rect());
    /***************************/
    QProgressDialog progressDialog(tr("Aciliyor..."), QString(), 0, 100, this);
    progressDialog.setWindowTitle(tr("Dashboard dosyasi aciliyor..."));
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.show();
    progressDialog.setValue(0);
    int totalChildCount = generatorElement.childNodes().count();

    //taking background Element
    QDomNode backgroundNode;
    QDomNodeList backgroundNodeList = generatorElement.elementsByTagName("background");
    if (backgroundNodeList.size() > 0)
    {
        backgroundNode = backgroundNodeList.at(0);
    }
    if (!backgroundNode.isNull())
    {
        CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(backgroundNode);
        mSceneItems.push_back(newSceneItem);
    }

    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    //taking lights elements
    QDomNode lightsNode;
    QDomNodeList lightsNodeList = generatorElement.elementsByTagName("lights");
    if (lightsNodeList.size() > 0)
    {
        lightsNode = lightsNodeList.at(0);
    }
    if (!lightsNode.isNull())
    {
        QDomNodeList lightList = lightsNode.toElement().elementsByTagName("light");
        for (int i = 0; i < lightList.size(); i++)
        {
            CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(lightList.at(i));
            mSceneItems.push_back(newSceneItem);
        }
    }
    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    //taking ibreler items
    QDomNode ibrelerNode;
    QDomNodeList ibrelerNodeList = generatorElement.elementsByTagName("ibreler");
    if (ibrelerNodeList.size() > 0)
    {
        ibrelerNode = ibrelerNodeList.at(0);
    }
    if (!ibrelerNode.isNull())
    {
        QDomNodeList ibrelerList = ibrelerNode.toElement().elementsByTagName("ibre");
        for (int i = 0; i < ibrelerList.size(); i++)
        {
            CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(ibrelerList.at(i));
            setIbrePropertiesForItem(ibrelerList.at(i), newSceneItem);
            mSceneItems.push_back(newSceneItem);
        }
    }
    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    //taking bars items
    QDomNode barsNode;
    QDomNodeList barsNodeList = generatorElement.elementsByTagName("bars");
    if (barsNodeList.size() > 0)
    {
        barsNode = barsNodeList.at(0);
    }
    if (!barsNode.isNull())
    {
        QDomNodeList barsList = barsNode.toElement().elementsByTagName("bar");
        for (int i = 0; i < barsList.size(); i++)
        {
            CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(barsList.at(i));
            setBarPropertiesForItem(barsList.at(i), newSceneItem);
            mSceneItems.push_back(newSceneItem);
        }
    }
    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    //taking sliders items
    QDomNode slidersNode;
    QDomNodeList slidersNodeList = generatorElement.elementsByTagName("sliders");
    if (slidersNodeList.size() > 0)
    {
        slidersNode = slidersNodeList.at(0);
    }
    if (!slidersNode.isNull())
    {
        QDomNodeList slidersList = slidersNode.toElement().elementsByTagName("slider");
        for (int i = 0; i < slidersList.size(); i++)
        {
            CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(slidersList.at(i));
            setSliderPropertiesForItem(slidersList.at(i), newSceneItem);
            mSceneItems.push_back(newSceneItem);
        }
    }
    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    //taking rects items
    QDomNode rectsNode;
    QDomNodeList rectsNodeList = generatorElement.elementsByTagName("rectItems");
    if (rectsNodeList.size() > 0)
    {
        rectsNode = rectsNodeList.at(0);
    }
    if (!rectsNode.isNull())
    {
        QDomNodeList rectsList = rectsNode.toElement().elementsByTagName("rectItem");
        for (int i = 0; i < rectsList.size(); i++)
        {
            CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(rectsList.at(i));
            setRectPropertiesForItem(rectsList.at(i), newSceneItem);
            mSceneItems.push_back(newSceneItem);
        }
    }
    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    //taking inner dashboard items
    QDomNode innerDashboardsNode;
    QDomNodeList innerDashboardNodeList = generatorElement.elementsByTagName("innerDashboards");
    if (innerDashboardNodeList.size() > 0)
    {
        innerDashboardsNode = innerDashboardNodeList.at(0);
    }
    if (!innerDashboardsNode.isNull())
    {
        QDomNodeList innerDashboardList = innerDashboardsNode.toElement().elementsByTagName("innerDashboard");
        for (int i = 0; i < innerDashboardList.size(); i++)
        {
            CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(innerDashboardList.at(i));
            setInnerDashboardPropertiesForItem(innerDashboardList.at(i), newSceneItem);
            mSceneItems.push_back(newSceneItem);
        }
    }
    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    //taking singleimages items
    QDomNode singleImageNode;
    QDomNodeList singleImageNodeList = generatorElement.elementsByTagName("singleImages");
    if (singleImageNodeList.size() > 0)
    {
        singleImageNode = singleImageNodeList.at(0);
    }
    if (!singleImageNode.isNull())
    {
        QDomNodeList singleImageList = singleImageNode.toElement().elementsByTagName("singleimage");
        for (int i = 0; i < singleImageList.size(); i++)
        {
            // CHECK THIS OUT
            CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(singleImageList.at(i));
            mSceneItems.push_back(newSceneItem);
        }
    }
    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    //taking text items
    QDomNode textsNode;
    QDomNodeList textsNodeList = generatorElement.elementsByTagName("texts");
    if (textsNodeList.size() > 0)
    {
        textsNode = textsNodeList.at(0);
    }
    if (!textsNode.isNull())
    {
        QDomNodeList textList = textsNode.toElement().elementsByTagName("text");
        for (int i = 0; i < textList.size(); i++)
        {
            CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(textList.at(i));
            mSceneItems.push_back(newSceneItem);
        }
    }
    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    //taking inputs items
    /********/
    QDomNode inputsNode;
    QDomNodeList inputsNodeList = generatorElement.elementsByTagName("inputs");
    if (inputsNodeList.size() > 0)
    {
        inputsNode = inputsNodeList.at(0);
    }
    if (!inputsNode.isNull())
    {
        QDomNodeList inputList = inputsNode.toElement().elementsByTagName("input");
        for (int i = 0; i < inputList.size(); i++)
        {
            CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(inputList.at(i));
            mSceneItems.push_back(newSceneItem);
        }
    }
    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    /**********/
    // taking inner widget items
    QDomNode innerWidgetsNode;
    QDomNodeList innerWidgetNodeList = generatorElement.elementsByTagName("innerWidgets");
    if (innerWidgetNodeList.size() > 0)
    {
        innerWidgetsNode = innerWidgetNodeList.at(0);
    }
    if (!innerWidgetsNode.isNull())
    {
        QDomNodeList innerWidgetList = innerWidgetsNode.toElement().elementsByTagName("innerWidget");
        for (int i = 0; i < innerWidgetList.size(); i++)
        {
            CustomSceneItem* newSceneItem = createSceneItemWithCommonProperties(innerWidgetList.at(i));
            setInnerWidgetPropertiesForItem(innerWidgetList.at(i), newSceneItem);
            mSceneItems.push_back(newSceneItem);
        }
    }
    progressDialog.setValue(progressDialog.value() + 100.0f / totalChildCount);
    //update all
    updateAddedElementsPart();
    mSetGroupScreenWindow->mGroupNames = mGroupNames;
    mSetGroupScreenWindow->updateScreen();
}



void DashboardGeneratorGui::on_pushButtonNewDashboard_clicked()
{
    this->showForgetDashboardDialog();
}

void DashboardGeneratorGui::on_pushButtonAddNormalImage_clicked()
{
    auto newMultipleElement = new AddMultipleSceneItem(GRAPHICS_PIXMAP_ITEM);
    auto isNameExists = false;
    do
    {
        if (isNameExists)
        {
            QMessageBox::critical(this, "Ayni isimde baska bir element var!", "Ayni isimde baska bir element var");
        }

        newMultipleElement->exec();
        isNameExists = this->checkNameExists(newMultipleElement->mName, -1);
    } while (isNameExists);

    if (newMultipleElement->mNewlyAddedImagePaths.empty())
    {
        return;
    }

    auto itemName = this->createUniqueItemName(newMultipleElement->mName);

    std::vector <QGraphicsItem*> graphicsItems;
    std::vector <QPixmap> originalPixmapItems;
    for (auto i = 0; i < newMultipleElement->mNewlyAddedImagePaths.size(); i++)
    {
        QPixmap comingPix(newMultipleElement->mNewlyAddedImagePaths.at(i));
        originalPixmapItems.push_back(comingPix);
        auto newPixmapItem = new QGraphicsPixmapItem(comingPix);
        graphicsItems.push_back(newPixmapItem);
        this->addItemToScene(newPixmapItem);
    }

    auto newCustomItem = new PixmapCustomSceneItem(originalPixmapItems, itemName, GRAPHICS_PIXMAP_ITEM, graphicsItems, newMultipleElement->mShowOnStart, newMultipleElement->mAttrName, newMultipleElement->mAttrType);
    newCustomItem->mConfigurationsMap = newMultipleElement->mConfigurations;
    this->mSceneItems.push_back(newCustomItem);
    this->updateAddedElementsPart();
}

bool DashboardGeneratorGui::checkNameExists(QString name, int exclusiveIndex) {
    for (int i = 0; i < mSceneItems.size(); i++) {
        if (i == exclusiveIndex) {
            continue;
        }
        if (name.compare(mSceneItems.at(i)->mItemName) == 0) {
            return true;
        }
    }
    return false;
}


void DashboardGeneratorGui::on_pushButtonSave_clicked()
{
    if (this->mProjectDir.isEmpty() || this->ui.lineEditDashboardName->text().size() <= 0)
    {
        this->on_pushButtonSaveAs_clicked(); // If there is no existing file path, act like 'Save As'

        return;
    }

    this->saveDashboard(this->ui.lineEditDashboardName->text());
}

void DashboardGeneratorGui::on_pushButtonSaveAs_clicked()
{
    auto dashboardName = this->ui.lineEditDashboardName->text();
    if (dashboardName.size() <= 0)
    {
        auto isOk = false;
        dashboardName = QInputDialog::getText(
            this,
            tr("Dashboard ismi girin"),
            tr("Dashboard ismi:"),
            QLineEdit::Normal,
            QDir::home().dirName(),
            &isOk);
        if (!isOk || dashboardName.size() <= 0)
        {
            return;
        }

        this->ui.lineEditDashboardName->setText(dashboardName);
    }

    QSettings settings;
    auto lastSaveDashboardDirectory = settings.value("GenericDashboardGenerator/LastSaveDashboardDirectory", "").toString();
    auto dir = QFileDialog::getExistingDirectory(
        this,
        QStringLiteral("Dashboard Projesinin Cikarilacagi Klasoru Sec"),
        lastSaveDashboardDirectory,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.size() <= 0)
    {
        return;
    }

    settings.setValue("GenericDashboardGenerator/LastSaveDashboardDirectory", dir); // Save last save path

    this->setProjectDirectory(dir);

    this->saveDashboard(dashboardName);
}

void DashboardGeneratorGui::on_pushButtonAddLight_clicked()
{
    auto newMultipleElement = new AddMultipleSceneItem();
    auto isNameExists = false;
    do
    {
        if (isNameExists)
        {
            QMessageBox::critical(this, "Ayni isimde baska bir element var!", "Ayni isimde baska bir element var");
        }

        newMultipleElement->exec();
        isNameExists = this->checkNameExists(newMultipleElement->mName, -1);
    } while (isNameExists);

    if (newMultipleElement->mNewlyAddedImagePaths.empty())
    {
        return;
    }

    auto itemName = this->createUniqueItemName(newMultipleElement->mName);

    std::vector <QGraphicsItem*> graphicsItems;
    std::vector <QPixmap> originalPixmapItems;
    for (auto i = 0; i < newMultipleElement->mNewlyAddedImagePaths.size(); i++)
    {
        QPixmap comingPix(newMultipleElement->mNewlyAddedImagePaths.at(i));
        originalPixmapItems.push_back(comingPix);
        auto newPixmapItem = new QGraphicsPixmapItem(comingPix);
        graphicsItems.push_back(newPixmapItem);
        this->addItemToScene(newPixmapItem);
    }

    auto newCustomItem = new PixmapCustomSceneItem(originalPixmapItems, itemName, GRAPHICS_PIXMAP_LIGHT_ITEM, graphicsItems, newMultipleElement->mShowOnStart, newMultipleElement->mAttrName, newMultipleElement->mAttrType);
    newCustomItem->mConfigurationsMap = newMultipleElement->mConfigurations;
    this->mSceneItems.push_back(newCustomItem);
    this->updateAddedElementsPart();
}

void DashboardGeneratorGui::on_pushButtonAddIbre_clicked()
{
    auto newMultipleElement = new AddMultipleSceneItem(GRAPHICS_PIXMAP_IBRE_ITEM);
    auto isNameExists = false;
    do
    {
        if (isNameExists)
        {
            QMessageBox::critical(this, "Ayni isimde baska bir element var!", "Ayni isimde baska bir element var");
        }

        newMultipleElement->exec();
        isNameExists = this->checkNameExists(newMultipleElement->mName, -1);
    } while (isNameExists);

    if (newMultipleElement->mNewlyAddedImagePaths.empty())
    {
        return;
    }

    auto itemName = this->createUniqueItemName(newMultipleElement->mName);

    std::vector <QGraphicsItem*> graphicsItems;
    std::vector <QPixmap> originalPixmapItems;
    for (auto i = 0; i < newMultipleElement->mNewlyAddedImagePaths.size(); i++)
    {
        QPixmap comingPix(newMultipleElement->mNewlyAddedImagePaths.at(i));
        originalPixmapItems.push_back(comingPix);
        auto newPixmapItem = new QGraphicsPixmapItem(comingPix);
        graphicsItems.push_back(newPixmapItem);
        this->addItemToScene(newPixmapItem);
    }

    auto newCustomItem = new IbreCustomSceneItem(itemName, GRAPHICS_PIXMAP_IBRE_ITEM, graphicsItems, newMultipleElement->mShowOnStart, newMultipleElement->mAttrName, newMultipleElement->mAttrType);
    newCustomItem->mConfigurationsMap = newMultipleElement->mConfigurations;
    this->mSceneItems.push_back(newCustomItem);
    this->updateAddedElementsPart();
}

void DashboardGeneratorGui::on_pushButtonAddBar_clicked()
{
    CustomBarProxyWidgetItem* barItem = new CustomBarProxyWidgetItem;
    std::vector <QGraphicsItem*> graphicsItems;
    graphicsItems.push_back(barItem);
    addItemToScene(barItem);
    BarCustomSceneItem* newBarItem = new BarCustomSceneItem(QString("yeni bar nesnesi"), GRAPHICS_BAR_ITEM, graphicsItems, false, "", "float");
    barItem->createDefaultProgressBar(newBarItem);

    // Radial progress bar için özel item oluştur ve scene'e ekle
    if (newBarItem->mBarType == ARC_BAR || newBarItem->mBarType == ARC_BAR_REVERSE)
    {
        newBarItem->mRadialBar = new CustomRadialProgressBar(newBarItem);
        newBarItem->mRadialBar->setParentItem(barItem);
        newBarItem->mRadialBar->setPos(0, 0);
        newBarItem->mRadialBar->setZValue(10); // Üstte görünsün

        // Progress bar'ı gizle
        newBarItem->mProgressBar->setVisible(false);
        newBarItem->mProgressBar->hide();
    }

    mSceneItems.push_back(newBarItem);
    updateAddedElementsPart();
    handleMouseDoubleClickFromSceneWithItem(newBarItem);
    //we use scene item edit  

}

void DashboardGeneratorGui::on_pushButtonSlider_clicked()
{
    CustomSliderProxyWidgetItem* sliderItem = new CustomSliderProxyWidgetItem;
    std::vector <QGraphicsItem*> graphicsItems;
    graphicsItems.push_back(sliderItem);
    addItemToScene(sliderItem);
    SliderCustomSceneItem* newSliderItem = new SliderCustomSceneItem(QString("yeni slider nesnesi"), GRAPHICS_SLIDER_ITEM, graphicsItems, false, "", "float");
    sliderItem->createDefaultSlider(newSliderItem);
    mSceneItems.push_back(newSliderItem);
    updateAddedElementsPart();
    //we use scene item edit  
    handleMouseDoubleClickFromSceneWithItem(newSliderItem);
}

void DashboardGeneratorGui::on_pushButtonAddDashboard_clicked()
{
    CustomInnerDashboardRectItem* innerDashboardRectItem = new CustomInnerDashboardRectItem();
    std::vector<QGraphicsItem*> graphicsItems;
    graphicsItems.push_back(innerDashboardRectItem);

    InnerDashboardItem* innerDashboardItem = new InnerDashboardItem(
        QStringLiteral("new inner dashboard item"),
        7,
        graphicsItems,
        false, "", ""
    );

    mSceneItems.push_back(innerDashboardItem);

    // InnerDashboardItemEdit'i aç
    InnerDashboardItemEdit* editWidget = new InnerDashboardItemEdit(innerDashboardItem, this);
    editWidget->show();

    updateAddedElementsPart();
}
void DashboardGeneratorGui::on_pushButtonAddRectItem_clicked()
{
    CustomQGraphicsRectItem* rectItem = new CustomQGraphicsRectItem();
    std::vector <QGraphicsItem*> graphicsItems;
    graphicsItems.push_back(rectItem);

    CustomRectSceneItem* newCustomRectItem = new CustomRectSceneItem(QString("new rect item"), GRAPHICS_RECT_ITEM, graphicsItems, false, "", "bool");

    mSceneItems.push_back(newCustomRectItem);
    addItemToScene(rectItem);
    handleMouseDoubleClickFromSceneWithItem(newCustomRectItem);
}

void DashboardGeneratorGui::on_pushButtonAddInput_clicked()
{
    auto newButtonInputDialog = new ButtonInputDialog();
    auto isNameExists = false;
    do
    {
        if (isNameExists)
        {
            QMessageBox::critical(this, "Ayni isimde baska bir element var!", "Ayni isimde baska bir element var");
        }

        newButtonInputDialog->exec();
        isNameExists = this->checkNameExists(newButtonInputDialog->mName, -1);
    } while (isNameExists);

    auto itemName = this->createUniqueItemName(newButtonInputDialog->mName);

    std::vector <QPixmap> originalPixmapItems;
    std::vector <QGraphicsItem*> graphicsItems;
    for (auto i = 0; i < newButtonInputDialog->mButtonInput->getStateItemList().size(); i++) {
        QPixmap comingPix(newButtonInputDialog->mButtonInput->getStateItemList().at(i)->getStatePixmapItem()->pixmap());
        originalPixmapItems.push_back(comingPix);
        auto newPixmapItem = new QGraphicsPixmapItem(comingPix);
        graphicsItems.push_back(newPixmapItem);
        this->addItemToScene(newPixmapItem);
    }

    auto newButtonInput = newButtonInputDialog->mButtonInput;
    auto newCustomItem = new PixmapInputSceneItem(newButtonInput, originalPixmapItems, itemName, GRAPHICS_PIXMAP_BUTTON_INPUT_ITEM, graphicsItems, newButtonInputDialog->mShowOnStart, newButtonInputDialog->mAttrName, newButtonInputDialog->mAttrType);
    newCustomItem->mConfigurationsMap = newButtonInputDialog->mConfigurations;

    this->mSceneItems.push_back(newCustomItem);
    this->updateAddedElementsPart();
}

static inline void addToSceneHelper(QGraphicsView* view, QGraphicsItem* it) {
    view->scene()->addItem(it);
}

void DashboardGeneratorGui::loadInnerDashboardAll(InnerDashboardItem* item)
{
    if (!item || !item->mCustomInnerDashboardRectItem) return;

    const QPointF basePos = item->mCustomInnerDashboardRectItem->scenePos();

    // 1) Panel arka plan
    QString panelPath = QString("Dashboards/%1/panel.png").arg(item->mItemName); // isme göre çöz
    QPixmap panelPx(panelPath);
    if (!panelPx.isNull()) {
        auto bg = new QGraphicsPixmapItem(panelPx);
        bg->setPos(basePos);
        bg->setZValue(-100);
        addToSceneHelper(mEditGraphicViewWidget->getGraphicsView(), bg);
    }

    // 2) İkonlar (örnek)
    struct ImgDef { QString path; int x; int y; int z; };
    QList<ImgDef> images{
        { "Dashboards/Common/icons/turn_left.png", 120, 45, 1 },
        { "Dashboards/Common/icons/stop.png",      266, 45, 1 },
    };
    for (const auto& im : images) {
        QPixmap px(im.path);
        if (px.isNull()) continue;
        auto it = new QGraphicsPixmapItem(px);
        it->setPos(basePos + QPointF(im.x, im.y));
        it->setZValue(im.z);
        addToSceneHelper(mEditGraphicViewWidget->getGraphicsView(), it);
    }

    // 3) Yazılar (örnek)
    struct TxtDef { QString text; QString font; int size; QColor color; int x; int y; int z; };
    QList<TxtDef> texts{
        { "rpmText", "Arial", 11, QColor("#FFFFFF"), 900, 680, 2 }
    };
    for (const auto& t : texts) {
        auto tx = new QGraphicsSimpleTextItem(t.text);
        tx->setFont(QFont(t.font, t.size));
        tx->setBrush(QBrush(t.color));
        tx->setPos(basePos + QPointF(t.x, t.y));
        tx->setZValue(t.z);
        addToSceneHelper(mEditGraphicViewWidget->getGraphicsView(), tx);
    }

    // 4) İbre/Gauge (kendi sınıfınla ekle)
    // auto spd = new IbreCustomSceneItem(...);
    // spd->setPos(basePos + QPointF(120, 420));
    // spd->setZValue(3);
    // addToSceneHelper(mEditGraphicViewWidget->getGraphicsView(), spd);

    // 5) Yeşil placeholder’ı gizle
    item->mCustomInnerDashboardRectItem->mFillColor = QColor(0, 0, 0, 0);
    item->mCustomInnerDashboardRectItem->update();
}


void DashboardGeneratorGui::on_pushButtonAddWidget_clicked()
{
    CustomInnerWidgetRectItem* innerWidgetRectItem = new CustomInnerWidgetRectItem();
    std::vector <QGraphicsItem*> graphicsItems;
    graphicsItems.push_back(innerWidgetRectItem);
    InnerWidgetItem* innerWidgetItem = new InnerWidgetItem(QString("new inner widget item"), INNER_WIDGET_ITEM, graphicsItems, false, "", "bool");
    mSceneItems.push_back(innerWidgetItem);
    addItemToScene(innerWidgetRectItem);
    handleMouseDoubleClickFromSceneWithItem(innerWidgetItem);
}

void DashboardGeneratorGui::createSceneItemsWithGivenImagePaths(QStringList imagePaths, bool isBackground)
{
    //for ( int i = 0; i < imagePaths.size ( );i++ )
    //{
    //    QString currentFileName = imagePaths.at ( i );
    //    QPixmap* readPixmap = new QPixmap ( currentFileName );
    //    if (readPixmap.isNull())
    //    {
    //        continue;
    //    }
    //    QGraphicsPixmapItem* newPixmapItem = new QGraphicsPixmapItem ( *readPixmap);
    //    QString currentName;
    //    bool showOnStart = false;
    //    int typeId = GRAPHICS_PIXMAP_ITEM;
    //    if (!isBackground)
    //    {
    //     mCurrentNameIndex++;
    //     currentName = "imageItem" + QString::number(mCurrentNameIndex);
    //    }
    //    else
    //    {
    //        currentName = "background";
    //        ui.graphicsViewMainScreen->setFixedSize ( readPixmap->size() );
    //        ui.graphicsViewMainScreen->scene ( )->setSceneRect ( ui.graphicsViewMainScreen->rect ( ) );
    //        newPixmapItem->setZValue ( -10 );
    //        showOnStart = true;
    //        typeId = GRAPHICS_PIXMAP_BACKGROUND_ITEM;
    //        //when background is added move all items to 0,0
    //        for ( int i = 0; i < mSceneItems.size ( );i++ )
    //        {
    //            for ( int j = 0; j < mSceneItems.at ( i )->mItems.size ( );j++ )
    //            {
    //            	mSceneItems.at(i)->mItems.at(j)->setPos ( 0 , 0 );
    //            }
    //        }
    //    }
    //    CustomSceneItem* newCustomItem = new CustomSceneItem ( currentName , typeId , std::vector<QGraphicsItem*> ( { newPixmapItem } ) , showOnStart );
    //    mSceneItems.push_back ( newCustomItem );
    //    addItemToScene ( newPixmapItem );
    //}
}

void DashboardGeneratorGui::addItemToScene(QGraphicsItem* item)
{
    /********************************/
    mEditGraphicViewWidget->addToGraphicsScene(item);
    /********************************/
    updateAddedElementsPart();
}

void DashboardGeneratorGui::createSceneTextItem(QString text, QFont font, QColor color)
{
    QGraphicsTextItem* item = new QGraphicsTextItem(text);
    item->setFont(font);
    item->setDefaultTextColor(color);
    mCurrentNameIndex++;
    mCurrentNameIndex++;
    QString currentName = "textItem" + QString::number(mCurrentNameIndex);
    CustomSceneItem* newCustomItem = new CustomSceneItem(currentName, GRAPHICS_TEXT_ITEM, std::vector<QGraphicsItem*>({ item }));
    mSceneItems.push_back(newCustomItem);
    addItemToScene(item);
}

void DashboardGeneratorGui::updateAddedElementsPart(bool newElement, bool groupIndexChanged)
{
    //add groups
    int currentGroupSelection = ui.comboBoxGroupSelect->currentIndex();
    if (groupIndexChanged || ui.comboBoxGroupSelect->count() != mGroupNames.size() + 1)
    {
        ui.comboBoxGroupSelect->clear();
        ui.comboBoxGroupSelect->addItem("No Group");
        for (int i = 0; i < mGroupNames.size(); i++)
        {
            ui.comboBoxGroupSelect->addItem(mGroupNames.at(i));
        }
        if (currentGroupSelection >= 0)
        {
            ui.comboBoxGroupSelect->setCurrentIndex(currentGroupSelection);
        }
    }
    //if it is a new element or some is deleted, delete all items and add again
    if (newElement)

    {
        //first delete all children of grid layout
        QList<AddedElement*> childrenOFGridLayout = this->findChildren<AddedElement*>();

        for (int i = 0; i < childrenOFGridLayout.size(); i++)
        {
            delete childrenOFGridLayout.at(i);
            //this->ui.gridLayoutAddedElements->removeWidget(childrenOFGridLayout.at(i));
            //childrenOFGridLayout.at(i)->hide();
        }
        mAddedElementsVector.clear();
        int currentRow = 0;
        int columnLimit = 3;
        int currentColumn = 0;
        for (int i = 0; i < mSceneItems.size(); i++)
        {
            if ((currentGroupSelection - 1 < 0 && mSceneItems.at(i)->mGroupIndex < 0) || mSceneItems.at(i)->mGroupIndex == currentGroupSelection - 1)
            {
                AddedElement* newAddedElement = new AddedElement(mSceneItems.at(i), this);
                connect(newAddedElement, SIGNAL(mouseDoubleClickedOnItem(CustomSceneItem*)), this, SLOT(handleMouseDoubleClickedOnItem(CustomSceneItem*)), Qt::QueuedConnection);
                connect(newAddedElement, SIGNAL(signalSelectedAddedElementPart(AddedElement*)), this, SLOT(handleSelectedAddedElementPart(AddedElement*)));
                connect(newAddedElement, SIGNAL(signalSetParentTriggered(AddedElement*)), this, SLOT(handleSignalSetParentTriggered(AddedElement*)));
                ui.gridLayoutAddedElements->addWidget(newAddedElement, currentRow, currentColumn);
                currentColumn++;
                if (currentColumn >= columnLimit)
                {
                    currentRow++;
                    currentColumn = 0;
                }
                newAddedElement->mCustomItem->updateShowStatus(newAddedElement->mCustomItem->mIsItemShown);
                mAddedElementsVector.push_back(newAddedElement);
            }
        }
    }
    //else its purpose is editing no need to add again
    else
    {
        for (int i = 0; i < mAddedElementsVector.size(); i++)
        {
            mAddedElementsVector.at(i)->updateParts();
        }
    }

}

void DashboardGeneratorGui::mouseDoubleClickEvent(QMouseEvent* event)
{
    //QPoint clickedPos = ui.graphicsViewMainScreen->mapFromParent ( event->pos ( ) );
    //mInDragState = false;
    //mDraggingItem = QDomElement();
    ////show edit screens

}

void DashboardGeneratorGui::mousePressEvent(QMouseEvent* event)
{
    /*if (!mIsResizeable)
    {
        mIsResizeable = false;
        QPoint clickedPos = event->pos();
        qDebug() << clickedPos;
        QGraphicsItem* itemClicked = mEditGraphicViewWidget->getGraphicsView()->scene()->itemAt(clickedPos,mEditGraphicViewWidget->getGraphicsView()->transform());
        if (itemClicked!=NULL)
        {
            int index;
            CustomSceneItem* clickedItem = findClickedItemWithGraphicsItem ( itemClicked , index);
            if (clickedItem!=NULL)
            {
                mResizeableItem = clickedItem;
            }
        }
    }
    else
    {
        mIsResizeable = false;
        mResizeableItem = QDomElement();
    }*/
}

/********/
void DashboardGeneratorGui::setCursorPosOnScene(CustomSceneItem* item) {
    QPointF currentSelectedCustomSceneItemPos = mCurrentSelectedCustomSceneItem->mItems.at(item->mCurrentActiveItem)->pos();

    mCurrentSelectedCustomSceneItemWidth = mCurrentSelectedCustomSceneItem->mItems.at(item->mCurrentActiveItem)->boundingRect().width();
    mCurrentSelectedCustomSceneItemHeight = mCurrentSelectedCustomSceneItem->mItems.at(item->mCurrentActiveItem)->boundingRect().height();
    // set the pos
    for (int i = 0; i < mResizeCursors.size(); i++) {

        mResizeCursors.at(i)->setVisible(true);

        mResizeCursors.at(i)->setZValue(10);
        mResizeCursors.at(i)->setOffset(currentSelectedCustomSceneItemPos);
    }
    setResizeCursorsPositions();
}

void DashboardGeneratorGui::createResizeCursors() {
    mResizeCursors.clear();
    mRightCursor = new QGraphicsPixmapItem(QPixmap("C:/Users/murat.aydin/Desktop/right-left.png"));
    mBottomTopCursor = new QGraphicsPixmapItem(QPixmap("C:/Users/murat.aydin/Desktop/bottom-top.png"));
    mCornerCursor = new QGraphicsPixmapItem(QPixmap("C:/Users/murat.aydin/Desktop/corner.png"));

    mResizeCursors.push_back(mRightCursor);
    mResizeCursors.push_back(mBottomTopCursor);
    mResizeCursors.push_back(mCornerCursor);

    for (int i = 0; i < mResizeCursors.size(); i++) {
        addItemToScene(mResizeCursors.at(i));
        mResizeCursors.at(i)->setVisible(false);
    }
}

void DashboardGeneratorGui::handleMousePressFromScene(QPointF param1) {
    // Find better solution
    tempParam1 = param1;
    handleMousePosOnCursor(param1);
    if (!mIsResizable) {
        mIsResizable = true;
        QGraphicsItem* itemClicked = mEditGraphicViewWidget->getGraphicsView()->scene()->itemAt(param1, mEditGraphicViewWidget->getGraphicsView()->transform());
        if (itemClicked != nullptr)
        {
            int index;
            CustomSceneItem* clickedItem = findClickedItemWithGraphicsItem(itemClicked, index);
            if (clickedItem != nullptr)
            {
                if (isPixmapItem(clickedItem) && clickedItem->mItems.size() > 0)
                {
                    mCurrentSelectedCustomSceneItem = clickedItem;
                    setCursorPosOnScene(mCurrentSelectedCustomSceneItem);
                }
                else
                {
                    mCurrentSelectedCustomSceneItem = nullptr;
                    for (int i = 0; i < mResizeCursors.size(); i++) {
                        mResizeCursors.at(i)->setVisible(false);
                    }
                }
            }
        }
    }
    else {
        mIsResizable = false;
    }
}

void DashboardGeneratorGui::handleMouseReleaseFromScene() {
    if (mCurrentSelectedCustomSceneItem != nullptr && mCurrentSelectedCustomSceneItem->mItems.size() > 0 && isMousePosOnCursor) {
                for (int i = 0; i < mCurrentSelectedCustomSceneItem->mItems.size(); i++) {
            if (i != mCurrentSelectedCustomSceneItem->mCurrentActiveItem) {
                QPixmap scaledPixmap;
                QGraphicsPixmapItem* currentPixmapItem = ((QGraphicsPixmapItem*)mCurrentSelectedCustomSceneItem->mItems.at(i));
                QPixmap currentResizePixmap = currentPixmapItem->pixmap();
                scaledPixmap = currentResizePixmap.scaled(mCurrentPixmapSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                currentPixmapItem->setPixmap(scaledPixmap);
            }
        }
    }
}

void DashboardGeneratorGui::resizeEvent(QPointF param1) {
    if (mCurrentSelectedCustomSceneItem != nullptr) 
    {
        mCursorPos = mEditGraphicViewWidget->getGraphicsView()->mapFromGlobal(QCursor::pos());
        QPointF clickedPos = mEditGraphicViewWidget->getGraphicsView()->mapFromScene(param1);
        QPixmap originalPixmap = ((PixmapCustomSceneItem*)mCurrentSelectedCustomSceneItem)->currentOriginalPixmapItem();
        QGraphicsPixmapItem* currentPixmapItem = ((QGraphicsPixmapItem*)(mCurrentSelectedCustomSceneItem->mItems.at(mCurrentSelectedCustomSceneItem->mCurrentActiveItem)));
        QPixmap scaledPixmap;
        QPixmap currentPixmap = currentPixmapItem->pixmap();
        mCurrentPixmapSize = currentPixmap.size();
        int PixmapSize_minWidth = 50;
        int PixmapSize_minHeight = 50;
        if (mCurrentPixmapSize.width() >= PixmapSize_minWidth && mCurrentPixmapSize.height() >= PixmapSize_minHeight) {
            if (_resizeMode == resizeMode::RIGHT)
            {
                if (mCursorPos.x() > clickedPos.x() || mCursorPos.x() == clickedPos.x()) {
                    mCurrentPixmapSize.setWidth(mCurrentSelectedCustomSceneItemWidth + (mCursorPos.x() - clickedPos.x()));
                }
                else if (mCursorPos.x() < clickedPos.x() || mCursorPos.x() == clickedPos.x()) {
                    mCurrentPixmapSize.setWidth(mCurrentSelectedCustomSceneItemWidth - abs((mCursorPos.x() - clickedPos.x())));
                }
            }
            else if (_resizeMode == resizeMode::BOTTOM) {
                if (mCursorPos.y() > clickedPos.y() || mCursorPos.y() == clickedPos.y()) {
                    mCurrentPixmapSize.setHeight(mCurrentSelectedCustomSceneItemHeight + (mCursorPos.y() - clickedPos.y()));
                }
                else if (mCursorPos.y() < clickedPos.y() || mCursorPos.y() == clickedPos.y()) {
                    mCurrentPixmapSize.setHeight(mCurrentSelectedCustomSceneItemHeight - abs((mCursorPos.y() - clickedPos.y())));
                }
            }
            else if (_resizeMode == resizeMode::BOTTOM_RIGHT) {
                if (mCursorPos.x() > clickedPos.x() && mCursorPos.y() > clickedPos.y() || mCursorPos.y() == clickedPos.y() || mCursorPos.x() == clickedPos.x()) {
                    mCurrentPixmapSize.setWidth(mCurrentSelectedCustomSceneItemWidth + (mCursorPos.x() - clickedPos.x()));
                    mCurrentPixmapSize.setHeight(mCurrentSelectedCustomSceneItemHeight + (mCursorPos.y() - clickedPos.y()));

                }
                else if (mCursorPos.x() < clickedPos.x() && mCursorPos.y() < clickedPos.y() || mCursorPos.y() == clickedPos.y() || mCursorPos.x() == clickedPos.x()) {
                    mCurrentPixmapSize.setWidth(mCurrentSelectedCustomSceneItemWidth - abs((mCursorPos.x() - clickedPos.x())));
                    mCurrentPixmapSize.setHeight(mCurrentSelectedCustomSceneItemHeight - abs((mCursorPos.y() - clickedPos.y())));
                }
            }
            if (mCurrentPixmapSize.width() < PixmapSize_minWidth) {
                mCurrentPixmapSize.setWidth(PixmapSize_minWidth);
            }
            if (mCurrentPixmapSize.height() < PixmapSize_minHeight) {
                mCurrentPixmapSize.setHeight(PixmapSize_minHeight);
            }
            scaledPixmap = originalPixmap.scaled(mCurrentPixmapSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            currentPixmapItem->setPixmap(scaledPixmap);

            setResizeCursorsPositions();
        }
    }
}

void DashboardGeneratorGui::handleMousePosOnCursor(QPointF param1) {
    if (mResizeCursors.at(0)->contains(mResizeCursors.at(0)->mapFromScene(param1))) {
        _resizeMode = resizeMode::RIGHT;
        isMousePosOnCursor = true;
    }
    else if (mResizeCursors.at(1)->contains(mResizeCursors.at(1)->mapFromScene(param1))) {
        _resizeMode = resizeMode::BOTTOM;
        isMousePosOnCursor = true;
    }
    else if (mResizeCursors.at(2)->contains(mResizeCursors.at(2)->mapFromScene(param1))) {
        _resizeMode = resizeMode::BOTTOM_RIGHT;
        isMousePosOnCursor = true;
    }
    else {
        _resizeMode = resizeMode::NONE;
        isMousePosOnCursor = false;
    }
}

void  DashboardGeneratorGui::handleMouseMoveFromScene()
{
    QPointF currentSelectedCustomSceneItemPos;
    if (isMousePosOnCursor) {
        resizeEvent(tempParam1);
    }
    else if (!isMousePosOnCursor) {
        resizeEvent(tempParam1);
    }
    if (mCurrentSelectedCustomSceneItem != nullptr) {
        currentSelectedCustomSceneItemPos = mCurrentSelectedCustomSceneItem->getActiveItemPos();
        for (int i = 0; i < mResizeCursors.size(); i++) {
            mResizeCursors.at(i)->setOffset(currentSelectedCustomSceneItemPos);
        }
    }
}

void DashboardGeneratorGui::setResizeCursorsPositions()
{
    if (mCurrentSelectedCustomSceneItem != nullptr)
    {
        QGraphicsPixmapItem* currentPixmapItem = ((QGraphicsPixmapItem*)mCurrentSelectedCustomSceneItem->mItems.at(mCurrentSelectedCustomSceneItem->mCurrentActiveItem));
        QPixmap currentPixmap = currentPixmapItem->pixmap();
        // TODO : CHECK
        mResizeCursors.at(0)->setPos(currentPixmap.width() - (mResizeCursors.at(0)->pixmap().width() / 2), currentPixmap.height() / 2);    // right
        mResizeCursors.at(1)->setPos((currentPixmap.width() / 2) - (mResizeCursors.at(1)->pixmap().width() / 2), currentPixmap.height() - (mResizeCursors.at(1)->pixmap().height() / 2));    // bottom
        mResizeCursors.at(2)->setPos(currentPixmap.width() - (mResizeCursors.at(2)->pixmap().width() / 2), currentPixmap.height() - (mResizeCursors.at(2)->pixmap().height() / 2));      // corner   
    }
}

bool DashboardGeneratorGui::isPixmapItem(CustomSceneItem* customSceneItem)
{
    return  customSceneItem->mTypeId == GRAPHICS_PIXMAP_BUTTON_INPUT_ITEM ||
        customSceneItem->mTypeId == GRAPHICS_PIXMAP_ITEM ||
        customSceneItem->mTypeId == GRAPHICS_PIXMAP_LIGHT_ITEM ||
        customSceneItem->mTypeId == GRAPHICS_PIXMAP_MULTIPLE_LIGHT_ITEM;
}

void DashboardGeneratorGui::mouseMoveEvent(QMouseEvent* event)
{
    /* int adjust = 10;
    if (mIsResizable && mResizableItem!.isNull())
    {
        if (cursorPos.x() > (mBoundingRect.height() - adjust) && cursorPos.x() < (mBoundingRect.height() + adjust))
        {
            setCursor(Qt::SizeHorCursor);
            QApplication::setOverrideCursor(Qt::SizeHorCursor);
        }
    }
    qDebug() << "current cursor pos" << movePos;*/
}
/********/
CustomSceneItem* DashboardGeneratorGui::findClickedItemWithGraphicsItem(QGraphicsItem* itemClicked, int& index)
{
    for (int i = 0; i < mSceneItems.size(); i++)
    {
        for (int j = 0; j < mSceneItems.at(i)->mItems.size(); j++)
        {
            if (mSceneItems.at(i)->mItems.at(j) == itemClicked)
            {
                index = i;
                return mSceneItems.at(i);
            }
        }
    }
    return NULL;
}

bool DashboardGeneratorGui::eventFilter(QObject* target, QEvent* event)
{
    if (target == mEditGraphicViewWidget->getGraphicsView()->scene())
    {
        if (event->type() == QEvent::GraphicsSceneMouseDoubleClick)
        {
            QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
            handleMouseDoubleClickFromScene(mouseEvent->scenePos());
        }
        else if (event->type() == QEvent::GraphicsSceneWheel)
        {
            QGraphicsSceneWheelEvent* mouseEvent = static_cast<QGraphicsSceneWheelEvent*>(event);
            handleWheelEventFromScene(mouseEvent->scenePos(), mouseEvent->delta());
        }
        else if (event->type() == QEvent::GraphicsSceneMousePress)
        {
            QGraphicsSceneMouseEvent* mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
            if (mouseEvent->button() == Qt::RightButton)
            {
                mPopUpMenu.popup(QPoint(mouseEvent->screenPos().x(), mouseEvent->screenPos().y()));
                return true;
            }
            if (mouseEvent->buttons() == Qt::LeftButton) {
                isMousePressed = true;
                handleMousePressFromScene(mouseEvent->scenePos());
            }
        }
        else if (event->type() == QEvent::GraphicsSceneMouseMove && isMousePressed) {
            handleMouseMoveFromScene();
        }
        else if (event->type() == QEvent::GraphicsSceneMouseRelease) {
            isMousePressed = false;
            handleMouseReleaseFromScene();
        }
        return false;
    }
    return false;
}

void DashboardGeneratorGui::handleMouseDoubleClickFromScene(QPointF param1)
{
    qDebug() << "***** DOUBLE CLICK DETECTED! *****";
    qDebug() << "Clicked position:" << param1;

    QGraphicsItem* itemClicked = mEditGraphicViewWidget->getGraphicsView()->scene()->itemAt(param1, mEditGraphicViewWidget->getGraphicsView()->transform());

    if (itemClicked != nullptr)
    {
        qDebug() << "Item found!";

        int index;
        CustomSceneItem* clickedItem = findClickedItemWithGraphicsItem(itemClicked, index);

        if (clickedItem != nullptr)
        {
            qDebug() << "Creating SceneItemEdit dialog...";
            qDebug() << "Item type ID:" << clickedItem->mTypeId;
            qDebug() << "Item name:" << clickedItem->mItemName;

            // *** BACKGROUND VE DİĞER ÖZEL ITEM'LARI HARİÇ TUT ***
            if (clickedItem->mTypeId == GRAPHICS_PIXMAP_BACKGROUND_ITEM)
            {
                qDebug() << "Background item clicked, ignoring";
                return;
            }

            // *** Diğer editable item'lar için dialog aç ***
            if (clickedItem->mTypeId == GRAPHICS_PIXMAP_BUTTON_INPUT_ITEM)
            {
                qDebug() << "Opening ButtonInputDialog...";

                ButtonInputDialog* buttonInputDialogItem = new ButtonInputDialog((PixmapInputSceneItem*)clickedItem);

                bool nameExists = false;
                do
                {
                    if (nameExists)
                    {
                        QMessageBox::critical(this, "Ayni isimde baska bir element var!", "Ayni isimde baska bir element var");
                    }
                    buttonInputDialogItem->exec();

                    nameExists = checkNameExists(mSceneItems.at(index)->mItemName, index);
                } while (nameExists);

                if (buttonInputDialogItem->mItemIsDeleted)
                {
                    for (int j = 0; j < clickedItem->mItems.size(); j++)
                    {
                        mEditGraphicViewWidget->getGraphicsView()->scene()->removeItem(clickedItem->mItems.at(j));
                    }
                    mSceneItems.erase(mSceneItems.begin() + index);
                }
                else
                {
                    for (QMap<QString, QString>::iterator iter = buttonInputDialogItem->mNewlyAddedImagePaths.begin(); iter != buttonInputDialogItem->mNewlyAddedImagePaths.end(); iter++)
                    {
                        QPixmap comingPix(iter.value());
                        QGraphicsPixmapItem* newPixmapItem = new QGraphicsPixmapItem(comingPix);
                        clickedItem->addNewItemWithProperties(newPixmapItem);
                        addItemToScene(newPixmapItem);
                    }
                }

                if (buttonInputDialogItem->mItemIsDeleted)
                {
                    updateAddedElementsPart();
                }
                else
                {
                    updateAddedElementsPart(false);
                }

                qDebug() << "ButtonInputDialog closed";
            }
            else
            {
                qDebug() << "Opening SceneItemEdit...";

                SceneItemEdit* newSceneItemEdit = new SceneItemEdit(clickedItem);

                qDebug() << "SceneItemEdit created, calling exec()...";

                /*********************************/
                bool nameExists = false;
                do
                {
                    if (nameExists)
                    {
                        QMessageBox::critical(this, "Ayni isimde baska bir element var!", "Ayni isimde baska bir element var");
                    }

                    qDebug() << "Calling newSceneItemEdit->exec()...";
                    newSceneItemEdit->exec();
                    qDebug() << "exec() returned";

                    nameExists = checkNameExists(mSceneItems.at(index)->mItemName, index);
                } while (nameExists);
                /************************************/

                if (newSceneItemEdit->mItemIsDeleted)
                {
                    qDebug() << "Item deleted, removing from scene...";
                    for (int j = 0; j < clickedItem->mItems.size(); j++)
                    {
                        mEditGraphicViewWidget->getGraphicsView()->scene()->removeItem(clickedItem->mItems.at(j));
                    }
                    mSceneItems.erase(mSceneItems.begin() + index);
                }
                else
                {
                    qDebug() << "Item saved, adding new images...";
                    for (int i = 0; i < newSceneItemEdit->mNewlyAddedImagePaths.size(); i++)
                    {
                        QPixmap comingPix(newSceneItemEdit->mNewlyAddedImagePaths.at(i));
                        QGraphicsPixmapItem* newPixmapItem = new QGraphicsPixmapItem(comingPix);
                        clickedItem->addNewItemWithProperties(newPixmapItem);
                        addItemToScene(newPixmapItem);
                    }
                }

                if (newSceneItemEdit->mItemIsDeleted)
                {
                    updateAddedElementsPart();
                }
                else
                {
                    updateAddedElementsPart(false);
                }

                qDebug() << "SceneItemEdit closed";
            }

            qDebug() << "Dialog handling completed";
        }
        else
        {
            qDebug() << "ERROR: clickedItem is NULL!";
        }
    }
    else
    {
        qDebug() << "ERROR: itemClicked is NULL (clicked on empty area)";
    }

    qDebug() << "***** DOUBLE CLICK HANDLER FINISHED *****";
}


void DashboardGeneratorGui::handleMouseDoubleClickFromSceneWithItem(CustomSceneItem* item)
{
    int index;
    //just to find necessary index
    CustomSceneItem* clickedItem = findClickedItemWithGraphicsItem(item->mItems.at(0), index);

    if (mSceneItemEditWindowStates.value(clickedItem, false))
    {
        return;
    }

    // ============ 1. TİP: BUTTON INPUT ITEM ============
    if (clickedItem->mTypeId == GRAPHICS_PIXMAP_BUTTON_INPUT_ITEM) {
        ButtonInputDialog* buttonInputDialogItem = new ButtonInputDialog((PixmapInputSceneItem*)clickedItem);
        mSceneItemEditWindowStates[clickedItem] = true;
        bool nameExists = false;
        do
        {
            if (nameExists)
            {
                QMessageBox::critical(this, "Ayni isimde baska bir element var!", "Ayni isimde baska bir element var");
            }
            buttonInputDialogItem->exec();

            nameExists = checkNameExists(mSceneItems.at(index)->mItemName, index);
        } while (nameExists);

        if (buttonInputDialogItem->mItemIsDeleted)
        {
            for (int j = 0; j < clickedItem->mItems.size(); j++)
            {
                mEditGraphicViewWidget->getGraphicsView()->scene()->removeItem(clickedItem->mItems.at(j));
            }
            mSceneItems.erase(mSceneItems.begin() + index);
        }
        else
        {
            for (QMap<QString, QString>::iterator iter = buttonInputDialogItem->mNewlyAddedImagePaths.begin(); iter != buttonInputDialogItem->mNewlyAddedImagePaths.end(); iter++)
            {
                QPixmap comingPix(iter.value());
                QGraphicsPixmapItem* newPixmapItem = new QGraphicsPixmapItem(comingPix);
                clickedItem->addNewItemWithProperties(newPixmapItem);
                addItemToScene(newPixmapItem);
            }
        }
        if (buttonInputDialogItem->mItemIsDeleted)
        {
            updateAddedElementsPart();
        }
        else
        {
            updateAddedElementsPart(false);
        }
        mSceneItemEditWindowStates[clickedItem] = false;
    }

    // ============ 2. TİP: INNERDASHBOARD ITEM (YENİ) ============
    else if (clickedItem->mTypeId == 7) {  // INNERDASHBOARDITEM
        InnerDashboardItem* innerItem = (InnerDashboardItem*)clickedItem;

        // Önce InnerDashboardItemEdit'i aç (modeless - arka planda kalır)
        InnerDashboardItemEdit* innerEdit = new InnerDashboardItemEdit(innerItem, this);
        innerEdit->setAttribute(Qt::WA_DeleteOnClose);
        innerEdit->show();

        // Sonra SceneItemEdit'i aç (modal - bekler)
        SceneItemEdit* newSceneItemEdit = new SceneItemEdit(item);
        mSceneItemEditWindowStates[clickedItem] = true;

        bool nameExists = false;
        do
        {
            if (nameExists)
            {
                QMessageBox::critical(this, "Ayni isimde baska bir element var!", "Ayni isimde baska bir element var");
            }
            newSceneItemEdit->exec();
            nameExists = checkNameExists(mSceneItems.at(index)->mItemName, index);
        } while (nameExists);

        if (newSceneItemEdit->mItemIsDeleted)
        {
            for (int j = 0; j < clickedItem->mItems.size(); j++)
            {
                mEditGraphicViewWidget->getGraphicsView()->scene()->removeItem(clickedItem->mItems.at(j));
            }
            mSceneItems.erase(mSceneItems.begin() + index);
        }
        else
        {
            for (int i = 0; i < newSceneItemEdit->mNewlyAddedImagePaths.size(); i++)
            {
                QPixmap comingPix(newSceneItemEdit->mNewlyAddedImagePaths.at(i));
                QGraphicsPixmapItem* newPixmapItem = new QGraphicsPixmapItem(comingPix);
                clickedItem->addNewItemWithProperties(newPixmapItem);
                addItemToScene(newPixmapItem);
            }
        }

        if (newSceneItemEdit->mItemIsDeleted)
        {
            updateAddedElementsPart();
        }
        else
        {
            updateAddedElementsPart(false);
        }

        mSceneItemEditWindowStates[clickedItem] = false;
    }

    // ============ 3. TİP: DİĞER TÜM İTEMLER ============
    else {
        SceneItemEdit* newSceneItemEdit = new SceneItemEdit(item);
        mSceneItemEditWindowStates[clickedItem] = true;
        /*********************************/
        bool nameExists = false;
        do
        {
            if (nameExists)
            {
                QMessageBox::critical(this, "Ayni isimde baska bir element var!", "Ayni isimde baska bir element var");
            }
            newSceneItemEdit->exec();
            nameExists = checkNameExists(mSceneItems.at(index)->mItemName, index);
        } while (nameExists);
        /************************************/
        if (newSceneItemEdit->mItemIsDeleted)
        {
            for (int j = 0; j < clickedItem->mItems.size(); j++)
            {
                mEditGraphicViewWidget->getGraphicsView()->scene()->removeItem(clickedItem->mItems.at(j));
            }
            mSceneItems.erase(mSceneItems.begin() + index);
        }
        else
        {
            for (int i = 0; i < newSceneItemEdit->mNewlyAddedImagePaths.size(); i++)
            {
                QPixmap comingPix(newSceneItemEdit->mNewlyAddedImagePaths.at(i));
                QGraphicsPixmapItem* newPixmapItem = new QGraphicsPixmapItem(comingPix);
                clickedItem->addNewItemWithProperties(newPixmapItem);
                addItemToScene(newPixmapItem);
            }
        }
        if (newSceneItemEdit->mItemIsDeleted)
        {
            updateAddedElementsPart();
        }
        else
        {
            updateAddedElementsPart(false);
        }
        mSceneItemEditWindowStates[clickedItem] = false;
    }
}


void DashboardGeneratorGui::handleMouseDoubleClickedOnItem(CustomSceneItem* item)
{
    handleMouseDoubleClickFromSceneWithItem(item);
}

bool DashboardGeneratorGui::saveDashboard(QString dashboardName)
{
    return this->saveDashboard(this->mProjectDir, dashboardName);
}

bool DashboardGeneratorGui::saveDashboard(QString directory, QString dashboardName)
{
    QProgressDialog progressDialog(tr("Kaydediliyor..."), QString(), 0, 100, this);
    progressDialog.setWindowTitle(tr("Dashboard dosyasini kaydediyor..."));
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.show();
    progressDialog.setValue(0);

    QString dashboardGeneratorFilesDir = directory + "/DashboardGeneratorFiles";
    QString resourcesFilesDir = directory + "/DashboardGeneratorResources";
    QString dashboardResourceFilesDir = resourcesFilesDir + "/" + dashboardName;
    if (!QDir(dashboardGeneratorFilesDir).exists())
    {
        QDir().mkdir(dashboardGeneratorFilesDir);
    }
    if (!QDir(resourcesFilesDir).exists())
    {
        QDir().mkdir(resourcesFilesDir);
    }
    if (!QDir(dashboardResourceFilesDir).exists())
    {
        QDir().mkdir(dashboardResourceFilesDir);
    }
    //creating xml and necessary files for project
    QDomDocument pDoc;
    QDomDocument resourceFile;

    QDomElement dashboardRoot = pDoc.createElement("Dashboard");
    pDoc.appendChild(dashboardRoot);

    QDomElement rccRoot = resourceFile.createElement("RCC");
    resourceFile.appendChild(rccRoot);

    if (pDoc.isNull() || resourceFile.isNull())
    {
        return false;
    }
    //rcc file resource tag
   // Resource file işlemleri [web:211][web:158]
    QDomElement rootElemResourceFiles = resourceFile.documentElement();
    QDomElement qResourceElement = resourceFile.createElement("qresource");
    qResourceElement.setAttribute("prefix", "/GenericSimulatorDashboard");
    rootElemResourceFiles.appendChild(qResourceElement);

    // Dashboard document işlemleri
    QDomElement rootElem = pDoc.documentElement();
    rootElem.setAttribute("name", dashboardName);  // dashboardRoot yerine rootElem
    rootElem.setAttribute("brightnessAttributeName", ui.lineEditDashboardBrightnessAttribute->text());
    rootElem.setAttribute("width", QString::number(mEditGraphicViewWidget->getGraphicsView()->scene()->width()));
    rootElem.setAttribute("height", QString::number(mEditGraphicViewWidget->getGraphicsView()->scene()->height()));

    QDomElement backgroundElement = pDoc.createElement("background");
    QDomElement lightsElement = pDoc.createElement("lights");
    QDomElement ibrelerElement = pDoc.createElement("ibreler");
    QDomElement singleImagesElement = pDoc.createElement("singleImages");
    QDomElement textsElement = pDoc.createElement("texts");
    QDomElement buttonInputsElement = pDoc.createElement("inputs");
    QDomElement barsElement = pDoc.createElement("bars");
    QDomElement slidersElement = pDoc.createElement("sliders");
    QDomElement rectsElement = pDoc.createElement("rectItems");
    QDomElement innerDashboardElement = pDoc.createElement("innerDashboards");
    QDomElement innerWidgetElement = pDoc.createElement("innerWidgets");


    for (int i = 0; i < mSceneItems.size(); i++)
    {
        QDomElement domElementInUsage = QDomElement();
        QDomElement childElement = QDomElement();
        if (mSceneItems.at(i)->mTypeId == GRAPHICS_PIXMAP_BACKGROUND_ITEM)
        {
            childElement = backgroundElement;
            domElementInUsage = backgroundElement;
        }

        else if (mSceneItems.at(i)->mTypeId == GRAPHICS_PIXMAP_ITEM)
        {
            childElement = pDoc.createElement("singleimage");
            domElementInUsage = singleImagesElement;
        }

        else if (mSceneItems.at(i)->mTypeId == GRAPHICS_TEXT_ITEM)
        {
            childElement = pDoc.createElement("text");
            domElementInUsage = textsElement;
        }

        else if (mSceneItems.at(i)->mTypeId == GRAPHICS_PIXMAP_BUTTON_INPUT_ITEM)
        {
            childElement = pDoc.createElement("input");
            domElementInUsage = buttonInputsElement;
            PixmapInputSceneItem* pisi = (PixmapInputSceneItem*)mSceneItems.at(i);


            for (int j = 0; j < pisi->mButtonInput->getStateItemList().size(); j++)
            {
                QDomElement stateElement = pDoc.createElement("state");

                if (pisi->mButtonInput->getStateItemList().at(j)->mType == BUTTON_STATE) {
                    StateOfInputs* currentState = pisi->mButtonInput->getStateItemList().at(j);
                    //QPixmap pixmapOfItem = currentState->getStatePixmapItem()->pixmap();//we use graphics pixmap item in main scene because of resizing
                    QPixmap pixmapOfItem = ((QGraphicsPixmapItem*)pisi->mItems.at(j))->pixmap();
                    QString fileExtension = mSceneItems.at(i)->mItemName + QString::number(j) + ".png";
                    pixmapOfItem.save(dashboardResourceFilesDir + "/" + fileExtension);
                    QString resourceFileElementContent = "DashboardGeneratorResources/" + dashboardName + "/" + fileExtension;
                    stateElement.setAttribute("stateImagePath", resourceFileElementContent);
                    stateElement.setAttribute("name", currentState->getStateName());
                    stateElement.setAttribute("type", QString::number(currentState->mType).toStdString().c_str());
                    QDomElement touchAreasParentElement = pDoc.createElement("touchAreas");
                    QDomElement transitionsParentElement = pDoc.createElement("transitions");
                    std::vector<TouchAreaOfState*> touchAreasOfState = currentState->getTouchAreaOfState();
                    for (int k = 0; k < touchAreasOfState.size(); k++)
                    {
                        QDomElement touchAreaElement = pDoc.createElement("touchArea");
                        TouchAreaOfState* touchAreaOfState = touchAreasOfState.at(k);
                        touchAreaElement.setAttribute("name", touchAreaOfState->getName());
                        QGraphicsPolygonItem* polygonItem = touchAreaOfState->getPolygonGraphicsItem();
                        QPolygonF polygonOfTouchArea = polygonItem->polygon();
                        for (int l = 0; l < polygonOfTouchArea.size(); l++)
                        {
                            float pointXPos = polygonOfTouchArea.at(l).x();
                            float pointYPos = polygonOfTouchArea.at(l).y();
                            QDomElement touchAreaPointElement = pDoc.createElement("point");
                            touchAreaPointElement.setAttribute("xPos",QString::number(pointXPos / currentState->getStatePixmapItem()->boundingRect().width()).toStdString().c_str());
                            touchAreaPointElement.setAttribute("yPos",QString::number(pointYPos / currentState->getStatePixmapItem()->boundingRect().height()).toStdString().c_str());
                            touchAreaElement.appendChild(touchAreaPointElement);
                        }
                        touchAreasParentElement.appendChild(touchAreaElement);
                    }

                    std::vector<TransitionOfState> transitionsOfState = currentState->getTransitionOfState();
                    for (int m = 0; m < transitionsOfState.size(); m++)
                    {
                        QDomElement transitionElement = pDoc.createElement("transition");
                        TransitionOfState transitionOfState = transitionsOfState.at(m);
                        transitionElement.setAttribute("name", transitionOfState.getTransitionName());
                        std::vector<Condition> transitionConditionList = transitionOfState.getConditionList();
                        for (int n = 0; n < transitionConditionList.size(); n++)
                        {
                            Condition transitionCondition = transitionConditionList.at(n);
                            QDomElement transitionConditionElement = pDoc.createElement("transitionCondition");
                            transitionConditionElement.setAttribute("attrType", transitionCondition.ConditionAttrType);
                            transitionConditionElement.setAttribute("attrName", transitionCondition.ConditionAttrName);
                            transitionConditionElement.setAttribute("isCommandId",QString::number(transitionCondition.ConditionIsCommanId).toStdString().c_str());
                            transitionConditionElement.setAttribute("comparisonType", QString::number(transitionCondition.ConditionComparisonType).toStdString().c_str());
                            transitionConditionElement.setAttribute("comparisonValue", transitionCondition.ConditionComparisonValue);
                            transitionElement.appendChild(transitionConditionElement);
                        }
                        transitionElement.setAttribute("logicType",(QString::number(transitionOfState.getConditionLogicalType()).toStdString().c_str()));
                        transitionElement.setAttribute("transitionCheckBehavior",QString::number(transitionOfState.getTransitionConditionCheckBehavior()).toStdString().c_str());
                        transitionElement.setAttribute("selectedTouchAreaIndex",QString::number(transitionOfState.getTargetTouchArea()).toStdString().c_str());
                        transitionElement.setAttribute("transitionStateIndex", QString::number(transitionOfState.getTargetStateIndex()).toStdString().c_str());
                        transitionElement.setAttribute("transitionSoundPath", transitionOfState.getTransitionSoundPath());

                        QDomElement inputToSetElement = pDoc.createElement("inputToSet");
                        inputToSetElement.setAttribute("attrType", transitionOfState.getInputToSet().InputAttrType);
                       inputToSetElement.setAttribute("attrName", transitionOfState.getInputToSet().InputAttrName);
                       inputToSetElement.setAttribute("isCommandId", QString::number(transitionOfState.getInputToSet().InputIsCommandId));
                       inputToSetElement.setAttribute("value", transitionOfState.getInputToSet().InputValue);





                        transitionElement.appendChild(inputToSetElement);
                        transitionsParentElement.appendChild(transitionElement);
                    }
                    stateElement.appendChild(touchAreasParentElement);
                    stateElement.appendChild(transitionsParentElement);
                    childElement.appendChild(stateElement);
                }

                // IF STATE IS ROTATABLE 
                else if (pisi->mButtonInput->getStateItemList().at(j)->mType == ROTATABLE_STATE) {
                    RotatableStateOfInputs* currentRotatableState = (RotatableStateOfInputs*)(pisi->mButtonInput->getStateItemList().at(j));

                    //QPixmap pixmapOfItem = currentState->getStatePixmapItem()->pixmap();//we use graphics pixmap item in main scene because of resizing
                    QPixmap pixmapOfItem = ((QGraphicsPixmapItem*)pisi->mItems.at(j))->pixmap();
                    QString fileExtension = mSceneItems.at(i)->mItemName + QString::number(j) + ".png";
                    pixmapOfItem.save(dashboardResourceFilesDir + "/" + fileExtension);
                    QString resourceFileElementContent = "DashboardGeneratorResources/" + dashboardName + "/" + fileExtension;
                    stateElement.setAttribute("stateImagePath", resourceFileElementContent);
                    stateElement.setAttribute("name", currentRotatableState->getStateName());
                    stateElement.setAttribute("type", (QString::number(currentRotatableState->mType).toStdString().c_str()));
                    QDomElement touchAreasParentElement = pDoc.createElement("touchAreas");
                    QDomElement transitionsParentElement = pDoc.createElement("transitions");

                    QDomElement rotatableStateValuesParentElement = pDoc.createElement("inputValues");
                    rotatableStateValuesParentElement.setAttribute("attributeName", currentRotatableState->mAttributeName);
                    rotatableStateValuesParentElement.setAttribute("attributeType", currentRotatableState->mAttributeType);
                    rotatableStateValuesParentElement.setAttribute("isCommandId", QString::number(currentRotatableState->mIsCommandId));

                    std::vector<TouchAreaOfState*> touchAreasOfState = currentRotatableState->getTouchAreaOfState();
                    for (int k = 0; k < touchAreasOfState.size(); k++)
                    {
                        QDomElement touchAreaElement = pDoc.createElement("touchArea");
                        TouchAreaOfState* touchAreaOfState = touchAreasOfState.at(k);
                        touchAreaElement.setAttribute("name", touchAreaOfState->getName());
                        QGraphicsPolygonItem* polygonItem = touchAreaOfState->getPolygonGraphicsItem();
                        QPolygonF polygonOfTouchArea = polygonItem->polygon();
                        for (int l = 0; l < polygonOfTouchArea.size(); l++)
                        {
                            float pointXPos = polygonOfTouchArea.at(l).x();
                            float pointYPos = polygonOfTouchArea.at(l).y();
                            QDomElement touchAreaPointElement = pDoc.createElement("point");
                            touchAreaPointElement.setAttribute("xPos",QString::number(pointXPos / currentRotatableState->getStatePixmapItem()->boundingRect().width()).toStdString().c_str());
                            touchAreaPointElement.setAttribute("yPos", QString::number(pointYPos / currentRotatableState->getStatePixmapItem()->boundingRect().height()).toStdString().c_str());
                            touchAreaElement.appendChild(touchAreaPointElement);
                        }
                        touchAreasParentElement.appendChild(touchAreaElement);
                    }

                    std::vector<TransitionOfState> transitionsOfState = currentRotatableState->getTransitionOfState();
                    for (int m = 0; m < transitionsOfState.size(); m++)
                    {
                        QDomElement transitionElement = pDoc.createElement("transition");
                        TransitionOfState transitionOfState = transitionsOfState.at(m);
                        transitionElement.setAttribute("name", transitionOfState.getTransitionName());
                        std::vector<Condition> transitionConditionList = transitionOfState.getConditionList();
                        for (int n = 0; n < transitionConditionList.size(); n++)
                        {
                            Condition transitionCondition = transitionConditionList.at(n);
                            QDomElement transitionConditionElement = pDoc.createElement("transitionCondition");
                            transitionConditionElement.setAttribute("attrType", transitionCondition.ConditionAttrType);
                            transitionConditionElement.setAttribute("attrName", transitionCondition.ConditionAttrName);
                            transitionConditionElement.setAttribute("isCommandId", QString::number(transitionCondition.ConditionIsCommanId).toStdString().c_str());
                            transitionConditionElement.setAttribute("comparisonType", QString::number(transitionCondition.ConditionComparisonType).toStdString().c_str());
                            transitionConditionElement.setAttribute("comparisonValue", transitionCondition.ConditionComparisonValue);
                            transitionElement.appendChild(transitionConditionElement);
                        }
                        transitionElement.setAttribute("logicType", QString::number(transitionOfState.getConditionLogicalType()).toStdString().c_str());
                        transitionElement.setAttribute("transitionCheckBehavior", QString::number(transitionOfState.getTransitionConditionCheckBehavior()).toStdString().c_str());
                        transitionElement.setAttribute("selectedTouchAreaIndex", QString::number(transitionOfState.getTargetTouchArea()).toStdString().c_str());
                        transitionElement.setAttribute("transitionStateIndex",QString::number(transitionOfState.getTargetStateIndex()).toStdString().c_str());

                        QDomElement inputToSetElement = pDoc.createElement("inputToSet");
                        // Muhtemelen farklı fonksiyonlar var:
                        inputToSetElement.setAttribute("attrType", transitionOfState.getInputToSet().InputAttrType);
                        inputToSetElement.setAttribute("attrName", transitionOfState.getInputToSet().InputAttrName);

                        inputToSetElement.setAttribute("isCommandId", QString::number(transitionOfState.getInputToSet().InputIsCommandId).toStdString().c_str());
                        inputToSetElement.setAttribute("value", transitionOfState.getInputToSet().InputValue.toStdString().c_str());

                        transitionElement.appendChild(inputToSetElement);
                        transitionsParentElement.appendChild(transitionElement);
                    }

                    if (currentRotatableState->mRotatableStateBehaviorType == DISCRETE) {
                        QDomElement rotationValues = pDoc.createElement("rotation");
                        rotationValues.setAttribute("maxAngleValue", QString::number(currentRotatableState->mMaxRotatableStateAngle).toStdString().c_str());
                        rotationValues.setAttribute("minAngleValue", QString::number(currentRotatableState->mMinRotatableStateAngle).toStdString().c_str());
                        rotationValues.setAttribute("maxValue", QString::number(currentRotatableState->mMaxRotatableStateValue).toStdString().c_str());
                        rotationValues.setAttribute("minValue", QString::number(currentRotatableState->mMinRotatableStateValue).toStdString().c_str());
                        for (int p = 0; p < currentRotatableState->mRotatableStateDiscreteAngleList.size(); p++)
                        {
                            QDomElement rotationDiscreteValues = pDoc.createElement("rotationDiscreteValue");
                            for (int j = 0; j < currentRotatableState->mRotatableStateDiscreteValueList.size() - 1; j++)
                            {
                                for (int k = 0; k < currentRotatableState->mRotatableStateDiscreteValueList.size() - j - 1; k++) {
                                    if (currentRotatableState->mRotatableStateDiscreteValueList.at(k) > currentRotatableState->mRotatableStateDiscreteValueList.at(k + 1)) {
                                        float tempValue = currentRotatableState->mRotatableStateDiscreteValueList.at(k);
                                        currentRotatableState->mRotatableStateDiscreteValueList.at(k) = currentRotatableState->mRotatableStateDiscreteValueList.at(k + 1);
                                        currentRotatableState->mRotatableStateDiscreteValueList.at(k + 1) = tempValue;

                                        float tempAngle = currentRotatableState->mRotatableStateDiscreteAngleList.at(k);
                                        currentRotatableState->mRotatableStateDiscreteAngleList.at(k) = currentRotatableState->mRotatableStateDiscreteAngleList.at(k + 1);
                                        currentRotatableState->mRotatableStateDiscreteAngleList.at(k + 1) = tempAngle;
                                    }
                                }
                            }
                            rotationDiscreteValues.setAttribute("discreteAngleValue",QString::number(currentRotatableState->mRotatableStateDiscreteAngleList.at(p)).toStdString().c_str());
                            rotationDiscreteValues.setAttribute("discreteValue", QString::number(currentRotatableState->mRotatableStateDiscreteValueList.at(p)).toStdString().c_str());
                            rotationValues.appendChild(rotationDiscreteValues);
                        }
                        rotatableStateValuesParentElement.appendChild(rotationValues);
                    }
                    else if (currentRotatableState->mRotatableStateBehaviorType == CONTINUOUS) {
                        QDomElement rotationValues = pDoc.createElement("rotation");
                        rotationValues.setAttribute("maxAngleValue", QString::number(currentRotatableState->mMaxRotatableStateAngle).toStdString().c_str());
                        rotationValues.setAttribute("minAngleValue", QString::number(currentRotatableState->mMinRotatableStateAngle).toStdString().c_str());
                        rotationValues.setAttribute("maxValue",QString::number(currentRotatableState->mMaxRotatableStateValue).toStdString().c_str());
                        rotationValues.setAttribute("minValue", QString::number(currentRotatableState->mMinRotatableStateValue).toStdString().c_str());
                        rotatableStateValuesParentElement.appendChild(rotationValues);
                    }
                    stateElement.appendChild(rotatableStateValuesParentElement);
                    stateElement.appendChild(touchAreasParentElement);
                    stateElement.appendChild(transitionsParentElement);
                    childElement.appendChild(stateElement);
                }
            }
        }

        else if (mSceneItems.at(i)->mTypeId == GRAPHICS_PIXMAP_IBRE_ITEM)
        {
            childElement = pDoc.createElement("ibre");
            IbreCustomSceneItem* ibreItem = (IbreCustomSceneItem*)(mSceneItems.at(i));

            QDomElement ibreValuesParentElement = pDoc.createElement("ibreValues");
            // Bubble Sort
            if (ibreItem->mDiscreteIbreValueList.size() > 0)
            {
                for (int j = 0; j < ibreItem->mDiscreteIbreValueList.size() - 1; j++)
                {
                    for (int k = 0; k < ibreItem->mDiscreteIbreValueList.size() - j - 1; k++) {
                        if (ibreItem->mDiscreteIbreValueList.at(k) > ibreItem->mDiscreteIbreValueList.at(k + 1)) {
                            float tempValue = ibreItem->mDiscreteIbreValueList.at(k);
                            ibreItem->mDiscreteIbreValueList.at(k) = ibreItem->mDiscreteIbreValueList.at(k + 1);
                            ibreItem->mDiscreteIbreValueList.at(k + 1) = tempValue;

                            float tempAngle = ibreItem->mDiscreteIbreAngleList.at(k);
                            ibreItem->mDiscreteIbreAngleList.at(k) = ibreItem->mDiscreteIbreAngleList.at(k + 1);
                            ibreItem->mDiscreteIbreAngleList.at(k + 1) = tempAngle;
                        }
                    }
                }
            }

            for (int l = 0; l < ibreItem->mDiscreteIbreValueList.size(); l++)
            {
                QDomElement ibreDiscreteValueElement = pDoc.createElement("ibreDiscreteValues");
                ibreDiscreteValueElement.setAttribute("discreteValue", QString::number(ibreItem->mDiscreteIbreValueList.at(l)).toStdString().c_str());
                ibreDiscreteValueElement.setAttribute("discreteAngleValue",QString::number(ibreItem->mDiscreteIbreAngleList.at(l)).toStdString().c_str());
                ibreValuesParentElement.appendChild(ibreDiscreteValueElement);
            }

            childElement.appendChild(ibreValuesParentElement);
            domElementInUsage = ibrelerElement;
        }

        else if (mSceneItems.at(i)->mTypeId == GRAPHICS_BAR_ITEM)
        {
            childElement = pDoc.createElement("bar");
            BarCustomSceneItem* barItem = (BarCustomSceneItem*)(mSceneItems.at(i));
            childElement.setAttribute("barType", QString::number(barItem->mBarType).toStdString().c_str());
            childElement.setAttribute("barColor", barItem->mBarColor.name(QColor::HexArgb).toStdString().c_str());
            childElement.setAttribute("barWidth", QString::number(barItem->mLineThickness).toStdString().c_str());
            childElement.setAttribute("barHeight", QString::number(barItem->mLineLength).toStdString().c_str());
            childElement.setAttribute("maxVal", QString::number(barItem->mMaxVal).toStdString().c_str());
            childElement.setAttribute("minVal", QString::number(barItem->mMinVal).toStdString().c_str());
            childElement.setAttribute("barWidgetCss", barItem->mBarWidgetCss);

            // Arc bar özelliklerini kaydet
            childElement.setAttribute("arcRadius", QString::number(barItem->mArcRadius).toStdString().c_str());
            childElement.setAttribute("arcStartAngle", QString::number(barItem->mArcStartAngle).toStdString().c_str());
            childElement.setAttribute("arcSpanAngle", QString::number(barItem->mArcSpanAngle).toStdString().c_str());
            childElement.setAttribute("barRadius", QString::number(barItem->mBarRadius).toStdString().c_str());
            childElement.setAttribute("arcBend", QString::number(barItem->mArcBend).toStdString().c_str());
            childElement.setAttribute("arcBendDirection", QString::number(barItem->mArcBendDirection).toStdString().c_str());
            childElement.setAttribute("arcRotation", QString::number(barItem->mArcRotation).toStdString().c_str());

            domElementInUsage = barsElement;
        }

        else if (mSceneItems.at(i)->mTypeId == GRAPHICS_SLIDER_ITEM)
        {
            childElement = pDoc.createElement("slider");
            SliderCustomSceneItem* sliderItem = (SliderCustomSceneItem*)(mSceneItems.at(i));
            childElement.setAttribute("sliderType", QString::number(sliderItem->mSliderType).toStdString().c_str());
            childElement.setAttribute("sliderWidth", QString::number(sliderItem->mLineThickness).toStdString().c_str());
            childElement.setAttribute("sliderHeight", (QString::number(sliderItem->mLineLength).toStdString().c_str()));
            childElement.setAttribute("maxVal", QString::number(sliderItem->mMaxVal).toStdString().c_str());
            childElement.setAttribute("minVal", QString::number(sliderItem->mMinVal).toStdString().c_str());
            childElement.setAttribute("sliderWidgetCss", sliderItem->mSliderWidgetCss);
            domElementInUsage = slidersElement;
        }

        else if (mSceneItems.at(i)->mTypeId == GRAPHICS_RECT_ITEM)
        {
            childElement = pDoc.createElement("rectItem");
            CustomRectSceneItem* rectItem = (CustomRectSceneItem*)(mSceneItems.at(i));
            CustomQGraphicsRectItem* rectQItem = rectItem->mCustomQGraphicsRectItem;
            childElement.setAttribute("rectWidth", QString::number(rectQItem->mWidth).toStdString().c_str());
            childElement.setAttribute("rectHeigth", QString::number(rectQItem->mHeight).toStdString().c_str());
            childElement.setAttribute("isFilled", QString::number(rectQItem->mIsFilled).toStdString().c_str());
            childElement.setAttribute("fillColor", rectQItem->mFillColor.name(QColor::HexArgb).toStdString().c_str());
            childElement.setAttribute("generalEdgeThickness", QString::number(rectQItem->mGeneralThickness).toStdString().c_str());
            childElement.setAttribute("generalEdgeColor", rectQItem->mGeneralEdgeColor.name(QColor::HexArgb).toStdString().c_str());
            QString customEdgeProperties;
            for (int i = 0; i < 4; i++)
            {
                customEdgeProperties.append(rectQItem->mEdgeProperties[i].isCustom ? "true" : "false");
                customEdgeProperties.append(":");
                customEdgeProperties.append(QString::number(rectQItem->mEdgeProperties[i].thickness));
                customEdgeProperties.append(":");
                customEdgeProperties.append(rectQItem->mEdgeProperties[i].color.name(QColor::HexArgb));
                customEdgeProperties.append(";");
            }
            childElement.setAttribute("customEdgeProperties", customEdgeProperties.toStdString().c_str());
            domElementInUsage = rectsElement;
        }

        else if (mSceneItems.at(i)->mTypeId == INNER_DASHBOARD_ITEM)
        {
            childElement = pDoc.createElement("innerDashboard");
            InnerDashboardItem* innerDashboardItem = (InnerDashboardItem*)(mSceneItems.at(i));
            childElement.setAttribute("newSizeAsPercentage", QString::number(innerDashboardItem->mInnerDashboardNewSizeAsPercentage).toStdString().c_str());
            CustomInnerDashboardRectItem* rectItem = innerDashboardItem->mCustomInnerDashboardRectItem;
            domElementInUsage = innerDashboardElement;
        }

        else if (mSceneItems.at(i)->mTypeId == INNER_WIDGET_ITEM)
        {
            childElement = pDoc.createElement("innerWidget");
            InnerWidgetItem* innerWidgetItem = (InnerWidgetItem*)(mSceneItems.at(i));
            CustomInnerWidgetRectItem* innerWidgetRectItem = innerWidgetItem->mCustomInnerWidgetRectItem;
            childElement.setAttribute("width", QString::number(innerWidgetRectItem->mWidth).toStdString().c_str());
            childElement.setAttribute("height", QString::number(innerWidgetRectItem->mHeight).toStdString().c_str());
            childElement.setAttribute("igWindowId", QString::number(innerWidgetRectItem->mIGWindowID).toStdString().c_str());
            domElementInUsage = innerWidgetElement;
        }

        else if (mSceneItems.at(i)->mTypeId == GRAPHICS_PIXMAP_LIGHT_ITEM)
        {
            childElement = pDoc.createElement("light");
            domElementInUsage = lightsElement;
        }

        //if not graphics item take pixmaps,save them and save info to resources file
        if (mSceneItems.at(i)->mTypeId != GRAPHICS_TEXT_ITEM && mSceneItems.at(i)->mTypeId != GRAPHICS_BAR_ITEM
            && mSceneItems.at(i)->mTypeId != GRAPHICS_RECT_ITEM && mSceneItems.at(i)->mTypeId != GRAPHICS_SLIDER_ITEM &&
            mSceneItems.at(i)->mTypeId != INNER_DASHBOARD_ITEM && mSceneItems.at(i)->mTypeId != GRAPHICS_PIXMAP_BUTTON_INPUT_ITEM && mSceneItems.at(i)->mTypeId != INNER_WIDGET_ITEM)
        {
            for (int j = 0; j < mSceneItems.at(i)->mItems.size(); j++)
            {
                QGraphicsPixmapItem* tempPixMapItem = (QGraphicsPixmapItem*)(mSceneItems.at(i)->mItems.at(j));
                QPixmap pixmapOfItem = tempPixMapItem->pixmap();
                QString fileExtension = mSceneItems.at(i)->mItemName + QString::number(j) + ".png";
                pixmapOfItem.save(dashboardResourceFilesDir + "/" + fileExtension);
                QDomElement fileElement = resourceFile.createElement("file");
                QString resourceFileElementContent = "DashboardGeneratorResources/" + dashboardName + "/" + fileExtension;
                QDomText textNode = resourceFile.createTextNode(resourceFileElementContent);
                fileElement.appendChild(textNode);

                qResourceElement.appendChild(fileElement);
                QDomElement pixmapItemElement = pDoc.createElement("pixmapItem");
                pixmapItemElement.setAttribute("path", resourceFileElementContent);
                childElement.appendChild(pixmapItemElement);
            }
        }
        else if (mSceneItems.at(i)->mTypeId == GRAPHICS_TEXT_ITEM)
        {
            for (int j = 0; j < mSceneItems.at(i)->mItems.size(); j++)
            {
                QGraphicsTextItem* textItem = (QGraphicsTextItem*)(mSceneItems.at(i)->mItems.at(j));
                QDomElement childTextElement = pDoc.createElement("textItem");
                //document is used because of encoding problem,using textitem's toHtmls causes encoding problem, also toLocal8Bit is used for encoding problem
                QString htmlContent = textItem->document()->toPlainText();
                std::string duzYazi2 = htmlContent.toStdString();
                QString color = textItem->defaultTextColor().name();
                childTextElement.setAttribute("text", htmlContent);
                childTextElement.setAttribute("color", color.toStdString().c_str());
                QString fontInfoStr = textItem->font().toString();
                if (fontInfoStr.contains(",Kal"))//QTBUG-80989
                {
                    fontInfoStr = fontInfoStr.left(fontInfoStr.lastIndexOf("Kal"));
                    fontInfoStr = fontInfoStr + "Bold";
                }
                childTextElement.setAttribute("font", fontInfoStr.toStdString().c_str());
                childElement.appendChild(childTextElement);
            }
        }

        if (childElement != domElementInUsage)
        {
            domElementInUsage.appendChild(childElement);
        }
        //because we changed transform origin
        float yAdditionForIbreItem = 0;
        float xAdditionForIbreItem = 0;
        if (mSceneItems.at(i)->mTypeId == GRAPHICS_PIXMAP_IBRE_ITEM)
        {
            mSceneItems.at(i)->mItems.at(mSceneItems.at(i)->mCurrentActiveItem)->setRotation(0);
            //xAdditionForIbreItem = -mSceneItems.at ( i )->mItems.at ( mSceneItems.at ( i )->mCurrentActiveItem )->boundingRect ( ).width ( ) / 4;
            //yAdditionForIbreItem = -mSceneItems.at ( i )->mItems.at ( mSceneItems.at ( i )->mCurrentActiveItem )->boundingRect ( ).height ( ) / 4;
        }

        childElement.setAttribute("posX", QString::number((mSceneItems.at(i)->mItems.at(mSceneItems.at(i)->mCurrentActiveItem)->scenePos().x() + xAdditionForIbreItem) / mEditGraphicViewWidget->getGraphicsView()->scene()->width()).toStdString().c_str());
        childElement.setAttribute("posY", QString::number((mSceneItems.at(i)->mItems.at(mSceneItems.at(i)->mCurrentActiveItem)->scenePos().y() + yAdditionForIbreItem) / mEditGraphicViewWidget->getGraphicsView()->scene()->height()).toStdString().c_str());

        childElement.setAttribute("name", mSceneItems.at(i)->mItemName);
        childElement.setAttribute("type", QString::number(mSceneItems.at(i)->mTypeId).toStdString().c_str());
        childElement.setAttribute("showOnStart", QString::number(mSceneItems.at(i)->mShowOnStart).toStdString().c_str());
        childElement.setAttribute("zValue", QString::number(mSceneItems.at(i)->mItems.at(0)->zValue()).toStdString().c_str());
        childElement.setAttribute("attributeName", mSceneItems.at(i)->mAttirubuteName);
        childElement.setAttribute("attributeType", mSceneItems.at(i)->mAttributeType.toStdString().c_str());
        childElement.setAttribute("rotation", QString::number(mSceneItems.at(i)->mRotationValue).toStdString().c_str());
        //childElement.setAttribute("sizeRatioWidth", X(QString::number(mSceneItems.at(i).mSizeRatio.x()).toStdString().c_str()));

        childElement.setAttribute("visibleOnEditor", QString::number(mSceneItems.at(i)->mIsItemShown).toStdString().c_str());
        QString groupName;
        if (mSceneItems.at(i)->mGroupIndex >= 0)
        {
            groupName = mGroupNames.at(mSceneItems.at(i)->mGroupIndex);
        }
        childElement.setAttribute("groupName", groupName);

        //setting configurations for this item (may be precision etc.)
        if (mSceneItems.at(i)->mConfigurationsMap.size() > 0)
        {
            std::string confStr = "";
            for (std::map<std::string, std::string>::iterator iter = mSceneItems.at(i)->mConfigurationsMap.begin();
                iter != mSceneItems.at(i)->mConfigurationsMap.end();
                iter++)
            {
                if (iter->first.size() > 0 && iter->second.size() > 0)
                {
                    confStr += iter->first + ":" + iter->second + ";";
                }
            }
            if (confStr.size() > 0)
            {
                childElement.setAttribute("configuration", confStr.c_str());
            }
        }
        progressDialog.setValue(progressDialog.value() + 100.0f / mSceneItems.size());
    }

    if (backgroundElement.hasChildNodes())
    {
        rootElem.appendChild(backgroundElement);
    }

    if (lightsElement.hasChildNodes())
    {
        rootElem.appendChild(lightsElement);
    }

    if (ibrelerElement.hasChildNodes())
    {
        rootElem.appendChild(ibrelerElement);
    }

    if (singleImagesElement.hasChildNodes())
    {
        rootElem.appendChild(singleImagesElement);
    }

    if (textsElement.hasChildNodes())
    {
        rootElem.appendChild(textsElement);
    }

    if (buttonInputsElement.hasChildNodes())
    {
        rootElem.appendChild(buttonInputsElement);
    }

    if (barsElement.hasChildNodes())
    {
        rootElem.appendChild(barsElement);
    }

    if (slidersElement.hasChildNodes())
    {
        rootElem.appendChild(slidersElement);
    }

    if (rectsElement.hasChildNodes())
    {
        rootElem.appendChild(rectsElement);
    }

    if (innerDashboardElement.hasChildNodes())
    {
        rootElem.appendChild(innerDashboardElement);
    }

    if (innerWidgetElement.hasChildNodes())
    {
        rootElem.appendChild(innerWidgetElement);
    }

    QString fullFilePath = dashboardGeneratorFilesDir + "/" + dashboardName + ".xml";
    QString fullFileForResourceFile = directory + "/" + dashboardName + ".qrc";

    try
    {
        QFile dashboardFile(fullFilePath);
        if (dashboardFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&dashboardFile);
            pDoc.save(stream, 4);
            dashboardFile.close();
        }
    }
    catch (...)
    {
        QMessageBox msgBox;
        msgBox.setText(QStringLiteral("Kaydetme islemi yapilamadi, sectiginiz klasorun isminde Turkce karakterler bulunabilir."));
        msgBox.setInformativeText(QStringLiteral("Kaydetme yapilamadi."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowIcon(this->windowIcon());
        msgBox.exec();

        return false;
    }

    // Update current file path after successful save
    this->ui.pushButtonSave->setToolTip(fullFilePath);
    this->ui.pushButtonSave->setEnabled(true);

    progressDialog.setValue(100);
    QMessageBox msgBox;
    msgBox.setText(QStringLiteral("Kaydetme islemi yapildi."));
    msgBox.setInformativeText(QStringLiteral("Kaydetme tamamlandi."));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowIcon(this->windowIcon());
    msgBox.exec();

    return true;
}

bool DashboardGeneratorGui::showForgetDashboardDialog()
{
    QMessageBox msgBox;
    msgBox.setText(QStringLiteral("Yaptiginiz degisiklikler kaybolacak emin misiniz?"));
    msgBox.setInformativeText(QStringLiteral("Yaptiginiz degisiklikleri kaydetmek icin Kaydet'e basin."));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowIcon(this->windowIcon());
    if (msgBox.exec() != QMessageBox::Ok)
    {
        return false;
    }

    this->resetDashboard();
    this->createResizeCursors();

    return true;
}

QString DashboardGeneratorGui::createUniqueItemName(QString itemName, const QString& defaultNamePrefix)
{
    if (!itemName.isEmpty())
    {
        return itemName;
    }

    this->mCurrentNameIndex++;
    itemName = defaultNamePrefix + QString::number(this->mCurrentNameIndex);

    return itemName;
}

void DashboardGeneratorGui::setLayoutWidgetsEnabled(const QLayout* layout, bool value)
{
    if (layout == nullptr)
    {
        return;
    }

    for (auto i = 0; i < layout->count(); ++i)
    {
        auto item = layout->itemAt(i);
        if (item == nullptr)
        {
            continue;
        }

        if (auto widget = item->widget())
        {
            widget->setEnabled(value);
        }
        else if (auto subLayout = item->layout())
        {
            this->setLayoutWidgetsEnabled(subLayout, value);
        }
    }
}

void DashboardGeneratorGui::setProjectDirectory(QString value)
{
    this->mProjectDir = value;

    auto hasValue = !value.isEmpty();

    this->ui.pushButtonNewDashboard->setEnabled(hasValue);
    this->ui.pushButtonSave->setEnabled(hasValue);
    this->setLayoutWidgetsEnabled(this->ui.verticalLayout_4, hasValue);
    this->setLayoutWidgetsEnabled(this->ui.verticalLayout_5, hasValue);

    if (!hasValue)
    {
        this->ui.pushButtonSave->setToolTip("");

        return;
    }

    QSettings settings;
    auto lastLoadDashboardPath = settings.value("GenericDashboardGenerator/LastLoadDashboardPath", "").toString();
    this->ui.pushButtonSave->setToolTip(!lastLoadDashboardPath.isEmpty() ? lastLoadDashboardPath : "");
}

void DashboardGeneratorGui::handleWheelEventFromScene(QPointF param1, int delta)
{
    QGraphicsItem* itemClicked = nullptr;
    if (mEditGraphicViewWidget->getGraphicsView()->scene()->selectedItems().size() > 0)
    {
        itemClicked = mEditGraphicViewWidget->getGraphicsView()->scene()->selectedItems().at(0);
    }
   
   if (itemClicked != nullptr)

    {
        int index;
        CustomSceneItem* clickedItem = findClickedItemWithGraphicsItem(itemClicked, index);
        if (clickedItem != nullptr)
        {
            if (clickedItem->mTypeId != GRAPHICS_PIXMAP_IBRE_ITEM)
            {
                int currentItem = clickedItem->mCurrentActiveItem;
                if (delta > 0)
                {
                    currentItem++;
                    if (currentItem >= clickedItem->mItems.size())
                    {
                        currentItem = 0;
                    }
                }
                else
                {
                    currentItem--;
                    if (currentItem < 0)
                    {
                        currentItem = clickedItem->mItems.size() - 1;
                    }
                }
                clickedItem->updateItemsAccordingToActiveItem();
                clickedItem->mCurrentActiveItem = currentItem;
                clickedItem->updateItemsAccordingToActiveItem();

            }
            else
            {
                clickedItem->mItems.at(0)->setRotation(clickedItem->mItems.at(0)->rotation() + delta * 0.01);
            }
        }
    }
}

void DashboardGeneratorGui::resetDashboard()
{
    this->mSceneItems.clear();
    this->mCurrentNameIndex = 0;
    this->mEditGraphicViewWidget->getGraphicsView()->scene()->clear();
    this->ui.lineEditDashboardName->setText("");
    this->ui.lineEditDashboardBrightnessAttribute->setText("");
    this->setProjectDirectory(""); // Clear the current dashboard file path
    this->updateAddedElementsPart();
}

CustomSceneItem* DashboardGeneratorGui::createSceneItemWithCommonProperties(QDomNode node)
{
    QDomElement element = node.toElement();
    QString name = element.attribute("name");
    int type = element.attribute("type").toInt();
    bool showOnstart = element.attribute("showOnStart").toInt();
    bool visibleOnEditor = element.attribute("visibleOnEditor", "1").toInt();//if no attr,then it is visible
    QString attrName = element.attribute("attributeName");
    QString attrType = element.attribute("attributeType");
    QString groupName = element.attribute("groupName", "");
    int rotation = element.attribute("rotation").toInt();
    //check for other groups
    int groupIndex = -1;
    if (groupName.size() > 0)
    {
        for (int i = 0; i < mGroupNames.size(); i++)
        {
            if (groupName.compare(mGroupNames.at(i)) == 0)
            {
                groupIndex = i;
                break;
            }
        }
        if (groupIndex < 0)
        {
            mGroupNames.append(groupName);
            groupIndex = mGroupNames.size() - 1;
        }
    }

    std::vector <QGraphicsItem*> graphicsItems;
    std::vector<QPixmap> originalPixmapItem;
    if (type != GRAPHICS_TEXT_ITEM && type != GRAPHICS_BAR_ITEM && type != GRAPHICS_RECT_ITEM && type != GRAPHICS_SLIDER_ITEM
        && type != INNER_DASHBOARD_ITEM && type != GRAPHICS_PIXMAP_BUTTON_INPUT_ITEM && type != INNER_WIDGET_ITEM)
    {
        QDomNodeList pixmapList = element.elementsByTagName("pixmapItem");
        for (int i = 0; i < pixmapList.size(); i++)
        {
            QString path = this->mProjectDir + "/" + pixmapList.at(i).toElement().attribute("path");
            QPixmap tempPix(path);
            if (tempPix.isNull())
            {
                tempPix = QPixmap(this->mProjectDir + "/DashboardGenerator" + pixmapList.at(i).toElement().attribute("path"));//because we renamed the resources folder to DashboardGeneratorResources for backward compatibility

            }
            originalPixmapItem.push_back(tempPix);
            QGraphicsPixmapItem* newPixmap = new QGraphicsPixmapItem(tempPix);
            newPixmap->setZValue(element.attribute("zValue").toInt());
            graphicsItems.push_back(newPixmap);
            addItemToScene(newPixmap);
            float posx = element.attribute("posX").toFloat() * mEditGraphicViewWidget->getGraphicsView()->width();
            float posy = element.attribute("posY").toFloat() * mEditGraphicViewWidget->getGraphicsView()->height();
            newPixmap->setPos(posx, posy);
            newPixmap->setRotation(rotation);
        }
    }
    else if (type == GRAPHICS_TEXT_ITEM)
    {
        QDomNodeList textList = element.elementsByTagName("textItem");

        for (int i = 0; i < textList.size(); i++)
        {
            QDomElement childElement = textList.at(i).toElement();
            QString htmlElement = childElement.attribute("text");
            QString color = childElement.attribute("color");
            QString fontStr = childElement.attribute("font");
            QFont font;
            font.fromString(fontStr);
            QGraphicsTextItem* item = new QGraphicsTextItem();
            QTextDocument* textDoc = new QTextDocument();
            textDoc->setPlainText(htmlElement);
            item->setDocument(textDoc);
            item->setDefaultTextColor(QColor(color));
            item->setFont(font);
            graphicsItems.push_back(item);
            item->setZValue(element.attribute("zValue").toInt());
            addItemToScene(item);
            float posx = element.attribute("posX").toFloat() * mEditGraphicViewWidget->getGraphicsView()->width();
            float posy = element.attribute("posY").toFloat() * mEditGraphicViewWidget->getGraphicsView()->height();
            item->setPos(posx, posy);
            item->setRotation(rotation);
        }
    }
    else if (type == GRAPHICS_BAR_ITEM)
    {
        CustomBarProxyWidgetItem* barItem = new CustomBarProxyWidgetItem;
        barItem->setZValue(element.attribute("zValue").toInt());
        graphicsItems.push_back(barItem);
        float posx = element.attribute("posX").toFloat() * mEditGraphicViewWidget->getGraphicsView()->width();
        float posy = element.attribute("posY").toFloat() * mEditGraphicViewWidget->getGraphicsView()->height();
        addItemToScene(barItem);
        barItem->setPos(posx, posy);
        barItem->setRotation(rotation);
    }
    else if (type == GRAPHICS_SLIDER_ITEM)
    {
        CustomSliderProxyWidgetItem* sliderItem = new CustomSliderProxyWidgetItem;
        sliderItem->setZValue(element.attribute("zValue").toInt());
        graphicsItems.push_back(sliderItem);
        float posx = element.attribute("posX").toFloat() * mEditGraphicViewWidget->getGraphicsView()->width();
        float posy = element.attribute("posY").toFloat() * mEditGraphicViewWidget->getGraphicsView()->height();
        addItemToScene(sliderItem);
        sliderItem->setPos(posx, posy);
        sliderItem->setRotation(rotation);
    }
    else if (type == GRAPHICS_RECT_ITEM)
    {
        CustomQGraphicsRectItem* rectItem = new CustomQGraphicsRectItem;
        rectItem->setZValue(element.attribute("zValue").toInt());
        graphicsItems.push_back(rectItem);
        float posx = element.attribute("posX").toFloat() * mEditGraphicViewWidget->getGraphicsView()->width();
        float posy = element.attribute("posY").toFloat() * mEditGraphicViewWidget->getGraphicsView()->height();
        addItemToScene(rectItem);
        rectItem->setPos(posx, posy);
        rectItem->setRotation(rotation);
    }
    else if (type == INNER_DASHBOARD_ITEM)
    {
        CustomInnerDashboardRectItem* rectItem = new CustomInnerDashboardRectItem;
        rectItem->setZValue(element.attribute("zValue").toInt());
        graphicsItems.push_back(rectItem);
        float posx = element.attribute("posX").toFloat() * mEditGraphicViewWidget->getGraphicsView()->width();
        float posy = element.attribute("posY").toFloat() * mEditGraphicViewWidget->getGraphicsView()->height();
        addItemToScene(rectItem);
        rectItem->setPos(posx, posy);
        rectItem->setRotation(rotation);
    }
    else if (type == INNER_WIDGET_ITEM)
    {
        CustomInnerWidgetRectItem* widgetRectItem = new CustomInnerWidgetRectItem;
        widgetRectItem->setZValue(element.attribute("zValue").toInt());
        graphicsItems.push_back(widgetRectItem);
        float posx = element.attribute("posX").toFloat() * mEditGraphicViewWidget->getGraphicsView()->width();
        float posy = element.attribute("posY").toFloat() * mEditGraphicViewWidget->getGraphicsView()->height();
        float innerWidgetWidth = element.attribute("width").toFloat();
        float innerWidgetHeight = element.attribute("height").toFloat();
        int widgetIGWindowId = element.attribute("igWindowId", "-1").toInt();
        widgetRectItem->mIGWindowID = widgetIGWindowId;
        widgetRectItem->setPos(posx, posy);
        widgetRectItem->setWidthHeight(innerWidgetWidth, innerWidgetHeight);
        widgetRectItem->setRotation(rotation);
        addItemToScene(widgetRectItem);

    }
    std::map<std::string, std::string> configurations;
    QString configurationsString = element.attribute("configuration");
    if (configurationsString.size() > 0)
    {
        QStringList confList = configurationsString.split(";");
        for (int i = 0; i < confList.size(); i++)
        {
            QStringList confPart = confList.at(i).split(":");
            if (confPart.size() > 1)
            {
                configurations[confPart.at(0).toStdString()] = confPart.at(1).toStdString();
            }
        }
    }
    if (type == GRAPHICS_PIXMAP_IBRE_ITEM)
    {
        ////because we change transform origin
        //float posx = element.attribute ( "posX" ).toFloat ( ) * ui.graphicsViewMainScreen->width ( ) + ;
        //float posy = element.attribute ( "posY" ).toFloat ( ) * ui.graphicsViewMainScreen->height ( );
        IbreCustomSceneItem* rtn = new IbreCustomSceneItem(name, type, graphicsItems, showOnstart, attrName, attrType);
        rtn->mConfigurationsMap = configurations;
        rtn->mIsItemShown = visibleOnEditor;
        rtn->updateShowStatus(rtn->mIsItemShown);
        rtn->mGroupIndex = groupIndex;
        rtn->mRotationValue = rotation;
        return rtn;
    }
    else if (type == GRAPHICS_BAR_ITEM)
    {
        BarCustomSceneItem* newBarItem = new BarCustomSceneItem(name, type, graphicsItems, showOnstart, attrName, attrType);
        newBarItem->mConfigurationsMap = configurations;
        newBarItem->mIsItemShown = visibleOnEditor;
        newBarItem->updateShowStatus(newBarItem->mIsItemShown);
        newBarItem->mGroupIndex = groupIndex;
        newBarItem->mRotationValue = rotation;
        return newBarItem;
    }
    else if (type == GRAPHICS_SLIDER_ITEM)
    {
        SliderCustomSceneItem* newSliderItem = new SliderCustomSceneItem(name, type, graphicsItems, showOnstart, attrName, attrType);
        newSliderItem->mConfigurationsMap = configurations;
        newSliderItem->mIsItemShown = visibleOnEditor;
        newSliderItem->updateShowStatus(newSliderItem->mIsItemShown);
        newSliderItem->mGroupIndex = groupIndex;
        newSliderItem->mRotationValue = rotation;
        return newSliderItem;
    }
    else if (type == GRAPHICS_RECT_ITEM)
    {
        CustomRectSceneItem* newRectItem = new CustomRectSceneItem(name, type, graphicsItems, showOnstart, attrName, attrType);
        newRectItem->mConfigurationsMap = configurations;
        newRectItem->mIsItemShown = visibleOnEditor;
        newRectItem->updateShowStatus(newRectItem->mIsItemShown);
        newRectItem->mGroupIndex = groupIndex;
        newRectItem->mRotationValue = rotation;
        return newRectItem;
    }
    else if (type == INNER_DASHBOARD_ITEM)
    {
        QString newSizeAsPercentage = element.attribute("newSizeAsPercentage");
        InnerDashboardItem* innerDashboardItem = new InnerDashboardItem(name, type, graphicsItems, showOnstart, attrName, attrType);
        innerDashboardItem->mConfigurationsMap = configurations;
        innerDashboardItem->mCustomInnerDashboardRectItem->mInnerDashboardName = QString::fromStdString(configurations["InnerDashboardName"]);
        innerDashboardItem->mCustomInnerDashboardRectItem->update();
        innerDashboardItem->mIsItemShown = visibleOnEditor;
        innerDashboardItem->updateShowStatus(innerDashboardItem->mIsItemShown);
        innerDashboardItem->mGroupIndex = groupIndex;
        innerDashboardItem->mInnerDashboardNewSizeAsPercentage = newSizeAsPercentage.toFloat();
        innerDashboardItem->mRotationValue = rotation;
        return innerDashboardItem;
    }
    else if (type == INNER_WIDGET_ITEM)
    {
        InnerWidgetItem* innerWidgetItem = new InnerWidgetItem(name, type, graphicsItems, showOnstart, attrName, attrType);
        innerWidgetItem->mConfigurationsMap = configurations;
        innerWidgetItem->mCustomInnerWidgetRectItem->mInnerWidgetName = QString::fromStdString(configurations["InnerWidgetName"]);
        innerWidgetItem->mCustomInnerWidgetRectItem->mOverrideSizeHeightAttrName = QString::fromStdString(configurations["OverrideHeightAttrName"]);
        innerWidgetItem->mCustomInnerWidgetRectItem->mOverrideSizeWidthAttrName = QString::fromStdString(configurations["OverrideWidthAttrName"]);
        innerWidgetItem->mCustomInnerWidgetRectItem->mOverridePosXAttrName = QString::fromStdString(configurations["OverrideXPosAttrName"]);
        innerWidgetItem->mCustomInnerWidgetRectItem->mOverridePosYAttrName = QString::fromStdString(configurations["OverrideYPosAttrName"]);

        innerWidgetItem->mCustomInnerWidgetRectItem->update();
        innerWidgetItem->mIsItemShown = visibleOnEditor;
        innerWidgetItem->mGroupIndex = groupIndex;
        innerWidgetItem->mRotationValue = rotation;
        return innerWidgetItem;
    }
    else if (type == GRAPHICS_PIXMAP_ITEM ||
        type == GRAPHICS_PIXMAP_LIGHT_ITEM ||
        type == GRAPHICS_PIXMAP_MULTIPLE_LIGHT_ITEM)
    {
        PixmapCustomSceneItem* newPixmapItem = new PixmapCustomSceneItem(originalPixmapItem, name, type, graphicsItems, showOnstart, attrName, attrType);
        newPixmapItem->mConfigurationsMap = configurations;
        newPixmapItem->mIsItemShown = visibleOnEditor;
        newPixmapItem->updateShowStatus(newPixmapItem->mIsItemShown);
        newPixmapItem->mGroupIndex = groupIndex;
        newPixmapItem->mRotationValue = rotation;
        return newPixmapItem;
    }
    else if (type == GRAPHICS_PIXMAP_BUTTON_INPUT_ITEM)
    {
        ButtonInput* buttonInput = new ButtonInput();
        QDomNodeList stateList = element.elementsByTagName("state");
        for (int i = 0; i < stateList.size(); i++)
        {
            QDomElement currentStateElement = stateList.at(i).toElement();
            QString stateImagePath = currentStateElement.attribute("stateImagePath");
            QString path = this->mProjectDir + "/" + stateImagePath;
            QPixmap tempPix(path);
            if (tempPix.isNull())
            {
                tempPix = QPixmap(this->mProjectDir + "/DashboardGenerator" + stateImagePath);
            }
            originalPixmapItem.push_back(tempPix);
            QGraphicsPixmapItem* newPixmap = new QGraphicsPixmapItem(tempPix);
            newPixmap->setZValue(element.attribute("zValue").toInt());
            graphicsItems.push_back(newPixmap);
            addItemToScene(newPixmap);
            float posx = element.attribute("posX").toFloat() * mEditGraphicViewWidget->getGraphicsView()->width();
            float posy = element.attribute("posY").toFloat() * mEditGraphicViewWidget->getGraphicsView()->height();
            newPixmap->setPos(posx, posy);
            QString name = currentStateElement.attribute("name");
            QGraphicsPixmapItem* graphicsPixmapItemForButtonInputState = new QGraphicsPixmapItem(tempPix);
            graphicsPixmapItemForButtonInputState->setZValue(element.attribute("zValue").toInt());
            int stateType = currentStateElement.attribute("type").toInt();
            buttonInput->setStateGraphicsPixmapItem(graphicsPixmapItemForButtonInputState);
            buttonInput->getStateItemList().at(buttonInput->getStateItemList().size() - 1)->mType = stateType;
            if (buttonInput->getStateItemList().at(buttonInput->getStateItemList().size() - 1)->mType == BUTTON_STATE) {
                StateOfInputs* latestState = buttonInput->getStateItemList().at(buttonInput->getStateItemList().size() - 1);
                latestState->mStateIndex = i;
                latestState->setStateName(name);
                QDomNodeList touchAreasTagList = currentStateElement.elementsByTagName("touchAreas");
                if (touchAreasTagList.size() > 0)
                {
                    QDomElement touchAreasParentElement = touchAreasTagList.at(0).toElement();
                    QDomNodeList touchAreaList = touchAreasParentElement.elementsByTagName("touchArea");
                    latestState->mStateTouchAreaIndex = touchAreaList.size();
                    for (int j = 0; j < touchAreaList.size(); j++)
                    {
                        QDomElement currentTouchAreaElement = touchAreaList.at(j).toElement();
                        QString touchAreaName = currentTouchAreaElement.attribute("name");

                        QDomNodeList touchAreaPointList = currentTouchAreaElement.elementsByTagName("point");
                        QPolygonF polygonOfTouchArea;
                        QGraphicsPolygonItem* touchAreaPolygonItem = new QGraphicsPolygonItem();
                        for (int k = 0; k < touchAreaPointList.size(); k++)
                        {
                            QDomElement currentPointElement = touchAreaPointList.at(k).toElement();
                            float xPos = currentPointElement.attribute("xPos").toFloat();
                            float yPos = currentPointElement.attribute("yPos").toFloat();
                            QPointF polygonPoints;
                            latestState->getStatePixmapItem()->boundingRect().width();
                            polygonPoints.setX(xPos * latestState->getStatePixmapItem()->boundingRect().width());
                            polygonPoints.setY(yPos * latestState->getStatePixmapItem()->boundingRect().height());
                            polygonOfTouchArea << polygonPoints;
                        }
                        touchAreaPolygonItem->setPolygon(polygonOfTouchArea);
                        touchAreaPolygonItem->setBrush(QBrush(Qt::red, Qt::FDiagPattern));
                        TouchAreaOfState* currentTouchArea = new TouchAreaOfState(touchAreaPolygonItem, touchAreaName);
                        latestState->addTouchArea(currentTouchArea);
                    }
                }

                QDomNodeList transitionTagList = currentStateElement.elementsByTagName("transitions");
                if (transitionTagList.size() > 0) {
                    QDomElement transitionParentElement = transitionTagList.at(0).toElement();
                    QDomNodeList transitionList = transitionParentElement.elementsByTagName("transition");
                    for (int j = 0; j < transitionList.size(); j++)
                    {
                        QDomElement currentTransitionElement = transitionList.at(j).toElement();
                        const QString transitionLogicType = currentTransitionElement.attribute("logicType");
                        const QString transitionName = currentTransitionElement.attribute("name");
                        const QString selectedTouchAreaIndex = currentTransitionElement.attribute("selectedTouchAreaIndex");
                        const QString transitionCheckBehavior = currentTransitionElement.attribute("transitionCheckBehavior");
                        const QString transitionStateIndex = currentTransitionElement.attribute("transitionStateIndex");
                        const QString transitionSoundPath = currentTransitionElement.attribute("transitionSoundPath");

                        TransitionOfState currentTransitionOfState;
                        currentTransitionOfState.setConditionLogicalType(transitionLogicType.toInt());
                        currentTransitionOfState.setTransitionName(transitionName);
                        currentTransitionOfState.setTargetTouchArea(selectedTouchAreaIndex.toInt());
                        currentTransitionOfState.setTransitionConditionCheckBehavior(transitionCheckBehavior.toInt());
                        currentTransitionOfState.setTargetStateIndex(transitionStateIndex.toInt());
                        currentTransitionOfState.setTransitionSoundPath(transitionSoundPath);
                        latestState->addTransition(currentTransitionOfState);

                        QDomNodeList transitionConditionList = currentTransitionElement.elementsByTagName("transitionCondition");
                        std::vector<Condition> currentConditionList;
                        for (int k = 0; k < transitionConditionList.size(); k++)
                        {
                            QDomElement currentTransitionConditionElement = transitionConditionList.at(k).toElement();
                            QString transitionConditionAttrName = currentTransitionConditionElement.attribute("attrName");
                            QString transitionConditionAttrType = currentTransitionConditionElement.attribute("attrType");
                            QString transitionConditionisCommandId = currentTransitionConditionElement.attribute("isCommandId");
                            QString transitionConditionComparisonType = currentTransitionConditionElement.attribute("comparisonType");
                            QString transitionConditionComparisonValue = currentTransitionConditionElement.attribute("comparisonValue");

                            Condition currentCondition;
                            currentCondition.ConditionAttrName = transitionConditionAttrName;
                            currentCondition.ConditionAttrType = transitionConditionAttrType;
                            currentCondition.ConditionIsCommanId = transitionConditionisCommandId.toInt();
                            currentCondition.ConditionComparisonValue = transitionConditionComparisonValue;
                            currentCondition.ConditionComparisonType = transitionConditionComparisonType.toInt();
                            currentConditionList.push_back(currentCondition);
                        }
                        latestState->getTransitionOfState().at(j).setConditionList(currentConditionList);

                        QDomNodeList inputToSetTagList = currentTransitionElement.elementsByTagName("inputToSet");
                        if (inputToSetTagList.size() > 0) {
                            QDomElement inputToSetElement = inputToSetTagList.at(0).toElement();
                            QString inputToSetAttrName = inputToSetElement.attribute("attrName");
                            QString inputToSetArreType = inputToSetElement.attribute("attrType");
                            QString inputToSetIsCommandId = inputToSetElement.attribute("isCommandId");
                            QString inputToSetValue = inputToSetElement.attribute("value");

                            InputToSet currentInputToSet;
                            currentInputToSet.InputAttrName = inputToSetAttrName;
                            currentInputToSet.InputAttrType = inputToSetArreType;
                            currentInputToSet.InputIsCommandId = inputToSetIsCommandId.toInt();
                            currentInputToSet.InputValue = inputToSetValue;
                            latestState->getTransitionOfState().at(j).setInputToSet(currentInputToSet);
                        }
                    }
                }
            }

            else if (buttonInput->getStateItemList().at(buttonInput->getStateItemList().size() - 1)->mType == ROTATABLE_STATE) {
                RotatableStateOfInputs* latestRotatableState = (RotatableStateOfInputs*)(buttonInput->getStateItemList().at(buttonInput->getStateItemList().size() - 1));

                latestRotatableState->mStateIndex = i;
                latestRotatableState->setStateName(name);
                QDomNodeList touchAreasTagList = currentStateElement.elementsByTagName("touchAreas");
                if (touchAreasTagList.size() > 0)
                {
                    QDomElement touchAreasParentElement = touchAreasTagList.at(0).toElement();
                    QDomNodeList touchAreaList = touchAreasParentElement.elementsByTagName("touchArea");
                    latestRotatableState->mStateTouchAreaIndex = touchAreaList.size();
                    for (int j = 0; j < touchAreaList.size(); j++)
                    {
                        QDomElement currentTouchAreaElement = touchAreaList.at(j).toElement();
                        QString touchAreaName = currentTouchAreaElement.attribute("name");

                        QDomNodeList touchAreaPointList = currentTouchAreaElement.elementsByTagName("point");
                        QPolygonF polygonOfTouchArea;
                        QGraphicsPolygonItem* touchAreaPolygonItem = new QGraphicsPolygonItem();
                        for (int k = 0; k < touchAreaPointList.size(); k++)
                        {
                            QDomElement currentPointElement = touchAreaPointList.at(k).toElement();
                            float xPos = currentPointElement.attribute("xPos").toFloat();
                            float yPos = currentPointElement.attribute("yPos").toFloat();
                            QPointF polygonPoints;
                            latestRotatableState->getStatePixmapItem()->boundingRect().width();
                            polygonPoints.setX(xPos * latestRotatableState->getStatePixmapItem()->boundingRect().width());
                            polygonPoints.setY(yPos * latestRotatableState->getStatePixmapItem()->boundingRect().height());
                            polygonOfTouchArea << polygonPoints;
                        }
                        touchAreaPolygonItem->setPolygon(polygonOfTouchArea);
                        touchAreaPolygonItem->setBrush(QBrush(Qt::red, Qt::FDiagPattern));
                        TouchAreaOfState* currentTouchArea = new TouchAreaOfState(touchAreaPolygonItem, touchAreaName);
                        latestRotatableState->addTouchArea(currentTouchArea);
                    }
                }

                QDomNodeList transitionTagList = currentStateElement.elementsByTagName("transitions");
                if (transitionTagList.size() > 0) {
                    QDomElement transitionParentElement = transitionTagList.at(0).toElement();
                    QDomNodeList transitionList = transitionParentElement.elementsByTagName("transition");
                    for (int j = 0; j < transitionList.size(); j++)
                    {
                        QDomElement currentTransitionElement = transitionList.at(j).toElement();
                        QString transitionLogicType = currentTransitionElement.attribute("logicType");
                        QString transitionName = currentTransitionElement.attribute("name");
                        QString selectedTouchAreaIndex = currentTransitionElement.attribute("selectedTouchAreaIndex");
                        QString transitionCheckBehavior = currentTransitionElement.attribute("transitionCheckBehavior");
                        QString transitionStateIndex = currentTransitionElement.attribute("transitionStateIndex");

                        TransitionOfState currentTransitionOfState;
                        currentTransitionOfState.setConditionLogicalType(transitionLogicType.toInt());
                        currentTransitionOfState.setTransitionName(transitionName);
                        currentTransitionOfState.setTargetTouchArea(selectedTouchAreaIndex.toInt());
                        currentTransitionOfState.setTransitionConditionCheckBehavior(transitionCheckBehavior.toInt());
                        currentTransitionOfState.setTargetStateIndex(transitionStateIndex.toInt());
                        latestRotatableState->addTransition(currentTransitionOfState);

                        QDomNodeList transitionConditionList = currentTransitionElement.elementsByTagName("transitionCondition");
                        std::vector<Condition> currentConditionList;
                        for (int k = 0; k < transitionConditionList.size(); k++)
                        {
                            QDomElement currentTransitionConditionElement = transitionConditionList.at(k).toElement();
                            QString transitionConditionAttrName = currentTransitionConditionElement.attribute("attrName");
                            QString transitionConditionAttrType = currentTransitionConditionElement.attribute("attrType");
                            QString transitionConditionisCommandId = currentTransitionConditionElement.attribute("isCommandId");
                            QString transitionConditionComparisonType = currentTransitionConditionElement.attribute("comparisonType");
                            QString transitionConditionComparisonValue = currentTransitionConditionElement.attribute("comparisonValue");

                            Condition currentCondition;
                            currentCondition.ConditionAttrName = transitionConditionAttrName;
                            currentCondition.ConditionAttrType = transitionConditionAttrType;
                            currentCondition.ConditionIsCommanId = transitionConditionisCommandId.toInt();
                            currentCondition.ConditionComparisonValue = transitionConditionComparisonValue;
                            currentCondition.ConditionComparisonType = transitionConditionComparisonType.toInt();
                            currentConditionList.push_back(currentCondition);
                        }
                        latestRotatableState->getTransitionOfState().at(j).setConditionList(currentConditionList);

                        QDomNodeList inputToSetTagList = currentTransitionElement.elementsByTagName("inputToSet");
                        if (inputToSetTagList.size() > 0) {
                            QDomElement inputToSetElement = inputToSetTagList.at(0).toElement();
                            QString inputToSetAttrName = inputToSetElement.attribute("attrName");
                            QString inputToSetArreType = inputToSetElement.attribute("attrType");
                            QString inputToSetIsCommandId = inputToSetElement.attribute("isCommandId");
                            QString inputToSetValue = inputToSetElement.attribute("value");

                            InputToSet currentInputToSet;
                            currentInputToSet.InputAttrName = inputToSetAttrName;
                            currentInputToSet.InputAttrType = inputToSetArreType;
                            currentInputToSet.InputIsCommandId = inputToSetIsCommandId.toInt();
                            currentInputToSet.InputValue = inputToSetValue;
                            latestRotatableState->getTransitionOfState().at(j).setInputToSet(currentInputToSet);
                        }
                    }
                }

                QDomNodeList inputValuesTagList = currentStateElement.elementsByTagName("inputValues");
                if (inputValuesTagList.size() > 0) {
                    QDomElement inputValuesParentElement = inputValuesTagList.at(0).toElement();
                    QString rotatableStateAttrName = inputValuesParentElement.attribute("attributeName");
                    QString rotatableStateAttrType = inputValuesParentElement.attribute("attributeType");
                    QString rotatableStateIsCommandId = inputValuesParentElement.attribute("isCommandId");
                    latestRotatableState->mAttributeName = rotatableStateAttrName;
                    latestRotatableState->mAttributeType = rotatableStateAttrType;
                    latestRotatableState->mIsCommandId = rotatableStateIsCommandId.toInt();

                    QDomNodeList rotationValuesList = inputValuesParentElement.elementsByTagName("rotation");
                    for (int j = 0; j < rotationValuesList.size(); j++) {
                        QDomElement currentRotationValue = rotationValuesList.at(j).toElement();
                        QString maxAngleValue = currentRotationValue.attribute("maxAngleValue");
                        QString maxValue = currentRotationValue.attribute("maxValue");
                        QString minAngleValue = currentRotationValue.attribute("minAngleValue");
                        QString minValue = currentRotationValue.attribute("minValue");

                        latestRotatableState->mMaxRotatableStateAngle = maxAngleValue.toFloat();
                        latestRotatableState->mMaxRotatableStateValue = maxValue.toFloat();
                        latestRotatableState->mMinRotatableStateAngle = minAngleValue.toFloat();
                        latestRotatableState->mMinRotatableStateValue = minValue.toFloat();

                        QDomNodeList rotationDiscreteValueList = currentRotationValue.elementsByTagName("rotationDiscreteValue");
                        for (int k = 0; k < rotationDiscreteValueList.size(); k++) {
                            QDomElement currentRotationDiscreteValue = rotationDiscreteValueList.at(k).toElement();
                            QString discreteAngleValue = currentRotationDiscreteValue.attribute("discreteAngleValue");
                            QString discreteValue = currentRotationDiscreteValue.attribute("discreteValue");
                            latestRotatableState->mRotatableStateDiscreteAngleList.push_back(discreteAngleValue.toFloat());

                            latestRotatableState->mRotatableStateDiscreteValueList.push_back(discreteValue.toFloat());
                            latestRotatableState->mRotatableStateBehaviorType = DISCRETE;
                        }
                    }
                }
            }
        }
        PixmapInputSceneItem* newPixmapInputItem = new PixmapInputSceneItem(buttonInput, originalPixmapItem, name, type, graphicsItems, showOnstart, attrName, attrType);
        newPixmapInputItem->mConfigurationsMap = configurations;
        newPixmapInputItem->mRotationValue = rotation;
        return newPixmapInputItem;
    }
    CustomSceneItem* rtn = new CustomSceneItem(name, type, graphicsItems, showOnstart, attrName, attrType);
    rtn->mConfigurationsMap = configurations;
    rtn->mIsItemShown = visibleOnEditor;
    rtn->updateShowStatus(rtn->mIsItemShown);
    rtn->mGroupIndex = groupIndex;
    rtn->mRotationValue = rotation;
    return rtn;
}

void DashboardGeneratorGui::setIbrePropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem)
{
    QDomElement element = param1.toElement();
    IbreCustomSceneItem* ibreItem = ((IbreCustomSceneItem*)newSceneItem);

    QDomNodeList ibreValuesParentList = element.elementsByTagName("ibreValues");
    for (int i = 0; i < ibreValuesParentList.size(); i++)
    {
        QDomElement currentIbreValuesParentElement = ibreValuesParentList.at(i).toElement();
        QDomNodeList ibreDiscreteValueList = currentIbreValuesParentElement.elementsByTagName("ibreDiscreteValues");
        for (int j = 0; j < ibreDiscreteValueList.size(); j++) {
            QDomElement currentIbreDiscreteValue = ibreDiscreteValueList.at(j).toElement();
            QString discreteAngleValue = currentIbreDiscreteValue.attribute("discreteAngleValue");
            QString discreteValue = currentIbreDiscreteValue.attribute("discreteValue");
            ibreItem->mDiscreteIbreValueList.push_back(discreteValue.toFloat());
            ibreItem->mDiscreteIbreAngleList.push_back(discreteAngleValue.toFloat());
            ibreItem->mIsDiscreteIbre = true;
        }

        if (ibreItem->mDiscreteIbreAngleList.size() > 0)
        {
            ibreItem->mMinIbreAngle = ibreItem->mDiscreteIbreAngleList.at(0);
            ibreItem->mMinIbreValue = ibreItem->mDiscreteIbreValueList.at(0);

            ibreItem->mMaxIbreAngle = ibreItem->mDiscreteIbreAngleList.at(ibreItem->mDiscreteIbreAngleList.size() - 1);
            ibreItem->mMaxIbreValue = ibreItem->mDiscreteIbreValueList.at(ibreItem->mDiscreteIbreValueList.size() - 1);
        }
    }

}

void DashboardGeneratorGui::handleSelectedAddedElementPart(AddedElement* addedElement)
{
    for (int i = 0; i < mAddedElementsVector.size(); i++)
    {
        bool selectedStatus = false;
        if (mAddedElementsVector.at(i) == addedElement)
        {
            selectedStatus = true;
        }
        mAddedElementsVector.at(i)->mSelected = selectedStatus;
        mAddedElementsVector.at(i)->handleSelectedStatus();
    }
}

void DashboardGeneratorGui::handleSignalSetParentTriggered(AddedElement* addedElement)
{
    mSetGroupScreenWindow->mCurrentSceneElement = addedElement->mCustomItem;
    mSetGroupScreenWindow->mGroupNames = mGroupNames;
    mSetGroupScreenWindow->updateScreen();
    mSetGroupScreenWindow->setWindowModality(Qt::WindowModal);
    mSetGroupScreenWindow->show();
}

void DashboardGeneratorGui::disableEnableAllItems()
{
    mDisableEnableAllFlag = !mDisableEnableAllFlag;
    for (int i = 0; i < mSceneItems.size(); i++)
    {
        mSceneItems.at(i)->updateShowStatus(mDisableEnableAllFlag);
    }
}

void DashboardGeneratorGui::setBarPropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem)
{
    QDomElement element = param1.toElement();
    BarCustomSceneItem* barItem = ((BarCustomSceneItem*)newSceneItem);
    barItem->mBarType = (BarType)element.attribute("barType").toInt();
    barItem->mBarColor = QColor(QString::fromStdString(element.attribute("barColor").toStdString()));
    barItem->mLineThickness = element.attribute("barWidth").toInt();
    barItem->mLineLength = element.attribute("barHeight").toFloat();
    barItem->mMaxVal = element.attribute("maxVal").toFloat();
    barItem->mMinVal = element.attribute("minVal").toFloat();
    barItem->mBarWidgetCss = QString::fromStdString(element.attribute("barWidgetCss").toStdString());

    // Arc bar özelliklerini yükle
    if (element.hasAttribute("arcRadius"))
        barItem->mArcRadius = element.attribute("arcRadius").toInt();
    if (element.hasAttribute("arcStartAngle"))
        barItem->mArcStartAngle = element.attribute("arcStartAngle").toInt();
    if (element.hasAttribute("arcSpanAngle"))
        barItem->mArcSpanAngle = element.attribute("arcSpanAngle").toInt();
    if (element.hasAttribute("barRadius"))
        barItem->mBarRadius = element.attribute("barRadius").toInt();
    if (element.hasAttribute("arcBend"))
        barItem->mArcBend = element.attribute("arcBend").toInt();
    if (element.hasAttribute("arcBendDirection"))
        barItem->mArcBendDirection = element.attribute("arcBendDirection").toInt();
    if (element.hasAttribute("arcRotation"))
        barItem->mArcRotation = element.attribute("arcRotation").toInt();

    ((CustomBarProxyWidgetItem*)(newSceneItem->mItems.at(0)))->createProgressBarWithGivenItem(barItem);
}

void DashboardGeneratorGui::setSliderPropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem)
{
    QDomElement element = param1.toElement();
    SliderCustomSceneItem* sliderItem = ((SliderCustomSceneItem*)newSceneItem);
    sliderItem->mSliderType = (SliderType)element.attribute("sliderType").toInt();
    sliderItem->mLineThickness = element.attribute("sliderWidth").toInt();
    sliderItem->mLineLength = element.attribute("sliderHeight").toFloat();
    sliderItem->mMaxVal = element.attribute("maxVal").toFloat();
    sliderItem->mMinVal = element.attribute("minVal").toFloat();
    sliderItem->mSliderWidgetCss = QString::fromStdString(element.attribute("sliderWidgetCss").toStdString());
    ((CustomSliderProxyWidgetItem*)(newSceneItem->mItems.at(0)))->createSliderWithGivenItem(sliderItem);
}

void DashboardGeneratorGui::setRectPropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem)
{
    QDomElement element = param1.toElement();
    CustomRectSceneItem* rectItem = ((CustomRectSceneItem*)newSceneItem);
    CustomQGraphicsRectItem* customQRectItem = rectItem->mCustomQGraphicsRectItem;
    customQRectItem->mWidth = element.attribute("rectWidth").toInt();
    customQRectItem->mHeight = element.attribute("rectHeigth").toInt();
    customQRectItem->mIsFilled = element.attribute("isFilled").toInt();
    customQRectItem->mFillColor = QColor(QString::fromStdString(element.attribute("fillColor").toStdString()));
    customQRectItem->mGeneralThickness = element.attribute("generalEdgeThickness").toInt();
    customQRectItem->mGeneralEdgeColor = QColor(QString::fromStdString(element.attribute("generalEdgeColor").toStdString()));
    QStringList edgePropertiesStringList = element.attribute("customEdgeProperties").split(";");
    for (int i = 0; i < edgePropertiesStringList.size(); i++)
    {
        if (edgePropertiesStringList.at(i).size() == 0)
        {
            continue;
        }
        QStringList subProperties = edgePropertiesStringList.at(i).split(":");
        customQRectItem->mEdgeProperties[i].isCustom = subProperties.at(0).compare("true") == 0;
        customQRectItem->mEdgeProperties[i].thickness = subProperties.at(1).toInt();
        customQRectItem->mEdgeProperties[i].color = QColor(subProperties.at(2));
    }
    customQRectItem->update();
}

void DashboardGeneratorGui::setInnerDashboardPropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem)
{
    QDomElement element = param1.toElement();
    InnerDashboardItem* innerDashboardItem = ((InnerDashboardItem*)newSceneItem);
    CustomInnerDashboardRectItem* customInnerDashboardRectItem = innerDashboardItem->mCustomInnerDashboardRectItem;
    customInnerDashboardRectItem->mInnerDashboardName = QString::fromStdString(innerDashboardItem->mConfigurationsMap["InnerDashboardName"]);
    customInnerDashboardRectItem->update();
}

void DashboardGeneratorGui::setInnerWidgetPropertiesForItem(QDomNode param1, CustomSceneItem* newSceneItem)
{
    QDomElement element = param1.toElement();
    InnerWidgetItem* innerWidgetItem = ((InnerWidgetItem*)newSceneItem);
    CustomInnerWidgetRectItem* customInnerWidgetRectItem = innerWidgetItem->mCustomInnerWidgetRectItem;
    customInnerWidgetRectItem->mInnerWidgetName = QString::fromStdString(innerWidgetItem->mConfigurationsMap["InnerWidgetName"]);
    customInnerWidgetRectItem->mOverrideSizeHeightAttrName = QString::fromStdString(innerWidgetItem->mConfigurationsMap["OverrideHeightAttrName"]);
    customInnerWidgetRectItem->mOverrideSizeWidthAttrName = QString::fromStdString(innerWidgetItem->mConfigurationsMap["OverrideWidthAttrName"]);
    customInnerWidgetRectItem->mOverridePosXAttrName = QString::fromStdString(innerWidgetItem->mConfigurationsMap["OverrideXPosAttrName"]);
    customInnerWidgetRectItem->mOverridePosYAttrName = QString::fromStdString(innerWidgetItem->mConfigurationsMap["OverrideYPosAttrName"]);
    customInnerWidgetRectItem->update();
}

void DashboardGeneratorGui::handleGroupNameEdited(int index, QString newName)
{
    mGroupNames[index] = newName;
    mSetGroupScreenWindow->mGroupNames = mGroupNames;
    updateAddedElementsPart();
    mSetGroupScreenWindow->updateScreen();
}

void DashboardGeneratorGui::handleGroupSet(CustomSceneItem* element, int index)
{
    element->mGroupIndex = index;
    updateAddedElementsPart();
}

void DashboardGeneratorGui::handleNewGroupAdded(QString groupName)
{
    mGroupNames.append(groupName);
    mSetGroupScreenWindow->mGroupNames = mGroupNames;
    updateAddedElementsPart(true, true);
    mSetGroupScreenWindow->updateScreen();
}

void DashboardGeneratorGui::handleCurrentGroupSelectionChanged(int index)
{
    if (index != mOldGroupSelectionIndex && index >= 0)
    {
        mOldGroupSelectionIndex = index;
        if (index >= 0)
        {
            updateAddedElementsPart();
        }
    }
}

void DashboardGeneratorGui::on_pushButtonToggleShowGroup_clicked()
{
    int currentGroupIndex = ui.comboBoxGroupSelect->currentIndex() - 1;
    bool showState = false;
    bool firstItemWithGroupIndexFound = false;
    for (int i = 0; i < mSceneItems.size(); i++)
    {
        if (mSceneItems.at(i)->mGroupIndex == currentGroupIndex)
        {
            if (!firstItemWithGroupIndexFound)
            {
                firstItemWithGroupIndexFound = true;
                showState = !mSceneItems.at(i)->mIsItemShown;
            }
            mSceneItems.at(i)->updateShowStatus(showState);
        }
    }
}
