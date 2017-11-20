

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Field.h"
#include "Compiler.h"
#include "Actor.h"

#include <string>
#include <unordered_map>
#include <map>
#include <list>
using namespace std;

class Actor;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	void updateDisplayText();
	int getTicks() { return m_ticks; }
	int createAnthills();
	void initField(Field::FieldItem item, int x, int y);
	bool getFieldFile(string errorMsg);
	void removeDead();
	bool checkForRock(int x, int y);
	int checkFoodAmt(int x, int y);
	void pushFood(int amt, int x, int y);	void removeFood(int amt, int x, int y);
	void pushAdultG(int x, int y);
	void pushAnt(Ant* ant);
	void pushAnthill(Anthill* anthill);
	void pushPheromone(int x, int y, int col);
	int checkNumEnemies(int x, int y, Actor* self);
	void bite(int x, int y, int amt, int which, Actor* self);
	bool checkForPoison(int x, int y);
	bool checkForWater(int x, int y);
	bool checkForDanger(int x, int y, Actor* actor);
	bool checkForPheromone(int x, int y, int col);
	void incrementAntCt(int colNum);
	bool checkForMax(int& maxNum);
	int checkForWinner();
	string getColNm(int col) { cout << "GETCOLNM";  return m_compNames[col]; }
	void addPheromone(int x, int y, int col);
	pair<int, int> getAnthillCoords(int col) { return m_anthillCoords[col]; }
	void killAnthill(int which);
	bool checkIfAnthillDead(int which);

private:
	std::map<pair<int, int>, std::list<Actor*>> m_actors;
	int m_ticks;
	pair<int, int> m_anthillCoords[4];
	int m_numComps = 0;
	string m_compNames[4];
	Compiler* m_comps[4];
	Anthill* m_anthills[4];
	bool m_areDeadAnthills[4] = { false, false, false, false };
	int m_colAntCt[4];
	bool m_antCtHasChanged;
	bool m_deleted = false;
};

#endif // STUDENTWORLD_H_