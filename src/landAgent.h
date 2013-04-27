#ifndef  __LANDAGENT_H__
#define  __LANDAGENT_H__

#include <cstdlib>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <repast_hpc/AgentId.h>

class LandAgent: public repast::Agent {

	friend class boost::serialization::access;

private:
	// Agent
	repast::AgentId id;
	int x;
	int y;
	int strategy; // 0 = Random; 1 = TFT; 2 = pTFT
	std::vector<LandAgent*> neighbors;

	// Trust information
	bool considerTrust;
	double deltaTrust;
	double trustThreshold;

	// Status information
	bool isIndependent;
	bool isLeader;

	// Leader information
	int leader[2];
	double trustLeader;

	// Dynamic information
	int action; // 0 = deffect; 1 = cooperate
	double payoff;
	int prevDefectors;

public:
	LandAgent(repast::AgentId _id, int _strategy, bool _considerTrust,
			double _deltaTrust, double _trustThreshold);

	LandAgent(repast::AgentId _id, int _x, int _y, int _strategy,
			bool _considerTrust, double _deltaTrust, double _trustThreshold,
			bool _isIndependent, bool isLeader, int _action);

	~LandAgent();

	repast::AgentId& getId();
	const repast::AgentId& getId() const;

	int getX();
	int getY();
	void setXY(int _x, int _y);

	const std::vector<LandAgent*> getNeighbors() const;
	void setNeighbors(std::vector<LandAgent*> _neighbours);

	int getStrategy();
	void setStrategy(int _strategy);

	int getConsiderTrust();
	void setConsiderTrust(bool _considerTrust);

	double getDeltaTrust();
	void setDeltaTrust(double _deltaTrust);

	double getTrustThreshold();
	void setTrustThreshold(double _threshold);

	bool getIsIndependent();
	void setIsIndependent(bool _isIndependent);

	bool getIsLeader();
	void setIsLeader(bool _isLeader);

	int getLeaderX();
	int getLeaderY();
	void setLeaderXY(int _x, int _y);

	double getTrustLeader();
	void setTrustLeader(double _trustLeader);

	void incTrustLeader();
	void decTrustLeader();

	int getAction();
	void setAction(int _action);

	double getPayoff();
	void setPayoff(double _payoff);

	int getPrevDefectors();
	void setPrevDefectors(int _prevDefectors);

	/**
	 * ACTIONS PERFORMED BY THE AGENTS DURING THE SIMULATION
	 */

	/**
	 * Agent decides its cycle action
	 */
	void decideAction();

	/**
	 * Agent calculates its payoff based on its own action and its neighbors' actions
	 */
	void calculatePayoff(int payoffT, int payoffR, int payoffP, int payoffS);

	/**
	 * Agent decides to join/leave a coalition or stay as it is
	 */
	void manageCoalition();
};

struct LandAgentPackage {
	friend class boost::serialization::access;
	template<class Archive>

	void serialize(Archive & ar, const unsigned int version) {
		ar & id;
		ar & proc;
		ar & type;
		ar & x;
		ar & y;
		ar & strategy;
		ar & considerTrust;
		ar & deltaTrust;
		ar & trustThreshold;
		ar & isIndependent;
		ar & isLeader;
		ar & leader;
		ar & action;
	}

	int id;
	int proc;
	int type;
	int x;
	int y;
	int strategy;
	bool considerTrust;
	double deltaTrust;
	double trustThreshold;
	bool isIndependent;
	bool isLeader;
	int leader[2];
	int action;

	repast::AgentId getId() const {
		return repast::AgentId(id, proc, type);
	}
};

#endif // __LANDAGENT_H__
