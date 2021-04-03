#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
#include <string>



using namespace std;
const double LEFT_EDGE = ROAD_CENTER - (ROAD_WIDTH/2);
const double RIGHT_EDGE = ROAD_CENTER + (ROAD_WIDTH/2);

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath) //constructor 
: GameWorld(assetPath)
{
    levelBonus = 5000; //initialize level bonus
}

StudentWorld:: ~StudentWorld(){ //destructor
   cleanUp(); // serves same purpose in destructing dynamically allocated objects
}

int StudentWorld::init()
{
    
    allActors.push_back(new GhostRacer(this)); //create a new ghost racer
    
    int N = VIEW_HEIGHT/SPRITE_HEIGHT;
  
    for (int i = 0; i < N; i++){ //yellow border lines
        allActors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, i*SPRITE_HEIGHT, this)); //left border line
        
        allActors.push_back(new BorderLine (IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i*SPRITE_HEIGHT, this)); //right border line
    }
    
    int M = VIEW_HEIGHT/(4*SPRITE_HEIGHT);
    for (int j = 0; j < M; j++){ //white border lines
        allActors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, j*(4*SPRITE_HEIGHT), this)); //left
        allActors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, j*(4*SPRITE_HEIGHT), this));
       

    
        lastYRoadMarker = j*(4*SPRITE_HEIGHT); //update last y coordinate of white border
        
        
        
        
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    levelBonus --; //decrement level bonus
    

    for (int i = 0; i < allActors.size() ; i++){ //call do something for every actor
        if(allActors[i]-> aliveDead() == true){
            allActors[i] -> doSomething(); //tell all actors to dosomething
            if (allActors[0]->aliveDead() == false) return GWSTATUS_PLAYER_DIED; // if ghost racer died
            else if (allActors[0]->getSavedSouls() == (getLevel()*2 + 5)){
                increaseScore(levelBonus); //award any bonus points
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
    
    
    
    // REMOVE AND DELETE DEAD GAME OBJECTS //
    vector <Actor*> :: iterator it;
    it = allActors.begin();
    while (it != allActors.end()){
        if ((*it)->aliveDead() == false){ //if the actor is dead
            delete *it;
            it = allActors.erase(it);
        }
        else {
            it++;
        }
    }
    
    
    //find updated borderline speed
    Actor* road = nullptr;
    for (int i = 0; i < allActors.size() ; i++){
        if(allActors[i]-> isBorderline() == true){ // check if its a borderline and get current speed of borderlines
            road = allActors[i];
        }
    }
    
    // ADD NEW ACTORS TO THE GAME //
    
    // new road markers //
 
    lastYRoadMarker += road->getVertSpeed() - (allActors[0]->getVertSpeed()); //update last y coordinate of white borderline
    
// ADDING NEW BORDERLINES TO GAME //
    
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - (lastYRoadMarker);
    //double delta_y = new_border_y - lastRoadMarker->getY();
    if (delta_y >= SPRITE_HEIGHT){
        allActors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, ROAD_CENTER-(ROAD_WIDTH/2), new_border_y, this)); //left border line
        Actor* tempborder = new BorderLine(IID_YELLOW_BORDER_LINE, ROAD_CENTER+(ROAD_WIDTH/2), new_border_y, this);
        allActors.push_back(tempborder);  //right border line
  
    }
    
    
    if (delta_y >= 4*SPRITE_HEIGHT){
        allActors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, ROAD_CENTER-(ROAD_WIDTH/2) + (ROAD_WIDTH/3), new_border_y, this));
        allActors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, ROAD_CENTER + (ROAD_WIDTH/2) - (ROAD_WIDTH/3), new_border_y, this));
        lastYRoadMarker = new_border_y;
        
    }
    
    // new human peds //
    addSomething (max(200-getLevel()*10, 30), 0);
    
   
    // new zombie peds //
    addSomething (max(100-getLevel()*10, 20), 1);
    
    // new oil slicks //
    addSomething (max(150-getLevel()*10, 40), 2);
    
    // new holy water refill goodies //
    addSomething (100 + 10*getLevel(), 3);
    
    // new Lost Soul goodie //
    addSomething (100, 4);
    

    
    
    //  new zombie cabs //
    
 
    int ChanceVehicle = max(100 - getLevel()*10, 20); // chance of adding a new zombie cab
    int randIntVehicle = randInt(0, ChanceVehicle-1);

    int chosenLane = 0; // vars to keep track of lane, y coordinate, and speed for new object
    int chosenYCoordinate = 0;
    int chosenSpeed = 0;
    
    if (randIntVehicle == 0){
        int cur_lane = randInt(1, 3); //choose a random lane

        for (int i = 0; i < 3; i++){
            Actor* bottom = getNearestCollisionWorthyBottom(cur_lane); // algorithm for determining position of new cab
            if (bottom == nullptr || bottom->getY() > VIEW_HEIGHT/3){
                chosenLane = cur_lane;
                chosenYCoordinate = SPRITE_HEIGHT/2;
                chosenSpeed = getGhost()->getVertSpeed() + randInt(2,4);
                break;
            }
            Actor* top = getNearestCollisionWorthyTop(cur_lane);
            if (top == nullptr || top->getY() < (VIEW_HEIGHT*2/3)){
                chosenLane = cur_lane;
                chosenYCoordinate = VIEW_HEIGHT - SPRITE_HEIGHT/2;
                chosenSpeed = getGhost()->getVertSpeed() - randInt(2,4);
                break;
            }
            
            
            
            if (cur_lane + 1 <= 3){ // check other lanes
                cur_lane += 1;
            } else cur_lane = 1;
        }
        
        
        if (chosenLane == 1){ //chosen lane above in algorithm
            Actor* tempZombie = new ZombieCab (this, ROAD_CENTER-(ROAD_WIDTH/3), chosenYCoordinate); //add new cabs accordingly
            tempZombie->setVertSpeed(chosenSpeed);
            allActors.push_back(tempZombie);
        }
        else if (chosenLane == 2){
            Actor* tempZombie = new ZombieCab (this, ROAD_CENTER, chosenYCoordinate); // coordinates specified in spec
            tempZombie->setVertSpeed(chosenSpeed);
            allActors.push_back(tempZombie);
        }
        else if (chosenLane == 3){
            Actor* tempZombie = new ZombieCab (this, (ROAD_CENTER + ROAD_WIDTH/3), chosenYCoordinate);
            tempZombie->setVertSpeed(chosenSpeed);
            allActors.push_back(tempZombie);
        }
        
        
        
    }
  
   
    
    
    // UPDATE THE GAME STATUS LINE //
    
    ostringstream status;
    status << "Score: " << getScore();
    status << setw(7) << "Lvl: ";
    status << getLevel();
    status << setw(14) << "Souls2Save: ";
    status << (2*getLevel() + 5) - (allActors[0]->getSavedSouls());
    status << setw(9) << "Lives: ";
    status << getLives();
    status << setw(10) << "Health: ";
    status << allActors[0]->getHealth();
    status << setw(10) << "Sprays: ";
    status << allActors[0]->getSpraysLeft();
    status << setw(9) << "Bonus: ";
    status << levelBonus;
    
    string s = status.str();
    setGameStatText(s);
   
    
    
    
    
    
    //decLives();
   
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    
    //frees memory and deletes all dynamically allocated objects
    vector <Actor*> :: iterator it;
    it = allActors.begin();
    while (it != allActors.end()){
            delete *it;
            it = allActors.erase(it);
        
    }
}


