/*
 * mv88e6161.cpp
 */

#include <cstdint>
#include <cstdio>
#include <cassert>

#include "port.h"

#include "device/emac/mv88e6161.h"

#include "debug.h"

namespace dsa {
static constexpr uint16_t MV88E6XXX_PHY_STS_DUPLEX = (1U << 13);
static constexpr uint16_t MV88E6XXX_PHY_STS_LINK = (1U << 10);

bool phy_get_status(const uint32_t nPhyIndex, dsa::PhyStatus& phyStatus) {
	DEBUG_ENTRY
	DEBUG_PRINTF("nPhyIndex=%u", nPhyIndex);

	if (nPhyIndex >= MV88E6161_NUM_INTERNAL_PHYS) {
		DEBUG_EXIT
		return false;
	}

	uint16_t nValue;

	if (phy_read(nPhyIndex, 17, nValue)) {
		DEBUG_EXIT
		return false;
	}

	phyStatus.link = ((nValue & MV88E6XXX_PHY_STS_LINK) == MV88E6XXX_PHY_STS_LINK) ? dsa::Link::STATE_UP : dsa::Link::STATE_DOWN;
	phyStatus.duplex = ((nValue & MV88E6XXX_PHY_STS_DUPLEX) == MV88E6XXX_PHY_STS_DUPLEX) ? dsa::Duplex::DUPLEX_FULL : dsa::Duplex::DUPLEX_HALF;

	DEBUG_EXIT
	return true;
}

bool port_get_status(const uint32_t nPortNumber, PhyStatus& phyStatus) {
	DEBUG_ENTRY
	DEBUG_PRINTF("nPortNumber=%u", nPortNumber);

	if (nPortNumber >= MV88E6161_NUM_PORTS) {
		DEBUG_EXIT
		return false;
	}

	uint16_t nValue;

	if (mv88e6xxx_port_read(nPortNumber, MV88E6XXX_PORT_STS, nValue) < 0) {
		DEBUG_EXIT
		return false;
	}

	phyStatus.link = ((nValue & MV88E6XXX_PORT_STS_LINK) == MV88E6XXX_PORT_STS_LINK) ? dsa::Link::STATE_UP : dsa::Link::STATE_DOWN;
	phyStatus.duplex = ((nValue & MV88E6XXX_PORT_STS_DUPLEX) == MV88E6XXX_PORT_STS_DUPLEX) ? dsa::Duplex::DUPLEX_FULL : dsa::Duplex::DUPLEX_HALF;
	phyStatus.flowcontrol = ((nValue & MV88E6XXX_PORT_STS_FLOW_CTL) == MV88E6XXX_PORT_STS_FLOW_CTL);

	switch (nValue & MV88E6XXX_PORT_STS_SPEED_MASK) {
		case MV88E6XXX_PORT_STS_SPEED_10:
			phyStatus.speed = dsa::Speed::SPEED10;
			break;
		case MV88E6XXX_PORT_STS_SPEED_100:
			phyStatus.speed = dsa::Speed::SPEED100;
			break;
		case MV88E6XXX_PORT_STS_SPEED_1000:
			phyStatus.speed = dsa::Speed::SPEED1000;
			break;
		default:
			DEBUG_EXIT
			return false;
			break;
	}

	DEBUG_EXIT
	return true;
}
}  // namespace dsa

namespace remoteconfig {
namespace dsa {
static const char *check_link(const ::dsa::Link link, const char *pString) {
	return link == ::dsa::Link::STATE_UP ? pString : "";
}

uint32_t json_get_portstatus(char *pOutBuffer, const uint32_t nOutBufferSize) {
	const auto nSize = static_cast<int32_t>(nOutBufferSize);
	auto nLength = static_cast<int32_t>(1);

	pOutBuffer[0] = '[';

	for (uint32_t nPortNumber = 0; nPortNumber < ::dsa::MV88E6161_NUM_PORTS; nPortNumber++) {
		::dsa::PhyStatus phyStatus;
		if (port_get_status(nPortNumber, phyStatus)) {
			assert((nSize - nLength) > 0);
			nLength += snprintf(&pOutBuffer[nLength], static_cast<size_t>(nSize - nLength),
				"{\"port\":\"%u\",\"link\":\"%s\",\"speed\":\"%s\",\"duplex\":\"%s\",\"flowcontrol\":\"%s\"},",
				nPortNumber,
				::dsa::phy_string_get_link(phyStatus.link),
				check_link(phyStatus.link, ::dsa::phy_string_get_speed(phyStatus.speed)),
				check_link(phyStatus.link, ::dsa::phy_string_get_duplex(phyStatus.duplex)),
				check_link(phyStatus.link, ::dsa::phy_string_get_flowcontrol(phyStatus.flowcontrol)));
		}
	}

	assert(nLength < static_cast<int32_t>(nOutBufferSize));
	//FIXME When port_get_status fails for all ports
	pOutBuffer[nLength - 1] = ']';

	return static_cast<uint32_t>(nLength);
}

uint32_t json_get_vlantable(char *pOutBuffer, const uint32_t nOutBufferSize) {
	const auto nSize = static_cast<int32_t>(nOutBufferSize);
	auto nLength = static_cast<int32_t>(1);

	pOutBuffer[0] = '[';

	for (uint32_t nPortNumber = 0; nPortNumber < ::dsa::MV88E6161_NUM_PORTS; nPortNumber++) {
		uint16_t nValue;
		if (::mv88e6xxx_port_read(nPortNumber, MV88E6XXX_PORT_BASE_VLAN, nValue) >= 0) {
			assert((nSize - nLength) > 0);
			nLength += snprintf(&pOutBuffer[nLength], static_cast<size_t>(nSize - nLength),
				"{\"port\":\"%u\",\"VLANTable\":\"0x%.2x\"},",
				nPortNumber,
				nValue & MV88E6XXX_PORT_BASE_VLAN_TABLE_MASK);
		}
	}

	assert(nLength < static_cast<int32_t>(nOutBufferSize));
	//FIXME When mv88e6xxx_port_read fails for all ports
	pOutBuffer[nLength - 1] = ']';

	return static_cast<uint32_t>(nLength);
}
}  // namespace dsa
}  // namespace remoteconfig
