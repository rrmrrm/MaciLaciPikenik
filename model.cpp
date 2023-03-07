#include "model.h"
#include "common.h"
//#include "mainwidget.h"
//#include <QWidget>
#include <QRandomGenerator>
#include <QVector>
#include <QTimer>
#include <QString>
#include <algorithm>
int Entity::getNextRow(){
    switch(dir){
    case UP: return (row - vel);
    case DO: return (row + vel);
    default: return row;
    }
}
int Entity::getNextCol(){
    switch(dir){
    case LE: return (col - vel);
    case RI: return (col + vel);
    default: return col;
    }
}
void Entity::setDirRand(){
    int r = rand()%4;
    if( 0 == r ){
        dir = UP;
    } else if( 1 == r ){
        dir = RI;
    } else if( 2 == r ){
        dir = DO;
    } else if( 3 == r ){
        dir = LE;
    }
}

Entity::~Entity() {}

Model::Model(double stepInterval_):
        laci( nullptr ),
        siz(0), timer( new QTimer(this) ),
        basketsNum(0),
        stepInterval(stepInterval_), elapsedTime(0)     {


    ///setting up timer to fire every 'stepInterval' seconds
    timer->setInterval( static_cast<int>(/*sec to ms*/1000.0 * stepInterval) );
}
void Model::newGame(int siz, int guardsNum, int treesNum, int basketsNum){
    disconnect(timer, SIGNAL( timeout() ), this, SLOT( step() ) );
    connect(timer, SIGNAL( timeout() ), this, SLOT( step() ) );
    this->basketsNum = basketsNum;

    elapsedTime = 0;
    timer->start();

    ///setting up laci
    delete laci;
    laci = new Maci(0,0);

    map.clear();

    for( auto ent = ents.begin() ; ent!=ents.end() ; ++ent ){
        delete (*ent);
        (*ent) = nullptr;
    }
    ents.clear();

    this->siz = siz;
    const int safeZoneR = 2;
    int freeFieldsNum = siz * siz - safeZoneR*safeZoneR;


    ///there needs to be safeZoneR^2 extra space for the players character
    if(guardsNum+treesNum+basketsNum > freeFieldsNum){
        guardsNum = treesNum = basketsNum = 0;
        //throw tooManyEntities;
    }
    if(siz <= safeZoneR){
        throw tooSmallMap;
    }

    ///initializing the map with nullptrs
    for(int row = 0 ; row < siz ; ++row){
        map.push_back( QVector<Entity*>() );
        for(int col = 0 ; col < siz ; ++col){
            map[row].push_back(nullptr);
        }
    }

    laci = new Maci(0,0);
    ents.push_back(laci);
    map[0][0] = laci;
    emit setButtonState(BEAR, 0, 0 );

    /// pushing guards
    for(int i = 0 ; i < guardsNum ; ++i){
        bool valid = false;
        int row = -1;
        int col = -1;
        while( !valid ){
            row = rand() % siz;
            col = rand() % siz;
            ///the bear will be on the top left corner, it will get a small safezone
            valid = ( ( row>=safeZoneR || col>=safeZoneR ) && nullptr == map[row][col] );
        }
        Guard* g = new Guard(row,col);
        ents.push_back(g);
        map[row][col] = g;
        g->setDirRand();
        emit setButtonState(GUARD, row, col );
    }
    ///pushing trees
    for(int i = 0 ; i < treesNum ; ++i){
        bool valid = false;
        int row = -1;
        int col = -1;
        while( !valid ){
            row = rand() % siz;
            col = rand() % siz;
            ///the bear will be on the top left corner, it will get a small safezone
            valid = ( ( row>=safeZoneR || col>=safeZoneR ) && nullptr == map[row][col] );
        }
        Tree* t = new Tree(row,col);
        ents.push_back(t);
        map[row][col] = t;
        t->setDirRand();
        emit setButtonState(TREE, row, col );
    }
    ///pushing baskets
    for(int i = 0 ; i < basketsNum ; ++i){
        bool valid = false;
        int row = -1;
        int col = -1;
        while( !valid ){
            row = rand() % siz;
            col = rand() % siz;
            ///the bear will be on the top left corner, it will get a small safezone
            valid = ( ( row>=safeZoneR || col>=safeZoneR ) && nullptr == map[row][col] );
        }
        Basket* b = new Basket(row,col);
        ents.push_back(b);
        map[row][col] = b;
        b->setDirRand();
        emit setButtonState(BASKET, row, col );
    }

}

