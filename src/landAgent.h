#ifndef  __LANDAGENT_H__
#define  __LANDAGENT_H__



class LandAgent : public repast::Agent {

private:

	repast::AgentId id;
	int rank;
	repast::Point<int> coord;
	repast::AgentId leaderId;
	float payoff;
	int strategy;
	int action;
	int NumOfDefNeighbours;
	int considerTrust;
	int trustLeader;
	std::vector<LandAgent*> neighbours;

public:

	LandAgent (repast::AgentId _id, int _rank, int _considerTrust);

	~LandAgent ();

	repast::AgentId& getId();
	const repast::AgentId& getId() const;

	int getX ();
	int getY ();
	repast::Point<int> getCoord ();
	void setX (int x);
	void setY (int y);
	void setCoord (repast::Point<int> _coord);

	repast::AgentId getLeaderId ();
	void setLeaderId (repast::AgentId _leaderId);

	float getPayoff ();
	void setPayoff (float _payoff);

	int getStrategy ();
	void setStrategy(int _strategy);

	int getAction ();
	void setAction (int _action);

	int getNumOfDefNeighbours ();
	void setNumOfDefNeighbours (int _numOfDefNeighbours);

	int getConsiderTrust ();
	void setConsiderTrust (int _considerTrust);

	int getTrsustLeader ();
	void setTrustLeader (int _trustLeader);
	void incTrust (int delta);
	void decTrust (int delta);

	std::vector<LandAgent*> getNeighbours ();
	void setNeighbours (std::vector<LandAgent*> _neighbours);

	/*
	 * Simply returns a random float value from 0 to 1
	 */
	float randomF ();

	/*
	 * Agent decides to cooperate or not with its neighbours
	 */
	void decideAction();
};

struct LandAgentPack {
	friend class boost::serialization::access;
	template <class Archive>

	void serialize (Archive & ar, const unsigned int version) {
		ar & id;
		ar & rank;
		ar & coord;
		ar & leaderId;
		ar & payoff;
		ar & strategy;
		ar & action;
		ar & NumOfDefNeighbours;
		ar & considerTrust;
		ar & trustLeader;
		ar & neighbours;
	}

	repast::AgentId id;
	int rank;
	repast::Point<int> coord;
	repast::AgentId leaderId;
	float payoff;
	int strategy;
	int action;
	int NumOfDefNeighbours;
	int considerTrust;
	int trustLeader;
	std::vector<LandAgent*> neighbours;
};

#endif // __LANDAGENT_H__
