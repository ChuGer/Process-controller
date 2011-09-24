#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QLabel>
#include <QList>
#include <QFileDialog>
#include "mainwindow.h"
#include "matrixwindow.h"
#include "graphscene.h"
#include "edge.h"
#include "graph.h"
#include <math.h>

//����������� �������� ����
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // ����� ������� �������� ����
    createMenus();
    // �������� ����������� �����
    scene = new GraphScene(itemMenu);
    // ������� ����� �������
    scene->setSceneRect(QRectF(0, 0, 2000, 2000));
    connect(scene, SIGNAL(setMoveMode()),this, SLOT(setMoveMode()));
    // �������� ����� � �������
    ui->graphicsView->setScene(scene);

    connect(ui->saveAction,SIGNAL(triggered()),this,SLOT(save()));
    connect(ui->saveAction,SIGNAL(triggered()),this,SLOT(saveAs()));
    ui->saveAsAction->setShortcut(tr("Ctrl+Shift+s"));
    connect(ui->loadAction,SIGNAL(triggered()),this,SLOT(load()));
    connect(ui->newAction,SIGNAL(triggered()),this,SLOT(newFile()));
    connect(ui->exitAction,SIGNAL(triggered()),this,SLOT(exit()));
    connect(ui->aboutAction,SIGNAL(triggered()),this,SLOT(about()));
    ui->exitAction->setShortcut(tr("Alt+F4"));

    // ��������� ��������� ����
    programmTitle = "��������� ���������� �������������";
    // ����� ������� �������� ������ �����
    newFile();

    // ��������� ������� �������� ����
    //setFixedSize(800,600);
    // ��������� �������� ���� �� ������ ������
    QRect frect = frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(frect.topLeft());
}

// ������� ��������� ������� ���������
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

// ������� �������� ���� ���������
void MainWindow::createMenus()
{
    // �������� ������ �������� ��������
    deleteAction = new QAction(QIcon(":/images/delete.png"),"������� �������", this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setToolTip("������� ������� �� �����");
    connect(deleteAction, SIGNAL(triggered()),this, SLOT(deleteButton_clicked()));

    // �������� ������ �������� ���� ���������
    QAction *deleteAllAction = new QAction(QIcon(":/images/deleteAll.png"),"������� ��� ��������", this);
    deleteAllAction->setShortcut(tr("Ctrl+Shift+Delete"));
    deleteAllAction->setToolTip("������� ��� ��������");
    connect(deleteAllAction, SIGNAL(triggered()),this, SLOT(deleteAll()));

    // �������� ������ ������ ���� ���������� �������
    QAction *infoAct = new QAction(QIcon(":/images/info.png"),"�������� ����������", this);
    infoAct->setShortcut(tr("Ctrl+i"));
    connect(infoAct, SIGNAL(triggered()),this, SLOT(infoExec()));

    // �������� ������������ ���� �������
    itemMenu = new QMenu("���� �������");
    // ���������� � ����������� ���� ������ ������� � �������� ����������
    itemMenu->addAction(deleteAction);
    itemMenu->addAction(infoAct);

    // �������� ������ ��������� �������
    viewMatrixAction = new QAction(QIcon(":/images/next.png"),"����������� �������", this);
    viewMatrixAction->setShortcut(tr("Ctrl+m"));
    viewMatrixAction->setToolTip("������� ������� �� �����");
    connect(viewMatrixAction, SIGNAL(triggered()),this, SLOT(viewMatrix_clicked()));

    // �������� ������ ���������� ��������� �����
    addArcAction = new QAction(QIcon(":/images/arc.png"),"�������� ����", this);
    addArcAction->setShortcut(tr("Ctrl+e"));
    addArcAction->setCheckable(true);
    addArcAction->setToolTip("������� ������� �� �����");
    connect(addArcAction, SIGNAL(triggered()),this, SLOT(addArc_clicked()));

    // ���������� � ������ ������ ����������, ��������, ���������
    ui->mainToolBar->addAction(addArcAction);
    ui->mainToolBar->addAction(deleteAction);
    ui->mainToolBar->addAction(deleteAllAction);
    ui->mainToolBar->addAction(viewMatrixAction);
    // ��������� ������ �������
    ui->mainToolBar->setFont(QFont("Arial Narrow", 10));  
}

// ������� ��������� ������� ������ �������� ������ �����
void MainWindow::newFile()
{
    // ������ �����
    scene->clear();
    // ������� ������ ���� ������
    scene->allVertexes.clear();
    scene->setVertexCount(1);
    setCurrentFileName("");
}

// ������ ���������� �������� �����
bool MainWindow::save()
{
    // ���� � ����� ����������� ���
    if (scene->getFileName().isEmpty())
    {
        // ������� ������� ��������� ���
        return saveAs();
    }
    // �������� ����� � ���������� ������
    QFile file(scene->getFileName());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        QMessageBox::warning(this,"������ ���������� �����",file.errorString());
    else
    {
        // ������� �����
        file.remove(scene->getFileName());
        // ����� ������� ����������
        scene->save(&file);
        scene->setModified(false);
        return true;
    }
    return false;
}

// ������� ���������� ����� � ����� ������
bool MainWindow::saveAs()
{
    // ��������� ����� ����� �� ��������� �������
    QString fileName = QFileDialog::getSaveFileName(this,"��������� ���...",".ctrl","���� ���������� (*.ctrl)");
    if (fileName.isEmpty())
        return false;

    // �������� �����
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this,"������ ���������� �����",file.errorString());
    }
    else
    {
        // ����� ������� ���������� �����
        if(scene->save(&file))
        {
            // ����� ������� ��������� ����� �����
            setCurrentFileName(fileName);
            scene->setModified(false);
            return true;
        }
        // �������� �����
        file.close();
    }
    return false;
}

