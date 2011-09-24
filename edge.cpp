#include <QtGui>
#include "edge.h"
#include <math.h>

// ����������� ����
Edge::Edge(Vertex *startItem, Vertex *endItem,QGraphicsItem *parent, QGraphicsScene *scene): QGraphicsLineItem(parent, scene)
{
    // ��������� � �������� �������
    myStartItem = startItem;
    myEndItem = endItem;

    // ���������� ���� � ��������� � �������� �������
    startItem->addArrow(this);
    endItem->addArrow(this);

    // ��������� ����������� ��������� ����
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

// ������� ������������ ������������� ���������
QRectF Edge::boundingRect() const
{
    qreal extra = 20;
    // ������� �������������� ����� ���������
    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),line().p2().y() - line().p1().y()))
    .normalized().adjusted(-extra, -extra, extra, extra);
}

// ������� ������������ ������� �������������� � �����
QPainterPath Edge::shape() const
{
    // �������� ����� �����
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(poly);
    // ��������� � ����� ��������� ����
    path.addPolygon(arrowHead);
    return path;
}

// ������� �������� ����
void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                 QWidget *)
{
    // ��� ����������� ������ ���� �� ��������������
    if (myStartItem->collidesWithItem(myEndItem))
        return;

    // ��������� ������� ����� ����
    qreal arrowSize = 15;
    painter->setRenderHint(QPainter::Antialiasing);

    // ���� ���� ��������
    if (isSelected())
    {
        // �������� ���� ���������
        painter->setPen(QPen(Qt::lightGray,2));
        painter->setBrush(Qt::lightGray);
    }
    // �����
    else
    {
        // ���������� ����������� ���� ���������
        painter->setPen(QPen(Qt::black,2));
        painter->setBrush(Qt::black);
    }

    QLineF centerLine(myStartItem->pos(), myEndItem->pos());
    // ����������� ����� �������� �������
    QPolygonF endPolygon = myEndItem->boundingRect();
    // ������� ����� ��������������� ���� � �������� �������
    QPointF p1 = endPolygon.first() + myEndItem->pos();
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    // ������������ ���� �� �������� �������� �������
    for (int i = 1; i < endPolygon.count(); ++i)
    {
        p2 = endPolygon.at(i) + myEndItem->pos();
        polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType =
                polyLine.intersect(centerLine, &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
            break;
        p1 = p2;
    }
    // ��������� ����� �����
    setLine(QLineF(intersectPoint, myStartItem->pos()));

    // ������������ ���� ������� �����
    const qreal Pi = 3.14;
    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

    // ���������� ����� ����� ����
    QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                            cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                            cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    // �������� ����� ����
    arrowHead << line().p1() << arrowP1 << arrowP2;
    // ��������� �����
    painter->drawLine(line());
    // ��������� ����� ����
    painter->drawPolygon(arrowHead);
}

// ������� ���������� ��������� ����
void Edge::updatePosition()
{
    // ��������� ����� ����� � ����������� �� ��������� ��������
    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
    setLine(line);
}
