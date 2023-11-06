/*
 * localconfig.h
 */

#ifndef LOCALCONFIG_H_
#define LOCALCONFIG_H_

#include "mcpbuttons.h"

class LocalConfig {
public:
	LocalConfig();

	void Run();

private:
	McpButtons m_McpButtons;
};

#endif /* LOCALCONFIG_H_ */
