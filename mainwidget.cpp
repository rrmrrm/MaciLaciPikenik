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
#include <iostream>
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

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(callAcceptSendTableSiz()));

    QSpinBox q;
}
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

FieldsWidget::FieldsWidget(MainWidget* aOwner)
    : owner(aOwner)
    {
        setFixedSize(QSize(owner->bSize*owner->siz,owner->bSize*owner->siz));
    }
void FieldsWidget::paintEvent(QPaintEvent* e){
    QPainter painter(this);
    painter.drawRect(0,0, width(), height());
    int bSize = owner->bSize;
    for(int row = 0 ; row < (int)owner->fieldTable.size() ; ++row){
        for(int j = 0 ; j < (int)owner->fieldTable[row].size() ; ++j){
            QRect r(j*bSize, row*bSize, bSize, bSize);

            //QString imagePath = owner::images[owner->fieldTable[row][j].state];
            //QImage image(imagePath);

            //painter.drawImage(r, MainWidget::images[owner->fieldTable[row][j].state]);

            painter.drawImage(r, owner->images[owner->fieldTable[row][j].state]);
        }
    }
}

MainWidget::MainWidget(int minSize_, double stepInterval, QWidget *parent) :
        QWidget(parent), ui(new Ui::MainWidget),
        newGameDialog( new NewGameDialog(minSize_) ),
        fields(nullptr),
        won( new QDialog() ),
        lost( new QDialog() ),
        model( new Model(stepInterval) ),
        minSize(minSize_),
        siz(0),
        bSize(40) {

    images = std::map<STATE, QImage> {
    {STATE::BASKET, QImage(":/basket.png")},
    {STATE::BEAR, QImage(":/bear.png")},
    {STATE::GRASS, QImage(":/grass.png")},
    {STATE::GUARD, QImage(":/guard.png")},
    {STATE::TREE, QImage(":/tree.png")},
    {STATE::BASKET, QImage(":/asd.png")},
    };

    ui->setupUi(this);

    ///setting the menu to minimal size
    QSize qS = ui->MenuLayout->minimumSize();
    QRect qr( 0, 0, qS.width(), qS.height() );
    ui->MenuLayout->setGeometry(qr);
    fields = new FieldsWidget(this);
    ui->MenuLayout->addWidget(fields);
    ///connecting model to time,-and basket indicator
    connect(model, SIGNAL( updateTime(double) ), ui->elapsedTimeIndicator, SLOT( display(double) ) );
    connect(model, SIGNAL(updateBasketIndicator(QString)), ui->basketIndicator, SLOT(setText(QString)) );
    connect(this, SIGNAL(keyReleased(Qt::Key)), model, SLOT(keyReleased(Qt::Key)));
    connect(this, SIGNAL(keyPressed(Qt::Key)), model, SLOT(keyPressed(Qt::Key)));
    ///connecting newGameButton to newGameAccepted(with the help of newGameDialog)
    connect(ui->newGameButton, SIGNAL( clicked() ), newGameDialog, SLOT( exec() )  );
    connect( newGameDialog, SIGNAL( acceptSendTableSiz(int) ), this, SLOT( newGameAcceptedSiz(int)) );
    ///enabling model to change fieldTable's content
    connect( model, SIGNAL( setButtonState(STATE,int,int) ),
           this, SLOT( setButtonState(STATE,int,int) ) );

    connect( ui->pauseButton, SIGNAL(clicked()), model, SLOT(pause()) );

    won->setWindowTitle("YOU WON!");
    lost->setWindowTitle("You lost");
    connect( model,SIGNAL(win()), won, SLOT(exec()) );
    connect( model,SIGNAL(lose()), lost, SLOT(exec()) );
}

MainWidget::~MainWidget(){
    delete ui;
}

void MainWidget::generateTable(){
    fields->setFixedSize(QSize(bSize,bSize)*siz);
    fieldTable =  std::vector<std::vector<Field> >{};

    for(int row = 0 ; row < siz ; ++row ){
        std::vector<Field> vCol{};
        for(int col = 0 ; col < siz ; ++col ){
            auto mQPB = Field(row, col, bSize);
            vCol.push_back( mQPB );
        }
        fieldTable.push_back(vCol);
    }
    fields->update();
}
void MainWidget::keyReleaseEvent(QKeyEvent *event){
    emit keyReleased( Qt::Key( event->key() ));
}
void MainWidget::keyPressEvent(QKeyEvent* event){
    emit keyPressed( Qt::Key( event->key() ) );
}
void MainWidget::newGameAcceptedSiz(int newSiz){
    siz = newSiz;
    int gNum = newGameDialog->guardsNum;
    int tNum = newGameDialog->treesNum;
    int bNum = newGameDialog->basketsNum;

    ///setting bSize according to the available space on screen:
    auto  qd = QGuiApplication::primaryScreen();
    QRect qr = qd->availableGeometry();
    bSize = qr.width()<qr.height() ? qr.width() : qr.height();
    /// should get the available space for the fields to take up
    /// but since im unable to make it work i just set it to whatever
    //bSize = bSize - ui->MenuLayout->geometry().height();
    bSize = bSize - 120;
    bSize = bSize - 30;///header height
    bSize = bSize < 0 ? 0 : bSize;///we set bSize to 0, if it became negative
    bSize /= siz;

    ///updating the MainWidget:
    generateTable();
    ///updating model:
    model->newGame(siz, gNum, tNum, bNum);
}
void MainWidget::setButtonState(STATE state, int row, int col ){
    fieldTable[row][col].state=state;
    fields->update();
}
