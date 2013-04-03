#include "model.h"

const std::string STOP_AT = "stop.at";
const std::string MIN_X = "min.x";
const std::string MIN_Y = "min.y";
const std::string MAX_X = "max.x";
const std::string MAX_Y = "max.y";
const std::string GRID_BUFFER = "grid.buffer";
const std::string PROC_X = "proc.per.x";
const std::string PROC_Y = "proc.per.y";

LandModel::LandModel (const std::string& propsFile, int argc, char* argv[], mpi::communicator* world) {

	repast::RepastProcess* rp = repast::RepastProcess::instance();
	rank = rp->rank();

	stopAt = repast::strToInt(props.getProperty(STOP_AT));

	// Size of the grid
	sizeX = repast::strToInt(props.getProperty(MAX_X))
			- repast::strToInt(props.getProperty(MIN_X)) + 1;
	sizeY = repast::strToInt(props.getProperty(MAX_Y))
			- repast::strToInt(props.getProperty(MIN_Y)) + 1;

	int procX = repast::strToInt(props.getProperty(PROC_X));
	int procY = repast::strToInt(props.getProperty(PROC_Y));

	std::vector<int> procDim;
	procDim.push_back(procX);
	procDim.push_back(procY);

	int gridBuffer = repast::strToInt(props.getProperty(GRID_BUFFER));

	// Create the Grid
	grid = new repast::SharedGrids<LandAgent>::SharedWrappedGrid("grid ",
			repast::GridDimensions(repast::Point<int>(sizeX, sizeY)), procDim,
			gridBuffer, world);
	agents.addProjection(grid);

	// Size of the grid for managed by each process
	dimX = sizeX / procX;
	dimY = sizeY / procY;

	int originX = grid->dimensions().origin().getX();
	int originY = grid->dimensions().origin().getY();

	// Create the agents
	LandAgent* bird;
	for (int i = 0; i < (dimX * dimY); i++) {
		repast::AgentId id(i, rank, 0);
		agent = new LandAgent(id, i * 2, i / 2);
		agents.addAgent(agent);
		grid->moveTo(agent, repast::Point<int>(originX + (i / dimX), originY + (i % dimY)));
		agent->setCoord( ( originX+(i/dimX) , originY+(i%dimY) ) );
		std::cout << id << " " << rank << " " << originX + (i / dimX) << " " << originY + (i % dimY) << std::endl;
	}

	grid->initSynchBuffer(agents);
	grid->synchBuffer<LandAgentPackage>(agents, providerUpdater, providerUpdater);

	for (int i = 0; i < dimX * dimY; i++) {
		repast::AgentId id = repast::AgentId(i, rank, 0);
		if (grid->getLocation(id, position)) {
			neighbourhoodTORUS(agents.getAgent(id), moore);
		}
	}

}

LandModel::~LandModel () {

}

void LandModel::initSchedule () {
	runner.scheduleStop(stopAt);

	runner.scheduleEvent(1, 1,
			repast::Schedule::FunctorPtr(
					new repast::MethodFunctor<LandModel>(this,
							&LandModel::step)));

	runner.scheduleEvent(1.05, 1,
			repast::Schedule::FunctorPtr(
					new repast::MethodFunctor<LandModel>(this,
							&LandModel::synchAgents)));

	// Create a builder for netcdf aggregate data collection
	repast::SVDataSetBuilder testBuilder("./logs/data.csv", ";",
			repast::RepastProcess::instance()->getScheduleRunner().schedule());
}

