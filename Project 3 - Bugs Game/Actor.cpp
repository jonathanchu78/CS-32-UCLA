#include "Actor.h"
#include "StudentWorld.h"
#include "GraphObject.h"

#include <random>
#include <cmath>
#include <map>

GraphObject::Direction randDir() {

	int a = randInt(1, 4);
	switch (a) {
	case 1:
		return GraphObject::Direction::up;
	case 2:
		return GraphObject::Direction::right;
	case 3:
		return GraphObject::Direction::down;
	case 4:
		return GraphObject::Direction::left;
	}

	return GraphObject::Direction::none;
}

////////////////////////            Actor functions             ///////////////////////////// 
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, int hp, Direction dir, int depth) 
	: GraphObject(imageID, startX, startY, dir, depth) {
	m_hp = hp;
	m_world = world;
}

Actor::~Actor() {

}

GraphObject::Direction Actor::randDirec(bool a[]) {
	int count = 0;
	GraphObject::Direction dirs[4];
	if (a[0]) {
		dirs[count] =  up;
		count++;
	} 
	if (a[1]) {
		dirs[count] = right;
		count++;
	}
	if (a[2]) {
		dirs[count] = down;
		count++;
	}
	if (a[3]) {
		dirs[count] = left;
		count++;
	}
	int rand = randInt(0, count - 1);
	return dirs[rand];
}

bool Actor::checkForRockDir(GraphObject::Direction d) {
	int x = getX();
	int y = getY();
	switch (getDirection()) {
	case up:
		return getWorld()->checkForRock(x, y + 1);
		break;
	case right:
		return getWorld()->checkForRock(x + 1, y);
		break;
	case down:
		return getWorld()->checkForRock(x, y - 1);
		break;
	case left:
		return getWorld()->checkForRock(x - 1, y);
		break;
	}
}

pair<int, int> Actor::getCoordinates(int x, int y) {
	pair<int, int> pair;
	switch (getDirection()) {
	case up:
		pair.first = x; pair.second = y + 1;
		break;
	case right:
		pair.first = x + 1; pair.second = y;
		break;
	case down:
		pair.first = x; pair.second = y - 1;
		break;
	case left:
		pair.first = x - 1; pair.second = y;
		break;
	}
	return pair;
}

void Actor::eat(int x, int y, int amt) {
	int food = getWorld()->checkFoodAmt(x, y); // eat if there is food
	if (food > 0) {
		if (food >= amt) {
			getWorld()->removeFood(amt, x, y); addHP(amt);
		}
		else {
			getWorld()->removeFood(food, x, y); addHP(food);
		}
	}
}

bool Actor::isDangerous(Actor* actor) {
	if (isInsect() && !isFriendlyAnt(actor))
		return true;
	if (isTriggered())
		return true;
}


////////////////////////            Triggered functions             /////////////////////////////
Triggered::Triggered(StudentWorld* world, int startX, int startY, int imageID) 
: Actor(world, imageID, startX, startY, 1, right, 2){ }

 
        ////////////////////        WaterPool functions             /////////////////////////////
WaterPool::WaterPool(StudentWorld* world, int startX, int startY) 
: Triggered(world, startX, startY, IID_WATER_POOL){ }


        /////////////////            Poison functions             /////////////////////////////
Poison::Poison(StudentWorld* world, int startX, int startY)
: Triggered(world, startX, startY, IID_POISON) { }



////////////////////////            Pebble functions             ///////////////////////////// 
Pebble::Pebble(StudentWorld* world, int startX, int startY)
	: Actor(world, IID_ROCK, startX, startY, 1, right, 1) { }


////////////////////////            Food functions             /////////////////////////////
Food::Food(StudentWorld* world, int startX, int startY, int amt) 
: Actor(world, IID_FOOD, startX, startY, amt, right, 2){ }


////////////////////////            Anthill functions             ///////////////////////////// 
Anthill::Anthill(StudentWorld* world, int startX, int startY, int colNum, Compiler* comp)
: Actor(world, IID_ANT_HILL, startX, startY, 8999, right, 2), m_colNum(colNum), m_comp(comp){}

bool Anthill::doSomething() {
	int x = getX(); int y = getY();
	decrementHP();
	if (getHP() <= 0) { // dead 
		getWorld()->pushFood(100, x, y);
		getWorld()->killAnthill(m_colNum);
		return false;
	}

	if (getWorld()->checkFoodAmt(x, y) > 0 && isntDead()) { // eat
		eat(x, y, 10000);
		return false;
	}

	while (getHP() >= 2000) { // new ant(s)
		giveBirth();
		loseHP(1500);
		getWorld()->incrementAntCt(m_colNum);
	}
	return false;
}

