#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "Compiler.h"
#include <string>

class StudentWorld;

class Actor : public GraphObject {
public:
	Actor(StudentWorld* world, int imageID, int startX, int startY, int hp, Direction dir, int depth);
	~Actor();
	virtual bool doSomething() { return false; }
	Direction randDirec(bool a[]);
	StudentWorld* getWorld() { return m_world; }
	std::pair<int, int> getCoordinates(int x, int y);

	void decrementHP() { m_hp--; }	void loseHP(int amt) { m_hp -= amt; }	void kill() { m_hp = 0; }
	int getHP() { return m_hp; }	void addHP(int amt) { m_hp += amt; } void setHP(int amt) { m_hp = amt; }

	void eat(int x, int y, int amt);

	bool checkForRockDir(GraphObject::Direction d);
	int getLastTick() { return m_lastTick; } void setLastTick(int a) { m_lastTick = a; }
	virtual bool isRock() { return false; }	virtual bool isFood() { return false; } virtual bool isInsect() { return false; }
	virtual bool isAdultG(){ return false; } virtual bool isPoison() { return false; } virtual bool isWater() { return false; }
	virtual bool isTriggered() { return false; }  virtual bool isAnt() { return false; } virtual bool isPheromone() { return false; }
	virtual bool isDangerous(Actor* actor);

	virtual void getStunned() {}
	virtual void getPoisoned() {}

	bool isntDead() { if (getHP() <= 0) return false; return true; }
	virtual bool isFriendlyAnt(Actor* actor) { return false; } 
	virtual int getColNum() { return -1; }
	virtual void getBitten() {} virtual void getBlocked() {}

private:
	int m_hp;
	StudentWorld* m_world;
	int m_lastTick;
};


//////////////////////            Triggered           //////////////////////////////
class Triggered : public Actor {
public:
	Triggered(StudentWorld* world, int startX, int startY, int imageID);
	virtual bool isTriggered() { return true; }
};

//////////////////////            Pool of Water           //////////////////////////////
class WaterPool : public Triggered {
public:
	WaterPool(StudentWorld* sw, int x, int y);
	virtual bool isWater() { return true; }
};


//////////////////////            Poison           //////////////////////////////
class Poison : public Triggered {
public:
	Poison(StudentWorld* sw, int x, int y);
	virtual bool isPoison() { return true; }
};


//////////////////////            Pebble           //////////////////////////////
class Pebble : public Actor {
public:
	Pebble(StudentWorld* world, int startX, int startY);
	~Pebble() {};
	virtual bool isRock() { return true; }
private:
};

//////////////////////            Food           //////////////////////////////
class Food : public Actor {
public:
	Food(StudentWorld* world, int startX, int startY, int amt);
	~Food() {};
	virtual bool isFood() { return true; }
private:
	// will use the m_hp data member to specify amt of food
};

#include<iostream>
//////////////////////            Anthill           //////////////////////////////
class Anthill : public Actor {
public:
	Anthill(StudentWorld* world, int startX, int startY, int colNum, Compiler* comp);
	~Anthill() { }
	virtual bool doSomething();
	void giveBirth();
private:
	int m_colNum;
	Compiler* m_comp;
};


//////////////////////            Pheromone           //////////////////////////////
class Pheromone : public Actor {
public:
	Pheromone(StudentWorld* world, int imageID, int startX, int startY);
	~Pheromone() {}
	virtual bool doSomething();
	virtual bool isPheromone() { return true; }
	int getColNum() { return m_colNum; }
private:
	int m_colNum;
};


//////////////////////            Insect           //////////////////////////////
class Insect : public Actor
{
public:
	Insect(StudentWorld* world, int imageID, int startX, int startY, int hp, int depth);
	virtual void getStunned() {	increaseSleepTicks(2); }
	virtual void getPoisoned(); 

	int getSleepTicks() { return m_sleepTicks; }
	void increaseSleepTicks(int amt) { m_sleepTicks += amt; } void decrementSleepTicks() { m_sleepTicks--; }

	void stunOrPoison(int x, int y); void gotStunned() { m_gotStunnedHere = true; }  
	void undoStunned() { m_gotStunnedHere = false; } bool checkStun() { return m_gotStunnedHere; }

	virtual bool isInsect() { return true; }
	void moveForward();
private:
	int m_sleepTicks;
	bool m_gotStunnedHere;
};


//////////////////////            Ant           //////////////////////////////
class Ant : public Insect {
public:
	Ant(StudentWorld* world, int imageID, int startX, int startY, Compiler* comp);
	~Ant() {};
	virtual int getColNum() { return m_colNum; }
	virtual bool doSomething();
	bool simpleInterpreter(bool& moved);
	bool move();
	virtual bool isFriendlyAnt(Actor* actor);
	virtual bool isAnt() { return true; }
	void getBlocked() { m_gotBlocked = true; }
	void getBitten() { m_gotBitten = true; }
	void rotate(std::string clockCounter);
	bool ifChain(int op1, int x, int y);
private:
	int m_colNum;
	Compiler* m_comp;
	int m_foodHeld;
	int m_cmdCt;
	bool m_gotBlocked;
	bool m_gotBitten;
	std::string m_colName;
	int m_instruction = 0;
	int m_randNum = 0;
};

//////////////////////            Grasshopper           //////////////////////////////
class Grasshopper : public Insect {
public:
	Grasshopper(StudentWorld* world, int imageID, int startX, int startY, int hp, int depth);
	~Grasshopper() {};
	int getDist() { return m_distToWalk; }
	void setDist(int dist) { m_distToWalk = dist; }
	void move(int x, int y);
	void eatFood(int x, int y, bool& toReturn);
private:
	int m_distToWalk;
};

///////////////////////           BabyG          ///////////////////////////////
class BabyG : public Grasshopper {
public:
	BabyG(StudentWorld* world, int startX, int startY);
	~BabyG() {}
	virtual bool doSomething();
private:
};

///////////////////////           AdultG          ///////////////////////////////
class AdultG : public Grasshopper {
public:
	AdultG(StudentWorld* world, int startX, int startY);
	~AdultG();
	virtual bool doSomething();
	virtual bool isAdultG(){ return true; }
	virtual void getStunned() {};
	virtual void getPoisoned() {};
private:
};

#endif // ACTOR_H_