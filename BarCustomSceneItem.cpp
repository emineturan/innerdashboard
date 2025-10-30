#include "BarCustomSceneItem.h"
#include "Logging.h"

#include <QGraphicsProxyWidget>
#include <QPainter>
#include <QStyleOption>
#include <QGraphicsSceneEvent>
#include <QRegularExpression>
#include <QColorDialog>
#include <QDebug>
#include <QLoggingCategory>
#include <cmath>
#include <algorithm>
#include <QtCore>
#include <QtGlobal>
#include <QApplication>
#include <QPainterPath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



BarCustomSceneItem::BarCustomSceneItem(QString name, int type, std::vector<QGraphicsItem*> item, bool showOnStart /*= false*/, QString attrName /*= ""*/, QString attrType /*= ""*/)
    : CustomSceneItem(name, type, item, showOnStart, attrName, attrType)
{
    mBarType = VERTICALBAR;
    mMaxVal = 100;
    mMinVal = 0;
    mLineThickness = 10;
    mLineLength = 30;
    mBarColor = QColor(Qt::white);
    mArcRadius = 50;
    mArcStartAngle = 225; // 7:00 pozisyonundan başla (sol alt) - yakıt göstergesi tarzı
    mArcSpanAngle = 90; // 90 derece - kesikli C şekli, görünür uzunluk
    mBarRadius = 0;
    mArcBend = 0; // Ortadan kırma efekti - default 0 (düz)
    mArcBendDirection = 0; // Default: ortadan kavis (0=ortadan, 1=yukarıdan, 2=aşağıdan)
    mArcRotation = 0; // Default: rotasyon yok (0-360 derece)
    // mLineThickness = arc kalınlığı, mLineLength = arc boyutu için kullanılacak
    mProgressBar = NULL;
    mRadialBar = NULL;
    mCurrentBarValue = 0.0f; // Arc bar için 0'dan başla
    // mBarRadius zaten var, onu kullanacağız
}

BarCustomSceneItem::~BarCustomSceneItem()
{
    // Radial bar'ı güvenli şekilde sil
    if (mRadialBar != NULL)
    {
        if (mRadialBar->scene())
        {
            mRadialBar->scene()->removeItem(mRadialBar);
        }
        delete mRadialBar;
        mRadialBar = NULL;
    }
}

void BarCustomSceneItem::changeBarType(BarType newBarType)
{
    if (mProgressBar == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    mBarType = newBarType;
    if (mBarType == VERTICALBAR)
    {
        mProgressBar->setOrientation(Qt::Vertical);
        mProgressBar->setInvertedAppearance(false);
    }
    else if (mBarType== HORIZONTALBAR)
    {
        mProgressBar->setOrientation(Qt::Horizontal);
        mProgressBar->setInvertedAppearance(false);
    }
    else if (mBarType == VERTICALBAR_REVERSE)
    {
        mProgressBar->setOrientation(Qt::Vertical);
        mProgressBar->setInvertedAppearance(true);

    }
    else if (mBarType == HORIZONTALBAR_REVERSE)
    {
        mProgressBar->setOrientation(Qt::Horizontal);
        mProgressBar->setInvertedAppearance(true);
    }
    else if (mBarType == ARC_BAR || mBarType == ARC_BAR_REVERSE)
    {
        // Progress bar'ı tamamen kaldır - proxy widget'ı tamamen gizle
        if (mItems.size() > 0)
        {
            // Proxy widget'ı tamamen gizle (progress bar container'ı)
            mItems[0]->setVisible(false);
            mItems[0]->hide();
            mItems[0]->setOpacity(0.0); // Tamamen şeffaf yap
            mItems[0]->setFlag(QGraphicsItem::ItemHasNoContents, true); // İçerik yok
        }
        
        // Progress bar'ı da tamamen gizle
        if (mProgressBar)
        {
            mProgressBar->setVisible(false);
            mProgressBar->hide();
            mProgressBar->setStyleSheet("QProgressBar { background: transparent; border: none; }");
            mProgressBar->setFixedSize(0, 0);
        }
        
        // Önceki radial bar varsa temizle
        if (mRadialBar != NULL)
        {
            if (mRadialBar->scene())
            {
                mRadialBar->scene()->removeItem(mRadialBar);
            }
            delete mRadialBar;
            mRadialBar = NULL;
        }
        
        // Radial progress bar için özel çizim kullan
        mRadialBar = new CustomRadialProgressBar(this);
        // Radial bar'ı parent scene'e direkt ekle, proxy widget'a değil
        if (mItems.size() > 0 && mItems[0]->scene())
        {
            mItems[0]->scene()->addItem(mRadialBar);
            // Proxy widget'ın merkezine yerleştir
            QPointF proxyCenter = mItems[0]->scenePos();
            QRectF proxyRect = mItems[0]->boundingRect();
            proxyCenter.setX(proxyCenter.x() + proxyRect.width() / 2);
            proxyCenter.setY(proxyCenter.y() + proxyRect.height() / 2);
            mRadialBar->setPos(proxyCenter);
            // Proxy widget'ın transform'unu da kopyala
            mRadialBar->setTransform(mItems[0]->transform());
            mRadialBar->setRotation(mItems[0]->rotation());
            
            // Scene'i güncelle
            mItems[0]->scene()->update();
        }
        // Z-value ayarla ki üstte görünsün
        mRadialBar->setZValue(10);
        
        // Radial bar'ın seçim davranışını ayarla - ancak hareket etmesin
        mRadialBar->setFlag(QGraphicsItem::ItemIsSelectable, true);
        mRadialBar->setFlag(QGraphicsItem::ItemIsMovable, false); // Hareket etmesin, proxy widget hareket etsin
        mRadialBar->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true); // Geometry değişikliklerini algıla
        
        // Radial bar'ı göster ve güncelle
        mRadialBar->setVisible(true);
        mRadialBar->updateProgress();
    }
    else
    {
        // Normal bar tipine geçerken radial bar'ı gizle
        if (mRadialBar)
        {
            mRadialBar->setVisible(false);
        }
        
        // Proxy widget'ı geri göster
        if (mItems.size() > 0)
        {
            mItems[0]->setVisible(true);
            mItems[0]->show();
            mItems[0]->setOpacity(1.0); // Opacity'yi geri yükle
            mItems[0]->setFlag(QGraphicsItem::ItemHasNoContents, false); // İçerik var
        }
        
        // Progress bar'ı geri yükle
        if (mProgressBar)
        {
            mProgressBar->setVisible(true);
            mProgressBar->show();
            // Boyutunu geri yükle
            mProgressBar->setFixedWidth(mLineThickness);
            mProgressBar->setFixedHeight(mLineLength);
        }
    }
}

