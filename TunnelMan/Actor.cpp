#include "Actor.h"
#include "StudentWorld.h"
using namespace std;

Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth), lifeStat(true), hitPoints(0), World(world), timer(0), displaySet(false)
{
	setVisible(true);
}
//------------------------------------------------------
Goodies::Goodies(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	: Actor(world, imageID, startX, startY, dir, size, depth)
{
	setVisible(true);
}
void Actor::decTimer()
{
	timer--;
	if (timer == 0) {
		return;
	}
}
//------------------------------------------------------
void Boulder::doSomething()
{
	TunnelMan* temp = getWorld()->getPlayer();
	if (getLifeStat() == false) {
		return;
	}

	if (getWorld()->checkBoulderCanFall(getX(), getY())) {
		setWaiting();
	}
	else {
		return;
	}

	if (getStatus() == "waiting") {
		if (getTimer() == 0) {
			setFalling();
			getWorld()->playSound(SOUND_FALLING_ROCK);
		}
	}

	if (getStatus() == "falling") {
		if (getWorld()->isThereBoulder(getX(), getY()) || getY() == 0) {
			setDead();
		}
		if (getWorld()->checkBoulderCanFall(getX(), getY()) )
		{
			moveTo(getX(), getY() - 1);
			setDead();
		}
		if (getWorld()->isThereBoulder(temp->getX(), temp->getY())) {
			getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
			temp->setDead();
			temp->addScore(100);
		}
		if (isProtesterUnderBoulder() == true) {
			temp->addScore(500);
		}
	}

}

bool Boulder::isProtesterUnderBoulder()
{
	auto Plist = getWorld()->getP_list();
	auto it = Plist.begin();
	while (it != Plist.end()) {
		if (getWorld()->isThereBoulder((*it)->getX(), (*it)->getY())) {
			(*it)->setLeaveOilField();
			(*it)->getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
			return true;
		}
		it++;
	}

	return false;
}

//------------------------------------------------------
Gold::Gold(StudentWorld* adrr, int startX, int startY, string owner) 
	: Goodies(adrr, TID_GOLD, startX, startY, right, 1.0, 2), Ownership(owner)
{
	if (owner == "Player") {
		setTimer(100);
		setVisible(true);
	}
	else {
		setTimer(0);
		setVisible(false);
	}
}

void Gold::doSomething()
{
	TunnelMan* temp = getWorld()->getPlayer();
	double d = getWorld()->disFormula(getX(), getY(), temp->getX(), temp->getY());

	if (getLifeStat() == false) {
		return;
	}

	if (getDisplaySet() == false && d <= 4.0) {
		setVisible(true);
		setGraphic();
		return;
	}
	else if (getDisplaySet() == true && d <= 3.0 && Ownership == "Earth") {
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		temp->addScore(10);
		temp->addNugget();
	}
	if (Ownership == "Player" && getTimer() == 0) {
		setDead();
	}
	if (Ownership == "Player" && getTimer() > 0) {
		pickUpByProtester();
	}
}

void Gold::pickUpByProtester()
{
	int currLevel = getWorld()->getLevel();
	TunnelMan* man = getWorld()->getPlayer();
	auto Plist = getWorld()->getP_list();
	auto it = Plist.begin();
	while (it != Plist.end()) {
		if (getWorld()->disFormula(getX(), getY(), (*it)->getX(), (*it)->getY()) <= 3.0) {
			if ((*it)->getcharacter() == "regular") {
				(*it)->getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
				(*it)->setLeaveOilField();
				man->addScore(25);
				setDead();
			}
			if ((*it)->getcharacter() == "hard") {
				int ticks_to_stare = max(50, 100 - currLevel * 10);
				(*it)->getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
				(*it)->setTickRest(ticks_to_stare);
				man->addScore(50);
				setDead();
			}
		}
		it++;
	}

}
//------------------------------------------------------
Water::Water(StudentWorld* world, int x, int y) 
	: Goodies(world, TID_WATER_POOL, x, y, right, 1.0, 2)
{
	setVisible(true);
	int currLevel = getWorld()->getLevel();
	int T = max(100, 300 - (10 * currLevel));
	setTimer(T);
}
void Water::doSomething()
{
	TunnelMan* temp = getWorld()->getPlayer();
	double d = getWorld()->disFormula(getX(), getY(), temp->getX(), temp->getY());

	if (getLifeStat() == false) {
		return;
	}
	if (getTimer() > 0 && d <= 3.0) {
		setDead();
		temp->addScore(100);
		temp->addWater();
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
	if (getTimer() == 0) {
		setDead();
	}
}
//------------------------------------------------------
Squirt::Squirt(StudentWorld* world, int x, int y, Direction dir)
	:Goodies(world, TID_WATER_SPURT, x, y, dir, 1.0, 1)
{
	setVisible(false);
}

void Squirt::doSomething()
{
	Direction dir = getDirection();
	int x = getX(), y = getY();
	switch (dir) {
	case left:
		x = getX() - 4;
		if (x < 0 || !getWorld()->isEarthEmpty(x, y) || getWorld()->isThereBoulder(x, y)) {
			setDead();
		}
		break;
	case right:
		x = getX() + 4;
		if (x > 64 || !getWorld()->isEarthEmpty(x, y) || getWorld()->isThereBoulder(x, y)) {
			setDead();
		}
		break;
	case up:
		y = getY() + 4;
		if (y > 60 || !getWorld()->isEarthEmpty(x, y) || getWorld()->isThereBoulder(x, y)) {
			setDead();
		}
		break;
	case down:
		y = getY() - 4;
		if (y < 0 || !getWorld()->isEarthEmpty(x, y) || getWorld()->isThereBoulder(x, y)) {
			setDead();
		}
		break;
	}

	if (getLifeStat() == true) {
		moveTo(x, y);
		setGraphic();
		protesterHurtedBySquirt(x, y);
		setDead();
	}

}

void Squirt::protesterHurtedBySquirt(int x, int y)
{
	TunnelMan* man = getWorld()->getPlayer();
	auto Plist = getWorld()->getP_list();
	int currLevel = getWorld()->getLevel();
	int N = max(50, 100 - currLevel * 10);
	auto it = Plist.begin();
	while (it != Plist.end()) {
		if (getWorld()->disFormula((*it)->getX(), (*it)->getY(), x, y) <= 3.0) {
			(*it)->decHitPts((*it)->getHitPts());
			(*it)->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
			if ((*it)->getHitPts() <= 0) {
				(*it)->getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
				(*it)->setLeaveOilField();
				man->addScore(100);
			}
			else { (*it)->setTickRest(N); }
		}
		it++;
	}


}
//------------------------------------------------------
Sonar::Sonar(StudentWorld* adrr, int starX, int startY) 
	: Goodies(adrr, TID_SONAR, 0, 60, right, 1.0, 2)
{
	setVisible(true);
	int currLevel = getWorld()->getLevel();
	int T = max(100, 300 - (10 * currLevel));
	setTimer(T);
}
void Sonar::doSomething()
{
	TunnelMan* temp = getWorld()->getPlayer();
	double d = getWorld()->disFormula(getX(), getY(), temp->getX(), temp->getY());

	if (getLifeStat() == false) {
		return;
	}
	if (getTimer() > 0 && d <= 3.0) {
		setDead();
		temp->addScore(75);
		temp->addSonar();
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
	if (getTimer() == 0) {
		setDead();
	}
}
//------------------------------------------------------
BarrelOil::BarrelOil(StudentWorld* adrr, int x, int y)
	: Goodies(adrr, TID_BARREL, x, y, right, 1.0, 2)
{
	setVisible(false);
};

void BarrelOil::doSomething()
{
	
	TunnelMan* temp = getWorld()->getPlayer();
	double d = getWorld()->disFormula(getX(), getY(), temp->getX(), temp->getY());

	if (getLifeStat() == false) {
		return;
	}

	if (getDisplaySet() == false && d <= 4.0) {
		setVisible(true);
		setGraphic();
		return;
	}
	else if (getDisplaySet() == true && d <= 3.0) {
		setDead();
		getWorld()->playSound(SOUND_FOUND_OIL);
		temp->addScore(1000);
		temp->addOil();
	}
	
}
//------------------------------------------------------
Protester::Protester(StudentWorld* world, int image):
	Actor(world, image, 60, 60, left, 1.0, 0), characterLev("")
{
	setVisible(true);
	int currLevel = getWorld()->getLevel();
	tickToRest = max(0, 3 - currLevel / 4);
	numSquares = rand() % 53 + 8;
	
}
void Protester::doSomething()
{
	int currLevel = getWorld()->getLevel();
	TunnelMan* player = getWorld()->getPlayer();
	double distance = getWorld()->disFormula(player->getX(), player->getY(), getX(), getY());
	if (getLifeStat() == false) {
		return;
	}
	if (tickToRest > 0) {
		tickToRest--;
		return;
	}
	else {
		tickToRest = max(0, 3 - currLevel / 4);
		tickNoShout++;
		tickFromLastTurn++;
	}
	if (leaveTheOilField == true) {
		if (getX() >= 60 && getY() >= 60) {
			getWorld()->decProtester();
			setDead();
			return;
		}
		getWorld()->moveToExit(this);
		return;
	}

	if (canSeeTunnelMan() == true) {
		if (tickNoShout > 15) {
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			player->decHitPts(player->getHitPts());
			tickNoShout = 0;
		}
		if (player->getHitPts() <= 0) {
			getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
			player->setDead();
		}
		return;
	}

	if (canGoStraight() && distance > 4.0) {
		turnToTunnelMan(player->getX(), player->getY(), getX(), getY());
		PmoveToDir(getDirection());
		numSquares = 0;
		return;
	}

	if (getcharacter() == "hard") {
		int M = 16 + currLevel * 2;
		getWorld()->moveToTunnelMan(this, M);
		PmoveToDir(getDirection());
		return;
	}

	numSquares--;
	if (numSquares <= 0) {
		while (true) {
			moveToRandomDir();
			if (moveToRandomDir() == true) {
				break;
			}
		}
		numSquares = rand() % 53 + 8;
	}

	if (tickFromLastTurn >= 200) {
		if (canMoveToPerpendicular("up") && 
			!canMoveToPerpendicular("down")) {
			setDirection(up);
			moveTo(getX(), getY() + 1);
			tickFromLastTurn = 0;
			numSquares = rand() % 53 + 8;
		}
		else if (!canMoveToPerpendicular("up") &&
			canMoveToPerpendicular("down")) {
			setDirection(down);
			moveTo(getX(), getY() - 1);
			tickFromLastTurn = 0;
			numSquares = rand() % 53 + 8;
		}
		else if (canMoveToPerpendicular("up") &&
			canMoveToPerpendicular("down"))
		{
			int randomDir = rand() % 2;
			switch (randomDir) {
			case 0:
				setDirection(up);
				moveTo(getX(), getY() + 1);
				break;
			case 1:
				setDirection(down);
				moveTo(getX(), getY() - 1);
				break;
			}
			tickFromLastTurn = 0;
			numSquares = rand() % 53 + 8;
		}
		else { PmoveToDir(getDirection()); }
	}

	PmoveToDir(getDirection());

	if (checkBlockingWay() == false) {
		numSquares = 0;
	}

	
}

void Protester::PmoveToDir(Direction dir)
{
	int x = getX();
	int y = getY();

	switch (dir)
	{
	case left:
		x = getX() - 1;
		if (x < 0 || !getWorld()->isEarthEmpty(x, y) || getWorld()->isThereBoulder(x, y)) {
			while (true) {
				moveToRandomDir();
				if (moveToRandomDir() == true) {
					break;
				}
			}
		}
		else {
			moveTo(x, y);
		}
		break;
	case right:
		x = getX() + 1;
		if (x > 60 || !getWorld()->isEarthEmpty(x, y) || getWorld()->isThereBoulder(x, y)) {
			while (true) {
				moveToRandomDir();
				if (moveToRandomDir() == true) {
					break;
				}
			}
		}
		else {
			moveTo(x, y);
		}
		break;
	case up:
		y = getY() + 1;
		if (y > 60 || !getWorld()->isEarthEmpty(x, y) || getWorld()->isThereBoulder(x, y)) {
			while (true) {
				moveToRandomDir();
				if (moveToRandomDir() == true) {
					break;
				}
			}
		}
		else {
			moveTo(x, y);
		}
		break;
	case down:
		y = getY() - 1;
		if (y < 0 || !getWorld()->isEarthEmpty(x, y) || getWorld()->isThereBoulder(x, y)) {
			while (true) {
				moveToRandomDir();
				if (moveToRandomDir() == true) {
					break;
				}
			}
		}
		else {
			moveTo(x, y);
		}
		break;
	}
}
bool Protester::checkBlockingWay()
{
	Direction dir = getDirection();
	switch (dir) {
	case left:
		if (getX() - 1 < 0 || !getWorld()->isEarthEmpty(getX() - 1, getY()) ||
			getWorld()->isThereBoulder(getX() - 1, getY())) {
			return false;
		}
		break;
	case right:
		if (getX() + 1 > 64 || !getWorld()->isEarthEmpty(getX() + 1, getY()) ||
			getWorld()->isThereBoulder(getX() + 1, getY())) {
			return false;
		}
		break;
	case up:
		if (getY() + 1 > 60 || !getWorld()->isEarthEmpty(getX(), getY() + 1) ||
			getWorld()->isThereBoulder(getX(), getY() + 1)) {
			return false;
		}
		break;
	case down:
		if (getY() - 1 < 0 || !getWorld()->isEarthEmpty(getX(), getY() - 1) ||
			getWorld()->isThereBoulder(getX(), getY() - 1)) {
			return false;
		}
		break;
	}
	return true;
}
bool Protester::canMoveToPerpendicular(string dir)
{
	if (dir == "up") {
		if (getY() + 1 < 60 && getWorld()->isEarthEmpty(getX(), getY()+1) &&
			!getWorld()->isThereBoulder(getX(), getY()+1)) {
			return true;
		}
	}
	if (dir == "down") {
		if (getY() - 1 > 0 && getWorld()->isEarthEmpty(getX(), getY() - 1) &&
			!getWorld()->isThereBoulder(getX(), getY() - 1)) {
			return true;
		}
	}

	return false;
}

bool Protester::moveToRandomDir()
{
	int randomDir = rand() % 4;
	switch (randomDir) {
	//left
	case 0:
		if(getX() - 1 > 0 && getWorld()->isEarthEmpty(getX() - 1 ,getY())&&
			!getWorld()->isThereBoulder(getX() - 1, getY())) {
			setDirection(left);
			moveTo(getX() - 1, getY());
			return true;
		}
		break;
	//right
	case 1:
		if (getX() + 1 < 64 && getWorld()->isEarthEmpty(getX() + 1, getY()) &&
			!getWorld()->isThereBoulder(getX() + 1, getY())) {
			setDirection(right);
			moveTo(getX() + 1, getY());
			return true;
		}
		break;
	//up
	case 2:
		if (getY() + 1 < 60 && getWorld()->isEarthEmpty(getX(), getY() + 1) &&
			!getWorld()->isThereBoulder(getX(), getY() + 1)) {
			setDirection(up);
			moveTo(getX(), getY() + 1);
			return true;
		}
		break;

	//down
	case 3:
		if (getY() -1 > 0 && getWorld()->isEarthEmpty(getX(), getY() - 1) &&
			!getWorld()->isThereBoulder(getX(), getY() - 1)) {
			setDirection(down);
			moveTo(getX(), getY() - 1);
			return true;
		}
		break;
	}

	return false;
}

void Protester::turnToTunnelMan(int Tx, int Ty, int x, int y)
{
	if (Tx == x && Ty < y) {
		setDirection(down);
	}
	if (Tx == x && Ty > y) {
		setDirection(up);
	}
	if (Ty == y && Tx < x) {
		setDirection(left);
	}
	if (Ty == y && Tx > x) {
		setDirection(right);
	}
}
bool Protester::canGoStraight()
{
	TunnelMan* man = getWorld()->getPlayer();
	double range = getWorld()->disFormula(man->getX(), man->getY(), getX(), getY());
	auto checkBoulder = getWorld()->getB_list();
	int playerX = man->getX();
	int playerY = man->getY();
	int protX = getX();
	int protY = getY();
	int x = 0, y = 0;
	if (playerX == protX && protY > playerY) {
		auto Boulder_it = checkBoulder.begin();
		while (Boulder_it != checkBoulder.end()) {
			if ((*Boulder_it)->getX() == protX && (*Boulder_it)->getY() <= protY 
				&& (*Boulder_it)->getY() >= playerY) {
				return false;
			}
			Boulder_it++;
		}
		y = playerY;
		while (y < protY) {
			if (getWorld()->isEarthEmpty(protX, y) == false) {
				return false;
			}
			y++;
		}
	}
	if (playerX == protX && protY < playerY) {
		auto Boulder_it = checkBoulder.begin();
		while (Boulder_it != checkBoulder.end()) {
			if ((*Boulder_it)->getX() == protX && (*Boulder_it)->getY() >= protY 
				&& (*Boulder_it)->getY() <= playerY) {
				return false;
			}
			Boulder_it++;
		}
		y = protY;
		while (y < playerY) {
			if (getWorld()->isEarthEmpty(protX, y) == false) {
				return false;
			}
			y++;
		}
	}
	if (playerY == protY && protX > playerX) {
		auto Boulder_it = checkBoulder.begin();
		while (Boulder_it != checkBoulder.end()) {
			if ((*Boulder_it)->getY() == protY && (*Boulder_it)->getX() <= protX
				&& (*Boulder_it)->getX() >= playerX) {
				return false;
			}
			Boulder_it++;
		}
		x = playerX;
		while (x < protX) {
			if (getWorld()->isEarthEmpty(x, protY) == false) {
				return false;
			}
			x++;
		}
	}
	if (playerY == protY && protX < playerX) {
		auto Boulder_it = checkBoulder.begin();
		while (Boulder_it != checkBoulder.end()) {
			if ((*Boulder_it)->getY() == protY && (*Boulder_it)->getX() >= protX
				&& (*Boulder_it)->getX() <= playerX) {
				return false;
			}
			Boulder_it++;
		}
		x = protX;
		while (x < playerX) {
			if (getWorld()->isEarthEmpty(x, protY) == false) {
				return false;
			}
			x++;
		}
	}

	return true;

}
bool Protester::canSeeTunnelMan()
{
	TunnelMan* man = getWorld()->getPlayer();
	bool check = false;
	Direction faceDir = getDirection();
	double range = getWorld()->disFormula(man->getX(), man->getY(), getX(), getY());
	switch(faceDir)
	{
	case left:
		if (man->getX() < getX() && range <= 4.0) {
			check = true;
		}
		break;
	case right:
		if (man->getX() > getX() && range <= 4.0) {
			check = true;
		}
		break;
	case up:
		if (man->getY() > getY() && range <= 4.0) {
			check = true;
		}
		break;
	case down:
		if (man->getY() < getY() && range <= 4.0) {
			check = true;
		}
		break;
	}

	return check;
}

//------------------------------------------------------
TunnelMan::TunnelMan(StudentWorld* adrr) :
	Actor(adrr, TID_PLAYER, 30, 60, right, 1.0, 0)
{
	setVisible(true);
	setHitPts(10);
}

void TunnelMan::useSonar()
{
	vector<Gold*> G_temp = getWorld()->getG_list();
	vector<BarrelOil*> O_temp = getWorld()->getO_list();
	if (numOfSonar == 0) {
		return;
	}
	numOfSonar--;
	auto Gold_it = G_temp.begin();
	while (Gold_it != G_temp.end()) {
		if ((*Gold_it)->getDisplaySet() == false && 
			getWorld()->disFormula((*Gold_it)->getX(), (*Gold_it)->getY(), getX(), getY()) <= 12.0) {
			(*Gold_it)->setGraphic();
		}
		Gold_it++;
	}

	auto Oil_it = O_temp.begin();
	while (Oil_it != O_temp.end()) {
		if ((*Oil_it)->getDisplaySet() == false &&
			getWorld()->disFormula((*Oil_it)->getX(), (*Oil_it)->getY(), getX(), getY()) <= 12.0) {
			(*Oil_it)->setGraphic();
		}
		Oil_it++;
	}
}

void TunnelMan::dropTheGoldNugget()
{
	if (numOfGN <= 0) {
		switchForTab = false;
		return;
	}
	else {
		switchForTab = true;
		numOfGN--;
	}
}

void TunnelMan::moveToDir(Direction dir)
{
	int x = getX();
	int y = getY();

	switch (dir)
	{
	case left:
		x = getX() - 1;
		if (x < 0) {
			x = 0;
		}
		if (getWorld()->isThereBoulder(x, y)) {
			x = getX();
			y = getY();
		}
		break;
	case right:
		x = getX() + 1;
		if (x > 60) {
			x = 60;
		}
		if (getWorld()->isThereBoulder(x, y)) {
			x = getX();
			y = getY();
		}
		break;
	case up:
		y = getY() + 1;
		if (y > 60) {
			y = 60;
		}
		if (getWorld()->isThereBoulder(x, y)) {
			x = getX();
			y = getY();
		}
		break;
	case down:
		y = getY() - 1;
		if (y < 0) {
			y = 0;
		}
		if (getWorld()->isThereBoulder(x, y)) {
			x = getX();
			y = getY();
		}
		break;
	}
	moveTo(x, y);
}

void TunnelMan::doSomething()
{
	if (getHitPts() == 0) {
		setDead();
		return;
	}

	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			moveToDir(left);
			setDirection(left);
			break;
		case KEY_PRESS_RIGHT:
			moveToDir(right);
			setDirection(right);
			break;
		case KEY_PRESS_DOWN:
			moveToDir(down);
			setDirection(down);
			break;
		case KEY_PRESS_UP:
			moveToDir(up);
			setDirection(up);
			break;
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_TAB:
			dropTheGoldNugget();
			break;
		case 'Z': 
		case 'z':
			useSonar();
			break;
		case KEY_PRESS_SPACE:
			if (numOfWater > 0) {
				switchForSB = true;
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				numOfWater--;
			}
			break;
		}
	}

	if (getWorld()->digEarth(getX(), getY())) {
		getWorld()->playSound(SOUND_DIG);
	}

}