void Anthill::giveBirth() // used to give birth to a new ant
{
	int imageID;
	switch (m_colNum) {
	case 0: imageID = IID_ANT_TYPE0; break;
	case 1: imageID = IID_ANT_TYPE1; break;
	case 2: imageID = IID_ANT_TYPE2; break;
	case 3: imageID = IID_ANT_TYPE3; break;
	}
	Ant* newAnt = new Ant(getWorld(), imageID, getX(), getY(), m_comp);
	getWorld()->pushAnt(newAnt);
}

////////////////////////            Pheromone functions             ///////////////////////////// 
Pheromone::Pheromone(StudentWorld* world, int imageID, int startX, int startY)
	: Actor(world, imageID, startX, startY, 256, right, 2) {
	switch (imageID) {
	case IID_PHEROMONE_TYPE0:
		m_colNum = 0; break;
	case IID_PHEROMONE_TYPE1:
		m_colNum = 1; break;
	case IID_PHEROMONE_TYPE2:
		m_colNum = 2; break;
	case IID_PHEROMONE_TYPE3:
		m_colNum = 3; break;
	}
}

bool Pheromone::doSomething() {
	decrementHP();
	// dont need to do annything for set status to dead
	return false;
}


////////////////////////            Insect functions             ///////////////////////////// 
Insect::Insect(StudentWorld* world, int imageID, int startX, int startY, int hp, int depth)
: Actor(world, imageID, startX, startY, hp, randDir(), depth){
	m_sleepTicks = 0;
	m_gotStunnedHere = false;
}

void Insect::getPoisoned() { 
	loseHP(150); 
}

void Insect::stunOrPoison(int x, int y) {
	if (!checkStun() && getWorld()->checkForWater(x, y)) {
		getStunned();
		gotStunned();
	}
	if (getWorld()->checkForPoison(x, y))
		getPoisoned();

}

void Insect::moveForward() {
	switch (getDirection()) {
	case up:
		moveTo(getX(), getY() + 1);
		return; 
	case right:
		moveTo(getX() + 1, getY());
		return; 
	case down:
		moveTo(getX(), getY() - 1);
		return; 
	case left:
		moveTo(getX() - 1, getY());
		return;
	}
}

////////////////////////            Ant functions             ///////////////////////////// 
Ant::Ant(StudentWorld* world, int imageID, int startX, int startY, Compiler* comp)
: Insect(world, imageID, startX, startY, 1500, 1), m_comp(comp), m_foodHeld(0), m_cmdCt(0){ 
	switch (imageID) {
	case IID_ANT_TYPE0:
		m_colNum = 0; break;
	case IID_ANT_TYPE1:
		m_colNum = 1; break;
	case IID_ANT_TYPE2:
		m_colNum = 2; break;
	case IID_ANT_TYPE3:
		m_colNum = 3; break;
	}
	m_colName = comp->getColonyName();
}

bool Ant::doSomething() {
	int x = getX(); int y = getY();
	decrementHP();
	if (getHP() <= 0) { // dead
		getWorld()->pushFood(100, x, y);
		return false;
	}
	stunOrPoison(x, y);
	if (getSleepTicks() > 0) {
		decrementSleepTicks();
		return false;
	}

	bool moved;
	if (simpleInterpreter(moved)) {
		if (moved) return true;
		else return false;
	}
}

