#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <memory>
#include <queue>
class StudentWorld;

class Actor :public GraphObject
{
public:
	Actor(StudentWorld* world, int imageID, int startX, int startY, 
		Direction dir, double size, unsigned int depth);
	virtual ~Actor() {};
	virtual void doSomething() = 0;

	void setHitPts(int p) { hitPoints = p; }
	void setDead() { lifeStat = false; }
	bool getLifeStat() { return lifeStat; }
	int getHitPts() { return hitPoints; }
	void decHitPts(int p) { hitPoints -= 2; }
	StudentWorld* getWorld() { return World; }

	void decTimer();
	void setTimer(int sec) { timer = sec; }
	int getTimer() { return timer; }
	void setGraphic() {
		setVisible(true);
		displaySet = true;
	}
	bool getDisplaySet() { return displaySet; }
private:
	bool lifeStat;
	int hitPoints;
	StudentWorld* World;
	int timer;
	bool displaySet;
};

class Goodies :public Actor
{
public:
	Goodies(StudentWorld* world, int imageID, int startX, int startY, 
		Direction dir, double size, unsigned int depth);
	virtual ~Goodies() {};

};
//------------------------------------------------------
class Boulder :public Goodies
{
public:
	Boulder(StudentWorld* world, int x, int y) :
		Goodies(world, TID_BOULDER, x, y, down, 1.0, 1){
		setVisible(true);
		setTimer(30);
	}
	virtual ~Boulder() {};
	virtual void doSomething();
	bool isProtesterUnderBoulder();
	void setWaiting() { status = "waiting"; }
	void setFalling() { status = "falling"; }
	std::string getStatus() { return status; }

private:
	std::string status = "stable";
	

};
//------------------------------------------------------
class Gold :public Goodies
{
public:
	Gold(StudentWorld* adrr, int StartX, int StartY, std::string owner);
	virtual ~Gold() {};
	virtual void doSomething();
	void pickUpByProtester();

private:
	std::string Ownership;
};
//------------------------------------------------------
class Water :public Goodies
{
public:
	Water(StudentWorld* world, int x, int y);
	virtual ~Water() {};
	virtual void doSomething();

};
//------------------------------------------------------
class Squirt : public Goodies
{
public:
	Squirt(StudentWorld* world, int x, int y, Direction dir);
	virtual ~Squirt() {};
	virtual void doSomething();
	void protesterHurtedBySquirt(int x, int y);
};
//------------------------------------------------------
class Sonar :public Goodies
{
public:
	Sonar(StudentWorld* adrr, int starX, int startY);
	virtual ~Sonar() {};
	virtual void doSomething();
	
};
//------------------------------------------------------
class BarrelOil :public Goodies
{
public:
	BarrelOil(StudentWorld* adrr, int x, int y);
	virtual ~BarrelOil() {};
	virtual void doSomething();

};
//------------------------------------------------------
class Earth : public GraphObject
{
public:
	Earth(int startX, int startY) :
		GraphObject(TID_EARTH, startX, startY, right, 0.25, 3) {
		setVisible(true);
	}
	virtual ~Earth() {};
};
//------------------------------------------------------
class Protester : public Actor
{
public:
	Protester(StudentWorld* world, int image);
	virtual ~Protester() {};
	virtual void doSomething();
	void PmoveToDir(Direction dir);
	bool checkBlockingWay();
	bool canMoveToPerpendicular(std::string dir);
	bool moveToRandomDir();
	void turnToTunnelMan(int Tx, int Ty, int x, int y);
	bool canGoStraight();
	bool canSeeTunnelMan();
	void setTickRest(int t) { tickToRest = t; }
	void setNumSquares(int num) { numSquares = num; }
	void setLeaveOilField() { leaveTheOilField = true; }
	void setCharacterLev(std::string character) { characterLev = character; }
	bool getLeaveOilFieldStat() { return leaveTheOilField; }
	int getNumSquares() { return numSquares; }
	std::string getcharacter() { return characterLev; }

private:
	std::string characterLev;
	int numSquares = 0;
	bool leaveTheOilField = false;
	int currMazeNum = 0;
	int tickToRest = 0;
	int tickNoShout = 15;
	int tickFromLastTurn = 200;
};

class Regular_Protester :public Protester
{
public:
	Regular_Protester(StudentWorld* world) :
		Protester(world, TID_PROTESTER)
	{
		setCharacterLev("regular");
		setHitPts(5);
	}
};
class Hardcore_Protester :public Protester
{
public:
	Hardcore_Protester(StudentWorld* world) :
		Protester(world, TID_HARD_CORE_PROTESTER)
	{
		setCharacterLev("hard");
		setHitPts(20);
	}
};
//------------------------------------------------------
class TunnelMan :public Actor
{
public:
	TunnelMan(StudentWorld* adrr);
	virtual ~TunnelMan() {};
	virtual void doSomething();
	void moveToDir(Direction dir);
	void dropTheGoldNugget();
	void useSonar();

	int getNumOfWater() { return numOfWater; }
	int getNumOfSonar() { return numOfSonar; }
	int getNumOfNugget() { return numOfGN; }
	int getNumOfOil() { return numOfOil; }
	int getScore() { return numOfScore; }
	bool getSwitchTab() { return switchForTab; }
	bool getSwitchSB() { return switchForSB; }

	void addWater() { numOfWater += 5; }
	void addNugget() { numOfGN++; }
	void addSonar() { numOfSonar += 1; }
	void addOil() { numOfOil++; }
	void addScore(int val) { numOfScore += val; }
	void setSwitchTabOff() { switchForTab = false; }
	void setSwitchSBOff() { switchForSB = false; }
	void decGN() { numOfGN--; }
private:
	int numOfWater = 5;
	int numOfSonar = 1;
	int numOfGN = 0;
	int numOfOil = 0;
	int numOfScore = 0;
	bool switchForTab = false;
	bool switchForSB = false;

};

#endif // ACTOR_H_
