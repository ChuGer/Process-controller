#include "infowindow.h"
#include "ui_infowindow.h"
#include "graphscene.h"
#include <QtGui/QKeyEvent>
#include <QMessageBox>

// ����������� ���� ����������
InfoWindow::InfoWindow(GraphScene *inScene,QWidget *parent) :
        QDialog(parent),
        ui(new Ui::InfoWindow)
{
    ui->setupUi(this);

    scene = inScene;
    connect(ui->lineName,SIGNAL(textChanged(QString)),this,SLOT(nameChanged(QString)));

    this->setWindowTitle("��������� ����������������� ���������");
    this->setWindowIcon(QIcon(":/images/icon.png"));
}

void InfoWindow::changeEvent(QEvent *e)
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

// ������� ��������� ����� ��������
void InfoWindow::setName(QString nm)
{
    // ������ ������� � ������ ��������������� ������
    ui->lineName->setText(nm);
    name = nm;
}

// ������� ��������� ���� ��������
void InfoWindow::setValue(QString vl)
{
    // ���������� ������� ��������������� ��������
    ui->lineValue->setValue(vl.toInt());
    value = vl;
    // �������� ����� � ������� � �������������
    ui->lineValue->selectAll();
    ui->lineValue->setFocus();
}


// ������� ���������� ������� ������ � ������ ��������
void InfoWindow::nameChanged(QString newName)
{
    bool isCopyName = false;
    // ��� ������ ������� �� �����
    foreach(Vertex *vertex, scene->allVertexes)
    {
        // ���� ��� ������� ��������� � ������ ����� �������
        if (vertex->getName() == newName && !scene->selectedItems().contains(vertex))
        {
            // ���������� �������� ���������� �������� �������
            isCopyName = true;
            break;
        }
    }
    // �������� ����� �������
    QFont font = ui->lineName->font();
    QPalette palette;
    // ���� ��� ����������� ������� ����� ������� ����� ������
    if (isCopyName)
        palette.setColor(QPalette::Text,Qt::red);
    else
        palette.setColor(QPalette::Text,Qt::black);

    // ���������� ������� �� ������
    ui->lineName->setPalette(palette);
}

// ������� ���������� ������� ������ ���������
void InfoWindow::on_addVertexButton_clicked()
{
    // ���� ���� �� ����� �� ��������� ������� ��������������
    if (ui->lineName->text() == "" || ui->lineValue->text() =="")
        QMessageBox::warning(this,"������","���� �� ����� �� ���������");
    // ����� ���� ��� �������� �� ����������� ������� ������� ���������� ������
    else if (ui->lineName->palette().color(QPalette::Text) != Qt::red)
    {
        emit infoUpdate(ui->lineName->text(),ui->lineValue->text());
        this->close();
    }
    // ����� ������� ��������������� ���������
    else
    {
        QMessageBox::warning(this,"������","������� � �������� ������ ��� ����������");
    }
}

// ������� ���������� ������� ������ ������
void InfoWindow::on_canselButton_clicked()
{
    this->close();
}

// ���������� ����
InfoWindow::~InfoWindow()
{
    delete ui;
}
