#include "mainwidget.h"
#include "model.h"
#include "ui_mainwidget.h"
#include "ui_dialog.h"
#include "common.h"
#include <algorithm>
#include <QWidget>
#include <QPixmap>
#include <QVector>
#include <QString>
#include <QPainter>
#include <QPushButton>
#include <QLayout>
//#include <QDesktopWidget>
#include <QDialog>
#include <QKeyEvent>
NewGameDialog::NewGameDialog( int minSize, QWidget *parent):
        ui( new Ui::NewGameDialog() ),
        size(minSize),
        guardsNum(0),
        treesNum(0),
        basketsNum(0)  {
    ui->setupUi(this);

    ui->sizeSpinBox->setValue(minSize);

    connect(  ui->sizeSpinBox,      SIGNAL( valueChanged(int) ), this, SLOT( changeValue(int) )  );
    connect(  ui->guardsSpinBox,    SIGNAL( valueChanged(int) ), this, SLOT( changeValue(int) )  );
    connect(  ui->treesSpinBox,     SIGNAL( valueChanged(int) ), this, SLOT( changeValue(int) )  );
    connect(  ui->basketsSpinBox,   SIGNAL( valueChanged(int) ), this, SLOT( changeValue(int) )  );


    QSpinBox q;
}
int NewGameDialog::getNewSize() { return size; }
void NewGameDialog::changeValue(int c){
    QSpinBox* source = qobject_cast<QSpinBox*>( sender() );

    if( source == ui->sizeSpinBox ){
        size = c;
    } else if( source == ui->guardsSpinBox ){
        guardsNum = c;
    } else if( source == ui->treesSpinBox ){
        treesNum = c;
    } else if( source == ui->basketsSpinBox ){
        basketsNum = c;
    }
}


MainWidget::MainWidget(int minSize_, double stepInterval, QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWidget),
        newGameDialog( new NewGameDialog(minSize_) ),
        won( new QDialog() ),
        lost( new QDialog() ),
        model( new Model(stepInterval) ),
        minSize(minSize_),
        siz(0),
        bSize(40) {

    ui->setupUi(this);

    ///setting the menu to minimal size
    QSize qS = ui->MenuLayout->minimumSize();
    QRect qr( 0, 0, qS.width(), qS.height() );
    ui->MenuLayout->setGeometry(qr);

    ///connecting model to time,-and basket indicator
    connect(model, SIGNAL( updateTime(double) ), ui->elapsedTimeIndicator, SLOT( display(double) ) );
    connect(model, SIGNAL(updateBasketIndicator(QString)), ui->basketIndicator, SLOT(setText(QString)) );

    ///connecting newGameButton to newGameAccepted(with the help of newGameDialog)
    connect(  ui->newGameButton, SIGNAL( clicked() ), newGameDialog, SLOT( exec() )  );
    connect(  newGameDialog, SIGNAL( accepted() ), this, SLOT( newGameAccepted() )  );
    ///enabling model to change buttonTable's content
    connect( model, SIGNAL( setButtonState(STATE,int,int) ),
           this, SLOT( setButtonState(STATE,int,int) ) );

    connect( ui->pauseButton, SIGNAL(clicked()), model, SLOT(pause()) );

    won->setWindowTitle("YOU WON!");
    lost->setWindowTitle("You lost");
    connect( model,SIGNAL(win()), won, SLOT(exec()) );
    connect( model,SIGNAL(lose()), lost, SLOT(exec()) );
}

MainWidget::~MainWidget(){
    clearTable();
    delete ui;
}

void MainWidget::generateTable(){
    clearTable();

    buttonTable =  QVector< QVector <My::QPushButton*> >{};

    for(int row = 0 ; row < siz ; ++row ){
        QVector <My::QPushButton*> vCol{};
        for(int col = 0 ; col < siz ; ++col ){
            My::QPushButton* mQPB = new My::QPushButton(row, col, bSize);
            vCol.push_back( mQPB );

            ui->TableLayout->addWidget(mQPB, row, col);
            mQPB->setFixedSize( QSize(bSize,bSize) );
        }
        buttonTable.push_back(vCol);
    }

    ui->MainLayout->setSizeConstraint(QLayout::SetFixedSize);
}
void MainWidget::clearTable(){
    ///NOTE: Here height not necessarily equals buttonTable.size(), and
    /// width not necessarily equals buttonTable[0].size()
    for(int row = 0 ; row < buttonTable.size() ; ++row ){
        ///
        for(int col = 0 ; col < buttonTable[0].size()/*!!*/ ; ++col ){
            My::QPushButton*& b = buttonTable[row][col];
            ui->TableLayout->removeWidget(b);
            delete b;
            b = nullptr;
        }
    }
}
void MainWidget::keyPressEvent(QKeyEvent* event){
    model->keyPressed( Qt::Key( event->key() ) );
}
void MainWidget::newGameAccepted(){
    siz = newGameDialog->size;
    int gNum = newGameDialog->guardsNum;
    int tNum = newGameDialog->treesNum;
    int bNum = newGameDialog->basketsNum;

    ///setting bSize according to the available space on screen:
    auto  qd = QGuiApplication::primaryScreen();
    QRect qr = qd->availableGeometry();
    bSize = qr.width()<qr.height() ? qr.width() : qr.height();
    bSize = bSize - ui->MenuLayout->geometry().height();///we want the available space, the BUTTONS can take up
    bSize = bSize - 30;///header height
    bSize = bSize < 0 ? 0 : bSize;///we set bSize to 0, if it became negative
    bSize /= siz;

    ///updating the MainWidget:
    generateTable();
    ///updating model:
    model->newGame(siz, gNum, tNum, bNum);
}
void MainWidget::setButtonState(STATE state, int row, int col ){

    QPixmap qpm(":/grass.png");
    QPainter painter(&qpm);
    QImage img;
    switch(state){
    case BEAR:{
        buttonTable[row][col]->changeIcon( QImage(":/bear.png") );
        break;
    }
    case GUARD:{
        buttonTable[row][col]->changeIcon( QImage(":/guard.png") );
        break;
    }
    case TREE:{
        buttonTable[row][col]->changeIcon( QImage(":/tree.png") );
        break;
    }
    case BASKET: {
        buttonTable[row][col]->changeIcon( QImage(":/basket.png") );
        break;
    }
    case GRASS: {
        buttonTable[row][col]->changeIcon( QImage(":/grass.png") );
        break;
    }
    }
}
