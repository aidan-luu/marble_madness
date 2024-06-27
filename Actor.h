#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject{
public:
    Actor(int IID, double x, double y, int dir, int hitPoints, StudentWorld* world) : GraphObject(IID, x, y, dir), m_alive(true), m_hitPoints(hitPoints), m_world(world) {setVisible(true);}
    virtual void doSomething(){return;}
    virtual bool canBePushed(int x, int y){return false;}
    virtual bool canTakeMarble() const {return false;}
    virtual bool isStealable() const {return false;}
    virtual bool isObstacle() const {return false;}
    virtual bool countsInFactoryCensus() const{return false;}
    virtual void isAttacked();
    double getHealth() const {return m_hitPoints;}
    void setHealth(int amt){m_hitPoints = amt;}
    bool isAlive() const {return m_alive;}
    void setAlive(bool alive){m_alive = alive;}
    StudentWorld* getWorld() const {return m_world;}
private:
    double m_hitPoints;
    bool m_alive;
    StudentWorld* m_world;
};

class Agent : public Actor {
public:
    Agent(int IID, double x, double y, int dir, int hp, StudentWorld* world) : Actor(IID, x, y, dir, hp, world) {setAlive(true);}
    virtual bool canMove(int x, int y) const;
    //all moving objects are obstacles
    virtual bool isObstacle() const {return true;}
};

class Avatar : public Agent{
public:
    Avatar(double x, double y, StudentWorld* world) : Agent(IID_PLAYER, x, y, right, 20, world), m_peas(20){}
    virtual void doSomething();
    virtual bool canMove(int x, int y, int dir) const;
    virtual void isAttacked();
    int getPeas(){return m_peas;}
    void addPeas(int numPeas){m_peas += numPeas;}
    void shoot(){m_peas--;}
    private:
        int m_peas;
};

class Robot : public Agent{
public:
    //Every robot doesn't move every tick
    Robot(int IID, double x, double y, int dir, int hp, int score, StudentWorld* world) : Agent(IID, x, y, dir, hp, world), curr_tick(0), m_score(score){
        m_ticks = (28 - getWorld()->getLevel())/4;
        if(m_ticks < 3){m_ticks = 3;}
    }
    virtual void isAttacked();
    //only robots check to see if there's a clear shot before shooting
    bool hasClearShot();
    int getTick(){return m_ticks;}
    int getCurr(){return curr_tick;}
    void add_tick(){curr_tick++;}
    void reset_tick(){curr_tick = 0;}
private:
    int m_score;
    int m_ticks;
    int curr_tick;
};

class Thiefbot : public Robot{
    public:
        Thiefbot(int IID, double x, double y, int hp, int score, StudentWorld* world) : Robot(IID, x, y, right, hp, score, world), m_everPickedUpGoodie(false), m_numSquaresMoved(0), m_goodie(nullptr){
            m_distanceBeforeTurning = randInt(1, 6);
        }
        virtual void doSomething();
        //thiefbot is the only actor that counts in factory census
        virtual bool countsInFactoryCensus() const {return true;}
        virtual void isAttacked();
    private:
        bool m_everPickedUpGoodie;
        int m_distanceBeforeTurning;
        int m_numSquaresMoved;
        Actor* m_goodie;
};

class RegularThief : public Thiefbot{
    public:
        RegularThief(double x, double y, StudentWorld* world) : Thiefbot(IID_THIEFBOT, x, y, 5, 10, world){}
        virtual void doSomething();
};

class MeanThief : public Thiefbot{
    public:
        MeanThief(double x, double y, StudentWorld* world) : Thiefbot(IID_MEAN_THIEFBOT, x, y, 8, 20, world){}
        virtual void doSomething();
};

class RobotFactory : public Actor{
public:
    enum ProductType { REGULAR, MEAN };
    RobotFactory(double x, double y, ProductType type, StudentWorld* world) : Actor(IID_ROBOT_FACTORY, x, y, none, none, world), m_type(type){}
    virtual void doSomething();
    virtual bool isObstacle() const {return true;}
private:
    //member variable to check what kind of factory
    ProductType m_type;
};

class Ragebot : public Robot{
public:
    Ragebot(double x, double y, int dir, StudentWorld* world) : Robot(IID_RAGEBOT, x, y, dir, 10, 100, world){}
};

class HorizontalRage : public Ragebot{
public:
    HorizontalRage(double x, double y, StudentWorld* world) : Ragebot(x, y, right, world){}
    virtual void doSomething();
};

class VerticalRage : public Ragebot{
public:
    VerticalRage(double x, double y, StudentWorld* world) : Ragebot(x, y, down, world){}
    virtual void doSomething();
};

class Wall : public Actor{
public:
    Wall(double x, double y, StudentWorld* world) : Actor(IID_WALL, x, y, none, none, world){}
    virtual bool isObstacle() const {return true;}
};

class Marble : public Actor{
    public:
    Marble(double x, double y, StudentWorld* world) : Actor(IID_MARBLE, x, y, none, 10, world){}
    virtual bool isObstacle() const {return true;}
    virtual bool canBePushed(int x, int y);
};

class Pea : public Actor{
    public:
        Pea(double x, double y, int dir, StudentWorld* world) : Actor(IID_PEA, x, y, dir, none, world){}
        virtual void doSomething();
};

class Pit : public Actor{
public:
    //Pit isn't an obstacle since peas move past it
    Pit(double x, double y, StudentWorld* world) : Actor(IID_PIT, x, y, none, none, world){}
    virtual void doSomething();
    virtual bool canTakeMarble() const {return true;}
};

class Exit : public Actor{
public:
    Exit(double x, double y, StudentWorld* world) : Actor(IID_EXIT, x, y, none, none, world){setVisible(false);}
    virtual void doSomething();
};

class Collectible : public Actor{
public:
    Collectible(int IID, double x, double y, int score, StudentWorld* world) : Actor(IID, x, y, none, none, world), m_score(score){}
    virtual void doSomething();
private:
    int m_score;
};

class Crystal : public Collectible{
    public:
    Crystal(double x, double y, StudentWorld* world) : Collectible(IID_CRYSTAL, x, y, 50, world){}
    virtual void doSomething();
};

class Goodie : public Collectible{
public:
    Goodie(int IID, double x, double y, int score, StudentWorld* world) : Collectible(IID, x, y, score, world){}
    virtual bool isStealable() const {return true;}
};

class Life : public Goodie{
    public:
        Life(double x, double y, StudentWorld* world) : Goodie(IID_EXTRA_LIFE, x, y, 1000, world){}
        virtual void doSomething();
};

class Health : public Goodie{
public:
    Health(double x, double y, StudentWorld* world) : Goodie(IID_RESTORE_HEALTH, x, y, 500, world){}
    virtual void doSomething();
};

class Ammo : public Goodie{
public:
    Ammo(double x, double y, StudentWorld* world) : Goodie(IID_AMMO, x, y, 100, world){}
    virtual void doSomething();
};

#endif // ACTOR_H_
