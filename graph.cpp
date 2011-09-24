#include "graph.h"
#include "math.h"
#include "resultwindow.h"

// ����������� �����������
Graph::Graph(QList<QStringList> m ,QStringList names,QStringList values)
{
    // ������� �������� �������, ���� � ����� ������
    matrix = m;
    vertexNames = names;   
    valueList = values;
}

// ������� ����������������� ���������
QList<QPair<QString,QString> > * Graph::startDealing(int countProc)
{
    // ��������� ���������� �����������
    procCount = countProc;
    // ��������� ������ ��� ������ �����������
    QList<QPair<QString,QString> > *result = new QList<QPair<QString,QString> >[procCount];

    // ���� ������� �������� ��������
    while (matrix.count() != 0)
    {
        // ����� ��� ����� � �������
        QList<QPair<QString,QString> > enters;
        enters = findEnters();

        // ��� ���� ������
        for (int i = 0 ; i < enters.count() ; i++)
        {
            int index = vertexNames.indexOf(enters[i].first);

            // ������� ����� �� �������
            for (int j = 0; j < matrix.count() ; j++)
                matrix[j].removeAt(index);
            matrix.removeAt(index);

            // ������� ����� �� ������� ���� � �����
            vertexNames.removeAt(index);
            valueList.removeAt(index);
        }

        int prC = procCount;
        int lastProc = procCount-1;
        // ���� ���������� ������������� ���������
        for (int i = 0 ; i < procCount-1; i++)
        {
            // ���������� ������������� ������ �� ���������� �����������
            QList<QPair<QString,QString> > toProcessor = dealingProcess(enters,prC);

            // ��������� ���������� �������������
            for (int j = 0; j < toProcessor.count() ; j++)
            {
                // ���������� �������� � ����������
                result[i] << toProcessor[j];
                // �������� �������� �� ������ ������
                enters.removeOne(toProcessor[j]);
            }
            prC--;
            // ���� ������ ������ ����
            if (enters.count() == 0)
            {
                // ��������� ����
                lastProc = i+1;
                break;
            }
        }
        // ��� ���������� ������
        for (int j = 0 ; j < enters.count() ; j++)
        {
            // ��������� ����� ���������� ����������
            result[lastProc] << enters[j];
        }        
        // ���������� ������������� ������� ���������� ���������
        int max=-1;
        for (int i = 0 ; i < procCount; i++)
        {
            int summ = 0 ;
            for (int j = 0 ; j < result[i].count(); j++)
                summ += result[i][j].second.toInt();
            if(max < summ)
                max = summ;
        }

        // ���� ����� ���������� ������ ������������� �� �������� ������� ������� ����������
        for (int i = 0 ; i < procCount; i++)
        {
            int summ = 0 ;
            for (int j = 0 ; j < result[i].count(); j++)
                summ += result[i][j].second.toInt();
            if(summ < max)
            {
                QPair<QString,QString> pair;
                pair.first = "wait";
                pair.second = QString::number(max-summ);
                result[i] << pair;
            }
        }
    }
    // ������� ������ �����������
    return result;
}

// ������� ���������� ������ �������
QList<QPair<QString,QString> > Graph::findEnters()
{
    QList<QPair<QString,QString> > enters;

    // ���������� ������� �������
    int tableSize = matrix.count();

    // ������ �� �������� �������
    for(int j = 0 ; j < tableSize ; j++)
    {
        bool isEnter = true;
        for (int i = 0 ; i < tableSize ; i++)
        {
            // ���� ������ ������� �������� ������� ������ ������� �� �������� ������
            if (matrix[i][j] == "1")
            {
                isEnter = false;
                break;
            }
        }
        // ���� ������� �������� ������
        if (isEnter == true)
        {
            // �������� ������� � ������ ������
            QPair<QString,QString> pair;
            pair.first = vertexNames[j];
            pair.second = valueList[j];
            enters << pair;
        }
    }
    // ������� ����������
    return enters;
}

