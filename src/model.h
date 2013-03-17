#ifndef  __MODEL_H__
#define  __MODEL_H__




class LandModel {

private:

	int rank;

public:

	repast::SharedContext<LandAgent> agents;
	repast::SharedNetwork <LandAgent, repast::RepastEdge<LandAgent> >* net;
	repast::SharedGrids<LandAgent>::SharedStrictGrid* grid;
	repast::DataSet* dataSet;

	LandModel (boost::mpi::communicator* world);

	~LandModel ();

	/*
	 * Creates a vector with pointers to all the agent's neighbours
	 */
	void neighbourhood (LandAgent* agent, bool moore, bool wrapped);

	/*
	 * Calculate each agent's payoff for this step and apply tax for members of coalitions
	 */
	void updatePayoff(LandAgent* agent);

	/*
	 * Join or leave a coalition, if not a leader
	 */
	void manageCoalition (LandAgent* agent);

	/*
	 * If the agent is the leader of a coalition and there are no members left in it, the agent becomes independent
	 */
	void amIStillLeader (LandAgent* agent);

	void step ();

	void initSchedule ();

};

#endif // __MODEL_H__
