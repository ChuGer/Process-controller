#include <QtGui>
#include "graphscene.h"
#include "edge.h"
#include "mainwindow.h"
#include "infowindow.h"

// ����������� ����������� �����
GraphScene::GraphScene(QMenu *itemMenu, QObject *parent) : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    // ��������� ��������� ����������� ��������
    myMode = MoveMode;
    // ��������� �������� ������ � �������
    vertexCount = 1;
    line = 0;
    isModified = false;
    // ��������� �������� ���������� �������� ��� ��������� �������
    lastValue = 1;
    // ��������� ������ ��� ���� ����������
    infoW = new InfoWindow(this);
    connect(infoW,SIGNAL(infoUpdate(QString,QString)),this,SLOT(infoUpdate(QString,QString)));
    // ��������� �������� �����
    thisParent = parent;
}

// ������� - ���������� ������� ������� ������ ����
void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // ���� ���� ������ �� ����� ������ ����
    if (mouseEvent->button() != Qt::LeftButton)
    {
        // ���� ���� ������ ������� ������ ����
        if (mouseEvent->button() == Qt::MidButton)
            // ����� ������� ��������� ������ �����
            emit setMoveMode();
        return;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);

    // ���� ����� ��������� � ��������� ���������� ���
    if(myMode == GraphScene::AddArc)
    {
        // �������� ������ ��� �����
        line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),mouseEvent->scenePos()));
        // ���������� �������� ��������� �����
        line->setPen(QPen(Qt::black, 1,Qt::DashLine));
        // �������� ����� �� �����
        addItem(line);
    }
    // ���� ����� ��������� � ������ ����������� �������� � ���������� �������� ���
    else if (myMode == GraphScene::MoveMode && selectedItems().isEmpty())
    {        
        // ���������� ����� ����� � ��������� ��������� ��������
        setMode(GraphScene::SelectMode);
        selectP = mouseEvent->scenePos();
        // �������� ������ ��� ��������������
        selectRect = new QGraphicsRectItem(QRectF(selectP,mouseEvent->scenePos()));
        // �������� ������������� �� �����
        addItem(selectRect);
    }
    moveBug = false;
}

// ���������� ������� ����������� ����
void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (moveBug)
        return;
    // ���� ����� ��������� � ��������� ����������� ��������
    if (myMode == GraphScene::MoveMode && !selectedItems().isEmpty())
    {
        // ��������� ���������� ��������� ������� ����
        int dx = mouseEvent->scenePos().x()-mouseEvent->lastScenePos().x();
        int dy = mouseEvent->scenePos().y()-mouseEvent->lastScenePos().y();
        // ���� ������� ����� ���������� �� ������� �����  - ���������
        if (itemsBoundingRect().left()+dx < sceneRect().left() || itemsBoundingRect().right()+dx > sceneRect().right() ||
            itemsBoundingRect().y()+dy < sceneRect().top() || itemsBoundingRect().bottom()+dy > sceneRect().bottom())
            return;
        // ��� ������� ����������� �������
        foreach(QGraphicsItem* selItem,selectedItems()){
            if (selItem->type() == Vertex::Type )
            {
                Vertex *item = qgraphicsitem_cast<Vertex *>(selItem);
                // ����������� ������ �� ������������ ����������
                item->moveBy(dx,dy);
            }
        }
    }

    // ����� ���� ����� � ������ ���������� ����
    else if (myMode == GraphScene::AddArc && line != 0)
    {
        // �������� �������� �����
        QLineF newLine(QPointF(line->line().p1().x(),line->line().p1().y()),
                       QPointF(mouseEvent->scenePos().x(),mouseEvent->scenePos().y()));
        line->setLine(newLine);
    }
    // ����� ���� ����� � ������ ���������
    else if (myMode == GraphScene::SelectMode)
    {
        // ������� ������ ������������� �� �����
        removeItem(selectRect);
        // ������� �����
        selectRect = new QGraphicsRectItem(QRectF(selectP,mouseEvent->scenePos()));
        selectRect->setZValue(300);
        // ���������� �������������� ����
        selectRect->setBrush(QColor(177, 235, 230, 100));
        // ��� ������� ������� �����
        foreach(QGraphicsItem* item,items())
            // ���� ������ ������������ � ��������������� - �������� ������
            if (item->collidesWithItem(selectRect))
                item->setSelected(true);
        // ����� - �������� ��������
        else
            item->setSelected(false);
        // �������� ������������� �� �����
        addItem(selectRect);
    }
    // �������� �����
    update();
}