void BarCustomSceneItem::changeBarWidth(int newWidth)
{
    mLineThickness = newWidth;
    
    // Arc bar için sadece kalınlığı güncelle (start angle'ı değiştirme)
    if (mBarType == ARC_BAR || mBarType == ARC_BAR_REVERSE)
    {
        if (mRadialBar)
        {
            mRadialBar->updateProgress();
            mRadialBar->update();
        }
        return;
    }
    
    // Normal bar için progress bar'ı güncelle
    if (mProgressBar == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    mProgressBar->setFixedWidth(newWidth);
    
    // Radial bar'ı da güncelle
    if (mRadialBar)
    {
        mRadialBar->updateProgress();
    }
}

void BarCustomSceneItem::changeBarHeight(int newHeight)
{
    mLineLength = newHeight;
    
    // Arc bar için height'ı Y ekseni uzatması olarak kullan
    if (mBarType == ARC_BAR || mBarType == ARC_BAR_REVERSE)
    {
        // Height değeri ile arc'ın Y ekseninde uzatılmasını kontrol et
        if (mRadialBar)
        {
            mRadialBar->updateProgress();
            mRadialBar->update();
        }
        return;
    }
    
    // Normal bar için progress bar'ı güncelle
    if (mProgressBar == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    mProgressBar->setFixedHeight(newHeight);
}

void BarCustomSceneItem::changeBarColor(QColor color)
{
    mBarColor = color;
    
    // Arc bar için sadece radial bar'ı güncelle
    if (mBarType == ARC_BAR || mBarType == ARC_BAR_REVERSE)
    {
        if (mRadialBar)
        {
            mRadialBar->updateProgress();
        }
        return;
    }
    
    // Normal bar için progress bar'ı güncelle
    if (mProgressBar == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    QString chunkCss = "QProgressBar::chunk{background-color:"+ mBarColor.name(QColor::HexArgb)+";}";
    mProgressBar->setStyleSheet(mBarWidgetCss + chunkCss);
    
    // Radial bar'ı da güncelle
    if (mRadialBar)
    {
        mRadialBar->updateProgress();
    }
}

void BarCustomSceneItem::changeBarPercentage(int param1)
{
    // Arc bar için progress bar yoksa direkt radial bar'ı güncelle
    if (mBarType == ARC_BAR || mBarType == ARC_BAR_REVERSE)
    {
        // Değeri direkt radial bar'a aktar
        float range = mMaxVal - mMinVal;
        if (range > 0)
        {
            // param1'i min-max aralığına çevir
            float normalizedValue = param1 / 100.0f; // 0-1 arası
            float actualValue = mMinVal + (normalizedValue * range);
            // Bu değeri radial bar için sakla
            mCurrentBarValue = actualValue;
        }
        else
        {
            mCurrentBarValue = param1; // Range yoksa direkt değeri kullan
        }
        
        if (mRadialBar && mRadialBar->isVisible())
        {
            mRadialBar->updateProgress();
        }
    }
    else
    {
        // Normal bar için progress bar kullan
        if (mProgressBar == NULL)
        {
            qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
            return;
        }

        mProgressBar->setValue(param1);
        
        // Radial bar'ı da güncelle
        if (mRadialBar && mRadialBar->isVisible())
        {
            mRadialBar->updateProgress();
        }
    }
}

void BarCustomSceneItem::changeBarCss(QString cssStr)
{
    if (mProgressBar == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    mBarWidgetCss = cssStr;
    QString chunkCss = "QProgressBar::chunk{background-color:" + mBarColor.name(QColor::HexArgb) + ";}";
    mProgressBar->setStyleSheet(mBarWidgetCss + chunkCss);
}

void BarCustomSceneItem::changeArcRadius(int radius)
{
    mArcRadius = radius;
    if (mRadialBar)
    {
        mRadialBar->updateProgress();
    }
}

void BarCustomSceneItem::changeArcStartAngle(int angle)
{
    mArcStartAngle = angle;
    if (mRadialBar)
    {
        mRadialBar->updateProgress();
    }
}

void BarCustomSceneItem::changeArcSpanAngle(int angle)
{
    mArcSpanAngle = angle;
    if (mRadialBar)
    {
        mRadialBar->updateProgress();
    }
}

void BarCustomSceneItem::updateArcBarStyle(bool reverse)
{
    if (mProgressBar == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    // Arc bar için özel CSS oluştur
    QString arcCss = QString(
        "QProgressBar {"
        "    border: none;"
        "    background: transparent;"
        "    border-radius: %1px;"
        "}"
        "QProgressBar::chunk {"
        "    background: conic-gradient(from %2deg, %3 %4deg, transparent %4deg);"
        "    border-radius: %1px;"
        "}"
    ).arg(mArcRadius)
     .arg(mArcStartAngle)
     .arg(mBarColor.name(QColor::HexArgb))
     .arg(mArcSpanAngle);

    if (reverse)
    {
        arcCss = QString(
            "QProgressBar {"
            "    border: none;"
            "    background: transparent;"
            "    border-radius: %1px;"
            "}"
            "QProgressBar::chunk {"
            "    background: conic-gradient(from %2deg, transparent %4deg, %3 %4deg);"
            "    border-radius: %1px;"
            "}"
        ).arg(mArcRadius)
         .arg(mArcStartAngle)
         .arg(mBarColor.name(QColor::HexArgb))
         .arg(mArcSpanAngle);
    }

    mProgressBar->setStyleSheet(arcCss);
}

void BarCustomSceneItem::changeBarRadius(int radius)
{
    mBarRadius = radius;
    
    // Arc bar için ortasından kavis verme
    if (mBarType == ARC_BAR || mBarType == ARC_BAR_REVERSE)
    {
        // Bar radius'u ortasından kavis için kullan
        if (mRadialBar)
        {
            mRadialBar->updateProgress(); // Değişiklikleri uygula
            mRadialBar->update(); // Yeniden çiz
        }
        return;
    }
    
    // Normal bar için progress bar CSS'i
    if (mProgressBar == NULL)
    {
        qCCritical(sliderComponent) << "Slider is Null for changeSliderType";
        return;
    }

    // Mevcut CSS'e radius ekle
    QString baseCss = mBarWidgetCss;
    if (mBarRadius > 0)
    {
        // CSS'e border-radius ekle
        if (baseCss.contains("border-radius"))
        {
            // Mevcut border-radius'u güncelle
            baseCss.replace(QRegularExpression("border-radius\\s*:\\s*\\d+px"), QString("border-radius: %1px").arg(mBarRadius));
        }
        else
        {
            // Yeni border-radius ekle
            baseCss += QString("border-radius: %1px;").arg(mBarRadius);
        }
    }
    
    QString chunkCss = "QProgressBar::chunk{background-color:" + mBarColor.name(QColor::HexArgb) + ";";
    if (mBarRadius > 0)
    {
        chunkCss += QString("border-radius: %1px;").arg(mBarRadius);
    }
    chunkCss += "}";
    
    mProgressBar->setStyleSheet(baseCss + chunkCss);
}

void BarCustomSceneItem::changeArcBend(int bend)
{
    mArcBend = bend;
    
    // Arc bar için ortadan kırma efekti
    if (mBarType == ARC_BAR || mBarType == ARC_BAR_REVERSE)
    {
        if (mRadialBar)
        {
            mRadialBar->updateProgress(); // Değişiklikleri uygula
            mRadialBar->update(); // Yeniden çiz
        }
    }
}

void BarCustomSceneItem::changeArcBendDirection(int direction)
{
    mArcBendDirection = direction;
    
    // Arc bar için kavis yönü değişikliği
    if (mBarType == ARC_BAR || mBarType == ARC_BAR_REVERSE)
    {
        if (mRadialBar)
        {
            mRadialBar->updateProgress(); // Değişiklikleri uygula
            mRadialBar->update(); // Yeniden çiz
        }
    }
}

void BarCustomSceneItem::changeArcRotation(int rotation)
{
    mArcRotation = rotation;
    
    // Arc bar için rotasyon değişikliği
    if (mBarType == ARC_BAR || mBarType == ARC_BAR_REVERSE)
    {
        if (mRadialBar)
        {
            // Rotasyon uygula
            mRadialBar->setRotation(rotation);
            mRadialBar->updateProgress(); // Değişiklikleri uygula
            mRadialBar->update(); // Yeniden çiz
        }
    }
}



// CustomRadialProgressBar Implementation
CustomRadialProgressBar::CustomRadialProgressBar(BarCustomSceneItem* barItem)
    : QGraphicsItem(), mBarItem(barItem)
{
    mRadius = barItem->mArcRadius;
    mThickness = barItem->mLineThickness;
    mColor = barItem->mBarColor;
    mValue = 50;
    mMinValue = barItem->mMinVal;
    mMaxValue = barItem->mMaxVal;
    mSpanAngle = barItem->mArcSpanAngle; // Bar uzunluğu
    // İç yarıçap barItem'da saklanacak
    
    // Item flag'lerini ayarla - rendering optimizasyonu için
    setFlag(QGraphicsItem::ItemIsMovable, false); // Hareket etmesin, proxy widget hareket etsin
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, false); // Cache optimizasyonu
    setFlag(QGraphicsItem::ItemHasNoContents, false); // İçerik var
    setAcceptHoverEvents(true);
    
    // Cache mode'u kalite için ayarla - bulanma önlemek için
    setCacheMode(QGraphicsItem::NoCache); // Cache yok - her zaman fresh render
    
    // Transform origin'i merkeze ayarla
    setTransformOriginPoint(0, 0);
}

QRectF CustomRadialProgressBar::boundingRect() const
{
    // Elips boyutları için hesaplama
    int radiusX = mRadius;
    int radiusY = (mBarItem && mBarItem->mLineLength > 10) ? mBarItem->mLineLength : mRadius;
    
    // Bar Radius efekti hesapla (elips deformasyonu)
    if (mBarItem && mBarItem->mBarRadius != 0)
    {
        float bendRatio = mBarItem->mBarRadius / 100.0f;
        if (bendRatio > 1.0f) bendRatio = 1.0f;
        if (bendRatio < -1.0f) bendRatio = -1.0f;
        
        // Paint ile aynı hesaplama - tutarlılık için
        float absBendRatio = abs(bendRatio);
        
        // Paint ile aynı değerler
        radiusX = radiusX * (1.0f - absBendRatio * 0.4f);
        
        if (bendRatio > 0)
        {
            radiusY = radiusY * (1.0f + absBendRatio * 0.3f); // Yukarı uzatma
        }
        else
        {
            radiusY = radiusY * (1.0f - absBendRatio * 0.2f); // Aşağı sıkıştırma
        }
    }
    
    // Arc Bend efekti hesapla (C şekli için bounding box)
    if (mBarItem && mBarItem->mArcBend != 0)
    {
        float arcBendRatio = mBarItem->mArcBend / 100.0f;
        if (arcBendRatio > 1.0f) arcBendRatio = 1.0f;
        if (arcBendRatio < -1.0f) arcBendRatio = -1.0f;
        
        // Maksimum generic seviyeler - ultra bend için
        float absRatio = abs(arcBendRatio);
        float bendStrength;
        if (absRatio <= 0.25f) {
            bendStrength = absRatio * 4.0f; // 0-1.0 arası
        } else if (absRatio <= 0.5f) {
            bendStrength = 1.0f + (absRatio - 0.25f) * 3.0f; // 1.0-1.75 arası
        } else if (absRatio <= 0.75f) {
            bendStrength = 1.75f + (absRatio - 0.5f) * 2.0f; // 1.75-2.25 arası
        } else {
            bendStrength = 2.25f + (absRatio - 0.75f) * 2.0f; // 2.25-2.75 arası (ULTRA!)
        }
        
        // C şekli için bounding box ayarlaması - güçlü efektler
        if (arcBendRatio > 0)
        {
            // İçe çekme efekti - bounding box'ı güçlü küçült
            radiusX = radiusX * (1.0f - bendStrength * 0.4f); // Güçlü küçültme
            radiusY = radiusY; // Y ekseni sabit
        }
        else
        {
            // Dışa itme efekti - bounding box'ı güçlü genişlet
            radiusX = radiusX * (1.0f + bendStrength * 0.5f); // Güçlü genişletme
            radiusY = radiusY; // Y ekseni sabit
        }
    }
    
    int padding = mThickness + 10; // Kalınlık için padding
    
    // En büyük boyutu al
    int maxRadius = qMax(radiusX, radiusY);
    
    return QRectF(-maxRadius - padding/2, -maxRadius - padding/2, 
                  (maxRadius * 2) + padding, (maxRadius * 2) + padding);
}

void CustomRadialProgressBar::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    // Painter state'ini sakla
    painter->save();
    
    // Kusursuz rendering için tüm kalite ayarları
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    
    // Kompozisyon modu - net çizim için
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    
    // Progress yüzdesini hesapla
    float range = mMaxValue - mMinValue;
    float percentage = 0.0f;
    if (range > 0)
    {
        percentage = (mValue - mMinValue) / range;
    }
    percentage = qBound(0.0f, percentage, 1.0f);
    
    // Çizim için dikdörtgen
    QRectF rect(-mRadius, -mRadius, mRadius * 2, mRadius * 2);
    
    // Reverse kontrol
    bool isReverse = (mBarItem->mBarType == ARC_BAR_REVERSE);
    
    // Modern arc progress bar çizimi - ortadan bükme desteği ile
    int outerRadius = mRadius; // Dış yarıçap (Arc Radius ile kontrol)
    int barThickness = mThickness; // Bar kalınlığı (Bar Kalınlığı ile kontrol)
    
    // Bar radius ile ortadan bükme efekti
    int bendingFactor = mBarItem->mBarRadius; // Bar radius = bükme faktörü
    
    // Elips şekli için boyutlar
    int radiusX = outerRadius;
    int radiusY = outerRadius;
    
    // mLineLength ile Y ekseni uzatması (yukarı aşağı uzatma)
    if (mBarItem->mLineLength > 10)
    {
        // Y ekseni uzatma faktörü
        float yStretchFactor = mBarItem->mLineLength / 50.0f; // 50 = base değer
        if (yStretchFactor < 0.2f) yStretchFactor = 0.2f; // Minimum
        if (yStretchFactor > 6.0f) yStretchFactor = 6.0f; // Maksimum
        
        radiusY = outerRadius * yStretchFactor; // Y ekseninde uzatma
        radiusX = outerRadius; // X ekseni sabit kalır
    }
    
    // Arc Bend efekti hesapla (öncelikli)
    if (mBarItem->mArcBend != 0)
    {
        float arcBendRatio = mBarItem->mArcBend / 100.0f;
        if (arcBendRatio > 1.0f) arcBendRatio = 1.0f;
        if (arcBendRatio < -1.0f) arcBendRatio = -1.0f;
        
        float absBendRatio = abs(arcBendRatio);
        
        // Yakıt göstergesi tarzı deformasyon
        if (arcBendRatio > 0)
        {
            // Dışa doğru genişleme
            radiusX = radiusX * (1.0f + absBendRatio * 0.5f);
            radiusY = radiusY * (1.0f + absBendRatio * 0.3f);
        }
        else
        {
            // İçe doğru daraltma
            radiusX = radiusX * (1.0f - absBendRatio * 0.3f);
            radiusY = radiusY * (1.0f - absBendRatio * 0.2f);
        }
    }
    // Bar Radius efekti (Arc Bend yoksa)
    else if (bendingFactor != 0)
    {
        float bendRatio = bendingFactor / 100.0f; // -1 ile +1 arası
        if (bendRatio > 1.0f) bendRatio = 1.0f;
        if (bendRatio < -1.0f) bendRatio = -1.0f;
        
        float absBendRatio = abs(bendRatio);
        
        // Daha yumuşak bükme efekti
        radiusX = radiusX * (1.0f - absBendRatio * 0.4f); // Orta seviye sıkıştırma
        
        // Y ekseni değişikliği
        if (bendRatio > 0)
        {
            radiusY = radiusY * (1.0f + absBendRatio * 0.3f); // Yukarı uzatma
        }
        else
        {
            radiusY = radiusY * (1.0f - absBendRatio * 0.2f); // Aşağı sıkıştırma
        }
    }
    
    // Sadece progress fill (renkli) çiz - gri background yok
    if (percentage > 0)
    {
        int totalFilledAngle = mSpanAngle * percentage;
        
        // Progress fill için normal kalınlık (background yok artık)
        painter->setPen(QPen(mColor, barThickness, Qt::SolidLine, Qt::FlatCap));
        
        // Progress için bent arc'ın ortalama şeklini kullan - şekil bozulması önlemek için
        if (mBarItem->mArcBend != 0 || bendingFactor != 0)
        {
            // Bent arc için progress elips hesapla (ortalama değerler)
            float progressRadiusX = radiusX;
            float progressRadiusY = radiusY;
            
            // Arc Bend efekti için ortalama koordinatlar
            if (mBarItem->mArcBend != 0)
            {
                float arcBendRatio = mBarItem->mArcBend / 100.0f;
                if (arcBendRatio > 1.0f) arcBendRatio = 1.0f;
                if (arcBendRatio < -1.0f) arcBendRatio = -1.0f;
                
                // Maksimum generic seviyeler - ultra bend için
                float absRatio = abs(arcBendRatio);
                float bendStrength;
                if (absRatio <= 0.25f) {
                    bendStrength = absRatio * 4.0f; // 0-1.0 arası
                } else if (absRatio <= 0.5f) {
                    bendStrength = 1.0f + (absRatio - 0.25f) * 3.0f; // 1.0-1.75 arası
                } else if (absRatio <= 0.75f) {
                    bendStrength = 1.75f + (absRatio - 0.5f) * 2.0f; // 1.75-2.25 arası
                } else {
                    bendStrength = 2.25f + (absRatio - 0.75f) * 2.0f; // 2.25-2.75 arası (ULTRA!)
                }
                
                // C şekli için ortalama efekt (0.5 = orta nokta)
                float avgCEffect = sin(0.5f * M_PI) * bendStrength;
                
                if (arcBendRatio > 0)
                {
                    // İçe çekme efekti - ultra C şekli progress
                    progressRadiusX = radiusX * 0.8f * (1.0f - avgCEffect * 0.9f); // %80 boyut + %90 çekme
                }
                else
                {
                    // Dışa itme efekti - ultra ters C şekli progress
                    progressRadiusX = radiusX * 0.8f * (1.0f + avgCEffect * 0.8f); // %80 boyut + %80 itme
                }
                progressRadiusY = radiusY * 0.8f; // Y ekseni de %80 boyut
            }
            else if (bendingFactor != 0)
            {
                // Bar Radius efekti için progress daha küçük boyutlarda
                float bendRatio = bendingFactor / 100.0f;
                float absBendRatio = abs(bendRatio);
                progressRadiusX = radiusX * 0.85f * (1.0f - absBendRatio * 0.5f); // %85 boyut + %50 deformasyon
                if (bendRatio > 0)
                {
                    progressRadiusY = radiusY * 0.85f * (1.0f + absBendRatio * 0.3f); // %85 boyut + %30 uzatma
                }
                else
                {
                    progressRadiusY = radiusY * 0.85f * (1.0f - absBendRatio * 0.2f); // %85 boyut + %20 sıkıştırma
                }
            }
            
            // Progress elips çiz - sabit şekil (bent arc'ın ortalaması)
            QRectF progressRect(-progressRadiusX, -progressRadiusY, progressRadiusX * 2, progressRadiusY * 2);
            
            if (isReverse)
            {
                int startAngle = mBarItem->mArcStartAngle + (mSpanAngle - totalFilledAngle);
                painter->drawArc(progressRect, startAngle * 16, totalFilledAngle * 16);
            }
            else
            {
                painter->drawArc(progressRect, mBarItem->mArcStartAngle * 16, totalFilledAngle * 16);
            }
        }
        else
        {
            // Normal progress çizimi - %85 boyutunda (içerde kalması için)
            float fillRadiusX = radiusX * 0.85f;
            float fillRadiusY = radiusY * 0.85f;
            QRectF arcRect(-fillRadiusX, -fillRadiusY, fillRadiusX * 2, fillRadiusY * 2);
            
            if (totalFilledAngle <= 360)
            {
                if (isReverse)
                {
                    int startAngle = mBarItem->mArcStartAngle + (mSpanAngle - totalFilledAngle);
                    painter->drawArc(arcRect, startAngle * 16, totalFilledAngle * 16);
                }
                else
                {
                    painter->drawArc(arcRect, mBarItem->mArcStartAngle * 16, totalFilledAngle * 16);
                }
            }
        }
        
    }
    
    // Painter state'ini restore et
    painter->restore();
}

void CustomRadialProgressBar::updateProgress()
{
    if (!mBarItem) return;
    
    // Önceki boundingRect'i sakla
    QRectF oldRect = boundingRect();
    
    // Mevcut progress değerini sakla
    float previousValue = mValue;
    
    mRadius = mBarItem->mArcRadius;
    mThickness = mBarItem->mLineThickness;
    mColor = mBarItem->mBarColor;
    mMinValue = mBarItem->mMinVal;
    mMaxValue = mBarItem->mMaxVal;
    mSpanAngle = mBarItem->mArcSpanAngle; // Bar uzunluğu
    
    // Progress bar'dan değeri al - eğer yoksa mCurrentBarValue kullan
    if (mBarItem->mProgressBar && mBarItem->mProgressBar->isVisible())
    {
        // Normal bar modunda progress bar'dan değer al
        mValue = mBarItem->mProgressBar->value();
    }
    else
    {
        // Arc bar modunda mCurrentBarValue kullan ve korumaya al
        if (mBarItem->mCurrentBarValue >= mMinValue && mBarItem->mCurrentBarValue <= mMaxValue)
        {
            mValue = mBarItem->mCurrentBarValue;
        }
        else if (previousValue >= mMinValue && previousValue <= mMaxValue)
        {
            // Eğer mCurrentBarValue geçersizse önceki değeri koru
            mValue = previousValue;
            mBarItem->mCurrentBarValue = previousValue;
        }
    }
    
    // Pozisyon ve transform güncellemeleri
    if (mBarItem->mItems.size() > 0)
        {
            QGraphicsItem* proxyWidget = mBarItem->mItems[0];
            if (proxyWidget)
            {
                // Pozisyonu proxy widget'ın merkezine güncelle
                QPointF proxyCenter = proxyWidget->scenePos();
                QRectF proxyRect = proxyWidget->boundingRect();
                proxyCenter.setX(proxyCenter.x() + proxyRect.width() / 2);
                proxyCenter.setY(proxyCenter.y() + proxyRect.height() / 2);
                if (pos() != proxyCenter)
                {
                    setPos(proxyCenter);
                }
                
                // Transform'u kopyala (rotasyon dahil)
                QTransform newTransform = proxyWidget->transform();
                if (transform() != newTransform)
                {
                    setTransform(newTransform);
                }
                
                // Rotation'u ayrıca ayarla
                qreal newRotation = proxyWidget->rotation();
                if (rotation() != newRotation)
                {
                    setRotation(newRotation);
                }
            }
        }
        
    // Yeni boundingRect'i hesapla
    QRectF newRect = boundingRect();
    
    // Eğer boyut değiştiyse geometry change'i bildir
    if (oldRect != newRect)
    {
        prepareGeometryChange();
    }
    
    // Eski çizim kalıntılarını temizle ve yeniden çiz
    if (scene())
    {
        scene()->update(mapRectToScene(oldRect)); // Eski alanı temizle
        scene()->update(mapRectToScene(newRect)); // Yeni alanı çiz
    }
    update(); // Item'ı yeniden çiz
}

void CustomRadialProgressBar::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Mevcut progress değerini sakla
    float currentProgress = mValue;
    
    // Mouse press event'ini parent proxy widget'a ilet
    if (mBarItem && mBarItem->mItems.size() > 0)
    {
        // Proxy widget'ı seçili yap
        mBarItem->mItems[0]->setSelected(true);
        
        // Event'i proxy widget'a cast yaparak ilet
        CustomBarProxyWidgetItem* proxyWidget = dynamic_cast<CustomBarProxyWidgetItem*>(mBarItem->mItems[0]);
        if (proxyWidget)
        {
            proxyWidget->mousePressEvent(event);
        }
    }
    
    // Progress değerini geri yükle
    mValue = currentProgress;
    mBarItem->mCurrentBarValue = currentProgress;
    
    QGraphicsItem::mousePressEvent(event);
}

void CustomRadialProgressBar::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Mevcut progress değerini sakla
    float currentProgress = mValue;
    
    // Mouse release event'ini proxy widget'a ilet
    if (mBarItem && mBarItem->mItems.size() > 0)
    {
        CustomBarProxyWidgetItem* proxyWidget = dynamic_cast<CustomBarProxyWidgetItem*>(mBarItem->mItems[0]);
        if (proxyWidget)
        {
            proxyWidget->mouseReleaseEvent(event);
        }
    }
    
    // Progress değerini geri yükle
    mValue = currentProgress;
    mBarItem->mCurrentBarValue = currentProgress;
    
    QGraphicsItem::mouseReleaseEvent(event);
}

void CustomRadialProgressBar::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Mevcut progress değerini sakla
    float currentProgress = mValue;
    
    // Mouse move event'ini proxy widget'a ilet ki rotasyon çalışsın
    if (mBarItem && mBarItem->mItems.size() > 0)
    {
        CustomBarProxyWidgetItem* proxyWidget = dynamic_cast<CustomBarProxyWidgetItem*>(mBarItem->mItems[0]);
        if (proxyWidget)
        {
            proxyWidget->mouseMoveEvent(event);
            // Transform değişikliklerini hemen uygula ama progress değerini koru
            updateProgress();
            // Progress değerini geri yükle
            mValue = currentProgress;
            mBarItem->mCurrentBarValue = currentProgress;
            // Scene'i yeniden çizdir ki rotasyon görünsün
            update();
        }
    }
    QGraphicsItem::mouseMoveEvent(event);
}


void CustomBarProxyWidgetItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void CustomBarProxyWidgetItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void CustomBarProxyWidgetItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void CustomBarProxyWidgetItem::createDefaultProgressBar(BarCustomSceneItem* customItem)
{
    QProgressBar* progressBar = new QProgressBar();
    progressBar->setOrientation(Qt::Vertical);
    progressBar->setInvertedAppearance(false);
    customItem->mBarType = VERTICALBAR;
    QString chunkBackGroundColor = "#0000ff";
    customItem->mBarColor = QColor(chunkBackGroundColor);
    QString chunkCss = "QProgressBar::chunk{background-color: #0000ff;}";
    QString progressBarMainCss = "QProgressBar{border: 0px solid grey;background-color:rgba(0, 0, 0, 0); }";
    customItem->mBarWidgetCss = progressBarMainCss;
    progressBar->setFixedWidth(20);
    customItem->mLineThickness = 20;
    progressBar->setFixedHeight(100);
    customItem->mLineLength = 100;
    progressBar->setStyleSheet(chunkCss + progressBarMainCss);
    
    // Arc bar için default progress değeri ayarlama
    if (customItem->mBarType == ARC_BAR || customItem->mBarType == ARC_BAR_REVERSE)
    {
        progressBar->setValue(0); // Arc bar için 0'dan başla
        customItem->mCurrentBarValue = customItem->mMinVal; // Minimum değerden başla
    }
    else
    {
        progressBar->setValue(100); // Normal bar için 100
    }
    
    progressBar->setTextVisible(false);
    customItem->mProgressBar = progressBar;
    this->setWidget(progressBar);
}