void LandModel::neighbourhood (LandAgent* agent, bool moore) {
	std::vector<LandAgent*> neighbours(0);
	LandAgent* out;
	int coord[2] = agent->getCoord();
	// if true, then the agent has a neighbour to that direction:
	bool N=false;
	bool S=false;
	bool E=false;
	bool W=false;
	if ((coord[0]+1) % sizeX)
		E = true;
	if (coord[0])
		W = true;
	if ((coord[1]+1) % sizeY)
		S = true;
	if (coord[1])
		N = true;

	if (E) {
		out = grid->getObjectAt(repast::Point<int>(coord[0] + 1, coord[1]));
		neighbours.push_back(out);
	}
	if (W) {
		out = grid->getObjectAt(repast::Point<int>(coord[0] - 1, coord[1]));
		neighbours.push_back(out);
	}
	if (S) {
		out = grid->getObjectAt(repast::Point<int>(coord[0], coord[1] + 1));
		neighbours.push_back(out);
	}
	if (N) {
		out = grid->getObjectAt(repast::Point<int>(coord[0], coord[1] - 1));
		neighbours.push_back(out);
	}
	if (moore) {
		if (E && S) {
			out = grid->getObjectAt(repast::Point<int>(coord[0] + 1, coord[1] + 1));
			neighbours.push_back(out);
		}
		if (W && S) {
			out = grid->getObjectAt(repast::Point<int>(coord[0] - 1, coord[1] + 1));
			neighbours.push_back(out);
		}
		if (E && N) {
			out = grid->getObjectAt(repast::Point<int>(coord[0] + 1, coord[1] - 1));
			neighbours.push_back(out);
		}
		if (W && N) {
			out = grid->getObjectAt(repast::Point<int>(coord[0] - 1, coord[1] - 1));
			neighbours.push_back(out);
		}
	}
}

void LandModel::neighbourhoodTORUS (LandAgent* agent, bool moore) {
	std::vector<LandAgent*> neighbours(0);
	LandAgent* out;
	int coord[2] = agent->getCoord();

	out = grid->getObjectAt(repast::Point<int>((coord[0] + 1) % sizeX, coord[1]));
	neighbours.push_back(out);
	out = grid->getObjectAt(repast::Point<int>(((coord[0] - 1)+sizeX) % sizeX, coord[1]));
	neighbours.push_back(out);
	out = grid->getObjectAt(repast::Point<int>(coord[0], (coord[1] + 1) % sizeY));
	neighbours.push_back(out);
	out = grid->getObjectAt(repast::Point<int>(coord[0], ((coord[1] - 1)+sizeY) % sizeY));
	neighbours.push_back(out);
	if (moore) {
		out = grid->getObjectAt(repast::Point<int>( (coord[0]  + 1) % sizeX,        (coord[1] + 1) % sizeY));
		neighbours.push_back(out);
		out = grid->getObjectAt(repast::Point<int>(((coord[0] - 1)+sizeX) % sizeX,  (coord[1] + 1) % sizeY));
		neighbours.push_back(out);
		out = grid->getObjectAt(repast::Point<int>( (coord[0]  + 1) % sizeX,       ((coord[1] - 1)+sizeY) % sizeY));
		neighbours.push_back(out);
		out = grid->getObjectAt(repast::Point<int>(((coord[0] - 1)+sizeX) % sizeX, ((coord[1] - 1)+sizeY) % sizeY));
		neighbours.push_back(out);
	}
	agent->setNeighbours(neighbours);
}

void LandModel::step () {
	// Update Payoff; All
	for (int i = 0; i < dimX * dimY; i++) {
		repast::AgentId id = repast::AgentId(i, rank, 0);
		if (grid->getLocation(id, position)) {
			updatePayoff(agents.getAgent(id));
		}
	}
	// Apply Tax; Only leaders
	for (int i = 0; i < dimX * dimY; i++) {
		repast::AgentId id = repast::AgentId(i, rank, 0);
		if (grid->getLocation(id, position)) {
			LandAgent* lead = agents.getAgent(id);
			if (lead->getId() == lead->getLeaderId())
				applyTax(lead);
		}
	}
	// Manage Coalitions; Only non-leaders
	for (int i = 0; i < dimX * dimY; i++) {
		repast::AgentId id = repast::AgentId(i, rank, 0);
		if (grid->getLocation(id, position)) {
			LandAgent* ag = agents.getAgent(id);
			if (id != ag->getId())
				manageCoalition(ag);
		}
	}
	// Manage Leaders; Only leaders
	for (int i = 0; i < dimX * dimY; i++) {
		repast::AgentId id = repast::AgentId(i, rank, 0);
		if (grid->getLocation(id, position)) {
			LandAgent* lead = agents.getAgent(id);
			if (lead->getId() == lead->getLeaderId())
				amIStillLeader(agents.getAgent(id));
		}
	}
}

