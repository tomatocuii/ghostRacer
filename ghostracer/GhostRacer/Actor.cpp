#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp



 void GhostRacer :: moveAlgorithm(){
    //step 5 in spec -- movement algorithm for ghost racer
    double max_shift_per_tick = 4.0;
    int dir = getDirection();
   double dirRadians = dir*(M_PI/180); // convert to radians
     
    double delta_x = cos(dirRadians)* (max_shift_per_tick);
    double cur_x = getX();
    double cur_y = getY();
    moveTo((cur_x + delta_x), cur_y);
}

bool Actor:: generalMove(){
    
    double vert_speed = getVertSpeed() - (getWorld()->getGhost()->getVertSpeed()); //access the pointer to the world to get the current ghost racer to then get the vertical speed
    int horiz_speed = getHorizSpeed();
    
    double new_y = getY() + vert_speed; //calculate new coordinates
    double new_x = getX() + horiz_speed;
    
    moveTo(new_x, new_y); // move
    
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY()> VIEW_HEIGHT){
        setDead(); //set status to dead if gone off screen
        return false;
    }
    return true;
}

// GHOSTRACER DO SOMETHING FUNCTION //

void GhostRacer :: doSomething(){
    
    if (aliveDead() == false) return; //return immediately if dead
    
    
    //  BORDER CLASHES //
    
    
    if (getX() <= (ROAD_CENTER - (ROAD_WIDTH/2))){ //less than or equal to left road boundary
        if (getDirection() > 90){
            damage(-10); // damage by 10 points
        }
        setDirection(82);
        getWorld()->playSound(SOUND_VEHICLE_CRASH); //play crash
    }
    
    if (getX() >= (ROAD_CENTER + (ROAD_WIDTH/2))){ //greater than or equal to right road boundary
        if (getDirection() < 90){
            damage(-10); //damage by 10 points
        }
        setDirection(98);
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    
    // KEY PRESS DIRECTION/SPEED CHANGE //
    
    int ch;
    getWorld() -> getKey(ch);

    
    if (ch == KEY_PRESS_SPACE && holyWaterUnits > 0){
        double delta_x = SPRITE_HEIGHT* cos(getDirection()*(M_PI/180));
        double delta_y = SPRITE_HEIGHT* sin(getDirection()*(M_PI/180));
        getWorld()->addNewHolyWater (getX() + delta_x, getY() + delta_y, getDirection());
        getWorld()->playSound(SOUND_PLAYER_SPRAY);
        holyWaterUnits--;
        
    }
    
    
    if (ch == KEY_PRESS_LEFT){
        if (getDirection() < 114){
            setDirection(getDirection() + 8); //increase direction by 8 degrees
        }
    }
    if (ch == KEY_PRESS_RIGHT){
        if (getDirection() > 66){
            setDirection(getDirection() - 8); //decrease by 8 degrees
        }
    }
    
    if (ch == KEY_PRESS_UP){
        if (getVertSpeed() < 5){
            setVertSpeed(getVertSpeed() + 1); //increase speed by 1
        }
    }
    if (ch == KEY_PRESS_DOWN){
        if (getVertSpeed() > -1){
            setVertSpeed(getVertSpeed() - 1); //decrease speed by 1
        }
    }
    moveAlgorithm();
   
}
       
// ZOMBIE CAB DO SOMETHING FUNCTION //

void ZombieCab :: doSomething() {
    
    cout <<"zombie cab do something " << endl;
    
    if(aliveDead() == false) return;
    bool x = checkOverlap(this, getWorld()->getGhost());
    if (x == true){
        if (damagedYet == false){ //hasn't damaged yet
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            getWorld()->getGhost()->damage(-20);
            if (getX() <= getWorld()->getGhost()->getX()){ //to the left of ghost racer
                setHorizSpeed(-5);
                setDirection(120 + randInt(0, 19));
            }
            else { //to the right
                setHorizSpeed(5);
                setDirection(60 - randInt(0, 19));
            }
            damagedYet = true; //remember that it damaged ghost racer

        }
    }
    
    
    bool validMove = generalMove(); //move w/ algorithm
    if (validMove == false) return;
    
    if (getVertSpeed() > getWorld()->getGhost()->getVertSpeed()){ //greater vertical speed
        // check collision avoidance worthy
        bool isCloseActor = getWorld()->collisionAvoidance(this, true);
        if (isCloseActor == true){
            setVertSpeed(getVertSpeed() - 0.5);
            return;
        }
        
    }
    if (getVertSpeed() <= getWorld()->getGhost()->getVertSpeed()){ //same or slower
        bool isCloseActor = getWorld()->collisionAvoidance(this, false);
        if (isCloseActor == true){
            setVertSpeed(getVertSpeed() + 0.5);
            return;
        }
    }
    

    planLength--; //decrement plan distance by 1
    if (planLength > 0) return;
    planLength = randInt(4, 32); // pick new movement plan
    setVertSpeed(getVertSpeed() + randInt(-2,2));
}



// BORDERLINE DO SOMETHING FUNCTION //

void BorderLine :: doSomething (){
    bool x = generalMove();
    if (x == false) return;
}

// HUMAN PEDESTRIAN DO SOMETHING FUNCTION //

void Pedestrian:: doSomething(){
    if (aliveDead() == false) return;
    
    if (checkOverlap(this, getWorld()->getGhost()) == true){ //if ped and ghostracer overlap
        ifOverlap(); //call virtual function for diff ped or zombie
        return;
    }
    moveToward(); //for zombie pedestrians
    
    bool x = generalMove(); //move
    if (x == false) return;

    bool check = checkMovementDistance();
    if (check == true) return;
  
  
        // PICK NEW MOVEMENT PLAN
        int randa = randInt(-3, -1); // not including 0
        int randb = randInt(1, 3);
        
        int choose = randInt(0,1);
        if (choose == 0){
            setHorizSpeed(randa); //set horizontal speed
        }
        else setHorizSpeed(randb);
        
        
        movementDis = randInt(4, 32); //set movement plan
        
        if(getHorizSpeed() < 0){
            setDirection(180); //set direction
        }
        else setDirection(0);
    
    
}

// OIL SLICK DO SOMETHING FUNCTION //

void OilSlick :: doSomething(){
    bool x = generalMove();
    if (x == false) return;
    bool overlap = checkOverlap(this, getWorld()->getGhost());
    if (overlap == true){
        getWorld()->playSound(SOUND_OIL_SLICK);
        spinGhost();
        // GHOST RACER SPIN
        
    }
}


// GOODIE DO SOMETHING FUNCTION //

void Goodies:: doSomething (){
    bool x = generalMove();
    if (x == false) return;
    
    if (checkOverlap (this, getWorld()->getGhost())){
        ifOverlapGhost();
    }
    onlySoulGoodie();
    
}

// GOODIE virtual if overlap functions //

void HealingGoodie :: ifOverlapGhost(){
    getWorld()->getGhost()->changeHealth(10); //increase health by 10 points
    setDead();
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->increaseScore(250); //increase score by 250 points
}

void HolyWaterGoodie :: ifOverlapGhost(){
    getWorld()->getGhost()->increaseSprays(10); //increase holy water charges
    setDead();
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->increaseScore(50); //increase score by 50 points
}

void SoulGoodie :: ifOverlapGhost(){
    getWorld()->getGhost()->increaseSoulsSaved();
    setDead();
    getWorld()->playSound(SOUND_GOT_SOUL);
    getWorld()->increaseScore(100); //increase score by 100 points
}

void SoulGoodie :: onlySoulGoodie(){
    setDirection(getDirection() -10);
}


// HOLY WATER PROJECTILE DO SOMETHING FUNCTION //

void HolyWaterProj :: doSomething(){
    if (aliveDead() == false) return; //check alive
    bool ifActivated = getWorld()->holyWaterOverlap(this);
    if (ifActivated){
        setDead();
        return;
    }

  
    for (int i = 0; i < SPRITE_HEIGHT; i++){
        moveForward(); //sprite_height pixels
    }
    travelDistance += SPRITE_HEIGHT; //update travel distance
   
    
    
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY()> VIEW_HEIGHT){
        setDead(); //set status to dead if gone off screen
        return;
    }
    
    if (travelDistance >= 160){
        setDead();
        return;
    }

}