// ������� �������� ����� �� �����
bool MainWindow::load()
{
    // ��������� ����� ������������ �����
    QString fileName =  QFileDialog::getOpenFileName(this,"������� ����",".ctrl","���� ���������� (*.ctrl)");
    QFile file(fileName);
    // �������� �����
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,"������ �������� �����",file.errorString());
    }
    else
    {
        // �������� ������ �����
        newFile();
        // ����� ������� �������� �����
        scene->load(&file);
        // ��������� ����� �����
        setCurrentFileName(fileName);
        // ����� ������� ������������� �����
        centerScrollBar();
        return true;
    }
    return false;
}

// ������� ������������ ��������� ���������
bool MainWindow::maybeSave()
{
    // ���� ����� �� ���� �������� ���������
    if (scene->getModified() == false)
        return true;
    if (scene->getFileName().startsWith(QLatin1String(":/")))
        return true;

    // �������� ���� ���������
    QMessageBox saveBox(this);
    saveBox.setIcon(QMessageBox::Information);
    saveBox.setWindowTitle(programmTitle);
    saveBox.setText("��������� ���������?");
    QPushButton *saveButton = saveBox.addButton("���������",QMessageBox::ActionRole);
    QPushButton *saveAsButton = saveBox.addButton("��������� ���",QMessageBox::ActionRole);
    saveBox.addButton("�� ���������",QMessageBox::ActionRole);
    QPushButton *abortButton = saveBox.addButton("������",QMessageBox::ActionRole);
    saveBox.exec();

    // ���������� ������� ������� �� ������������ ������
    if (saveBox.clickedButton() == saveButton)
        save();
    if (saveBox.clickedButton() == saveAsButton)
        saveAs();
    else if (saveBox.clickedButton() == abortButton)
        return false;

    return true;
}

// ������� ��������� ����� �����
void MainWindow::setCurrentFileName(const QString &fileName)
{
    // ��������� ����� �����
    scene->setFileName(fileName);
    QString shownName;
    // ���������� ������� ���������� ��������� ������
    static int sequenceNumber = 1;

    // ���� � ����� ��� ����� ������� ������������ ������ ����������
    if (scene->getFileName().isEmpty())
        shownName = QString("%2 | ���������� %1.ctrl").arg(sequenceNumber++).arg(programmTitle);
    // ����� ������� ������ ��� �����
    else
        shownName = programmTitle + " | " + QFileInfo(scene->getFileName()).fileName();

    // ��������� ����� ����
    setWindowTitle(shownName);
    scene->setModified(false);
}

// ������� ���������� ������� ������ �������� ����
void MainWindow::addArc_clicked()
{
    // ���� ������ ������  ������������� ����� � ����� ���������� ����
    if (addArcAction->isChecked())
        scene->setMode(GraphScene::AddArc);
    // ����� ������������� ����� � ����� �����������
    else
        scene->setMode(GraphScene::MoveMode);
}

