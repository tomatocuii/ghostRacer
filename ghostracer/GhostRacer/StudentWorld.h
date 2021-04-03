#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>
using namespace std; 

#include "Actor.h"

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld(); 
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addSomething (int chance, int type);
    
    //for use by actor.cpp //
   void addNewHolyWater (int x, int y, int dir);
    void addNewOilSlick (int x, int y);
    void addNewHealingGoodie (int x, int y); 
    
    Actor* getGhost (){ // return pointer to ghost
            return allActors[0];
    }
    
    bool collisionAvoidance (Actor *x, bool ahead); // check for collision avoidance worthy actor ahead of or behind
    Actor* getNearestCollisionWorthyTop (int lane); // return collision avoidance worthy actor nearest to top
    Actor* getNearestCollisionWorthyBottom (int lane); // return collision avoidance worthy actor nearest to bottom
    
    bool holyWaterOverlap(Actor* holyWater); // check and damage actors who overlap with holy water 

private:
    vector <Actor*> allActors; //stl to hold all actors
    double lastYRoadMarker;
    int levelBonus; 
};

#endif // STUDENTWORLD_H_
