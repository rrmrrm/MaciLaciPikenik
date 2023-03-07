#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QObject>
#include <QWidget>
#include <QVector>
#include <QPushButton>
#include "common.h"
#include <QPixmap>
#include <QIcon>
#include <QDialog>
#include <QPainter>
#include <QMessageBox>

namespace Ui {
    class MainWidget;
    class NewGameDialog;
}
class Model;

namespace My {
    class QPushButton;
}



class My::QPushButton : public ::QPushButton {
    Q_OBJECT
public:
    int sFentrol, oBalrol;
    ///button's size in pixels
    int bSize;
    STATE state;
    QPixmap* pixmap;

    QPushButton(int sFentrol_, int oBalrol_, int s, const QString& text = "", QWidget* parent = nullptr):  ::QPushButton(text, parent),
            sFentrol(sFentrol_), oBalrol(oBalrol_) ,
            bSize(s),
            state(GRASS), pixmap( new QPixmap(bSize,bSize) ) {

            setIcon( QPixmap(":/grass.png") );
            setIconSize( QSize(bSize,bSize) );
    }

    void changeIcon ( const QImage& foreground){
        QPixmap background(":/grass.png");
        QPainter painter(&background);

        painter.drawImage(QPoint(0,0), foreground);

        setIcon(background);
        setIconSize( QSize(width(),height()) );
    }
};

class NewGameDialog: public QDialog {
    Q_OBJECT
public:
    enum ValueType {SIZE, GUARDSNUM, TREESNUM, BASKETSNUM};
    Ui::NewGameDialog* ui;

    int size;
    int guardsNum;
    int treesNum;
    int basketsNum;


    NewGameDialog(int minSize_, QWidget *parent = nullptr);


    int getNewSize();
public slots:
    void changeValue(int c);
};

///TODO: dtor-ok megírása

class MainWidget : public QWidget {
    Q_OBJECT

public:
    MainWidget(int minSize_, double stepInterval, QWidget *parent = nullptr);
    ~MainWidget();

    void generateTable();
    void clearTable();
    friend class Tester;
private:
    Ui::MainWidget *ui;

    NewGameDialog* newGameDialog;
    QDialog* won;
    QDialog* lost;
    Model* model;

    QVector< QVector <My::QPushButton*> > buttonTable;
    const int minSize;/// minimal number of buttons in each column and row
    int siz;
    int bSize;///size of the buttons


public slots:
    void keyReleaseEvent(QKeyEvent *event);
    void keyPressEvent(QKeyEvent* event);
    void newGameAccepted();
    void setButtonState(STATE state, int row, int col );///signals the MainWidget, to change a buttons state
signals:
    void keyReleased(Qt::Key);
    void keyPressed(Qt::Key);
};

#endif // MAINWIDGET_H
