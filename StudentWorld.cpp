#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_bonus(1000), m_crystals(0), m_levelComplete(false)
{
}

StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{

    Level lev(assetPath());
    
    //adjust curlevel based on current level #
    ostringstream oss;
    int level = getLevel();
    oss << "level" << setw(2) << setfill('0') << level << ".txt";
    string curLevel = oss.str();
    
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found ||
        result == Level:: load_fail_bad_format){return GWSTATUS_LEVEL_ERROR;}
    
    //loop through level txt and instantiate new actor based on level's char
    for(int x = 0; x < VIEW_HEIGHT; ++x){
        for(int y = 0; y < VIEW_WIDTH; ++y){
            Level::MazeEntry item = lev.getContentsOf(x, y);
            if(item == Level::wall){acts.push_back(new Wall(x, y, this));}
            else if(item == Level::player){m_player = new Avatar(x, y, this);}
            else if(item == Level::marble){acts.push_back(new Marble(x, y, this));}
            //increment m_crystals for each crystal in the level
            else if(item == Level::crystal){acts.push_back(new Crystal(x, y, this)); m_crystals++;}
            else if(item == Level::vert_ragebot){acts.push_back(new VerticalRage(x, y, this));}
            else if(item == Level::horiz_ragebot){acts.push_back(new HorizontalRage(x, y, this));}
            else if(item == Level::thiefbot_factory){acts.push_back(new RobotFactory(x, y, RobotFactory::REGULAR, this));}
            else if(item == Level::mean_thiefbot_factory){acts.push_back(new RobotFactory(x, y, RobotFactory::MEAN, this));}
            else if(item == Level::pit){acts.push_back(new Pit(x, y, this));}
            else if(item == Level::ammo){acts.push_back(new Ammo(x, y, this));}
            else if(item == Level::restore_health){acts.push_back(new Health(x, y, this));}
            else if(item == Level::extra_life){acts.push_back(new Life(x, y, this));}
            else if(item == Level::exit){acts.push_back(new Exit(x, y, this));}
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //check for m_levelComplete each tick and reset m_levelComplete to false
    //and bonus back to 1000
    if(m_levelComplete){
        m_levelComplete = false;
        //reset the bonus amount
        m_bonus = 1000;
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    //deplete bonus until 0 each tick
    if(m_bonus > 0){m_bonus--;}
    
    //update stat display
    setDisplayText();
    
    //check for player's status and doSomething if alive and decLives otherwise
    //checked separately because player is not in actor collection
    if(m_player && m_player->isAlive()){m_player->doSomething();}
    else if(!m_player->isAlive() && getLives() > 0){decLives(); return GWSTATUS_PLAYER_DIED;}
            
    vector<Actor*>::iterator it;

    //check each actor in actor collection
    for(it = acts.begin(); it != acts.end();){
        if((*it)->isAlive()){(*it)->doSomething(); ++it;}
        //delete dead actors
        else{delete *it; it = acts.erase(it);}
    }
    
    //add new actors in after looping through otherwise iterator
    //becomes invalid
    acts.insert(acts.end(), newActs.begin(), newActs.end());
    newActs.clear();
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    //delete all dynamically allocated actors
    delete m_player;
    m_player = nullptr;
    vector<Actor*>::iterator it;
    for(it = acts.begin(); it != acts.end();){
        delete *it;
        it = acts.erase(it);
    }
}

string StudentWorld::displayFormat(int score, int level, int lives, int health, int ammo, int bonus) const{
    ostringstream oss;

    // Use setw to ensure fixed width and setfill to pad with zeros if necessary
    oss << "Score: " << setw(7) << setfill('0') << score
        << "  Level: " << setw(2) << level
        << "  Lives: " << setw(2) << setfill(' ') <<  lives
        << "  Health: " << setw(3) << health << "%"
        << "  Ammo: " << setw(3) << ammo
        << "  Bonus: " << setw(4) << bonus;

    return oss.str();
}

void StudentWorld::setDisplayText()
{
     int score = getScore();
     int level = getLevel();
     unsigned int bonus = m_bonus;
     int livesLeft = getLives();
     int health = (m_player->getHealth() / 20) * 100;
     // Next, create a string from your statistics, of the form:
     // Score: 0000100 Level: 03 Lives: 3 Health: 70% Ammo: 216 Bonus: 34
     string s = displayFormat(score, level, livesLeft, health, m_player->getPeas(), bonus);
    // Finally, update the display text at the top of the screen with your
    // newly created stats
     setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

bool StudentWorld::anyObstaclesOnCol(int x, int x2, int y) const {
    for(Actor* act : acts){
        //obstacle between the 2 x-coordinates
        if(act->getX() > x && act->getX() < x2 && act->getY() == y && act->isObstacle()){return true;}
    }
    return false;
}

bool StudentWorld::anyObstaclesOnRow(int y, int y2, int x) const {
    for(Actor* act : acts){
        //obstacle between the 2 x-coordinates
        if(act->getY() > y && act->getY() < y2 && act->getX() == x && act->isObstacle()){return true;}
    }
    return false;
}

//check location of marble and see if it's on pit to swallow
bool StudentWorld::swallowSwallowable(Actor* a){
    for(Actor* act: acts){
        if(a->getX() == act->getX() && a->getY() == act->getY() && act->isObstacle() && act->getHealth() > 0){
            act->setAlive(false);
            return true;
        }
    }
    return false;
}

//see if player is on coordinates (x, y)
bool StudentWorld::isPlayerColocatedWith(int x, int y) const{
    if(x == m_player->getX() && y == m_player->getY()){return true;}
    return false;
}

//tally up # of thiefbots within distance of 3 in all directions
bool StudentWorld::doFactoryCensus(int x, int y, int distance, int& count) const{

    int minX = x - distance;
    int maxX = x + distance;
    int minY = y - distance;
    int maxY = y + distance;

    for (Actor* actor : acts) {
        if (actor->countsInFactoryCensus()) {
            int actorX = actor->getX();
            int actorY = actor->getY();

            //Actor on factory
            if (actorX == x && actorY == y) {return false;}
                
            //Actor within range of census
            if (actorX >= minX && actorX <= maxX && actorY >= minY && actorY <= maxY) {count++;}
        }
    }

    return true;
}

void StudentWorld::completeLevel(){
    m_levelComplete = true;
    playSound(SOUND_FINISHED_LEVEL);
    increaseScore(2000);
    increaseScore(m_bonus);
}

void StudentWorld::increaseAmmo(int amt){
    m_player->addPeas(amt);
}

void StudentWorld::restorePlayerHealth(){
    m_player->setHealth(20);
}

//return goodie at (x,y)
Actor* StudentWorld::getColocatedStealable(int x, int y) const{
    for(Actor* act : acts) {
        if(act->isStealable() && act->getX() == x && act->getY() == y){return act;}
    }
    return nullptr;
}

//Return pointer to actor at (x, y)
Actor* StudentWorld::getActorAt(int x, int y) const {
    //If we have multiple actors at same square
    //we want to return the alive one not the collectible
    Actor* result[100];
    int count = 0;
    
    for (Actor* actor : acts) {
        if (actor->getX() == x && actor->getY() == y) {
            result[count] = actor;
            count++;
        }
    }
    
    switch(count){
        //if no actor found return nullptr
        case 0:
            return nullptr;
        //return the 1 actor found
        case 1:
            return result[0];
        //return the actor that's alive
        default:
            for(int i = 0; i < count; ++i){
                if(result[i]->getHealth() > 0){return result[i];}
            }
    }
    return nullptr;
}
