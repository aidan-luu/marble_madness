#include "Actor.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

void Avatar::doSomething(){
    
    int num = 0;
    //position of avatar
    int col = getX();
    int row = getY();
    if(getWorld()->getKey(num)){
        switch(num){
            //player loses a life
            case KEY_PRESS_ESCAPE:
                getWorld()->playSound(SOUND_PLAYER_DIE);
                setAlive(false);
                break;
            // player shoots
            case KEY_PRESS_SPACE:{
                if(m_peas == 0){return;}
                int dir = getDirection();
                shoot();
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                switch(dir){
                    //instantiate pea at proper location
                    case right:
                        getWorld()->addActor(new Pea(col+1, row, dir, getWorld()));
                        break;
                    case left:
                        getWorld()->addActor(new Pea(col-1, row, dir, getWorld()));
                        break;
                    case up:
                        getWorld()->addActor(new Pea(col, row+1, dir, getWorld()));
                        break;
                    case down:
                        getWorld()->addActor(new Pea(col, row-1, dir, getWorld()));
                        break;
                }
                break;
            }
            //move if possible
            case KEY_PRESS_UP:
                if(canMove(col, row+1, up)){moveTo(col, row+1);}
                setDirection(up);
                break;
            case KEY_PRESS_DOWN:
                if(canMove(col, row-1, down)){moveTo(col, row-1);}
                setDirection(down);
                break;
            case KEY_PRESS_RIGHT:
                if(canMove(col+1, row, right)){moveTo(col+1, row);}
                setDirection(right);
                break;
            case KEY_PRESS_LEFT:
                if(canMove(col-1, row, left)){moveTo(col-1, row);}
                setDirection(left);
                break;
        }
    }
}

void Thiefbot::doSomething(){
    
    int col = getX();
    int row = getY();
    
    bool hasMoved = false;
    
    //get goodie at thiefbot's location
    Actor* goodie = getWorld()->getColocatedStealable(col, row);
    
    //pick up goodie
    if(goodie && !m_everPickedUpGoodie && randInt(1, 10) == 1){
        m_goodie = goodie;
        goodie->setVisible(false);
        getWorld()->playSound(SOUND_ROBOT_MUNCH);
        m_everPickedUpGoodie = true;
        return;
    }
    
    //move if thiefbot can
    if(m_numSquaresMoved < m_distanceBeforeTurning){
        switch(getDirection()){
            case right:
                if(canMove(col+1, row)){
                    moveTo(col+1, row);
                    m_numSquaresMoved++;
                    hasMoved = true;
                    return;
                }
                break;
            case left:
                if(canMove(col-1, row)){
                    moveTo(col-1, row);
                    m_numSquaresMoved++;
                    hasMoved = true;
                    return;
                }
                break;
            case up:
                if(canMove(col, row+1)){
                    moveTo(col, row+1);
                    m_numSquaresMoved++;
                    hasMoved = true;
                    return;
                }
                break;
            case down:
                if(canMove(col, row-1)){
                    moveTo(col, row-1);
                    m_numSquaresMoved++;
                    hasMoved = true;
                    return;
                }
                break;
        }
    }
    
    //find new random direction to move
    if(m_numSquaresMoved >= m_distanceBeforeTurning || !hasMoved){
        m_distanceBeforeTurning = randInt(1, 6);
        m_numSquaresMoved = 0;
        
        int numDirectionsVisited = 0;
        
        std::vector<int> dir = {right, left, down, up};
        
        while(!hasMoved && numDirectionsVisited < 4){
            
            //random index to choose direction from dir
            int d = rand() % (4 - numDirectionsVisited);
            std::vector<int>::iterator it;
            
            switch(dir[d]){
                case right:
                    if(canMove(col+1, row)){
                        moveTo(col+1, row);
                        setDirection(dir[d]);
                        m_numSquaresMoved++;
                        return;
                    }
                    else{
                        for(it = dir.begin(); it != dir.end(); ++it){
                            if(*it == dir[d]){break;}
                        }
                        //erase direction from vector since already visited
                        it = dir.erase(it);
                        numDirectionsVisited++;
                    }
                    break;
                case left:
                    if(canMove(col-1, row)){
                        moveTo(col-1, row);
                        setDirection(dir[d]);
                        m_numSquaresMoved++;
                        return;
                    }
                    else{
                        for(it = dir.begin(); it != dir.end(); ++it){
                            if(*it == dir[d]){break;}
                        }
                        it = dir.erase(it);
                        numDirectionsVisited++;
                    }
                    break;
                case up:
                    if(canMove(col, row+1)){
                        moveTo(col, row+1);
                        m_numSquaresMoved++;
                        setDirection(dir[d]);
                        return;}
                    else{
                        for(it = dir.begin(); it != dir.end(); ++it){
                            if(*it == dir[d]){break;}
                        }
                        it = dir.erase(it);
                        numDirectionsVisited++;
                    }
                    break;
                case down:
                    if(canMove(col, row-1)){
                        moveTo(col, row-1);
                        setDirection(dir[d]);
                        m_numSquaresMoved++;
                        return;
                    }
                    else{
                        for(it = dir.begin(); it != dir.end(); ++it){
                            if(*it == dir[d]){break;}
                        }
                        it = dir.erase(it);
                        numDirectionsVisited++;
                    }
                    break;
            }
        }
        //set random direction if obstruction in all 4 directions
        setDirection(dir[rand()%4]);
    }
}

