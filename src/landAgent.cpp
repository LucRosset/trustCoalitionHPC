#include "landAgent.h"

LandAgent::LandAgent (repast::AgentId _id, int _rank) {

}

LandAgent::LandAgent (repast::AgentId _id, int _rank, int _coord[2], float _payoff, int _strategy, int _action,
			int _numOfDefNeighbours, int _considerTrust, int _trustLeader, int _threshold) {

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
	return coord[1];
}
const int* LandAgent::getCoord () const{
	return coord;
}
void LandAgent::setX (int x) {
	coord[0] = x;
}
void LandAgent::setY (int y) {
	coord[1] = y;
}
void LandAgent::setCoord (int* _coord) {
	coord[0] = _coord[0];
	coord[1] = _coord[1];
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

int LandAgent::getThreshold () {
	return threshold;
}

void LandAgent::setThreshold (int _threshold) {
	threshold = _threshold;
}

const std::vector<LandAgent*> LandAgent::getNeighbours () const {
	return neighbours;
}
void LandAgent::setNeighbours (std::vector<LandAgent*> _neighbours) {
	for (int i=0; i<_neighbours.size(); i++)
		neighbours[i] = _neighbours[i];
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