// ������� ���������� ������� ���������� ������ ����
void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    moveBug = true;
    // ���� ����� � ������ ���������� ����
    if (myMode == GraphScene::AddArc && line!=0)
    {        
        // �������� ������ �������� ��� ��������� ������� �����
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        
        // ���� ������ �� �����
        if (!startItems.isEmpty() && !endItems.isEmpty())
        {
            // ���� ������ � ������ ������ ���� �������
            if (startItems.first()->type() == Vertex::Type && endItems.first()->type() == Vertex::Type)
            {
                // ���������� �������������� ��������
                Vertex *startItem = qgraphicsitem_cast<Vertex *>(startItems.first());
                Vertex *endItem = qgraphicsitem_cast<Vertex *>(endItems.first());
                
                // ���������� �������� ����� �� �����
                if (checkCycle(endItem, startItem))
                {
                    QWidget* w = qobject_cast<QWidget*>(parent());
                    QMessageBox::warning(w,"������","�������� ������ ����������.");
                }
                // ���� ������ ��� � ��������� ������� �� ��������� � ��������
                else if(startItem!=endItem)
                {
                    // ������� ����� ����
                    Edge *edge = new Edge(startItem,endItem);
                    edge->setZValue(-100.0);
                    // �������� ���� �� �����
                    addItem(edge);
                    // �������� ������� ����
                    edge->updatePosition();
                    isModified = true;
                }
            }
        }
        // ������� ����� �� �����
        removeItem(line);
    }
    // ���� ����� � ������ ���������
    else if (myMode == GraphScene::SelectMode)
    {
        // ������� ������������� ���������
        removeItem(selectRect);
        // ���������� ����� ����� � ����� �����������
        setMode(GraphScene::MoveMode);
    }
    // ��� ���� �������� ��� �� ����� �������� �������
    foreach(QGraphicsItem* selItem,items())
    {
        if(selItem->type() == Edge::Type)
        {
            Edge *item = qgraphicsitem_cast<Edge *>(selItem);
            item->updatePosition();
        }
    }
    // ����� ��������� ��������
    selectedItems().clear();
    // �������� �����
    update();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

// ������� ���������� ������� �������� ����� ����
void GraphScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    line = 0;
    // ���� ������� ���� ��� �������� �� �������
    if (!selectedItems().isEmpty())
    {
        // �������� ���� ������ ����������
        infoW->move(mouseEvent->screenPos().x()-100,mouseEvent->screenPos().y()-200);
        infoExec();        
    }
    // �����
    else
    {
        QString lastText = QString::number(lastValue);
        QString name = "process"+QString::number(vertexCount++);
        
        bool isCopy = false;
        do
        {
            foreach(Vertex* v, allVertexes)
            {
                if (v->getName() == name)
                {
                    isCopy = true;
                    vertexCount++;
                    name = "process"+QString::number(vertexCount++);
                    break;
                }
                else
                {
                    isCopy = false;
                }
            }
        }
        while (isCopy);

        // ������� ���� ������ ���������� �������
        infoW->setName(name);
        infoW->setValue(lastText);
        infoW->move(mouseEvent->screenPos().x()-100,mouseEvent->screenPos().y()-200);
        infoW->exec();

        // ���� ��� � ��� �����
        if (vName != " " && vValue != "")
        {
            lastValue = vValue.toInt();
            // ������� ����� �������
            Vertex *vertex = new Vertex(myItemMenu);
            vertex->setPos(mouseEvent->scenePos());
            vertex->setZValue(200);
            // ���������� ������� ���������
            vertex->setName(vName);
            vertex->setValue(vValue);
            vName = vValue = "";
            // �������� ������� �� �����
            addItem(vertex);
            // �������� ������� � ������ ���� ������ �����
            allVertexes.append(vertex);
            isModified = true;
        }
    }
}