void Model::keyReleased(Qt::Key key){
    switch( key ){
    case Qt::Key_A:
    case Qt::Key_W:
    case Qt::Key_D:
    case Qt::Key_S:{
        laci->vel=0;
        break;
    }
    default: {break;}
    }
}
void Model::keyPressed(Qt::Key key){
    ///laci might be nullptr here, because the event can be fired before newGame() is called
    if(!laci)
        return;
     
    switch( key ){
    case Qt::Key_A:
    case Qt::Key_W:
    case Qt::Key_D:
    case Qt::Key_S:{
        laci->vel=1;
        break;
    }
    default: {break;}
    }
    switch( key ){
    case Qt::Key_A:{laci->dir= Entity::LE;break;}
    case Qt::Key_W:{laci->dir= Entity::UP;break;}
    case Qt::Key_D:{laci->dir= Entity::RI;break;}
    case Qt::Key_S:{laci->dir= Entity::DO;break;}
    default: {break;}
    }

}

struct Pred{
    const Entity * const sought_ent;
    Pred( const Entity* const ent_) : sought_ent(ent_){}
    bool operator()(Entity* cmp) const{
        assert(sought_ent!=nullptr);
        assert(cmp!=nullptr);
        return ( *sought_ent == *cmp );
    }
};

void Model::step(){
    elapsedTime += stepInterval;
    emit updateTime(elapsedTime);

    QString indicator( QString::number(laci->basketNum) + " / " + QString::number(basketsNum) );
    emit updateBasketIndicator(indicator);

    ///NOTE: entities at smaller indexes move first
    for(auto ent = ents.begin(); ent != ents.end();++ent){
        int row = (*ent)->row;
        int col = (*ent)->col;
        int nRow = (*ent)->getNextRow();
        int nCol = (*ent)->getNextCol();

        if( (row != nRow || col != nCol) ){
            if( isInMapBounds(nRow, nCol) ){
                if( nullptr==map[nRow][nCol] ){
                    ///moving ent in map
                    map[nRow][nCol] = (*ent);
                    ///setting ent's coordinates
                    (*ent)->row = nRow;
                    (*ent)->col = nCol;
                    map[row][col] = nullptr;

                    emit setButtonState( (*ent)->race, nRow, nCol);
                    emit setButtonState(GRASS, row, col);
                }
                else {
                    if( (*ent)->tryStepOn(map[nRow][nCol]/*!!*/->/*!!*/race) ){
                        ///finding map[nRow][nCol] in ents
                        Pred pred(map[nRow][nCol]);
                        auto toDel = std::find_if(ents.begin(), ents.end(), pred);
                        ///checking if the sought entity really exists
                        assert( toDel!=ents.end() );

                        ///erasing the found entity
                        ///NOTE: erasing *ent would cause undefined behavior(using invalid iterator),
                        /// so all objects in ents must be different(operator==)
                        ents.erase(toDel);
                        ///deleting entity currently at **ent's future position in map
                        delete map[nRow][nCol];

                        ///moving ent in map
                        map[nRow][nCol] = (*ent);
                        ///setting ent's coordinates
                        (*ent)->row = nRow;
                        (*ent)->col = nCol;
                        map[row][col] = nullptr;

                        emit setButtonState( (*ent)->race, nRow, nCol);
                        emit setButtonState(GRASS, row, col);
                    }
                }
            }
            else{
                ///if the entity tried to move off the map we have to call
                ///  invalidMove() directly
                (*ent)->invalidMove();
            }
        }
    }
    winLose();

}
void Model::pause(){

    if( timer->isActive() )
        timer->stop();
    else
        timer->start();
}

bool Model::winLose(){
    if( basketsNum == laci->basketNum ){
        ///we dont want to emit more win() in the next ticks
        timer->stop();
        emit win();
        return true;
    }

    auto ent = ents.begin() ;
    bool lost = false;
    int rng = 1;///guards' range of sight
    while( !lost && ent!=ents.end() ){

        int rm = laci->row;
        int cm = laci->col;
        int re = (*ent)->row;
        int ce = (*ent)->col;

        lost = ( GUARD==(*ent)->race &&
                re >= rm-rng && re <= rm+rng &&
                ce >= cm-rng && ce <= cm+rng );

        ++ent;
    }
    if(lost){
        ///we dont want to emit more win() in the next ticks
        timer->stop();
        emit lose();
        return true;
    }
    return false;
}
