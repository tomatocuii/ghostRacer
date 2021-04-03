#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp



class StudentWorld;
// BASE ACTOR CLASS //

class Actor : public GraphObject {
    public:
    Actor (int imageID, double startX, double startY, int dir, double size, unsigned int depth, double verticalSpeed, int horizontalSpeed, bool alivedead, StudentWorld* worldin, bool collisionWorthy, int begHealth) : GraphObject (imageID, startX, startY, dir, size, depth){ //constructor
            verticalspeed = verticalSpeed;
            horizontalspeed = horizontalSpeed;
            alive = alivedead;
            world = worldin;
            collision = collisionWorthy;
            health = begHealth;
        }
    
    // DO SOMETHING FUNCTION //
    
        virtual void doSomething () = 0;
    
    //   FUNCTIONS TO RETRIEVE //
    
        double getVertSpeed() const {
            return verticalspeed; }
        int getHorizSpeed() const {
            return horizontalspeed; }
        int getHealth() const {
            return health; }
        virtual bool aliveDead () const{
            return alive; }
        virtual int getSavedSouls () const { // only useful for ghostracer
            return 0; }
        virtual int getSpraysLeft () const { // only useful for ghostracer
            return 0;}
        bool checkCollisionWorthy() {
            return collision; }// to get whether or not its a collision worthy actor
        virtual bool affectedByHolyWater(){
            return false; } //human pedestrian, zombie pedestrian, zombie cabs, healing goodie, holy water goodie
    
    
        StudentWorld* getWorld() { //return pointer to world
            return world; }
    
    
    // FUNCTIONS TO SET //
        void setVertSpeed(double x){ // set speed functions
            verticalspeed = x; }
        void setHorizSpeed(double x){
            horizontalspeed = x;  }
        void setDead () { //to set health to dead
            alive = false;  }
        virtual void damage(int x){};
        virtual void moveAlgorithm (){} // move alg for ghostracer
        bool generalMove(); // move algorithm for other actors
        virtual void increaseSprays(int x){} // only for ghostracer
        virtual void increaseSoulsSaved(){} // only for ghost racer
  
        void changeHealth(int x){
            if (health + x >= 100) {
                health = 100;
                return;
            }
            health += x;
        }
        
    
    
    // IDENTIFY ACTORS
        virtual bool isBorderline(){
            return false; }
    

    
    
    private:
        double verticalspeed;
        int horizontalspeed;
        bool alive;
        StudentWorld* world;
        bool collision;
        int health;
};

// GHOSTRACER CLASS //


class GhostRacer : public Actor {
    public:
        GhostRacer (StudentWorld* world): Actor (IID_GHOST_RACER, 128, 32, 90, 4.0, 0, 0, 0, true, world, true, 100){
            savedSouls = 0; //initialize priv member variables of ghost racer
            holyWaterUnits = 10;
            
        }
        
        virtual bool aliveDead() const{ //access health status
            if (getHealth() <= 0 || Actor::aliveDead() == false) {
                return false;
            }
            else return true;
        }
        virtual int getSpraysLeft() const{
            return holyWaterUnits;
        }
        virtual void increaseSprays(int x){
            holyWaterUnits += x;
        }
       
        
        virtual void doSomething(); 
        virtual int getSavedSouls () const{
            return savedSouls;
        }
        virtual void moveAlgorithm (); //specific move algorithm for ghost racer
        virtual void damage(int x);
        virtual void increaseSoulsSaved(){
            savedSouls++;
        }

    
        
        
    
    private:
        int holyWaterUnits;
        int savedSouls;

      
};

// BORDERLINE CLASS //

class BorderLine : public Actor {
    public:
            BorderLine (int imageID, double startX, double startY, StudentWorld* world): Actor (imageID, startX, startY, 0, 2.0, 2, -4, 0, true, world, false, 0){
                
            }
        virtual void doSomething();
        virtual bool isBorderline(){ // to identify borderlines for y coordinate updating in studentWorld
            return true;
        }
        
    private:
       
};

// HUMAN PEDESTRIAN CLASS //

class Pedestrian : public Actor{
    public:
        Pedestrian (StudentWorld* world, int x, int y, int ID, int size): Actor (ID, x, y, 0, size, 0, -4, 0, true, world, true , 2){
            
            movementDis = 0;
        }
        virtual void doSomething();
        virtual void ifOverlap() = 0;
        virtual void moveToward (){
        }
        virtual bool checkMovementDistance() = 0;
        void decrementMovementDis(){
            movementDis --;
        }
        int getMovementDis(){
            return movementDis;
        }
        virtual bool affectedByHolyWater(){
            return true;
        }
        
        
    
