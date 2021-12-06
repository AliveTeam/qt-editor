#include "resizeablerectitem.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneMoveEvent>
#include <QPainter>
#include <QTimer>
#include <QPointF>
#include <QCursor>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPixmapCache>
#include "model.hpp"
#include "PropertyTreeWidget.hpp"

const quint32 ResizeableRectItem::kMinRectSize = 10;


ResizeableRectItem::ResizeableRectItem(QGraphicsView* pView, MapObject* pMapObject, ISyncPropertiesToTree& propSyncer)
      : QGraphicsRectItem( pMapObject->XPos(), pMapObject->YPos(), pMapObject->Width(), pMapObject->Height(), nullptr ), mView(pView), mMapObject(pMapObject), mPropSyncer(propSyncer)
{
    Init();
    setZValue(3.0 + CalcZPos());
}

qreal ResizeableRectItem::CalcZPos() const
{
    // Why isn't area == 1 ?
    QRectF ourRect = rect();

    float area = ((float)ourRect.width() * (float)ourRect.height()) / 4294836225.0f;
    float percentArea = area * 100.0f;

    qreal zpos = 999999.0f - (percentArea*1000.0f);
    // Negative zpos underflows and breaks resize/selection.
    if (zpos < 0)
    {
        zpos = 0;
    }
    return zpos;
}

void ResizeableRectItem::mousePressEvent( QGraphicsSceneMouseEvent* aEvent )
{
    if ( aEvent->button() == Qt::LeftButton )
    {
        m_ResizeMode = getResizeLocation( aEvent->pos(), boundingRect() );
        if ( m_ResizeMode == eResize_None )
        {
            SetViewCursor( Qt::ClosedHandCursor );
        }
    }
    QGraphicsRectItem::mousePressEvent( aEvent );
}

void ResizeableRectItem::mouseMoveEvent( QGraphicsSceneMouseEvent* aEvent )
{
    if ( m_ResizeMode != eResize_None )
    {
        onResize( aEvent->pos() );
        return;
    }
    QGraphicsRectItem::mouseMoveEvent( aEvent );
}

void ResizeableRectItem::mouseReleaseEvent( QGraphicsSceneMouseEvent* aEvent )
{
    if ( aEvent->button() == Qt::LeftButton )
    {
        m_ResizeMode = eResize_None;
    }
    QGraphicsRectItem::mouseReleaseEvent( aEvent );
}

void ResizeableRectItem::paint( QPainter* aPainter, const QStyleOptionGraphicsItem* aOption, QWidget* aWidget /*= nullptr*/ )
{
    Q_UNUSED( aWidget );

    //aPainter->setClipRect( aOption->exposedRect );

    QRectF cRect = boundingRect();
    /*
    cRect.setX( cRect.x() + 1 );
    cRect.setY( cRect.y() + 1 );
    cRect.setWidth( cRect.width() - 1 );
    cRect.setHeight( cRect.height() - 1 );
*/
    // Draw the rect middle
   // aPainter->fillRect( cRect, Qt::SolidPattern );

    if ( isSelected() )
    {
        aPainter->setPen( QPen ( Qt::red, 2, Qt::DashLine ) );
    }
    else
    {
        // Draw normal rect
        aPainter->setPen( QPen( Qt::black, 2, Qt::SolidLine ) );
    }
    
    if ( m_Pixmap.isNull() )
    {
        aPainter->setBrush( Qt::darkGray );
    }
    else
    {
       // aPainter->setBrush( QBrush() );
        aPainter->drawPixmap( cRect.x(), cRect.y(), cRect.width(), cRect.height(), m_Pixmap );
    }

    // Draw the rect outline.
    aPainter->drawRect(cRect);

    // Draw the object name on the rect if no image is provided
    if (m_Pixmap.isNull())
    {
        const auto objectName = mMapObject->mObjectStructureType.c_str();
        for (int sizeCandidate = 8; sizeCandidate > 1; sizeCandidate--)
        {
            QFont f = aPainter->font();
            QFontMetricsF fm(f);
            const auto textRect = fm.boundingRect(cRect, Qt::AlignCenter | Qt::TextWrapAnywhere, objectName);

            if (textRect.width() > cRect.width() ||
                textRect.height() > cRect.height())
            {
               f.setPointSize(--sizeCandidate);
            }
            else
            {
                break;
            }
            aPainter->setFont(f);
        }
        aPainter->drawText(cRect, Qt::AlignCenter | Qt::TextWrapAnywhere, objectName);
    }
}

