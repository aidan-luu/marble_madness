#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>

class Actor;
class Avatar;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    std::string displayFormat(int score, int level, int lives, int health, int ammo, int bonus) const;
    void setDisplayText();
    bool anyObstaclesOnCol(int x, int x2, int y) const;
    bool anyObstaclesOnRow(int y, int y2, int x) const;
    bool isPlayerColocatedWith(int x, int y) const;
    bool swallowSwallowable(Actor* a);
    int getCrystalCount() const {return m_crystals;}
    void crystalFound(){m_crystals--;}
    void completeLevel();
    void increaseAmmo(int amt);
    void restorePlayerHealth();
    void addActor(Actor* act){newActs.push_back(act);}
    bool doFactoryCensus(int x, int y, int distance, int& count) const;
    Actor* getColocatedStealable(int x, int y) const;
    Actor* getActorAt(int x, int y) const;
    Avatar* getPlayer() const {return m_player;}
private:
    int m_bonus;
    int m_crystals;
    bool m_levelComplete;
    std::vector<Actor*> acts;
    //used to hold any newly added actors like thiefbots or peas
    std::vector<Actor*> newActs;
    Avatar* m_player;
};

#endif // STUDENTWORLD_H_