// PEDESTRIAN VIRTUAL FUNCTIONS //
void HumanPedestrian:: ifOverlap() {
    getWorld()->decLives();
    getWorld()->getGhost()->setDead();
    

}
void ZombiePedestrian:: ifOverlap(){
    getWorld()->getGhost()->damage(-5); //ghost racer 5 points of damage
    this->damage(-2); //2 points of damage
}

void ZombiePedestrian:: moveToward(){
    if (abs(this->getX() - (getWorld()->getGhost()->getX())) <= 30 && this->getY() > getWorld()->getGhost()->getY()){ //within 30 pixels and in front of
        setDirection(270);
        if (getX() < getWorld()->getGhost()->getX()){ //to the left of
            setHorizSpeed(1);
        }
        else if (getX() > getWorld()->getGhost()->getX()){ //to the right of
            setHorizSpeed(-1);
        }
        else setHorizSpeed(0);
    }
    ticks--; //decrement ticks
    if (ticks <= 0){
        getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
        ticks = 20;
    }
}

 // EXTRANEOUS FUNCTIONS //

 bool checkOverlap(Actor*a, Actor*b){
     double ax = a->getX();
     double bx = b->getX();
     double delta_x = abs(ax - bx); //absolute value of x coordinate diff
     
     double ay = a->getY();
     double by = b->getY();
     double delta_y = abs(ay - by); // absolute value of y coordinate diff
     
     double radius_sum = (a->getRadius()) + (b->getRadius()); //sum of radiuses
     
     if (delta_x < (radius_sum*0.25) && delta_y < (radius_sum*0.6)){
         return true;
     }
     return false;
 }