void CustomBarProxyWidgetItem::createProgressBarWithGivenItem(BarCustomSceneItem* customItem)
{
    QPointF scenePosBefore = this->scenePos(); //scene pos changes when widget is added
    QProgressBar* progressBar = new QProgressBar();
    customItem->mProgressBar = progressBar;
    this->setWidget(progressBar);
    customItem->changeBarType(customItem->mBarType);
    customItem->changeBarColor(customItem->mBarColor);
    customItem->changeBarCss(customItem->mBarWidgetCss);
    customItem->changeBarWidth(customItem->mLineThickness);
    customItem->changeBarHeight(customItem->mLineLength);
    
    // Arc bar için default progress değeri ayarlama
    if (customItem->mBarType == ARC_BAR || customItem->mBarType == ARC_BAR_REVERSE)
    {
        progressBar->setValue(0); // Arc bar için 0'dan başla
        customItem->mCurrentBarValue = customItem->mMinVal; // Minimum değerden başla
    }
    else
    {
        progressBar->setValue(100); // Normal bar için 100
    }
    progressBar->setTextVisible(false);
    
    // Radial progress bar için özel item oluştur
    if (customItem->mBarType == ARC_BAR || customItem->mBarType == ARC_BAR_REVERSE)
    {
        // Önceki radial bar varsa temizle
        if (customItem->mRadialBar != NULL)
        {
            if (customItem->mRadialBar->scene())
            {
                customItem->mRadialBar->scene()->removeItem(customItem->mRadialBar);
            }
            delete customItem->mRadialBar;
            customItem->mRadialBar = NULL;
        }
        
        customItem->mRadialBar = new CustomRadialProgressBar(customItem);
        
        // Radial bar'ı scene'e direkt ekle
        if (this->scene())
        {
            this->scene()->addItem(customItem->mRadialBar);
            customItem->mRadialBar->setPos(this->scenePos());
        }
        customItem->mRadialBar->setZValue(10); // Üstte görünsün
        
        // Progress bar'ı tamamen gizle
        progressBar->setVisible(false);
        progressBar->hide();
        progressBar->setStyleSheet("QProgressBar { background: transparent; border: none; }");
        progressBar->setFixedSize(0, 0); // Boyutunu sıfırla
        
        // Proxy widget'ı da gizle
        this->setVisible(false);
        this->hide();
    }
    
    this->setPos(scenePosBefore);
}

