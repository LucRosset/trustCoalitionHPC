#ifndef __PROVIDERUPDATER_H_INCLUDED__
#define __PROVIDERUPDATER_H_INCLUDED__

#include <vector>

#include <repast_hpc/AgentRequest.h>
#include <repast_hpc/SharedContext.h>

#include "landAgent.h"

class ProviderUpdater {

private:
	repast::SharedContext<LandAgent> agents;

public:
	ProviderUpdater(repast::SharedContext<LandAgent> _agents);

	~ProviderUpdater();

	LandAgent* createAgent(LandAgentPackage& content);

	void createAgents(std::vector<LandAgentPackage>& contents,
			std::vector<LandAgent*>& out);

	void provideContent(LandAgent* agent, std::vector<LandAgentPackage>& out);

	void provideContent(const repast::AgentRequest& request,
			std::vector<LandAgentPackage>& out);

	void updateAgent(const LandAgentPackage& content);
};

#endif // PROVIDERUPDATER_H_INCLUDED