void ResizeableRectItem::hoverMoveEvent( QGraphicsSceneHoverEvent* aEvent )
{ 
    //qDebug("Resize mode = %d", m_ResizeMode);
    if ( !( flags() & QGraphicsItem::ItemIsSelectable ) )
    {
        SetViewCursor( Qt::OpenHandCursor );
        return;
    }

    const eResize resizeLocation = getResizeLocation( aEvent->pos(), boundingRect() );
    switch ( resizeLocation )
    {
    case eResize_None:
        SetViewCursor( Qt::OpenHandCursor );
        break;

    case eResize_TopLeftCorner:
        SetViewCursor( Qt::SizeFDiagCursor );
        break;

    case eResize_TopRightCorner:
        SetViewCursor( Qt::SizeBDiagCursor );
        break;

    case eResize_BottomLeftCorner:
        SetViewCursor( Qt::SizeBDiagCursor );
        break;

    case eResize_BottomRightCorner:
        SetViewCursor( Qt::SizeFDiagCursor );
        break;

    case eResize_Top:
        SetViewCursor( Qt::SizeVerCursor );
        break;

    case eResize_Left:
        SetViewCursor( Qt::SizeHorCursor );
        break;

    case eResize_Right:
        SetViewCursor( Qt::SizeHorCursor );
        break;

    case eResize_Bottom:
        SetViewCursor( Qt::SizeVerCursor );
        break;
    }
}

void ResizeableRectItem::hoverLeaveEvent( QGraphicsSceneHoverEvent* aEvent )
{
    SetViewCursor( Qt::ArrowCursor );
    QGraphicsItem::hoverLeaveEvent( aEvent );
}

QVariant ResizeableRectItem::itemChange( GraphicsItemChange aChange, const QVariant& aValue )
{
    if (aChange == ItemPositionChange)
    {
        //QPointF newPos = aValue.toPointF();
        // TODO: Change rect instead
        //return QVariant();
    }
    if ( aChange == ItemPositionHasChanged )
    {
        PosOrRectChanged();
    }
    return QGraphicsRectItem::itemChange( aChange, aValue );
}

void ResizeableRectItem::Init()
{
    m_ResizeMode = eResize_None;

    // Must set pen width for bounding rect calcs
    setPen( QPen( Qt::black, 2, Qt::SolidLine ) );

    setAcceptHoverEvents( true );

    // Change mouse cursor on hover so its easy to see when you can click to move the item
    SetViewCursor( Qt::PointingHandCursor );

    // Allow select and move.
    setFlags( ItemSendsScenePositionChanges |  ItemSendsGeometryChanges | ItemIsMovable | ItemIsSelectable );

    QString images_path = ":/object_images/rsc/object_images/";
    if ( !QPixmapCache::find(images_path + mMapObject->mObjectStructureType.c_str() + ".bmp", &m_Pixmap ) )
    {
        m_Pixmap = QPixmap(images_path + mMapObject->mObjectStructureType.c_str() + ".bmp");
        QPixmapCache::insert(images_path + mMapObject->mObjectStructureType.c_str() + ".bmp", m_Pixmap );
    }
    
    if (m_Pixmap.isNull())
    {
        this->setOpacity(0.5);
    }
    else
    {
        this->setOpacity(0.7);
    }
}