    private:
        int movementDis;
};

class HumanPedestrian: public Pedestrian{
    public:
        HumanPedestrian (StudentWorld*world, int startx, int starty) : Pedestrian (world, startx, starty, IID_HUMAN_PED, 2.0){
            
        }
        virtual void ifOverlap();
        virtual bool checkMovementDistance(){ // differentiated behavior between pedestrians
            decrementMovementDis(); // decrement first
            if (getMovementDis() > 0) return true;
            else return false;
        }
        virtual void damage(int x);


};


class ZombiePedestrian : public Pedestrian {
    public:
        ZombiePedestrian (StudentWorld*world, int startx, int starty) : Pedestrian (world, startx, starty, IID_ZOMBIE_PED, 3.0){
            ticks = 0; //starts with 0 ticks
            
        }
        virtual void ifOverlap();
        virtual void damage(int x);
        virtual void moveToward();
        virtual bool checkMovementDistance(){ // differentiated behavior between pedestrians
            if (getMovementDis() > 0){
                decrementMovementDis();
                return true;
            }
            else return false;
        }
    
    private:
        int ticks;
};


// OIL SLICK CLASS //

class OilSlick : public Actor {
public:
    OilSlick(StudentWorld*world, int x, int y) : Actor (IID_OIL_SLICK, x, y, 0, randInt(2,5), 2, -4, 0, true, world, false, 0){
        
    }
    virtual void doSomething();
    void spinGhost ();
};


// HOLY WATER PROJECTILE //

class HolyWaterProj : public Actor {
    public:
        HolyWaterProj (StudentWorld*world, int x, int y, int dir): Actor (IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1, 0, 0, true, world, false, 0){
            travelDistance = 0;
        }
        virtual void doSomething();
       
        
    private:
        int travelDistance;

};


// GOODIES BASE CLASS //

class Goodies : public Actor {
    public:
        Goodies(StudentWorld* world, int x, int y, int imageID, int dir, double size): Actor (imageID, x, y, dir, size, 2, -4, 0, true, world, false, 0){
            
        }
    virtual void doSomething();
    virtual void ifOverlapGhost() = 0;
    virtual void onlySoulGoodie(){}
    virtual bool affectedByHolyWater()
    {
        return true;
    }
    virtual void damage(int x)= 0;

    
};

class HealingGoodie : public Goodies {
    public:
        HealingGoodie (StudentWorld* world, int x, int y) : Goodies (world, x, y, IID_HEAL_GOODIE, 0, 1.0){
                
        }
    virtual void ifOverlapGhost();
    virtual void damage(int x){
        setDead();
    }
    
            
};


class HolyWaterGoodie : public Goodies {
    public:
        HolyWaterGoodie (StudentWorld* world, int x, int y): Goodies (world, x, y, IID_HOLY_WATER_GOODIE, 90, 2.0){
            
        }
    virtual void ifOverlapGhost();
    virtual void damage(int x){
        setDead();
    }

};


class SoulGoodie : public Goodies {
public:
    SoulGoodie (StudentWorld* world, int x, int y) : Goodies (world, x, y, IID_SOUL_GOODIE, 0, 4.0){
        
    }
    virtual void ifOverlapGhost();
    virtual void onlySoulGoodie();
    virtual bool affectedByHolyWater(){ // only goodie thats not affected by holy water 
        return false;
    }
    virtual void damage(int x){}

};

// ZOMBIE CAB CLASS //


class ZombieCab : public Actor {
    public:
        ZombieCab (StudentWorld* world, int x, int y): Actor (IID_ZOMBIE_CAB, x, y, 90, 4.0, 0, 0, 0, true, world, true, 3){
            planLength = 0;
            damagedYet = false;
        }
    virtual void doSomething();
    virtual void damage(int x);
    virtual bool affectedByHolyWater(){
        return true;
    }

        
    private:
        int planLength;
        bool damagedYet;
};




// EXTRANEOUS FUNCTIONS //

bool checkOverlap(Actor*a, Actor*b); //function to check overlap between objects
int getLane (Actor* a );


#endif // ACTOR_H_
