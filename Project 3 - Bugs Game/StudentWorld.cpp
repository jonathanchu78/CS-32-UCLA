#include "StudentWorld.h"
#include "GraphObject.h"
#include "Actor.h"
#include "Field.h"
#include "GameConstants.h"
#include "Compiler.h"

#include <map>
#include <vector>
#include <string>
#include <sstream>  // defines the type std::ostringstream
using namespace std;

int N = 0;

GameWorld* createStudentWorld(string assetDir) {
	return new StudentWorld(assetDir);
}


//////////////// member functions ////////////////
StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir), m_ticks(2000) {
}

StudentWorld::~StudentWorld() {
	cleanUp();
}

int StudentWorld::init() {
	Field f;
	Field::LoadResult l = f.loadField(getFieldFilename());
	string s;
	if (!getFieldFile(s))
		return GWSTATUS_LEVEL_ERROR;

	vector<string> names = getFilenamesOfAntPrograms();
	m_numComps = names.size();
	for (int k = 0; k < names.size(); k++) {
		m_compNames[k] = names[k];
	}

	for (int k = 0; k < 64; k++) // k is x and j is y
		for (int j = 0; j < 64; j++) {
			initField(f.getContentsOf(k, j), k, j);
		}

	createAnthills();

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
	m_ticks--;
	
	for (std::map<pair<int, int>, std::list<Actor*>>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
	{
		for (std::list<Actor*>::iterator it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++) {
			if ((*it2)->isntDead() && (*it2)->getLastTick() != m_ticks) {
				double x, y;
				(*it2)->getAnimationLocation(x, y);

				if ((*it2)->doSomething()) {
					double x2, y2;
					(*it2)->getAnimationLocation(x2, y2);
					(*it2)->setLastTick(m_ticks);
					if (x != x2 || y != y2) { // coordinates changed, move
						pair<int, int> tempPair(x2, y2);
						(m_actors[tempPair]).push_back(*it2);
						(*it).second.erase(it2);
					}
				}
			}

		}
	}

	removeDead();

	updateDisplayText();

	if (m_ticks == 0) {
		return checkForWinner();
	}
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
	if (!m_deleted) {
		for (std::map<pair<int, int>, std::list<Actor*>>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
			for (std::list<Actor*>::iterator it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++) {
				delete (*it2);

				/*if (it3 != (*it).second.begin()) {
					it3--;
					(*it).second.erase(it3);
				}
				it4++;
				if (it4 == (*it).second.end()) {
					(*it).second.erase(it2);
				}*/
			}
		}

		/*for (std::map<pair<int, int>, std::list<Actor*>>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
			if (it != m_actors.begin()) {
				auto it2 = it;
				it2--;
				m_actors.erase(it2);
			}
			auto it3 = it;
			it3++;
			if (it3 == m_actors.end())
				m_actors.erase(it);
		}*/

		for (int k = 0; k < m_numComps; k++) {
			delete m_comps[k];
		}
		m_deleted = true;
	}
}

void StudentWorld::updateDisplayText() {
	int ticks = m_ticks;
	int winningAntNumber;
	string colons[4] = { ": ", ": ", ": ", ": " };
	if (checkForMax(winningAntNumber))
		colons[winningAntNumber] = "*: ";
	string numbers[4];
	string strings[4];
	for (int k = 0; k < m_numComps; k++) {
		numbers[k] = std::to_string(m_colAntCt[k]);
		if (numbers[k].size() == 1)
			numbers[k] = "0" + numbers[k];
	}

	ostringstream oss, oss0, oss1, oss2, oss3;
	oss.setf(ios::fixed);	oss0.setf(ios::fixed);	oss1.setf(ios::fixed);	oss2.setf(ios::fixed);	oss3.setf(ios::fixed);
	oss.precision(2);	oss0.precision(2);	oss1.precision(2);	oss2.precision(2);	oss3.precision(2);
	oss << "Ticks: " << m_ticks << " - ";
	string tickstr = oss.str();
	oss0 << m_compNames[0] << colons[0] << numbers[0];
	strings[0] = oss0.str();
	oss1 << m_compNames[1] << colons[1] << numbers[1];
	strings[1] = oss1.str();
	oss2 << m_compNames[2] << colons[2] << numbers[2];
	strings[2] = oss2.str();
	oss3 << m_compNames[3] << colons[3] << numbers[2];
	strings[3] = oss3.str();
	string total = tickstr;
	for (int k = 0; k < m_numComps; k++) {
		total += strings[k]; total += "  ";
	}
	
	setGameStatText(total);
}

int StudentWorld::createAnthills() {
	std::string error;
	for (int k = 0; k < m_numComps; k++) {
		m_comps[k] = new Compiler;

		if (!m_comps[k]->compile(m_compNames[k], error)) {
			setError(m_compNames[k] + " " + error);
			return GWSTATUS_LEVEL_ERROR;
		}

		pair<int, int> pair = m_anthillCoords[k];
		m_anthills[k] = new Anthill(this, pair.first, pair.second, k, m_comps[k]);
		pushAnthill(m_anthills[k]);
	}
	return 0;
}

void StudentWorld::initField(Field::FieldItem item, int x, int y) {
	if (item == Field::FieldItem::empty)
		return;
	pair<int, int> pair(x, y);
	if (item == Field::FieldItem::anthill0) {
		m_anthillCoords[0] = pair;
	}
	if (item == Field::FieldItem::anthill1) {
		m_anthillCoords[1] = pair;
	}
	if (item == Field::FieldItem::anthill2) {
		m_anthillCoords[2] = pair;
	}
	if (item == Field::FieldItem::anthill3) {
		m_anthillCoords[3] = pair;
	}

	if (item == Field::FieldItem::food)
		m_actors[pair].push_front(new Food(this, x, y, 6000));
	if (item == Field::FieldItem::grasshopper)
		m_actors[pair].push_back(new BabyG(this, x, y));
	if (item == Field::FieldItem::water)
		m_actors[pair].push_back(new WaterPool(this, x, y));
	if (item == Field::FieldItem::rock)
		m_actors[pair].push_back(new Pebble(this, x, y));
	if (item == Field::FieldItem::poison)
		m_actors[pair].push_back(new Poison(this, x, y));
}

bool StudentWorld::getFieldFile(string errorMsg) {
	string fieldFile = this->GameWorld::getFieldFilename();

	Field f;
	string error;
	if (f.loadField(fieldFile, error) != Field::LoadResult::load_success)
	{
		setError(fieldFile + " " + error);
		return false; 
	}
	else {
		return true;
	}
}

void StudentWorld::removeDead() {
	std::list<Actor*>::iterator it2;
	for (std::map<pair<int, int>, std::list<Actor*>>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
	{
		it2 = (*it).second.begin();
		while (it2 != (*it).second.end()) {
			if ((*it2)->getHP() <= 0) { // dead; erase
				auto it3 = it2;
				it2++;
				delete (*it3);
				(*it).second.erase(it3);
			}
			else
				it2++;
		}
	}
}

bool StudentWorld::checkForRock(int x, int y) { // true means there is a rock
	pair<int, int> pair(x, y);
	for (auto it = m_actors[pair].begin(); it != m_actors[pair].end(); it++) {
		if ((*it)->isRock())
			return true;
	}
	return false;
}

int StudentWorld::checkFoodAmt(int x, int y) {
	pair<int, int> pair(x, y);
	for (auto it = m_actors[pair].begin(); it != m_actors[pair].end(); it++)
		if ((*it)->isFood())
			return (*it)->getHP();
	return 0;
}

void StudentWorld::pushFood(int amt, int x, int y) {
	pair<int, int> pair(x, y);
	for (auto it = m_actors[pair].begin(); it != m_actors[pair].end(); it++)
		if ((*it)->isFood()) {
			(*it)->addHP(amt);
			return;
		}

	m_actors[pair].push_front(new Food(this, x, y, amt));
}

void StudentWorld::removeFood(int amt, int x, int y) {
	pair<int, int> pair(x, y);
	for (auto it = m_actors[pair].begin(); it != m_actors[pair].end(); it++)
		if ((*it)->isFood()) {
			(*it)->loseHP(amt);
			return;
		}
	cerr << "NEGATIVE FOOD HP NEGATIVE FOOD HP NEGATIVE FOOD HP" << endl;
}

void StudentWorld::pushAdultG(int x, int y) {
	pair<int, int> pair(x, y);
	m_actors[pair].push_back(new AdultG(this, x, y));
}

void StudentWorld::pushAnt(Ant* ant) {
	pair<int, int> pair(ant->getX(), ant->getY());
	m_actors[pair].push_back(ant);
}

void StudentWorld::pushAnthill(Anthill* anthill) {
	pair<int, int> pair(anthill->getX(), anthill->getY());
	m_actors[pair].push_back(anthill);
}

void StudentWorld::pushPheromone(int x, int y, int col) {
	pair<int, int> pair(x, y);
	int imageID;
	switch (col) {
	case 0: imageID = IID_PHEROMONE_TYPE0; break;
	case 1: imageID = IID_PHEROMONE_TYPE1; break;
	case 2: imageID = IID_PHEROMONE_TYPE2; break;
	case 3: imageID = IID_PHEROMONE_TYPE3; break;
	}
	m_actors[pair].push_back(new Pheromone(this, imageID, x, y));
}

int StudentWorld::checkNumEnemies(int x, int y, Actor* self) {
	int count = 0;
	pair<int, int> pair(x, y);
	for (auto it = m_actors[pair].begin(); it != m_actors[pair].end(); it++)
		if ((*it)->isInsect() && (*it)->isntDead() && !(*it)->isFriendlyAnt(self) && self != (*it)) {
			count++;
		}
	return count;
}

void StudentWorld::bite(int x, int y, int amt, int which, Actor* self) {
	int counter = 0;
	pair<int, int> pair(x, y);
	for (auto it = m_actors[pair].begin(); it != m_actors[pair].end(); it++) {
			if ((*it)->isInsect() && (*it) != self && (*it)->isntDead()) {
				counter++;
				if (counter == which) {
					(*it)->loseHP(amt);
					if (!(*it)->isntDead()) { // dead, erase
						auto it2 = it;
						it++;
						delete (*it2);
						m_actors[pair].erase(it2);
						return;
					}

					if ((*it)->isAnt()) { // if ant, need to track that got bit
						(*it)->getBitten();
					}

					if ((*it)->isAdultG()) { // adult grasshopper biting back
						if (randInt(1, 2) == 1)
							self->loseHP(50);
					}
					return;
				}
			}
		}
}

bool StudentWorld::checkForPoison(int x, int y) {
	pair<int, int> pair(x, y);
	for (list<Actor*>::iterator it = m_actors[pair].begin(); it != m_actors[pair].end(); it++)
		if ((*it)->isPoison())
			return true;

	return false;
}

bool StudentWorld::checkForWater(int x, int y) {
	pair<int, int> pair(x, y);
	for (list<Actor*>::iterator it = m_actors[pair].begin(); it != m_actors[pair].end(); it++)
		if ((*it)->isWater())
			return true;

	return false;
}

bool StudentWorld::checkForDanger(int x, int y, Actor* actor) { // true means dangerous
	pair<int, int> pair(x, y);
	for (list<Actor*>::iterator it = m_actors[pair].begin(); it != m_actors[pair].end(); it++)
		if ((*it)->isDangerous(actor))
			return true;
	return false;
}

bool StudentWorld::checkForPheromone(int x, int y, int col) {
	pair<int, int> pair(x, y);
	for (list<Actor*>::iterator it = m_actors[pair].begin(); it != m_actors[pair].end(); it++)
		if ((*it)->isPheromone() && (*it)->getColNum() == col && (*it)->getHP() > 0)
			return true;
	return false;
}

void StudentWorld::incrementAntCt(int colNum) {
	switch (colNum) {
	case 0: m_colAntCt[0]++; break;
	case 1: m_colAntCt[1]++; break;
	case 2: m_colAntCt[2]++; break;
	case 3: m_colAntCt[3]++; break;
	}
}

bool StudentWorld::checkForMax(int& maxNum) {
	if (m_numComps == 0)
		return false;
	bool b = true;
	for (int k = 0; k < m_numComps; k++) {
		for (int j = 0; j < m_numComps; j++) {
			if (k != j && m_colAntCt[j] >= m_colAntCt[k])
				b = false;
		}
		if (b) {
			maxNum = k; 
			return true;
		}
		b = true;
	}
	return false;
}

int StudentWorld::checkForWinner() {
	string winner; bool theresAWinner = false; bool b = true;
	for (int k = 0; k < m_numComps; k++) {
		for (int j = 0; j < m_numComps; j++) {
			if (k != j && m_colAntCt[j] >= m_colAntCt[k])
				b = false;
		}
		if (b) {
			winner = m_compNames[k];
			theresAWinner = true;
			break;
		}
		b = true;
	}

	if (theresAWinner)
	{
		setWinner(winner);
		return GWSTATUS_PLAYER_WON;
	}
	else
		return GWSTATUS_NO_WINNER;
}

void StudentWorld::addPheromone(int x, int y, int col) {
	int strength;
	pair<int, int> pair(x, y);
	for (list<Actor*>::iterator it = m_actors[pair].begin(); it != m_actors[pair].end(); it++)
		if ((*it)->isPheromone() && (*it)->getColNum() == col) {
			strength = (*it)->getHP();
			if (strength + 256 <= 768)
				(*it)->addHP(256);
			else
				(*it)->setHP(768);
			return;
		}
	// end of loop, no friendly pheromone objects


}

void StudentWorld::killAnthill(int which) {
	m_areDeadAnthills[which] = true;
}

bool StudentWorld::checkIfAnthillDead(int which) {
	return m_areDeadAnthills[which];
}