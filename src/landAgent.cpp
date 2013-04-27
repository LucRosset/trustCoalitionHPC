#include "landAgent.h"

LandAgent::LandAgent(repast::AgentId _id, int _strategy, bool _considerTrust,
		double _deltaTrust, double _trustThreshold) {
	id = _id;
	x = 0;
	y = 0;
	strategy = _strategy;

	considerTrust = _considerTrust;
	deltaTrust = _deltaTrust;
	trustThreshold = _trustThreshold;

	isIndependent = true;
	isLeader = false;

	leader[0] = -1;
	leader[Y] = -1;
	trustLeader = 0;

	action = 0;
	payoff = 0;
	prevDefectors = 0;
}

LandAgent::LandAgent(repast::AgentId _id, int _x, int _y, int _strategy,
		bool _considerTrust, double _deltaTrust, double _trustThreshold,
		bool _isIndependent, bool _isLeader, int _action) {
	id = _id;
	x = _x;
	y = _y;
	strategy = _strategy;

	considerTrust = _considerTrust;
	deltaTrust = _deltaTrust;
	trustThreshold = _trustThreshold;

	isIndependent = _isIndependent;
	isLeader = _isLeader;

	leader[0] = leader[1] -1;
	trustLeader = 0;

	action = _action;
	payoff = 0;
	prevDefectors = 0;
}

LandAgent::~LandAgent() {
}

repast::AgentId& LandAgent::getId() {
	return id;
}
const repast::AgentId& LandAgent::getId() const {
	return id;
}

int LandAgent::getX() {
	return x;
}
int LandAgent::getY() {
	return y;
}
void LandAgent::setXY(int _x, int _y) {
	x = _x;
	y = _y;
}

const std::vector<LandAgent*> LandAgent::getNeighbors() const {
	return neighbors;
}
void LandAgent::setNeighbors(std::vector<LandAgent*> _neighbors) {
	neighbors = _neighbors;
}

int LandAgent::getStrategy() {
	return strategy;
}
void LandAgent::setStrategy(int _strategy) {
	strategy = _strategy;
}

int LandAgent::getConsiderTrust() {
	return considerTrust;
}
void LandAgent::setConsiderTrust(bool _considerTrust) {
	considerTrust = _considerTrust;
}

double LandAgent::getDeltaTrust() {
	return deltaTrust;
}

void LandAgent::setDeltaTrust(double _deltaTrust) {
	deltaTrust = _deltaTrust;
}

double LandAgent::getTrustThreshold() {
	return trustThreshold;
}

void LandAgent::setTrustThreshold(double _trustThreshold) {
	trustThreshold = _trustThreshold;
}

bool LandAgent::getIsIndependent() {
	return isIndependent;
}
void LandAgent::setIsIndependent(bool _isIndependent) {
	isIndependent = _isIndependent;
}

bool LandAgent::getIsLeader() {
	return isLeader;
}
void LandAgent::setIsLeader(bool _isLeader) {
	isLeader = _isLeader;
}

int LandAgent::getLeaderX() {
	return leader[0];
}
int LandAgent::getLeaderY() {
	return leader[1];
}
void LandAgent::setLeaderXY(int _x, int _y) {
	leader[0] = _x;
	leader[1] = _y;
}

double LandAgent::getTrustLeader() {
	return trustLeader;
}
void LandAgent::setTrustLeader(double _trustLeader) {
	trustLeader = _trustLeader;
}
void LandAgent::incTrustLeader() {
	trustLeader += deltaTrust;
}
void LandAgent::decTrustLeader() {
	trustLeader -= deltaTrust;
}

int LandAgent::getAction() {
	return action;
}
void LandAgent::setAction(int _action) {
	action = _action;
}

double LandAgent::getPayoff() {
	return payoff;
}
void LandAgent::setPayoff(double _payoff) {
	payoff = _payoff;
}

int LandAgent::getPrevDefectors() {
	return prevDefectors;
}
void LandAgent::setPrevDefectors(int _prevDefectors) {
	prevDefectors = _prevDefectors;
}

void LandAgent::decideAction() {
	int Nbrs = neighbors.size();
	int proportion = prevDefectors / (float)Nbrs;
	Nbrs /= 2.0;
	// Random
	if (strategy == 0)
		action = std::rand()%2;
	// TFT | pTFT
	else {
		if (action == 0 && proportion < Nbrs)
			if (strategy == 2 && proportion < std::rand()/(float)RAND_MAX) // pTFT, esle TFT
				return;
			action = 1;
		else if (action == 1 && proportion > Nbrs)
			if (strategy == 2 && proportion > std::rand()/(float)RAND_MAX) // pTFT, else TFT
				return;
			action = 0;
	}
}

void LandAgent::calculatePayoff(int payoffT, int payoffR, int payoffP, int payoffS) {
	int NbrAction = 0;
	payoff = 0;
	// is a coalition member/leader
	if (! isIndependent)
		for (std::vector<LandAgent*>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
			if(*it->getLeaderX() == leader[0] && *it->getLeaderY() == leader[1])
				payoff += payoffR;
			else {
				NbrAction = *it->getAction();
				payoff += (NbrAction == 0) * payoffP + (NbrAction == 1) * payoffT;
			}
		}
	// cooperates
	else if (action)
		for (std::vector<LandAgent*>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
			NbrAction = *it->getAction();
			payoff += (NbrAction == 0) * payoffS + (NbrAction == 1) * payoffR;
		}
	// deffects
	else
		for (std::vector<LandAgent*>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
			NbrAction = *it->getAction();
			payoff += (NbrAction == 0) * payoffP + (NbrAction == 1) * payoffT;
		}

	payoff =/ (float) neighbors.size();
}

void LandAgent::manageCoalition() {
	if (x == leader[0] && y == leader[1]) // Leaders cannot leave nor join coalitions, neither they consider trust on a leader
		return;
	bool worstPayoff = true;
	LandAgent* best = neighbors[0];
	int nSize = neighbors.size();
	for (int i=0; i < nSize; i++) {
		if (neighbors[i]->getPayoff() < payoff)
			worstPayoff = false;
		if (neighbors[i]->getPayoff() > best->getPayoff())
			best = neighbors[i];
	}
	if (worstPayoff) {
		if (leader[0] == -1) {
			if (best->getLeaderX() == -1) {
				best->setLeaderXY(best->getX(), best->getY());
				best->setIsLeader(true);
				best->setIsIndependent(false);
			}
			leader[0] = best->getLeaderX();
			leader[1] = bets->getLeaderY();
		} else {
			if (considerTrust) {
				trustLeader += deltaTrust;
				if (trustThreshold > trustLeader) {
					leader[0] = -1;
					leader[1] = -1;
				}
			} else {
				leader[0] = -1;
				leader[1] = -1;
			}
		}
	} else if (leader[0] != -1)
		trustLeader += deltaTrust;
}
