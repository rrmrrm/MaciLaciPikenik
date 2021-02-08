

#ifndef TESTER_H
#define TESTER_H

#include "main.h"

#ifdef MYTEST

#include <QtTest>
#include <QObject>
#include "mainwidget.h"
#include "model.h"
#include "common.h"

///Emlékeztető nekem:
///-------------------------------------------------------------
///Az ellenőrzéseket makrók segítségével valósítjuk meg, pl.:
///• Logikai kifejezés ellenőrzése: QVERIFY(<kifejezés>)
///• Összehasonlítás:
///    QCOMPARE(<aktuális érték>, <várt érték>)
///• hiba: QFAIL(<üzenet>)
///• figyelmeztetés: QWARN(<üzenet>)
///-------------------------------------------------------------
/// A teszt futtatását a QTEST_MAIN(<osztálynév>) vagy a QTEST
///_APPLESS_MAIN(<osztálynév>) makróval végezhetjük, amely
///automatikusan legenerál egy főprogramot, és végrehajtja a
///teszteseteket, így a tesztek egyszerű konzolos alkalmazásként
///futtathatóak.
class Tester : public QObject{
    Q_OBJECT
    Model* model;
public:
    Tester(){}

private slots:
    void initTestCase(){
        ///3000 mp-enkent fog lejárni a timer, hogy ne zavarjon be
        model = new Model(3000);
    }
    void cleanupTestCase()
    {
        delete model;
    }

    void testNewGame(){
        model->newGame(6, 0, 0, 0);
        ///0 ido telt e
        QCOMPARE(model->elapsedTime, 0);
        ///a bal felso sarokban maci van
        QCOMPARE( model->map[0][0]->race, BEAR);
            for (int i = 0; i < 6; i++){
                for (int j =0; j < 6; j++){
                    ///mindenhol mashol ures mezo vann
                    if( i!=0 || j!=0)
                        QCOMPARE(model->map[i][j], nullptr);
                }
            }

    }
    void testStepGame(){
        model->newGame(6,1,0,0);

        Maci* laci = static_cast<Maci*>( model->ents.at(0) );
        Guard* guard = static_cast<Guard*>( model->ents.at(1) );

        ///manuálisan áthelyezzük az őrt és megváltoztatjuk az irányát
        model->map[guard->row][guard->col] = nullptr;
        guard->row = 0;
        guard->col = 5;
        guard->dir = Entity::LE;///a maci felé tart
        model->map[0][5] = guard;
        int rL = 0;
        int cL = 0;
        int rG = guard->row;// = 0
        int cG = guard->col;// = 5

        model->step();

        ///ellenorizzuk, hogy az or egyel balra  lepett
        QCOMPARE( model->ents.at(1)->row, rG );
        QCOMPARE( model->ents.at(1)->col, cG-1 );
        QVERIFY( model->map[rG][cG-1] != nullptr );
        QCOMPARE( model->map[rG][cG-1]->race, GUARD );
        /// ellenőrizzük, hogy közben más mező nem változott
        for (int i = 0; i < 6; i++){
            for (int j = 0; j < 6; j++){
                if( (i!=rL || j!=cL) && (i!=rG || j != cG-1) )
                    QCOMPARE(model->map[i][j], nullptr);
            }
        }
    }
    void testLoseGame(){
        model->newGame(6,1,0,1);///egy őr és egy kosár

        ///a játék még nem ért véget
        QCOMPARE( model->winLose(), false );
        ///az időzítő megy, így a játék is.
        QCOMPARE( model->timer->isActive(), true );

        Maci* laci = static_cast<Maci*>( model->ents.at(0) );
        Guard* guard = static_cast<Guard*>( model->ents.at(1) );
        Basket* basket = static_cast<Basket*>( model->ents.at(2) );
        ///manuálisan áthelyezzük az őrt és megváltoztatjuk az irányát
        model->map[guard->row][guard->col] = nullptr;
        guard->row = 0;
        guard->col = 3;
        guard->dir = Entity::LE;
        model->map[0][5] = guard;

        ///manuálisan áthelyezzük a kosarat, nehogy az őr és a maci között legyen
        /// (akkor a maci nyerne, lásd testWinGame)
        model->map[basket->row][basket->col] = nullptr;
        basket->row = 5;
        basket->col = 5;
        model->map[5][5] = basket;

        ///megváltoztatjuk a maci irányát, hogy az őrnek menjen
        model->keyPressed(Qt::Key_D);

        model->step();

        ///a játéknak vége és nem vette fel az összes kosarat, azaz vesztett
        QCOMPARE( model->winLose() && (model->basketsNum != model->laci->basketNum), true );
        ///az időzítő leállt, így a játék is.
        QCOMPARE( model->timer->isActive(), false );
    }
    void testWinGame(){
        model->newGame(6,1,0,1);///egy őr és egy kosár

        ///a játék még nem ért véget
        QCOMPARE( model->winLose(), false );
        ///az időzítő megy, így a játék is.
        QCOMPARE( model->timer->isActive(), true );

        Maci* laci = static_cast<Maci*>( model->ents.at(0) );
        Guard* guard = static_cast<Guard*>( model->ents.at(1) );
        Basket* basket = static_cast<Basket*>( model->ents.at(2) );

        ///manuálisan áthelyezzük az őrt és megváltoztatjuk az irányát
        model->map[guard->row][guard->col] = nullptr;
        guard->row = 0;
        guard->col = 3;
        guard->dir = Entity::LE;
        model->map[0][5] = guard;
        ///manuálisan áthelyezzük a kosarat, hogy az őr és a maci között legyen
        model->map[basket->row][basket->col] = nullptr;
        basket->row = 0;
        basket->col = 1;
        model->map[0][1] = basket;

        ///megváltoztatjuk a maci irányát, hogy a kosárnak és az őrnek menjen
        model->keyPressed(Qt::Key_D);

        model->step();

        ///a játéknak vége, és felvette az összes kosarat, azaz nyert
        QCOMPARE( model->winLose() && (model->basketsNum == model->laci->basketNum), true );
        ///az időzítő leállt, így a játék is.
        QCOMPARE( model->timer->isActive(), false );
    }
};

QTEST_APPLESS_MAIN(Tester)
//#include "tester.moc"
#endif // MYTEST

#endif /// TESTER_H