void MeanThief::doSomething(){
    //do nothing if tick to doSomething not reached yet
    if(getCurr() != getTick()){add_tick(); return;}
    else{reset_tick();}
    
    //check if there's clear shot first
    if(hasClearShot()){return;}
    
    Thiefbot::doSomething();
}

void RegularThief::doSomething(){
    if(getCurr() != getTick()){add_tick(); return;}
    else{reset_tick();}
    
    Thiefbot::doSomething();
}

void RobotFactory::doSomething(){
    int count = 0;
    bool noActorOnFactory = getWorld()->doFactoryCensus(getX(), getY(), 3, count);
    
    if(count < 3 && noActorOnFactory){
        if(randInt(1, 50) == 1){
            //create new thiefbot depending on type of factory
           if(m_type == REGULAR){getWorld()->addActor(new RegularThief(getX(), getY(), getWorld()));}
            else{getWorld()->addActor(new MeanThief(getX(), getY(), getWorld()));}
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
    }
}

void HorizontalRage::doSomething(){
    if(getCurr() != getTick()){add_tick(); return;}
    else{reset_tick();}
    
    int col = getX();
    int row = getY();
    
    if(hasClearShot()){return;}
    
    if(getDirection() == right){
        if(canMove(col+1, row)){moveTo(col+1, row);}
        //turn around
        else{setDirection(left);}
    }
    else{
        if(canMove(col-1, row)){moveTo(col-1, row);}
        else{setDirection(right);}
    }
    
    reset_tick();
}

void VerticalRage::doSomething(){
    if(getCurr() != getTick()){add_tick(); return;}
    else{reset_tick();}
    
    int col = getX();
    int row = getY();
    
    if(hasClearShot()){return;}
    
    if(getDirection() == down){
        if(canMove(col, row-1)){moveTo(col, row-1);}
        //turn around
        else{setDirection(up);}
    }
    else{
        if(canMove(col, row+1)){moveTo(col, row+1);}
        else{setDirection(down);}
    }
    
    reset_tick();
}

void Pea::doSomething(){
    
    Actor* act = getWorld()->getActorAt(getX(), getY());
    Avatar* player = getWorld()->getPlayer();
    
    //check for player
    if(getWorld()->isPlayerColocatedWith(getX(), getY())){
        setAlive(false);
        player->isAttacked();
        return;
    }
    
    //if not empty space
    if(act != nullptr){
        //attack if object with hp
        if(act->getHealth() > 0){act->isAttacked(); setAlive(false); return;}
        else if(act->isObstacle()){setAlive(false); return;}
    }
    
    //check location in front of pea
    switch(getDirection()){
        case right:{
            if(getWorld()->isPlayerColocatedWith(getX()+1, getY())){
                setAlive(false);
                player->isAttacked();
                return;
            }
            
            Actor* act2 = getWorld()->getActorAt(getX()+1, getY());
            
            if(act2 != nullptr){
                if(act2->getHealth() > 0){
                    setAlive(false);
                    act2->isAttacked();
                    return;
                }
                else if(act2->isObstacle()){
                    setAlive(false);
                    return;
                }
            }

            
            moveTo(getX()+1, getY());
            
            break;
        }
        case left:{
            if(getWorld()->isPlayerColocatedWith(getX()-1, getY())){
                setAlive(false);
                player->isAttacked();
                return;
            }
            
            Actor* act2 = getWorld()->getActorAt(getX()-1, getY());
            
            if(act2 != nullptr){
                if(act2->getHealth() > 0){
                    setAlive(false);
                    act2->isAttacked();
                    return;
                }
                else if(act2->isObstacle()){
                    setAlive(false);
                    return;
                }
            }
            
            moveTo(getX()-1, getY());
            break;
        }
        case up:{
            if(getWorld()->isPlayerColocatedWith(getX(), getY()+1)){
                setAlive(false);
                player->isAttacked();
                return;
            }
            
            Actor* act2 = getWorld()->getActorAt(getX(), getY()+1);
            
            if(act2 != nullptr){
                if(act2->getHealth() > 0){
                    setAlive(false);
                    act2->isAttacked();
                    return;
                }
                else if(act2->isObstacle()){
                    setAlive(false);
                    return;
                }
            }
            
            moveTo(getX(), getY()+1);
            
            break;
        }
        case down:{
            if(getWorld()->isPlayerColocatedWith(getX(), getY()-1)){
                setAlive(false);
                player->isAttacked();
                return;
            }
            
            Actor* act2 = getWorld()->getActorAt(getX(), getY()-1);
            
            if(act2 != nullptr){
                if(act2->getHealth() > 0){
                    act2->isAttacked();
                    setAlive(false);
                    return;
                }
                else if(act2->isObstacle()){
                    setAlive(false);
                    return;
                }
            }
            
            moveTo(getX(), getY()-1);
            break;
        }
    }
}

void Pit::doSomething(){
    //check if marble collocated and swallow if true
    if(getWorld()->swallowSwallowable(this)){setAlive(false);}
}

void Exit::doSomething(){
    //once no crystals left reveal exit
    if(getWorld()->getCrystalCount() == 0 && !isVisible()){
        getWorld()->playSound(SOUND_REVEAL_EXIT);
        setAlive(true);
        setVisible(true);
    }
    
    //complete level only if exit visible and player on exit
    if(isVisible() && getWorld()->isPlayerColocatedWith(getX(), getY())){getWorld()->completeLevel();}
}

void Collectible::doSomething(){
    //handle appropriate stat updates if player collocated
    if(getWorld()->isPlayerColocatedWith(getX(), getY())){
        getWorld()->increaseScore(m_score);
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
}

void Crystal::doSomething(){
    Collectible::doSomething();
    //if collectible sets alive to false then player collocated
    if(isAlive() == false){getWorld()->crystalFound();}
}

void Life::doSomething(){
    Collectible::doSomething();
    if(isAlive() == false){getWorld()->incLives();}
}

void Health::doSomething(){
    Collectible::doSomething();
    if(isAlive() == false){getWorld()->restorePlayerHealth();}
}

void Ammo::doSomething(){
    Collectible::doSomething();
    if(isAlive() == false){getWorld()->increaseAmmo(20);}
}


// Helper functions used in doSomething()


void Actor::isAttacked(){
    m_hitPoints -= 2;
    if(getHealth() <= 0){setAlive(false);}
}

void Avatar::isAttacked(){
    Actor::isAttacked();
    if(getHealth() <= 0){getWorld()->playSound(SOUND_PLAYER_DIE);}
    else{getWorld()->playSound(SOUND_PLAYER_IMPACT);}
}

void Robot::isAttacked(){
    Actor::isAttacked();
    if(getHealth() <= 0){
        getWorld()->increaseScore(m_score);
        getWorld()->playSound(SOUND_ROBOT_DIE);
    }
    else{getWorld()->playSound(SOUND_ROBOT_IMPACT);}
}

void Thiefbot::isAttacked(){
    Robot::isAttacked();
    if(getHealth() <= 0){
        if(m_goodie){
            //update position of goodie and make it visible
            m_goodie->moveTo(getX(), getY());
            m_goodie->setVisible(true);
        }
    }
}

bool Robot::hasClearShot(){
    Avatar* player = getWorld()->getPlayer();
    
    int col = getX();
    int row = getY();
    
    //check if bot facing player and if any obstacles in between
    switch(getDirection()){
        case right:
            if((player->getX() - col > 0) && row == player->getY() && !getWorld()->anyObstaclesOnCol(col, player->getX(), row)){
                getWorld()->addActor(new Pea(col+1, row, right, getWorld()));
                getWorld()->playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            break;
        case left:
            if((col - player->getX() > 0) && row == player->getY() && !getWorld()->anyObstaclesOnCol(player->getX(), col, row)){
                getWorld()->addActor(new Pea(col-1, row, left, getWorld()));
                getWorld()->playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            break;
        case down:
            if((row - player->getY() > 0) && col == player->getX() && !getWorld()->anyObstaclesOnRow(player->getY(), row, col)){
                getWorld()->addActor(new Pea(col, row-1, down, getWorld()));
                getWorld()->playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            break;
        case up:
            if((player->getY() - row > 0) && col == player->getX()&& !getWorld()->anyObstaclesOnRow(row, player->getY(), col)){
                getWorld()->addActor(new Pea(col, row+1, up, getWorld()));
                getWorld()->playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            break;
    }
    return false;
}

bool Agent::canMove(int x, int y) const {
    Actor* dest = getWorld()->getActorAt(x, y);
    if(getWorld()->isPlayerColocatedWith(x, y)){return false;}
    //empty space
    if(dest == nullptr){return true;}
    else if(!dest->isObstacle() && !getWorld()->isPlayerColocatedWith(x, y)){
        //if pit then return false
        if(dest->canTakeMarble()){return false;}
        return true;
    }
    else{return false;}
}

//Only avatars need to check if they can push a marble
bool Avatar::canMove(int x, int y, int dir) const {
    Actor* dest = getWorld()->getActorAt(x, y);
    //empty space
    if(Agent::canMove(x, y)){return true;}
    
    if(dest->isObstacle()){
        switch(dir){
            case up:
                if(dest->canBePushed(x, y+1)){return true;}
                break;
            case down:
                if(dest->canBePushed(x, y-1)){return true;}
                break;
            case right:
                if(dest->canBePushed(x+1, y)){return true;}
                break;
            case left:
                if(dest->canBePushed(x-1, y)){return true;}
                break;
        }
    }
    return false;
}

bool Marble::canBePushed(int x, int y){
    Actor* dest = getWorld()->getActorAt(x, y);
    //if empty space or pit marble can be pushed
    if(dest == nullptr || dest->canTakeMarble()){moveTo(x, y); return true;}
    else{return false;}
}