bool Ant::simpleInterpreter(bool& moved) {
	moved = false;
	int x = getX(); int y = getY();
	string error;
	if (!m_comp->compile(m_comp->getColonyName(), error)) {
		kill();
		return true;
	}

	int counter = 0;
	Compiler::Command cmd;
	for (;;) 
	{
		if (!m_comp->getCommand(m_instruction, cmd)) {
			kill(); // set status to dead
			return true; // error - no such instruction!
		}

		int enemies, rand, food;
		switch (cmd.opcode)
		{
		case Compiler::Opcode::moveForward:
			move(); moved = true;
			m_instruction++; return true;
		case Compiler::Opcode::eatFood:
			if (m_foodHeld > 100) {m_foodHeld -= 100; addHP(100);}
			else if (m_foodHeld > 0) {addHP(m_foodHeld); m_foodHeld = 0;}
			m_instruction++; return true;
		case Compiler::Opcode::dropFood:
			getWorld()->pushFood(m_foodHeld, x, y); 
			m_foodHeld = 0;  
			m_instruction++; return true;
		case Compiler::Opcode::bite:
			enemies = getWorld()->checkNumEnemies(getX(), getY(), this);
			rand = randInt(1, enemies);
			getWorld()->bite(x, y, 15, rand, this);
			m_instruction++; return true;
		case Compiler::Opcode::pickupFood:
			food = getWorld()->checkFoodAmt(x, y);
			if (food >= 400 && m_foodHeld < 1800) { // if theres over 400 available
				if (m_foodHeld + 400 <= 1800) { // if 400 wouldnt reach cap 
					getWorld()->removeFood(400, x, y);
					m_foodHeld += 400;
				}
				else { // if 400 would reach food held cap
					getWorld()->removeFood(1800 - m_foodHeld, x, y);
					m_foodHeld = 1800;
				}
			}
			else if (food > 0 && food < 400) { // if not 400 food is available
				if (m_foodHeld + food <= 1800) { // if cap is not reached
					getWorld()->removeFood(food, x, y);
					m_foodHeld += food;
				}
				else { // if cap is reached
					getWorld()->removeFood(1800 - m_foodHeld, x, y);
					m_foodHeld = 1800;
				}
			}
			m_instruction++; return true; 
		case Compiler::Opcode::emitPheromone:
			getWorld()->addPheromone(x, y, m_colNum);
			m_instruction++; return true;
		case Compiler::Opcode::faceRandomDirection:
			setDirection(randDir());
			m_instruction++; return true;
		case Compiler::Opcode::rotateClockwise:
			rotate("clockwise");
			m_instruction++; return true;
		case Compiler::Opcode::rotateCounterClockwise:
			rotate("counterclockwise");
			m_instruction++; return true;
		case Compiler::Opcode::generateRandomNumber:
			if (stoi(cmd.operand1) == 0)
				m_randNum = 0;
			else
				m_randNum = randInt(1, stoi(cmd.operand1) - 1);
			m_instruction++; break;
		case Compiler::Opcode::goto_command:
			m_instruction = stoi(cmd.operand1);
			break;
		case Compiler::Opcode::if_command:
			if (ifChain(stoi(cmd.operand1), x, y)) {
				m_instruction = stoi(cmd.operand2);
			}
			else
				m_instruction++;
			break;
		}
		counter++;
		if (counter == 10)
			return true;
	}
}

bool Ant::move() {
	if (checkForRockDir(getDirection())) {
		getBlocked();
		return false;
	}
	moveForward();
	m_gotBitten = false;
	m_gotBlocked = false;
	undoStunned();
	return true;
}

bool Ant::isFriendlyAnt(Actor* actor) {
	if (!actor->isAnt())
		return false;
	else {
		if (getColNum() == actor->getColNum())
			return true;
	}
	return false;
}

void Ant::rotate(string clockCounter) {
	Direction d = getDirection();
	if (clockCounter == "clockwise") {
		switch (d) {
		case up: setDirection(right); break;
		case right: setDirection(down); break;
		case down: setDirection(left); break;
		case left: setDirection(up); break;
		}
	}
	else if (clockCounter == "counterclockwise") {
		switch (d) {
		case up: setDirection(left); break;
		case right: setDirection(up); break;
		case down: setDirection(right); break;
		case left: setDirection(down); break;
		}
	}
}

bool Ant::ifChain(int op1, int x, int y) {
	pair<int, int> cur(x, y);
	pair<int, int> next = getCoordinates(x, y);
	switch (op1) {
	case 0:
		return getWorld()->checkForDanger(next.first, next.second, this);
	case 1:
		return getWorld()->checkForPheromone(x, y, m_colNum);
	case 2:
		return m_gotBitten;
	case 3:
		if (m_foodHeld > 0) return true;
		else return false;
	case 4:
		if (getHP() <= 25) return true;
		else return false;
	case 5:
		if (!getWorld()->checkIfAnthillDead(m_colNum) && cur == getWorld()->getAnthillCoords(m_colNum)) return true;
		else return false;
	case 6:
		if (getWorld()->checkFoodAmt(x, y) > 0) return true;
		else return false;
	case 7:
		return getWorld()->checkForDanger(x, y, this);
	case 8:
		return m_gotBlocked;
	case 9:
		if (m_randNum == 0) return true;
		else return false;
	}

}

