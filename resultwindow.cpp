#include "resultwindow.h"
#include "ui_resultwindow.h"
#include "graph.h"
#include <QMessageBox>
#include "matrixwindow.h"
#include <QDesktopWidget>
#include <QTime>
#include <QFileDialog>
#include <QTextStream>
#include <QResizeEvent>

// ����������� ���� �����������
ResultWindow::ResultWindow(QTableWidget* mT, QTableWidget* vT,GraphScene *scn, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ResultWindow)
{
    ui->setupUi(this);
    matrixTable = mT;
    valueTable = vT;

    // ��������� ������ �������
    ui->spinBox->selectAll();
    ui->spinBox->setFocus();

    // ��������� ����� ���� � ������
    this->setWindowTitle("��������� ����������������� ���������");
    this->setWindowIcon(QIcon(":/images/icon.png"));

    scene = scn;

    // ��������� ���� �� ������ ������
    this->setGeometry(this->geometry().x(),this->geometry().y(),this->geometry().width(),130);
    QRect frect = frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(frect.topLeft());

    // �������� �������� ��������
    ui->resultTable->hide();
    ui->label_2->hide();
    ui->lineEdit_2->hide();
    ui->makeScreen->hide();
    ui->saveExcel->hide();
}

void ResultWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

// ������� ������ ����������������� ���������
void ResultWindow::on_startDeal_clicked()
{
    QStringList row,valueList,tableLabels;
    QList<QStringList > matrix;
    // ���������� ������� �������
    int tableSize = matrixTable->columnCount();

    // ���������� ������ �� ������
    for (int i = 0 ; i < tableSize ; i++)
    {
        for (int j = 0 ; j < tableSize ; j++)
        {
            row << matrixTable->item(i,j)->text();
            if (i == 0)
                valueList << valueTable->item(i,j)->text();
        }
        matrix << row;
        row.clear();
        tableLabels << matrixTable->horizontalHeaderItem(i)->text();
    }

    // �������� �����������
    Graph graph(matrix,tableLabels,valueList);

    // ���������� ���������� �����������
    int procCount = ui->spinBox->text().toInt();//ui->lineEdit->text().toInt();

    // ���������� ������������� ���������
    QList<QPair<QString,QString> > *result =  new QList<QPair<QString,QString> >[procCount];
    result =  graph.startDealing(procCount);
    //     graph.printR();
    //     graph.getResult();

    // ����������� ���������� ��������������� �����������
    newProcCount = procCount;
    for (int i = 0 ; i < newProcCount; i++)
    {
        bool isDelete = true;
        for (int j = 0 ; j < result[i].count(); j++)
        {
            if(result[i][j].first != "wait")
            {
                isDelete = false;
                break;
            }
        }
        // �������� ����������������� �����������
        if(isDelete)
        {
            newProcCount --;
            for (int k = i ; k < newProcCount; k++)
                result[k] = result[k+1];
            result[newProcCount].clear();
            i=-1;
        }
    }
    // ��������� ������� �������
    ui->resultTable->setRowCount(newProcCount);
    QStringList headers;
    headers << "���������" << "��������" << "����� ����������";
    // ��������� ���������� �������
    ui->resultTable->setHorizontalHeaderLabels(headers);

    // ������ �� ��������� ��������������
    for (int i = 0 ; i < newProcCount; i++)
    {
        // ��������� ������� ������� �������
        QTableWidgetItem *prName = new QTableWidgetItem("��������� �" + QString::number(i+1));
        ui->resultTable->setItem(i,0,prName);

        int fullTime = 0;
        QString processString;

        // ���������� ������ �� ����� ����������
        for (int j = 0 ; j < result[i].count(); j++)
        {
            QString addDigit = "";
            if(result[i][j].first == "wait")
                addDigit = result[i][j].second;

            processString += result[i][j].first + addDigit + " ";

            fullTime += result[i][j].second.toInt();
        }
        // ������ ������ � ���������� �������
        QTableWidgetItem *prStr = new QTableWidgetItem(processString);
        ui->resultTable->setItem(i,1,prStr);
        QTableWidgetItem *prTime = new QTableWidgetItem(QString::number(fullTime));
        ui->resultTable->setItem(i,2,prTime);

        ui->lineEdit_2->setText(QString::number(fullTime));
    }
    // ������������ �������
    ui->resultTable->resizeColumnsToContents();
    // ����������� ���������� ��������
    ui->resultTable->show();
    ui->label_2->show();
    ui->lineEdit_2->show();

    // ����� ������� ��������� �������
    paint();
    // ���������� ����� ������ ������������ ��������������
//    int dh = 100 / newProcCount;
    //    int dw = 500 / ui->lineEdit_2->text().toInt();
    int dw = 50;
    int dh = 40;



//    // ��������� �������� �� ���
//    for (int y = 100 ; y < 200 - dh/2 ; y+=dh)
//    {
//        //  gra->addLine(endY.x()-5,y,endY.x()+5,y,QPen(Qt::black,2));
//        QGraphicsTextItem* value = new QGraphicsTextItem(QString::number((int)(y-100)/dh + 1));
//        value->setPos(endY.x()-20,y);
//        value->setFont(QFont("Arial",10,QFont::Bold));
//        gra->addItem(value);
//    }

    // ������ �� ���� ���������
    for (int i = 0 ; i < newProcCount; i++)
    {
        int curX = 50;
        for (int j = 0 ; j < result[i].count(); j++)
        {
            // ���������� ���������� ����� ���������� ��������������
            QPoint topleft(curX,i*dh+100);
            // ���������� ������ ��������������
            int rectW = result[i][j].second.toInt()*dw;
            QPoint lowright(curX+=rectW,(i+1)*dh+100);
            int c1 = qrand()%257+0, c2 = qrand()%257+0, c3 = qrand()%257+0;
            // ��������� ����� ��������������
            QBrush brush(QColor(c1,c2,c3));
            QString procName = result[i][j].first;
            if(procName == "wait")
            {
                brush.setColor(Qt::black);
                brush.setStyle(Qt::BDiagPattern);
            }
            // ���������� ������������� �� �����
            gra->addRect(QRectF(topleft,lowright),QPen(Qt::black),brush);
            int length = procName.length();

            // ���������� ������ ����� ��������
            int fontSize = (rectW / length < dh / 2) ? (rectW / length) : dh / 2;

            fontSize = (fontSize < 12) ? (fontSize):(12);
            // ���������� ����� �� �������������
            QGraphicsTextItem* prN = new QGraphicsTextItem(procName);
            prN->setPos(topleft);
            prN->setFont(QFont("Arial",fontSize,QFont::Bold));
            if (procName == "wait")
            prN->setDefaultTextColor(Qt::darkBlue);
            else
                prN->setDefaultTextColor(Qt::white);
            gra->addItem(prN);
        }
    }
    // ������������ ����
    QTime time;
    time.start();
    while (this->geometry().height() < 665)
    {
        this->setGeometry(this->geometry().x(),this->geometry().y() -1,this->geometry().width(),this->geometry().height() +2);
    }
    // ����������� ������ ���������
    ui->makeScreen->show();
    ui->saveExcel->show();
}