// EXTRANEOUS FUNCTIONS //
void StudentWorld:: addSomething (int chance, int type){
    int randomInt = randInt (0, chance-1);
    if (randomInt == 0 && type == 0) { //human pedestrian
        allActors.push_back(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }
    if (randomInt == 0 && type == 1){ //zombie pedestrian
        allActors.push_back(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }
    if (randomInt == 0 && type == 2){ //oil slick
        allActors.push_back(new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
    if (randomInt == 0 & type == 3){ //holy water refill
        allActors.push_back(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
    if (randomInt == 0 & type == 4){ // soul goodie
        allActors.push_back(new SoulGoodie(this, randInt (LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
}


void StudentWorld:: addNewHolyWater( int x, int y, int dir){ //used in actor.cpp
    allActors.push_back(new HolyWaterProj (this, x, y, dir));
}


void StudentWorld :: addNewOilSlick (int x, int y){ // used in actor.cpp
    allActors.push_back(new OilSlick (this, x, y));
}
void StudentWorld:: addNewHealingGoodie (int x, int y){ // used in actor.cpp
    allActors.push_back(new HealingGoodie(this,x, y ));
}


bool StudentWorld :: holyWaterOverlap(Actor *holyWater){ // check if there are objects that overlap with holy water
    for (int i = 0; i < allActors.size(); i++){
        if ( checkOverlap(holyWater, allActors[i]) && allActors[i]->affectedByHolyWater()){ //overlap and affected by holy water
            allActors[i]->damage(-1); // damage actors who overlap
            return true;
        }
    }
    return false;
}








// check for a collision avoidance worthy actor in front of or behind the zombie cab //
bool StudentWorld :: collisionAvoidance(Actor *x, bool ahead){
    int lane = getLane(x); // get lane of zombie cab
    
    for (int i = 0; i < allActors.size(); i++){ //loop through actors
        if (allActors[i]->checkCollisionWorthy() == true && getLane(allActors[i]) == lane){
            if (ahead == true){
                if (allActors[i]->getY() -x->getY() >= 0 && allActors[i]->getY()- x->getY() < 96) return true; // in front of and less than 96 pixels away
            }
            if (ahead == false){
                if (x->getY() - allActors[i]->getY() > 0 && x->getY() - allActors[i]->getY() < 96 && i != 0){ // behind and less than 96 pixels away
                    return true;
                }
            }
        }
    }
    
return false;
}


//return the nearest collision worthy actor to the top //
Actor* StudentWorld :: getNearestCollisionWorthyTop (int lane){
    int distanceToTop = 1000;
    Actor* top = nullptr;
    
    for (int i = 0; i < allActors.size(); i++){ // loop through actors
        if (lane == getLane(allActors[i]) && allActors[i]->checkCollisionWorthy()){ // check lane and collision avoidance worthy
            if (VIEW_HEIGHT - allActors[i]->getY() < distanceToTop){ // get actor closest to top
                distanceToTop = VIEW_HEIGHT - allActors[i]->getY();
                top = allActors[i];
            }
        }
    }
    
    if (top != nullptr)return top;
    return nullptr;
}

Actor* StudentWorld :: getNearestCollisionWorthyBottom(int lane){
    int distanceToBottom = 1000;
    Actor* bottom = nullptr;
    
    for (int i = 0; i < allActors.size(); i++){ // loop through actors
        if (lane == getLane(allActors[i]) && allActors[i]->checkCollisionWorthy()){ // check lane and collision avoidance worthy
            if (allActors[i]->getY() < distanceToBottom){ // get actor closest to bottom
                distanceToBottom = allActors[i]->getY();
                bottom = allActors[i];
            }
        }
    }
    
    if (bottom != nullptr) return bottom;
    return nullptr;

}

