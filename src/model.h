#ifndef  __MODEL_H__
#define  __MODEL_H__

#include <repast_hpc/AgentId.h>
#include <repast_hpc/AgentRequest.h>
#include <repast_hpc/Point.h>
#include <repast_hpc/Properties.h>
#include <repast_hpc/RepastProcess.h>
#include <repast_hpc/Schedule.h>
#include <repast_hpc/SharedContext.h>
#include <repast_hpc/SharedSpace.h>
#include <repast_hpc/SVDataSetBuilder.h>
#include <repast_hpc/Utilities.h>

#include "landAgent.h"

namespace mpi = boost::mpi;

class LandModel {

private:

	int rank;
	int stopAt;
	// Grid size
	int sizeX, sizeY;
	// Process size
	int dimX, dimY;
	repast::SharedContext<Bird> agents;
	repast::SharedGrids<LandAgent>::SharedWrappedGrid* grid;
	repast::Properties props;
	ProviderUpdater providerUpdater;

	int payoffT, payoffR, payoffP, payoffS;
	float tax;
	int deltaTrust;

public:

	repast::SharedContext<LandAgent> agents;
	repast::SharedNetwork <LandAgent, repast::RepastEdge<LandAgent> >* net;
	repast::SharedGrids<LandAgent>::SharedStrictGrid* grid;
	repast::DataSet* dataSet;

	LandModel (const std::string& propsFile, int argc, char* argv[], mpi::communicator* world);

	virtual ~LandModel ();

	void init ();

	void initSchedule ();

	/*
	 * Creates a vector with pointers to all the agent's neighbours
	 */
	void neighbourhood (LandAgent* agent, bool moore);

	void step ();

	void synchAgents();

	//// Step sub-methods ////

	/*
	 * Calculate each agent's payoff for this step
	 */
	void updatePayoff(LandAgent* agent);

	/*
	 * Leaders apply taxes and distribute total payoff among coalition members
	 */
	void applyTax (LandAgent* lead);

	/*
	 * Join or leave a coalition, if not a leader
	 */
	void manageCoalition (LandAgent* agent);

	/*
	 * If the agent is the leader of a coalition and there are no members left in it, the agent becomes independent
	 */
	void amIStillLeader (LandAgent* lead);

};

#endif // __MODEL_H__
