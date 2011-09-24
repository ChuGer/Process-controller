#include <QtGui>
#include "edge.h"
#include "vertex.h"
#include "math.h"
#include "graphscene.h"
#include "mainwindow.h"

// ����������� �������
Vertex::Vertex(QMenu *contextMenu, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsItem(parent, scene)
{
    // ��������� ������ ����������� � ��������� �������
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myContextMenu = contextMenu;
    name="";
}

// �������, ������������ �������������� ������������� ��� ���������
QRectF Vertex::boundingRect() const
{
    return QRectF(-maxWidth/2,-12,maxWidth,maxWidth);
}

// ������� ��������� �������
void Vertex::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // ���� ������� �������� ���������� ������ �����
    if (isSelected())
    {
        painter->setPen(Qt::lightGray);
        painter->setBrush(Qt::lightGray);
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    double nameWidth = name.length() * 8;
    double valueWidth = value.length() * 8;

    // ���������� ����������� ������ �������
    maxWidth = (nameWidth > valueWidth) ? nameWidth : valueWidth;
    maxWidth = (maxWidth > 40) ? maxWidth : 40;

    //maxWidth += 20;

    // ���� ������� �� �������� ��������� ����� � ���������
    if (!isSelected())
    {
        painter->setPen(Qt::black);
        painter->setBrush(Qt::black);
    }
    // ��������� �������� ��������
    painter->drawRoundedRect(QRectF(-maxWidth/2,-12,maxWidth,maxWidth),5,5);

    if (!isSelected())
    {
        painter->setPen(Qt::blue);
        painter->setBrush(Qt::blue);
    }
    // ��������� �������� ��������������
    painter->drawRoundedRect(QRectF(-maxWidth/2,-12,maxWidth,24),5,5);

    // ��������� ����� ���������
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial",8,QFont::Bold));
    // ��������� ����� �� �������
    painter->drawText(QPoint(-nameWidth  *.75 / 2  , 5),name);
    painter->setFont(QFont("Arial",11,QFont::Bold));
    // ��������� ���� �� �������
    painter->drawText(QPoint(-valueWidth *.8 / 2  , maxWidth/2 + 5),value);
}

// ������� ���������� ���� � ������ ���
void Vertex::addArrow(Edge *edge)
{
    edges.append(edge);
}

// ������� �������� ���� �� ������ ��� �������
void Vertex::removeArrow(Edge *edge)
{
    int index = edges.indexOf(edge);
    // �������� ���� �� ������
    if (index != -1)
        edges.removeAt(index);
}

// ������� ���������� ������� ������������ ����
void Vertex::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}