void CustomRadialProgressBar::drawBentArc(QPainter* painter, int x, int y, int width, int height, 
                                         int startAngle, int spanAngle, int thickness, int bendFactor)
{
    float bendRatio = bendFactor / 100.0f;
    if (bendRatio > 1.0f) bendRatio = 1.0f;
    if (bendRatio < -1.0f) bendRatio = -1.0f;
    
    // Basit elips deformasyonu ile bükme efekti
    float radiusX = width / 2.0f;
    float radiusY = height / 2.0f;
    
    float absBendRatio = abs(bendRatio);
    
    // Daha yumuşak bükme efekti - boundingRect ile tutarlı
    radiusX = radiusX * (1.0f - absBendRatio * 0.4f);
    
    if (bendRatio > 0)
    {
        radiusY = radiusY * (1.0f + absBendRatio * 0.3f); // Yukarı uzatma
    }
    else
    {
        radiusY = radiusY * (1.0f - absBendRatio * 0.2f); // Aşağı sıkıştırma
    }
    
    // Basit elips arc çizimi
    QRectF arcRect(x + (width/2.0f - radiusX), y + (height/2.0f - radiusY), 
                   radiusX * 2, radiusY * 2);
    
    // Kalın çizgi ile çiz
    QPen pen = painter->pen();
    pen.setWidth(thickness);
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    
    if (abs(spanAngle) <= 360)
    {
        painter->drawArc(arcRect, startAngle * 16, spanAngle * 16);
    }
    else
    {
        // Çoklu çember çizimi
        int fullCircles = abs(spanAngle) / 360;
        int remainingAngle = abs(spanAngle) % 360;
        
        for (int i = 0; i < fullCircles; i++)
        {
            painter->drawArc(arcRect, (startAngle + i * 360) * 16, 360 * 16);
        }
        
        if (remainingAngle > 0)
        {
            painter->drawArc(arcRect, (startAngle + fullCircles * 360) * 16, remainingAngle * 16);
        }
    }
}