// ������� ������ ���� ����������
void GraphScene::infoExec()
{
    if (!selectedItems().isEmpty())
    {
        if(selectedItems().first()->type() == Vertex::Type)
        {
            Vertex *vertex = qgraphicsitem_cast<Vertex *>(selectedItems().first());
            infoW->setName(vertex->getName());
            infoW->setValue(vertex->getValue());
            infoW->exec();

            if (vName != " " && vValue != "")
            {
                lastValue = vValue.toInt();
                vertex->setName(vName);
                vertex->setValue(vValue);
                vName = vValue = "";
                isModified = true;
            }
        }
    }
}

// ������� ���������� ���������� �������
void GraphScene::infoUpdate(QString name,QString value)
{
    vName = name;
    vValue = value;
}

// ������� �������� ����� �� �����
bool GraphScene::checkCycle(Vertex *startV, Vertex *endV)
{
    // ��� ������� ����� �������� �������
    foreach(Edge* edge, endV->edges)
    {
        if (edge->endItem() == endV)
        {
            // ����� ��� �������� ���� � ������ �������
            QList<QStringList> result = getInputPath(endV);
            // �������� ���� �������� �����
            for (int i = 0 ; i < result.count() ; i++)
            {
                for (int j = 0 ; j < result[i].count() ; j++)
                {
                    // ���� ��������� ���� ��������� ������� ���� ������
                    if (result[i][j] == startV->getName())
                        return true;
                }
            }
        }
    }
    return false;
}

// ������� ������������ ��� �������� ���� � �������
QList<QStringList> GraphScene::getInputPath(Vertex* vertex)
{
    QList<QStringList> returnList;

    // ��� ������� ����� �� ������ ����� ������� �������
    foreach(Edge* edge, vertex->edges)
    {
        QStringList list;
        if (edge->endItem() == vertex)
        {
            // ����������� ��������� �������� ��� � ��������� �������
            QList<QStringList>  appList = getInputPath(edge->startItem());
            // ���� ���� �� �������
            if (appList.isEmpty())
            {
                // ���������� � ������ �������
                list << edge->startItem()->getName();
                appList << list;
            }
            // �����
            else
            {
                // ���������� � ������ ���������� ������
                for (int i = 0 ; i < appList.count() ; i++)
                {
                    appList[i].push_front(edge->startItem()->getName());
                }
            }
            // ����������� � ������������� ������ ��������������� ������
            returnList += appList;
        }
    }
    // ����������� ����������
    return returnList;
}