// ������� ��������� �������
void ResultWindow::paint()
{
    // �������� ����������� �����
    gra = new QGraphicsScene();
    int sceneWidth = (ui->lineEdit_2->text().toInt()*50+150 > 600 ) ? (ui->lineEdit_2->text().toInt()*50+150):(600);
    int sceneHeigth = (newProcCount*40+200 > 300 ) ? (newProcCount*40+200):(300);

    gra->setSceneRect(0,0,sceneWidth,sceneHeigth);
  //  gra->setSceneRect(0,0,600,300);
    ui->graphicsView->setScene(gra);

    // ��������� ����� �������
    QPoint zero(50,sceneHeigth-50);
    endX.setX(sceneWidth-50); endX.setY(sceneHeigth-50);
    endY.setX(50); endY.setY(50);

    QLine yLine(zero,endY);
    QLine xLine(zero,endX);

    // ���������� �� ������ ����
    gra->addLine(yLine,QPen(Qt::black,2));
    gra->addLine(xLine,QPen(Qt::black,2));
    gra->addLine(endY.x(),endY.y(),endY.x()-5,endY.y()+10,QPen(Qt::black,2));
    gra->addLine(endY.x(),endY.y(),endY.x()+5,endY.y()+10,QPen(Qt::black,2));
    gra->addLine(endX.x(),endX.y(),endX.x()-10,endX.y()+5,QPen(Qt::black,2));
    gra->addLine(endX.x(),endX.y(),endX.x()-10,endX.y()-5,QPen(Qt::black,2));

    gra->setFont(QFont("arial",10));

    // ���������� �� ������ ��������
    QGraphicsTextItem* yName = new QGraphicsTextItem("���������");
    yName->setPos(20,20);
    gra->addItem(yName);

    QGraphicsTextItem* xName = new QGraphicsTextItem("�����");
    xName->setPos(endX.x()-20,endX.y()+20);
    gra->addItem(xName);

    int dw = 50;
    for (int x = 1 ; x <= ui->lineEdit_2->text().toInt() ; x++)
    {
        gra->addLine(50*x + 50,endX.y()-5,50*x + 50,endX.y()+5,QPen(Qt::black,2));
        QGraphicsTextItem* value = new QGraphicsTextItem(QString::number(x));
        value->setPos(50*x + 50,endX.y()+5);
        gra->addItem(value);
    }
    int dh = 40;
    for (int y = 1 ; y <= newProcCount ; y++)
    {
        QGraphicsTextItem* value = new QGraphicsTextItem(QString::number(y));
        value->setPos(endY.x() - 20 , dh*y + 70);
        value->setFont(QFont("Arial",10,QFont::Bold));
        gra->addItem(value);
    }

}