void CustomRadialProgressBar::drawBentArcWithBreak(QPainter* painter, int x, int y, int width, int height, 
                                                   int startAngle, int spanAngle, int thickness, int bendFactor)
{
    // C şeklinde eğim için özel algoritma
    float bendRatio = bendFactor / 100.0f;
    if (bendRatio > 1.0f) bendRatio = 1.0f;
    if (bendRatio < -1.0f) bendRatio = -1.0f;
    
    if (abs(bendRatio) < 0.01f) 
    {
        // Bend yoksa normal arc çiz
        QRectF arcRect(x, y, width, height);
        painter->drawArc(arcRect, startAngle * 16, spanAngle * 16);
        return;
    }
    
    // C şeklinde eğim için QPainterPath kullan
    QPainterPath path;
    
    float radiusX = width / 2.0f;
    float radiusY = height / 2.0f;
    float centerX = x + radiusX;
    float centerY = y + radiusY;
    
    // Maksimum generic seviyeler: 0-25-50-75-100 (ultra bend için)
    float bendStrength;
    float absRatio = abs(bendRatio);
    if (absRatio <= 0.25f) {
        bendStrength = absRatio * 4.0f; // 0-1.0 arası
    } else if (absRatio <= 0.5f) {
        bendStrength = 1.0f + (absRatio - 0.25f) * 3.0f; // 1.0-1.75 arası
    } else if (absRatio <= 0.75f) {
        bendStrength = 1.75f + (absRatio - 0.5f) * 2.0f; // 1.75-2.25 arası
    } else {
        bendStrength = 2.25f + (absRatio - 0.75f) * 2.0f; // 2.25-2.75 arası (ULTRA!)
    }
    
    // Segment sayısı - smooth C şekli için
    int segments = qMax(40, abs(spanAngle) / 2);
    float angleStep = spanAngle / (float)segments;
    
    bool firstPoint = true;
    
    for (int i = 0; i <= segments; i++)
    {
        float currentAngle = startAngle + (i * angleStep);
        float angleRad = currentAngle * M_PI / 180.0f;
        
        // Yönlü kavis efekti
        float normalizedPos = (float)i / segments; // 0-1 arası
        
        // Kavis yönüne göre efekt hesapla
        float cShapeEffect;
        if (mBarItem && mBarItem->mArcBendDirection == 1) 
        {
            // Yukarıdan kavis: başlangıçta maksimum, sonda minimum
            cShapeEffect = (1.0f - normalizedPos) * bendStrength;
        }
        else if (mBarItem && mBarItem->mArcBendDirection == 2)
        {
            // Aşağıdan kavis: başlangıçta minimum, sonda maksimum
            cShapeEffect = normalizedPos * bendStrength;
        }
        else
        {
            // Ortadan kavis (default): ortada maksimum, uçlarda minimum
            cShapeEffect = sin(normalizedPos * M_PI) * bendStrength;
        }
        
        // Yarıçapları C şekline göre ayarla
        float currentRadiusX = radiusX;
        float currentRadiusY = radiusY;
        
        if (bendRatio > 0)
        {
            // Pozitif: ortadan içe çekme (maksimum C şekli)
            currentRadiusX = radiusX * (1.0f - cShapeEffect * 0.995f); // Maksimum çekme (%99.5!)
            currentRadiusY = radiusY; // Y ekseni sabit
        }
        else
        {
            // Negatif: ortadan dışa itme (maksimum ters C şekli)
            currentRadiusX = radiusX * (1.0f + cShapeEffect * 1.2f); // Maksimum itme (%120!)
            currentRadiusY = radiusY; // Y ekseni sabit
        }
        
        // Nokta hesapla
        float pointX = centerX + currentRadiusX * cos(angleRad);
        float pointY = centerY + currentRadiusY * sin(angleRad);
        
        if (firstPoint)
        {
            path.moveTo(pointX, pointY);
            firstPoint = false;
        }
        else
        {
            path.lineTo(pointX, pointY);
        }
    }
    
    // Kalın çizgi ile C şeklini çiz
    QPen pen = painter->pen();
    pen.setWidth(thickness);
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->drawPath(path);
}