ResizeableRectItem::eResize ResizeableRectItem::getResizeLocation( QPointF aPos, QRectF aRect )
{
    const auto x = aPos.x();
    const auto y = aPos.y();
    const auto& bRect = aRect;

    bool xPosNearRectX = IsNear( x, bRect.x() );
    bool yPosNearRectY = IsNear( y, bRect.y() );
    bool xPosNearRectW = IsNear( x, bRect.x() + bRect.width() );
    bool yPosNearRectH = IsNear( y, bRect.y() + bRect.height() );

    // Top right corner
    if ( xPosNearRectW && yPosNearRectY )
    {
        return eResize_TopRightCorner;
    }

    // Bottom left corner
    if ( xPosNearRectX && yPosNearRectH )
    {
        return eResize_BottomLeftCorner;
    }

    // Top left corner
    if ( xPosNearRectX && yPosNearRectY )
    {
        return eResize_TopLeftCorner;
    }

    // Bottom right corner
    if ( xPosNearRectW && yPosNearRectH )
    {
        return eResize_BottomRightCorner;
    }

    // Left edge
    if ( xPosNearRectX && !yPosNearRectY )
    {
        return eResize_Left;
    }

    // Top edge
    if ( !xPosNearRectX && yPosNearRectY )
    {
        return eResize_Top;
    }

    // Right edge
    if ( xPosNearRectW && !yPosNearRectH )
    {
        return eResize_Right;
    }

    // Bottom edge
    if ( !xPosNearRectW && yPosNearRectH )
    {
        return eResize_Bottom;
    }

    return eResize_None;
}

bool ResizeableRectItem::IsNear( qreal xP1, qreal xP2 )
{
    qreal tolerance = 8; // aka epsilon
    if ( rect().width() <= kMinRectSize || rect().height() <= kMinRectSize )
    {
        tolerance = 1;
    }
    if ( abs( xP1-xP2 ) <= tolerance )
    {
        return true;
    }
    return false;
}

void ResizeableRectItem::onResize( QPointF aPos )
{
    QRectF curRect = rect();
    const bool isLeft = ( m_ResizeMode == eResize_Left )     || ( m_ResizeMode == eResize_TopLeftCorner )    || ( m_ResizeMode == eResize_BottomLeftCorner );
    const bool isRight = ( m_ResizeMode == eResize_Right )   || ( m_ResizeMode == eResize_TopRightCorner )   || ( m_ResizeMode == eResize_BottomRightCorner );
    const bool isTop = ( m_ResizeMode == eResize_Top )       || ( m_ResizeMode == eResize_TopLeftCorner )    || ( m_ResizeMode == eResize_TopRightCorner );
    const bool isBottom = ( m_ResizeMode == eResize_Bottom ) || ( m_ResizeMode == eResize_BottomLeftCorner ) || ( m_ResizeMode == eResize_BottomRightCorner );

    if ( isRight )
    {
        qreal newWidth = aPos.x() - curRect.x();
        if ( newWidth < kMinRectSize )
        {
            newWidth = kMinRectSize;
        }
        curRect.setWidth( newWidth );
    }
    else if ( isLeft )
    {
        qreal newx = aPos.x();
        if ( newx > (curRect.x()+curRect.width())-kMinRectSize )
        {
            newx = (curRect.x()+curRect.width())-kMinRectSize;
        }
        curRect.setX( newx );
    }

    if ( isTop )
    {
        qreal newy = aPos.y();
        if ( newy > (curRect.y()+curRect.height()-kMinRectSize))
        {
            newy = curRect.y()+curRect.height()-kMinRectSize;
        }
        curRect.setY( newy );
    }
    else if ( isBottom )
    {
        qreal newHeight = aPos.y() - curRect.y();
        if ( newHeight < kMinRectSize )
        {
            newHeight = kMinRectSize;
        }
        curRect.setHeight( newHeight );
    }

    prepareGeometryChange();
    setRect( curRect );
    PosOrRectChanged();
}

void ResizeableRectItem::SetViewCursor(Qt::CursorShape cursor)
{
    mView->setCursor(cursor);
}

QRectF ResizeableRectItem::SaveRect() const
{
    return rect();
}

void ResizeableRectItem::RestoreRect(const QRectF& rect)
{
    prepareGeometryChange();
    setRect(rect);
    PosOrRectChanged();
}

void ResizeableRectItem::SyncToMapObject()
{
    setRect(mMapObject->XPos(), mMapObject->YPos(), mMapObject->Width(), mMapObject->Height());
}

void ResizeableRectItem::PosOrRectChanged()
{
    QRectF curRect = rect();

    // Sync the model to the graphics item
    mMapObject->SetXPos(static_cast<int>(pos().x() + curRect.x()));
    mMapObject->SetYPos(static_cast<int>(pos().y() + curRect.y()));
    mMapObject->SetWidth(static_cast<int>(curRect.width()));
    mMapObject->SetHeight(static_cast<int>(curRect.height()));

    // Update the property tree view
    mPropSyncer.Sync(this);
}
