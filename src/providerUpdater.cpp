#include "providerUpdater.h"

ProviderUpdater::ProviderUpdater(repast::SharedContext<LandAgent> _agents) {
	agents = _agents;
}

ProviderUpdater::~ProviderUpdater() {
}

LandAgent* ProviderUpdater::createAgent(LandAgentPackage& content) {
	return new LandAgent(content.id, content.rank, content.coord, content.payoff, content.strategy, content.action,
			content.numOfDefNeighbours, content.considerTrust, content.trustLeader, content.threshold, content.neighbours);
}

void ProviderUpdater::createAgents(std::vector<LandAgentPackage>& contents,
		std::vector<LandAgent*>& out) {
	for (std::vector<LandAgentPackage>::iterator agent = contents.begin();
			agent != contents.end(); ++agent) {
		out.push_back(new LandAgent(agent->getId(), agent->getRank(), agent->getCoord(), agent->getPayoff(), agent->getStrategy(),
				agent->getAction(), agent->getNumOfDefNeighbours(), agent->getConsiderTrust(), agent->getTrustLeader(),
				agent->getThreshold(), agent->getNeighbours()));
	}
}

void ProviderUpdater::provideContent(LandAgent* agent,
		std::vector<LandAgentPackage>& out) {
	repast::AgentId id = agent->getId();
	LandAgentPackage package = { id.id(), id.startingRank(), id.agentType(),
			agent->getHeading(), agent->getSpeed() };
	out.push_back(package);
}

void ProviderUpdater::provideContent(const repast::AgentRequest& request,
		std::vector<LandAgentPackage>& out) {
	const std::vector<repast::AgentId>& ids = request.requestedAgents();
	for (int i = 0, size = ids.size(); i < size; i++) {
		repast::AgentId id = ids[i];

		if (agents.contains(id)) {
			LandAgent* ag = agents.getAgent(id);
			LandAgentPackage content = { id.id(), id.startingRank(), id.agentType(),
					ag->getHeading(), ag->getSpeed() }; ////////////////////////////////////////////TODO
			out.push_back(content);
		}
	}
}

void ProviderUpdater::updateAgent(const LandAgentPackage& content) {
	repast::AgentId id = content.getId();

	if (agents.contains(id)) {
		LandAgent* copy = agents.getAgent(id);
		copy->setCoord(content.coord);
		copy->setLeaderId(content.leaderId);
		copy->setPayoff(content.payoff);
		copy->setStrategy(content.strategy);
		copy->setAction(content.action);
		copy->setNumOfDefNeighbours(content.numOfDefNeighbours);
		copy->setConsiderTrust(content.considerTrust);
		copy->setTrustLeader(content.trustLeader);
		copy->setThreshold(content.threshold);
		copy->setNeighbours(content.neighbours);
	}
}