// ������� ���������� ���� � ����
bool GraphScene::save(QFile *file)
{
    // �������� �������� � ����
    QXmlStreamWriter xmlWriter(file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeComment("Filesave Controller");
    xmlWriter.writeStartElement("doc");
    // ��� ������ ������� �� ������ ������
    foreach(Vertex* item, allVertexes)
    {
        // ������ ���� �������
        xmlWriter.writeStartElement("Item");
        xmlWriter.writeAttribute("Type",QString::number(item->type()));
        // ������ ��������� �������
        xmlWriter.writeEmptyElement("ItemPos");
        xmlWriter.writeAttribute("x",QString::number(item->pos().x()));
        xmlWriter.writeAttribute("y",QString::number(item->pos().y()));
        xmlWriter.writeAttribute("z",QString::number(item->zValue()));
        // ������ ����� � ���� �������
        xmlWriter.writeEmptyElement("ItemName");
        xmlWriter.writeAttribute("name",item->getName());
        xmlWriter.writeAttribute("value",item->getValue());
        xmlWriter.writeEndElement();
    }
    // ��� ������ ���� ������ �������� ���������� � ����
    foreach(QGraphicsItem* selItem, items())
    {
        if (selItem->type() == Edge::Type)
        {
            Edge *item = qgraphicsitem_cast<Edge *>(selItem);

            xmlWriter.writeStartElement("Item");
            xmlWriter.writeAttribute("Type",QString::number(item->type()));
            xmlWriter.writeEmptyElement("ItemPos");
            xmlWriter.writeAttribute("x",QString::number(item->pos().x()));
            xmlWriter.writeAttribute("y",QString::number(item->pos().y()));
            xmlWriter.writeAttribute("z",QString::number(item->zValue()));
            xmlWriter.writeEmptyElement("ItemName");
            xmlWriter.writeAttribute("start_name",item->startItem()->getName());
            xmlWriter.writeAttribute("end_name",item->endItem()->getName());
            xmlWriter.writeEndElement();
        }
    }
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    return true;
}

// ������� �������� ����� �� �����
bool GraphScene::load(QFile *file)
{
    // �������� ����������� ������
    QXmlStreamReader xmlReader(file);
    int ItemType = 0;
    QPointF mPos;
    qreal z;
    Vertex *insertedItem = 0,*startItm, *endItm;
    Edge  * insertedArrow = 0;

    // ���� �� ���������� ����
    while(!xmlReader.atEnd()){
        xmlReader.readNext();
        if(xmlReader.isStartDocument()) continue;
        if(xmlReader.isStartElement()){
            if(xmlReader.name()=="doc") continue;
            if(!ItemType and (xmlReader.name()=="Item")){
                bool ok;
                // ���������� ���� �������
                ItemType = xmlReader.attributes().value("Type").toString().toInt(&ok);
                if(!ok)
                {
                    xmlReader.raiseError(tr("Item: type number conversion failed"));
                }
                continue;
            }
            // ���������� ��������� �������
            if(ItemType and (xmlReader.name()=="ItemPos")){
                bool ok,okay;
                okay=true;
                mPos.setX(xmlReader.attributes().value("x").toString().toDouble(&ok));
                okay&=ok;
                mPos.setY(xmlReader.attributes().value("y").toString().toDouble(&ok));
                okay&=ok;
                z=xmlReader.attributes().value("z").toString().toDouble(&ok);
                okay&=ok;
                if(!okay)
                {
                    xmlReader.raiseError(tr("Pos: number conversion failed"));
                }
                continue;
            }
            if(ItemType and (xmlReader.name()=="ItemName")){
                // � ����������� �� ���� �������
                switch (ItemType) {
                case QGraphicsItem::UserType+69:
                    {
                        // �������� �������
                        insertedItem = new Vertex(myItemMenu);
                        insertedItem->setPos(mPos);
                        insertedItem->setZValue(z);

                        QString name  = xmlReader.attributes().value("name").toString();
                        QString value = xmlReader.attributes().value("value").toString();
                        insertedItem->setName(name);
                        insertedItem->setValue(value);
                        insertedItem->update();
                        // ���������� �� �����
                        addItem(insertedItem);
                        allVertexes.append(insertedItem);

                        break;
                    }
                case QGraphicsItem::UserType+4:
                    {
                        // ���������� ����� ��������� � �������� �������
                        QString start = xmlReader.attributes().value("start_name").toString();
                        QString end = xmlReader.attributes().value("end_name").toString();

                        bool isStFound =false,isEndFound = false;
                        // ������� ��� �������
                        foreach(QGraphicsItem* selItem, items())
                        {
                            if(selItem->type() == Vertex::Type)
                            {
                                Vertex *item = qgraphicsitem_cast<Vertex *>(selItem);
                                // ������� ��������� �������
                                if (item->getName() == start)
                                {
                                    startItm = item;
                                    isStFound = true;
                                }
                                // � �������� �������
                                if(item->getName() == end)
                                {
                                    endItm = item;
                                    isEndFound = true;
                                }
                            }
                        }
                        // ���� ������� ��������� � �������� �������
                        if (isStFound && isEndFound)
                        {
                            // ������� ����� ����
                            insertedArrow = new Edge(startItm, endItm);
                            insertedArrow->setZValue(z);
                            // ��������� ���� �� ����� � ��������� �������
                            addItem(insertedArrow);
                            insertedArrow->updatePosition();
                        }
                        else
                        {
                            QMessageBox::warning(0,"������ ��������","����� ����� �� ����� ���� ���������.");
                            return false;
                        }
                        break;
                    }
                default:
                    break;
                }
                continue;
            }
            continue;
        }
        if(xmlReader.isEndElement()){
            if(ItemType and (xmlReader.name()=="Item")){
                ItemType = 0;
                insertedItem = 0;
                insertedArrow = 0;
            }
        }
    }

    insertedItem = 0;
    insertedArrow = 0;
    setMode(MoveMode);
    update();
    return true;
}