// ������� ������������� ��������� �� �������� ���������� �����������
QList<QPair<QString,QString> > Graph::dealingProcess(QList<QPair<QString,QString> > procList, int prCount)
{
    // ���������� �������� ������� ���������� ���� ���������
    double t0 = 0;
    for (int i = 0; i < procList.count() ; i++)
        t0 += procList[i].second.toInt();
    t0 /= prCount;

    // ���������� ��������� � ������� ���������� ������� ���������
    for (int i = 0 ; i < procList.count()-1 ; i++)
    {
        if (procList[i].second.toInt() > procList[i+1].second.toInt())
        {
            procList.swap(i,i+1);
            i=-1;
        }
    }

    double ds = 0;
    int index;    
    // ���������� ������� ��� ������ ������ ���������
    for (int i = 0; i < procList.count() ; i++)
    {
        ds += procList[i].second.toInt();
        if (ds > t0)
        {
            index = i;
            break;
        }
    }  
    // �������� �������� ������
    QStringList mLine;
    for (int i = 0; i < procList.count() ; i++)
    {
        if (i == index)
            mLine << "1";
        else
            mLine << "0";
    }
    double min = t0+1;
    QStringList minMLine;
    bool cont = true;
    // ��������� ���������� ������ ���� �� ����� ������� ����� ��������� � ������������
    while(cont)
    {       
        // ���������� ������� �������� � �������� ������� ��������� ���������
        double sd=0;
        for (int i = 0; i < mLine.count() ; i++)
        {
            if (mLine[i] == "1")
                sd += procList[i].second.toInt();
        }
        sd -= t0;

        // ���� ����������� �������� ������ ������ ��������
        if (min >= fabs(sd))
        {
            // ��������� ������������ �������� ����� ��������
            min = fabs(sd);
            // ���������� ����� ����������� �������� ������
            minMLine = mLine;
        }        
        // ���� ������� ����� ���� ������ ���� ������� ��������� ������������� ���������
        if (sd == 0)
        {
            cont = false;
            break;
        }
        // ���� ������� ������ ����
        if (sd > 0)
        {
            // ���� � �������� ������ ���� ������� ������ ������������ ���� �������
            if (mLine.count("1") == 1)
            {
                cont = false;
                break;
            }
            // ����� ���� ����� ����� �������
            else if (mLine.count("1") > 1)
            {               
                // ���� ���������� ������ �������� ������ ������� �� ����� ���������� ������������
                if (mLine.last() == "1")
                {
                    cont = false;
                    break;
                }
                // ����� ���������� �������������� �������� ������
                else
                {
                    // ����� ������ ������ �������
                    int firInd = mLine.indexOf("1");
                    int secInd = mLine.indexOf("1",firInd+1);
                    int thInd = mLine.indexOf("0",secInd+1);
                    secInd = thInd-1;
                    // ��� �������� ������ �� ������� ��������, � ��������� ���������� ������ �������
                    for (int i = 0 ; i <= secInd ; i++)
                    {
                        if (mLine[i] == "1")
                            mLine[i] = "0";
                    }
                    mLine[secInd+1] = "1";                    
                    continue;
                }
            }
        }        
        // ���������� ���������� �������� ������ �� �������
        for (int i = 0 ; i < mLine.count() ; i++)
        {
            if( mLine[i] == "1")
            {
                mLine[i] = "0";
            }
            else
            {
                mLine[i] = "1";
                break;
            }
        }
        // ���� �������� ������ �� �������� ������� ���������� �����
        if (!mLine.contains("1"))
            cont = false;
    }

    // ���������������� ����������� �������� � ����������� ������
    QList<QPair<QString,QString> > result;
    for (int i = 0 ; i < minMLine.count() ; i++)
    {
        // �������� � ������ ����������� �������� ��������������� ������� � ������
        if (minMLine[i] == "1")
            result << procList[i];
    }
    // ���������� �������������
    return result;
}

// ���������� �����������
Graph::~Graph()
{
    matrix.clear();
    vertexNames.clear();
    valueList.clear();
}