////////////////////////            Grasshopper functions             ///////////////////////////// 
Grasshopper::Grasshopper(StudentWorld* world, int imageID, int startX, int startY, int hp, int depth)
	: Insect(world, imageID, startX, startY, hp, depth) { 
	m_distToWalk = randInt(2, 10);
}

void Grasshopper::move(int x, int y) {
	if (getDist() <= 0 || checkForRockDir(getDirection())) {
		// find out which bordering spots have pebbles
		bool b[4] = { true, true, true, true };
		if (getWorld()->checkForRock(x, y + 1)) b[0] = false;	if (getWorld()->checkForRock(x + 1, y)) b[1] = false;
		if (getWorld()->checkForRock(x, y - 1)) b[2] = false;	if (getWorld()->checkForRock(x - 1, y)) b[3] = false;

		if (!checkForRockDir(getDirection())) {
			setDirection(randDirec(b));
			setDist(randInt(2, 10));
		}
		else {
			setDirection(randDirec(b));
			setDist(randInt(2, 10));
			return;
		}
	}

	setDist(getDist() - 1);
	moveForward();
}

void Grasshopper::eatFood(int x, int y, bool& toReturn) {
	eat(x, y, 200);

	if (randInt(1, 2) == 1) // 50% chance want to rest
		toReturn = true;
}

      //////////////////            BabyG functions             ///////////////////////////// 
BabyG::BabyG(StudentWorld* world, int startX, int startY)
	: Grasshopper(world, IID_BABY_GRASSHOPPER, startX, startY, 500, 1) {}

bool BabyG::doSomething() {
	int x = getX(); int y = getY();
	decrementHP();
	if (getHP() <= 0) { // dead
		getWorld()->pushFood(100, x, y);
		return false;
	}

	stunOrPoison(x, y);

	if (getSleepTicks() > 0) {
		decrementSleepTicks();
		return false;
	}

	if (getSleepTicks() == 0) { // every 3 turns
		if (getHP() >= 1600) { // turn into an adult grasshopper
			getWorld()->pushAdultG(x, y);
			kill();
			getWorld()->pushFood(100, x, y);
			return false;
		}
		bool b = false;
		if (getWorld()->checkFoodAmt(x, y) > 0) {
			eatFood(x, y, b);
			if (b) {
				increaseSleepTicks(2);
				return false;
			}
		}
		move(getX(), getY());
		undoStunned();
		increaseSleepTicks(2);
		return true;
	}
	return false;
}


       //////////////////            AdultG functions             ///////////////////////////// 
AdultG::AdultG(StudentWorld* world, int startX, int startY)
	: Grasshopper(world, IID_ADULT_GRASSHOPPER, startX, startY, 1600, 1) {
}

AdultG::~AdultG() {}

bool AdultG::doSomething() {
	decrementHP();
	if (getHP() <= 0) { // dead
		getWorld()->pushFood(100, getX(), getY());
		return false;
	}

	if (getSleepTicks() > 0) {
		decrementSleepTicks();
		return false;
	}

	bool moved = false;
	if (getSleepTicks() == 0) { // going to do something this round
		int x = getX();
		int y = getY();

		int enemies = getWorld()->checkNumEnemies(x, y, this);
		if (randInt(1, 3) == 2 && enemies > 0) { 
			int rand = randInt(1, enemies);
			getWorld()->bite(x, y, 50, rand, this);
		}
		else if (randInt(1, 10) == 5) {
			int deg; 
			double rad; 
			double dist; 
			bool b = true;
			int x1;
			int y1;

			while (b) {
				deg = randInt(1, 360);
				rad = deg * (2 * 3.1415926535) / 360;
				dist = randInt(1, 1000);
				dist /= 100;
				x1 = x + cos(rad)*dist;
				y1 = y + sin(rad)*dist;
				
				if (x1 > 0 && x1 < 64 && y1 > 0 && y1 < 64 && !getWorld()->checkForRock(x1, y1)) {
					moveTo(x1, y1);
					b = false;
				}
			}
		}
		else {
			bool b = false;
			if (getWorld()->checkFoodAmt(x, y) > 0) {
				eatFood(x, y, b);
				if (b)
					return false;
			}
			move(x, y); moved = true;
		}
		increaseSleepTicks(2);
	}
	if (moved) return true;
	else return false;
}



