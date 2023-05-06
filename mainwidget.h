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




class Field {
public:
    int sFentrol, oBalrol;\
    STATE state;

    Field(int sFentrol_, int oBalrol_, int s):
            sFentrol(sFentrol_), oBalrol(oBalrol_) ,
            state(GRASS)
    {}

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


    //int getNewSize();
private slots:
    // adds the new table siz to the callchain
    void callAcceptSendTableSiz() {
        assert(size != 0);
        emit acceptSendTableSiz(size);
    }
public slots:
    void changeValue(int c);
signals:
    void acceptSendTableSiz(int);
};


class MainWidget;
///TODO: dtor-ok megírása
class FieldsWidget : public QWidget{
    Q_OBJECT
    MainWidget* owner;
    void paintEvent(QPaintEvent* e) override;
public:
    FieldsWidget(MainWidget* owner);

};

class MainWidget : public QWidget {
    Q_OBJECT

public:
    MainWidget(int minSize_, double stepInterval, QWidget *parent = nullptr);
    ~MainWidget();

    void generateTable();
    friend class Tester;
    friend FieldsWidget;
private:
    Ui::MainWidget *ui;
    NewGameDialog* newGameDialog;
    QWidget* fields;
    QDialog* won;
    QDialog* lost;
    Model* model;
    std::vector<std::vector<Field> > fieldTable;
    const int minSize;/// minimal number of buttons in each column and row
    int siz;
    int bSize;///size of the buttons
    //static std::map<STATE, QString> images;
    std::map<STATE, QImage> images;
public slots:
    void keyReleaseEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void newGameAcceptedSiz(int);
    void setButtonState(STATE state, int row, int col );///signals the MainWidget, to change a buttons state
signals:
    void keyReleased(Qt::Key);
    void keyPressed(Qt::Key);
};

#endif // MAINWIDGET_H