int getLane (Actor*a ){
    double leftRoadBorder = ROAD_CENTER-(ROAD_WIDTH/2);
    //double rightRoadBorder = ROAD_CENTER+(ROAD_WIDTH/2);
    double middleLeft = ROAD_CENTER-(ROAD_WIDTH/2) + (ROAD_WIDTH/3);
    double middleRight = ROAD_CENTER+(ROAD_WIDTH/2) - (ROAD_WIDTH/3);
    
    
    
    if (a->getX() >= leftRoadBorder && a->getX() < middleLeft){
        return 1; //left most lane
    }
    
    if (a->getX() >= middleLeft && a->getX() < middleRight){
        return 2;
    }
    
    return 3;
  
}





// DAMAGE FUNCTIONS //
// take x as a negative parameter

void GhostRacer:: damage(int x){ //algorithm for when ghostracer is damaged
    this->changeHealth(x); //lower health points
    if (getHealth() <= 0){
        getWorld()->decLives();
        setDead(); //set dead
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
}

void HumanPedestrian:: damage(int x){ // algorithm for when human ped is damaged
    setHorizSpeed(getHorizSpeed()*-1); //change speed
    setDirection(getDirection()+180);
    getWorld()->playSound(SOUND_PED_HURT);
}

void ZombiePedestrian:: damage(int x){ //algorithm for when zombie ped is damaged
    int zombiex = getX();
    int zombiey = getY();
    changeHealth(x);
    if (getHealth()<= 0){
        setDead();
        getWorld()->playSound(SOUND_PED_DIE);
        
        if (checkOverlap(this, getWorld()->getGhost()) == false) { //if it didn't die from ghostracer overlap
            int chanceOfNewHealingGoodie = randInt(1,5);
            if (chanceOfNewHealingGoodie == 1){
                getWorld()->addNewHealingGoodie(zombiex, zombiey);
            }
        }
        
        getWorld()->increaseScore(150); //increase player score if zombie ped dies

    }
    else getWorld()->playSound(SOUND_PED_HURT);
}

void ZombieCab :: damage (int x){ //zombie cab damage algorithm
    changeHealth(x);
    if (getHealth() <= 0){
        setDead();
        getWorld()->playSound(SOUND_VEHICLE_DIE);
        
        int possibleOilSlick = randInt(1, 5);
        if (possibleOilSlick == 1){ // 1 in 5 chance of adding a new oil slick
            getWorld()->addNewOilSlick(this->getX(), this->getY());
        }
        getWorld()->increaseScore(200); // increase score for dead zombiecab
        return;
    }
    
    getWorld()->playSound(SOUND_VEHICLE_HURT);
    
}


//  SPIN GHOSTRACER FUNCTION


void OilSlick :: spinGhost(){ //function to spin a ghost when it overlaps with an oilslick
    int randomInt = randInt(5, 20);
    int clock = randInt(0,1); // randomize direction
    if (clock == 1){ //counterclockwise
        if (getWorld()->getGhost()->getDirection() + randomInt <= 120){
            getWorld()->getGhost()->setDirection(getWorld()->getGhost()->getDirection() + randomInt);
            return;
        }
        else { // if it goes out of the angular bounds, spin the other direction
            getWorld()->getGhost()->setDirection(getWorld()->getGhost()->getDirection() - randomInt); //clockwise
            return;
        }
    }
    else if (clock == 0){ //counterclockwise
        if (getWorld()->getGhost()->getDirection() - randomInt >= 60){
            getWorld()->getGhost()->setDirection(getWorld()->getGhost()->getDirection() - randomInt);
            return;
        }
        else { // if it goes out of the angular bounds, spin the other direction
            getWorld()->getGhost()->setDirection(getWorld()->getGhost()->getDirection() + randomInt); //counter
            return;
        }
    }
    
    

}
