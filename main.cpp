
#include "main.h"
#include "mainwidget.h"
#include "model.h"
#include <QApplication>
#include <QDateTime>
#ifdef MYTEST
void semmi(){}

#else


int main(int argc, char *argv[]){
	srand(QDateTime::currentDateTime().currentMSecsSinceEpoch());
    QApplication a(argc, argv);
    double T = 0.16;
    int minSize = 6;
    MainWidget w(minSize, T);
    w.show();

    return a.exec();
}

#endif ///MYTEST
