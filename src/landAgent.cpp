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

	//leaderId
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

	//leaderId
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

repast::AgentId LandAgent::getLeaderId() {
	return leaderId;
}
void LandAgent::setLeaderId(repast::AgentId _leaderId) {
	leaderId = _leaderId;
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

}

void LandAgent::calculatePayoff() {

}

void LandAgent::manageCoalition() {
	if (id == leaderId) // Leaders cannot leave nor join coalitions, neither they consider trust on a leader
		return;
	bool worstPayoff = true;
	LandAgent* best = neighbours[0];
	int nSize = neighbours.size();
	for (int i=0; i < nSize; i++) {
		if (neighbours[i]->getPayoff() < payoff)
			worstPayoff = false;
		if (neighbours[i]->getPayoff() > best->getPayoff())
			best = neighbours[i];
	}
	if (worstPayoff) {
		if (leaderId == NULL) {
			if (best->getLeaderId() == NULL)
				best->setLeaderId(best->getId());
			leaderId = best->getId();
		} else {
			if (considerTrust) {
				trustLeader += deltaTrust;
				if (trustThreshold > trustLeader)
					leaderId = NULL;
			} else {
				leaderId = NULL;
			}
		}
	} else if (leaderId != NULL)
		incTrust += deltaTrust;
}
