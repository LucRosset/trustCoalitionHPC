#include "model.h"

LandModel::LandModel(const std::string& propsFile, int argc, char* argv[],
		mpi::communicator* world) :
		props(propsFile, argc, argv, world), providerUpdater(agents) {

	repast::initializeRandom(props, world);

	repast::RepastProcess* rp = repast::RepastProcess::instance();
	rank = rp->rank();

	// Size of the grid
	sizeX = repast::strToInt(props.getProperty(GRID_MAX_X))
			- repast::strToInt(props.getProperty(GRID_MIN_X)) + 1;
	sizeY = repast::strToInt(props.getProperty(GRID_MAX_Y))
			- repast::strToInt(props.getProperty(GRID_MIN_Y)) + 1;

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

	// Size of the grid managed by each process
	dimX = sizeX / procX;
	dimY = sizeY / procY;

	int originX = grid->dimensions().origin().getX();
	int originY = grid->dimensions().origin().getY();

	// Payoff information
	payoffT = repast::strToInt(props.getProperty(PAYOFF_T));
	payoffR = repast::strToInt(props.getProperty(PAYOFF_R));
	payoffP = repast::strToInt(props.getProperty(PAYOFF_P));
	payoffS = repast::strToInt(props.getProperty(PAYOFF_S));

	// Model information
	rounds = repast::strToInt(props.getProperty(MODEL_ROUNDS));
	tax = repast::strToDouble(props.getProperty(MODEL_TAX));
	considerTrust = repast::strToDouble(
			props.getProperty(MODEL_CONSIDER_TRUST));
	deltaTrust = repast::strToDouble(props.getProperty(MODEL_DELTA_TRUST));
	trustThreshold = repast::strToDouble(
			props.getProperty(MODEL_TRUST_THRESHOLD));
	strategyType = repast::strToInt(props.getProperty(MODEL_STRATEGY_TYPE));
	neighborhood = repast::strToInt(props.getProperty(MODEL_NEIGHBORHOOD));

	repast::IntUniformGenerator randStrategy =
			repast::Random::instance()->createUniIntGenerator(0, 2);

	repast::DoubleUniformGenerator randConsiderTrust =
			repast::Random::instance()->createUniDoubleGenerator(0, 1);

	// Create the agents
	int strategy = strategyType;
	bool cTrust;
	LandAgent* agent;
	for (int i = 0; i < (dimX * dimY); i++) {
		repast::AgentId id(i, rank, 0);

		// Define the strategy
		if (strategyType == 3) {
			strategy = (int) randStrategy.next();
			std::cout << strategy << ";" << std::endl;
		}

		// Define if the agent should consider trust
		if (randConsiderTrust.next() <= considerTrust) {
			cTrust = true;
		} else {
			cTrust = false;
		}

		agent = new LandAgent(id, strategy, cTrust, deltaTrust, trustThreshold);
		agents.addAgent(agent);
		grid->moveTo(agent,
				repast::Point<int>(originX + (i / dimX), originY + (i % dimY)));
		agent->setXY(originX + (i / dimX), originY + (i % dimY));

		// Print on screen
		std::cout << id << " " << rank << " " << originX + (i / dimX) << " "
				<< originY + (i % dimY) << std::endl;
	}

	grid->initSynchBuffer(agents);
	grid->synchBuffer<LandAgentPackage>(agents, providerUpdater,
			providerUpdater);
}

LandModel::~LandModel() {
}

void LandModel::init() {
}

void LandModel::initSchedule(repast::ScheduleRunner& runner) {
	runner.scheduleStop(rounds);

	runner.scheduleEvent(1, 1,
			repast::Schedule::FunctorPtr(
					new repast::MethodFunctor<LandModel>(this,
							&LandModel::step)));

	runner.scheduleEvent(1.05, 1,
			repast::Schedule::FunctorPtr(
					new repast::MethodFunctor<LandModel>(this,
							&LandModel::synchAgents)));
}

void LandModel::neighbourhood(LandAgent* agent, bool moore) {
	std::vector<LandAgent*> neighbours(0);
	LandAgent* out;
	int x = agent->getX();
	int y = agent->getY();
	// if true, then the agent has a neighbor to that direction:
	bool N = false;
	bool S = false;
	bool E = false;
	bool W = false;
	if ((x + 1) % sizeX) {
		out = grid->getObjectAt(repast::Point<int>(x + 1, y));
		neighbours.push_back(out);
		E = true;
	}

	if (x) {
		out = grid->getObjectAt(repast::Point<int>(x - 1, y));
		neighbours.push_back(out);
		W = true;
	}

	if ((y + 1) % sizeY) {
		out = grid->getObjectAt(repast::Point<int>(x, y + 1));
		neighbours.push_back(out);
		S = true;
	}

	if (y) {
		out = grid->getObjectAt(repast::Point<int>(x, y - 1));
		neighbours.push_back(out);
		N = true;
	}

	if (moore) {
		if (E && S) {
			out = grid->getObjectAt(repast::Point<int>(x + 1, y + 1));
			neighbours.push_back(out);
		}
		if (W && S) {
			out = grid->getObjectAt(repast::Point<int>(x - 1, y + 1));
			neighbours.push_back(out);
		}
		if (E && N) {
			out = grid->getObjectAt(repast::Point<int>(x + 1, y - 1));
			neighbours.push_back(out);
		}
		if (W && N) {
			out = grid->getObjectAt(repast::Point<int>(x - 1, y - 1));
			neighbours.push_back(out);
		}
	}
}

void LandModel::step() {
	// Decide Action; All
	for (int i = 0; i < dimX * dimY; i++) {
		repast::AgentId id = repast::AgentId(i, rank, 0);
		if (grid->getLocation(id, position)) {
			agents.getAgent(id)->decideAction();
		}
	}
	// Update Payoff; All
	for (int i = 0; i < dimX * dimY; i++) {
		repast::AgentId id = repast::AgentId(i, rank, 0);
		if (grid->getLocation(id, position)) {
			agents.getAgent(id)->updatePayoff();
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
	// Agent decides to join a coalition, leave the coalition or stay as it is
	for (int i = 0; i < dimX * dimY; i++) {
		repast::AgentId id = repast::AgentId(i, rank, 0);
		if (grid->getLocation(id, position))
			agents.getAgent(id)->manageCoalition();
	}
	// Leaders check if coalition still has other members
	for (int i = 0; i < dimX * dimY; i++) {
		repast::AgentId id = repast::AgentId(i, rank, 0);
		if (grid->getLocation(id, position)) {
			LandAgent* lead = agents.getAgent(id);
			if (lead->getId() == lead->getLeaderId())
				amIStillLeader(agents.getAgent(id));
		}
	}
}

void LandModel::applyTax(LandAgent* lead) {
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

void LandModel::synchAgents() {
	repast::syncAgents<LandAgentPackage>(providerUpdater, providerUpdater);
	grid->synchBuffer<LandAgentPackage>(agents, providerUpdater,
			providerUpdater);
}
