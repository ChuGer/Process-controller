#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTextCodec>

// ������� ������� ���������
int main(int argc, char *argv[])
{
    // �������� ����������
    QApplication a(argc, argv);
    // ����������� ��������
    Q_INIT_RESOURCE(resource);
    // ��������� ���������� ������ ��� ���������
    QTextCodec *codec = QTextCodec::codecForName("CP1251");
    QTextCodec::setCodecForCStrings(codec);
    // �������� �������� ����
    MainWindow w;
    // ����������� �������� ����
    w.show();
    return a.exec();
}
