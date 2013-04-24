#include "providerUpdater.h"

ProviderUpdater::ProviderUpdater(repast::SharedContext<LandAgent> _agents) {
	agents = _agents;
}

ProviderUpdater::~ProviderUpdater() {
}

LandAgent* ProviderUpdater::createAgent(LandAgentPackage& content) {
	return new LandAgent(content.getId(), content.x, content.y,
			content.strategy, content.considerTrust, content.deltaTrust,
			content.trustThreshold, content.isIndependent, content.isLeader,
			content.action);
}

void ProviderUpdater::createAgents(std::vector<LandAgentPackage>& contents,
		std::vector<LandAgent*>& out) {
	for (std::vector<LandAgentPackage>::iterator agent = contents.begin();
			agent != contents.end(); ++agent) {
		out.push_back(
				new LandAgent(agent->getId(), agent->x, agent->y,
						agent->strategy, agent->considerTrust,
						agent->deltaTrust, agent->trustThreshold,
						agent->isIndependent, agent->isLeader, agent->action));
	}
}

void ProviderUpdater::provideContent(LandAgent* agent,
		std::vector<LandAgentPackage>& out) {
	repast::AgentId id = agent->getId();
	LandAgentPackage package = { id.id(), id.startingRank(), id.agentType(),
			agent->getX(), agent->getY(), agent->getStrategy(),
			agent->getConsiderTrust(), agent->getDeltaTrust(),
			agent->getTrustThreshold(), agent->getIsIndependent(),
			agent->getIsLeader(), agent->getAction() };
	out.push_back(package);
}

void ProviderUpdater::provideContent(const repast::AgentRequest& request,
		std::vector<LandAgentPackage>& out) {
	const std::vector<repast::AgentId>& ids = request.requestedAgents();
	for (int i = 0, size = ids.size(); i < size; i++) {
		repast::AgentId id = ids[i];

		if (agents.contains(id)) {
			LandAgent* agent = agents.getAgent(id);
			LandAgentPackage content = { id.id(), id.startingRank(),
					id.agentType(), agent->getX(), agent->getY(),
					agent->getStrategy(), agent->getConsiderTrust(),
					agent->getDeltaTrust(), agent->getTrustThreshold(),
					agent->getIsIndependent(), agent->getIsLeader(),
					agent->getAction() };
			out.push_back(content);
		}
	}
}

void ProviderUpdater::updateAgent(const LandAgentPackage& content) {
	repast::AgentId id = content.getId();

	if (agents.contains(id)) {
		LandAgent* copy = agents.getAgent(id);
		copy->setXY(content.x, content.y);
		copy->setStrategy(content.strategy);
		copy->setConsiderTrust(content.considerTrust);
		copy->setDeltaTrust(content.deltaTrust);
		copy->setTrustThreshold(content.trustThreshold);
		copy->setIsIndependent(content.isIndependent);
		copy->setIsLeader(content.isLeader);
		copy->setAction(content.action);
	}
}