// ������� ���������� ������� ������ �������
void MainWindow::deleteButton_clicked()
{
    // ��� ������� ������� �� �����
    foreach (QGraphicsItem *item, scene->selectedItems())
    {
        // ���� ��� ������� - �������
        if (item->type() == Vertex::Type)
        {
            // ���������� �������������� ����� ���������
            Vertex *curItem = qgraphicsitem_cast<Vertex *>(item);
            // ��� ������� ����� �� ������ ����� ������� �������
            foreach(Edge* edge, curItem->edges)
            {
                // ������� ����� �� ������ ����� �������� �������
                edge->endItem()->edges.removeOne(edge);
                // ������� ����� �� �����
                scene->removeItem(edge);
            }
            // ������� ������� �� ������ ������
            scene->allVertexes.removeOne(curItem);
        }
        // ���� ��� ������� - �����
        if (item->type() == Edge::Type)
        {
            // ���������� �������������� ����� ���������
            Edge *edge = qgraphicsitem_cast<Edge *>(item);
            // �� ������ ����� ��������� � �������� ������� ������� �����
            edge->startItem()->removeArrow(edge);
            edge->endItem()->removeArrow(edge);
        }
        // ������� ����� �� �����
        scene->removeItem(item);
    }
    // ����� ����������
    scene->setModified(true);
    // ���� ������ ������ ����
    if(scene->allVertexes.isEmpty())
    {
        // ������������� � 1 ������� ������
        scene->setVertexCount(1);
    }
}

// ������� �������� ���� �������� �� �����
void MainWindow::deleteAll()
{
    // ������� �����
    scene->clear();
    // ��������� �������� ������ � �������
    scene->setVertexCount(1);
    if (!scene->allVertexes.isEmpty())
    {
        // ������� ������ ���� ������
        scene->allVertexes.clear();
    }
    // ���������� �����
    scene->update();
    // ������� ���������� ����������� �����
    scene->setModified(false);
}


// ������� ���������� ������� ������ ��������� �������
void MainWindow::viewMatrix_clicked()
{
    // ���� �� ����� ��� �������� ������ ���������
    if (scene->allVertexes.isEmpty())
    {
        QMessageBox::warning(this,"������","�������� ��������");
    }
    // ����� ������� � ��������� ���� ����������� �������
    else
    {
        MatrixWindow *matrix = new MatrixWindow(scene);
        matrix->exec();
    }
}

// ������� ������������� �����
void MainWindow::centerScrollBar()
{
    // ��������� ������ � ������ �������
    int w = ui->graphicsView->width(), h = ui->graphicsView->height();
    // ��������� �������������� ��������������� ��� �������
    QRectF  itemsRect = scene->itemsBoundingRect();
    // ��������� ���������� �� ������ ����
    QScrollBar *vBar = ui->graphicsView->verticalScrollBar(), *hBar = ui->graphicsView->horizontalScrollBar();
    // ���������� ��������� ������ �����
    vBar->setValue(itemsRect.top() + itemsRect.height()/2 - h/2);
    hBar->setValue(itemsRect.left() + itemsRect.width()/2 - w/2);
}

// ������� ��������� ��������� �����
void MainWindow::setMoveMode()
{
    // ���� ���� ������ ������ ���������� ����
    if (addArcAction->isChecked())
    {
        // ������ ������
        addArcAction->setChecked(false);
        // ���������� ��������� ����� � ����� �����������
        scene->setMode(GraphScene::MoveMode);
    }
    else
    {
        // ����� ������ �� ������
        addArcAction->setChecked(true);
        // �������� ��������� �����
        scene->setMode(GraphScene::AddArc);
    }
}

// ������� �������� �������� ����
void MainWindow::exit()
{
    close();
}

// ������� ������ ���� ���������� ������
void MainWindow::infoExec()
{
    scene->infoExec();
}

// ������� ������ ���� �� ��������
void MainWindow::about()
{
    // �������� ����
    QMessageBox msgBox(QMessageBox::Information,QString("� ���������..."),
                       QString("\n\n��������� ������������� ��� ����������������� ��������� � ������� ���������� �������������."));
    msgBox.setIconPixmap(QPixmap(":/images/icon.png"));
    msgBox.setFont(QFont("Arial",12));
    // ������ ����
    msgBox.exec();
}


// ������� ���������� ������� �������� ����
void MainWindow::closeEvent(QCloseEvent *event)
{
    // ���� �������� �� ���������� ��������� ��������� �������
    if (maybeSave())
        event->accept();
    // ����� ���������
    else
        event->ignore();
}

// ���������� ����
MainWindow::~MainWindow()
{
    delete ui;
}
