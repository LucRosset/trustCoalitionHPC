#include "landAgent.h"

LandAgent::LandAgent (repast::AgentId _id, int _rank, int _considerTrust) {

}

LandAgent::~LandAgent () {

}

repast::AgentId& LandAgent::getId() {
	return id;
}
const repast::AgentId& LandAgent::getId() const {
	return id;
}

int LandAgent::getX () {
	return coord[0];
}
int LandAgent::getY () {
	return coord[y];
}
repast::Point<int> LandAgent::getCoord () {
	return coord;
}
void LandAgent::setX (int x) {
	coord[0] = x;
}
void LandAgent::setY (int y) {
	coord[1] = y;
}
void LandAgent::setCoord (repast::Point<int> _coord) {
	coord = _coord;
}

repast::AgentId LandAgent::getLeaderId () {
	return leaderId;
}
void LandAgent::setLeaderId (repast::AgentId _leaderId) {
	leaderId = _leaderId;
}

float LandAgent::getPayoff () {
	return payoff;
}
void LandAgent::setPayoff (float _payoff) {
	payoff = _payoff;
}

int LandAgent::getStrategy () {
	return strategy;
}
void LandAgent::setStrategy(int _strategy) {
	strategy = _strategy;
}

int LandAgent::getAction () {
	return action;
}
void LandAgent::setAction (int _action) {
	action = _action;
}

int LandAgent::getNumOfDefNeighbours () {
	return NumOfDefNeighbours;
}
void LandAgent::setNumOfDefNeighbours (int _numOfDefNeighbours) {
	numOfDefNeighbours = _numOfDefNeighbours;
}

int LandAgent::getConsiderTrust () {
	return considerTrust;
}
void LandAgent::setConsiderTrust (int _considerTrust) {
	considerTrust = _considerTrust;
}

int LandAgent::getTrsustLeader () {
	return trustLeader;
}
void LandAgent::setTrustLeader (int _trustLeader) {
	trustLeader = _trustLeader;
}
void LandAgent::incTrust (int delta) {
	trustLeader += delta;
}
void LandAgent::decTrust (int delta) {
	trustLeader -= delta;
}

std::vector<LandAgent*> LandAgent::getNeighbours () {
	return neighbours;
}
void LandAgent::setNeighbours (std::vector<LandAgent*> _neighbours) {
	neighbours = _neighbours;
}

float LandAgent::randomF () {
	return ((float) std::rand())/RAND_MAX;
}

void LandAgent::decideAction() {
	if (leader == NULL) { // Agent is independent
		// pTFT: _action = 0 if condition is satisfied, 1 otherwise.
		if (strategy == 0)
			action = (! _oldNumOfDefNeighbours/_numNeighbours > randomF() );
		// TFT: _action = 0 if condition is satisfied, 1 otherwise.
		else if (strategy == 1)
			action = (! _oldNumOfDefNeighbours > _numNeighbours/2 );
		// _action is chosen randomly.
		else
			action = rand()%2;
	} else // Agent is part of a coalition
		action = 0; // Will cooperate with members of coalition anyway
}