//// Step sub-methods ////

void LandModel::updatePayoff(LandAgent* agent) {
	int aAction, nAction; // agent's and neighbour's actions
	int leader = agent->getLeaderId();
	int numOfNeighbours = agent->getNeighbours().size();
	int numOfDefNeighbours = 0;
	int payoff = 0;

	if (leader == NULL) { // Agent is independent
		for (int i=0; i < numOfNeighbours; i++) {
			aAction = agent->getAction();
			nAction = agent->getNeighbours()[i]->action();
			if (aAction == 0) {
				if (nAction == 0)
					payoff += payoffR;
				else {
					payoff += payoffS;
					numOfDefNeighbours += 1;
				}
			} else {
				if (nAction == 0)
					payoff += payoffT;
				else {
					payoff += payoffP;
					numOfDefNeighbours += 1;
				}
			}
		}
	} else { // Agent is part of a coalition
		for (int i=0; i < numOfNeighbours; i++) {
			if (leader == agent->getNeighbours()[i]->getLeaderId())
				payoff += payoffR;
			else {
				if (agent->getNeighbours()[i]->getAction() == 0) {
					payoff += payoffP;
					numOfDefNeighbours += 1;
				} else // agent->getNeighbours[i]->getAction() == 1
					payoff += payoffT;
			}
		}
	}
	agent->setPayoff(payoff);
	agent->setNumOfDefNeighbours(numOfDefNeighbours);
}

void LandModel::applyTax (LandAgent* lead) {
	int total = 0;
	int coalSize;
	LandAgent* ag;
	std::vector<LandAgent*> coalition(0);
	repast::AgentId leadId = lead->getId();

	for (int i = 0; i < dimX; i++) {
		for (int j = 0; j < dimY; j++) {
			ag = grid->getObjectAt(repast::Point<int>(i, j));
			if (ag->getLeaderId() == leadId && ag->getId() != lead->getId()) {
				coalition.push_back(ag);
				total += ag->getPayoff();
			}
		}
	}
	coalSize = coalition.size();
	if (coalSize != 0) {
		float calcTax = total * (tax/100.0);
		lead->setPayoff(lead->getPayoff() + calcTax);
		total = (total - calcTax)/(1.0 * coalSize);
		for (std::vector<LandAgent*>::iterator it = coalition.begin(); it != coalition.end(); ++it)
			*it->setPayoff(total);
	}
}

void LandModel::manageCoalition (LandAgent* agent) {
	if (agent->getId() == agent->getLeaderId()) ///////////////// Devo deixar isso aqui? Fica redundante...
		return;
	bool worstPayoff = true;
	std::vector<LandAgent*> neighbours = agent->getNeighbours();
	LandAgent* best = neighbours[0];
	int nSize = neighbours.size();
	for (int i=0; i < nSize; i++) {
		if (neighbours[i]->getPayoff() < agent->getPayoff())
			worstPayoff = false;
		if (neighbours[i]->getPayoff() > best->getPayoff())
			best = neighbours[i];
	}
	if (worstPayoff) {
		if (agent->getLeaderId() == NULL) {
			if (best->getLeaderId() == NULL)
				best->setLeaderId(best->getId());
			agent->setLeaderId(best->getId());
		} else {
			if (agent->getConsiderTrust()) {
				agent->decTrust(deltaTrust);
				if (agent->getThreshold() > agent->getTrsustLeader())
					agent->setLeaderId(NULL);
			} else {
				agent->setLeaderId(NULL);
			}
		}
	} else if (agent->getLeaderId() != NULL)
		agent->incTrust(deltaTrust);
}

void LandModel::amIStillLeader (LandAgent* lead) {
	LandAgent* ag;
	for (int i = 0; i < dimX; i++) {
		for (int j = 0; j < dimY; j++) {
			ag = grid->getObjectAt(repast::Point<int>(i, j));
			if (ag->getLeaderId() == leadId && ag->getId() != lead->getId()) {
				return;
			}
		}
	}
	lead->setLeaderId(NULL);
}
