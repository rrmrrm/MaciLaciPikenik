#ifndef MODEL_H
#define MODEL_H

#include <QWidget>
#include <QTimer>
#include <QString>
#include <QVector>
#include <QDialog>
#include <QList>
#include "common.h"

class MainWidget;

struct Entity{
    enum DIR{ UP, LE, DO, RI};
    STATE race;
    DIR dir;

    ///velocity(cell/tick)
    int vel;

    ///position
    int row;
    ///position
    int col;

    Entity( STATE ra, DIR di, int v, int ro, int co )
            : race(ra), dir(di), vel(v), row(ro), col(co) {}

    bool operator==(const Entity& ent) const{
        return (race==ent.race && dir==ent.dir && vel==ent.vel && row==ent.row && col==ent.col);
    }
    ///these return, where would be the entity in the next tick,
    /// even if that's an invalid move (it needs to be checked elsewhere).
    int getNextRow();
    int getNextCol();

    void setDirRand();
    virtual bool tryStepOn(STATE race) { return true; }
    virtual void invalidMove(){}
    virtual ~Entity();
};
struct Maci : public Entity {
    int basketNum;

    Maci( int r0 = 0, int c0 = 0 ) : Entity (BEAR, UP, 1, r0, c0), basketNum(0) {}
    virtual bool tryStepOn(STATE race) {
        switch(race){
        case GRASS: {return true;}
        case BEAR: {}
        case GUARD: {}
        case TREE: {return false;}
        case BASKET: {++basketNum;return true;}
        }
        return false;
    }
};
struct Guard : public Entity {
    QString favFood;

    Guard( int r0 = 0, int c0 = 0 ) : Entity (GUARD, UP, 1, r0, c0), favFood("antropomorphic bears") {}
    virtual bool tryStepOn(STATE race) {
        switch(race){
        case GRASS: {return true;}
        case BEAR: {}
        case GUARD: {}
        case TREE: {}
        case BASKET: {invalidMove();return false;}
        }
        return false;
    }
    virtual void invalidMove(){
        switch(dir){
        case UP: {dir=DO;break;}
        case DO: {dir=UP;break;}
        case LE: {dir=RI;break;}
        case RI: {dir=LE;break;}
        }
    }
};
struct Tree : public Entity {
    Tree( int r0 = 0, int c0 = 0 ) : Entity (TREE, UP, 0, r0, c0) {}
};
struct Basket : public Entity {
    Basket( int r0 = 0, int c0 = 0 ) : Entity (BASKET, UP, 0, r0, c0) {}
};

class Model : public QObject{
    Q_OBJECT

    bool isInMapBounds(int row, int col){
        return ( row >= 0 && row < map.size() && col >= 0 && col < map[0]/*!!*/.size() );
    }
    ///returns whether the game is over
    bool winLose();

    enum exceptions{ tooManyEntities, tooSmallMap };

    Maci* laci;

    int siz;

    QTimer* timer;

    QVector< QVector<Entity*> > map;
    QList<Entity*> ents;
    int basketsNum;
    ///time span of one step in seconds
    double stepInterval;
    ///elapsed time in seconds
    double elapsedTime;
public:
    Model(double stepInterval_);
    void newGame(int siz, int guardsNum, int treesNum, int basketsNum);

   friend class Tester;

public slots:
    void keyPressed(Qt::Key key);
    void keyReleased(Qt::Key);
    void step();
    void pause();
signals:
    void setButtonState(STATE state, int row, int col );
    void updateTime(double);
    void updateBasketIndicator(QString);



    void win();
    void lose();
};




#endif //MODEL_H