// ������� ���������� ������� ������ ��������� ���� �������
void ResultWindow::on_backButton_clicked()
{
    this->close();
    // �������� ���� �������
    MatrixWindow *matrix = new MatrixWindow(scene);
    // ����������� ����
    matrix->exec();
}

// ������� ���������� ������� ������ ������� ��������
void ResultWindow::on_makeScreen_clicked()
{
    // ���������� ��� ����� �� ��������� �������
    QString fileName = QFileDialog::getSaveFileName(this,"��������� ��������",".png",tr("Png (*.png)"));

//    QTime time;
//    time.start();
//    while (time.elapsed() != 1000);
//    // ������� ������ ����
//    QPixmap pix = QPixmap::grabWindow(QApplication::desktop()->winId(),x()+30,y()+50,width()-30,height()-50);
//    // ��������� ������ � ����
//    pix.save(fileName);

    QPixmap pixmap(gra->width(),gra->height());
    pixmap.fill();
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QRectF rect=gra->itemsBoundingRect();
    rect.adjust(-10,-10,10,10);
    gra->render(&painter,QRectF(),rect);
    painter.end();

    pixmap.save(fileName);
}

ResultWindow::~ResultWindow()
{
    delete ui;
}

void ResultWindow::on_saveExcel_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,"��������� ���...",".xls","��������� �������� (*.xls)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        QMessageBox::warning(this,"������ ���������� ������",file.errorString());
    else
    {
        QByteArray data;
        data.append("���������\t��������\t����� ����������\n");
        for (int i = 0 ; i < ui->resultTable->rowCount(); i++)
        {
            for (int j = 0 ; j < ui->resultTable->columnCount() ; j++)
                data.append(ui->resultTable->item(i,j)->text() + "\t");
            data.append("\n");
        }

        file.write(data);
    }
}

void ResultWindow::resizeEvent(QResizeEvent *e)
{
    // ��������� �������� ������
    ui->resultTable->resize(e->size().width() * 620 / 660 , 170);
    ui->graphicsView->resize(e->size().width() * 620 / 660 ,320);
}
