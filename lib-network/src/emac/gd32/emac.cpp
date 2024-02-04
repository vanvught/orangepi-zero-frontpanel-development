/**
 * emac.cpp
 *
 */
/* Copyright (C) 2021-2024 by Arjan van Vught mailto:info@gd32-dmx.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <cstdint>
#include <cstdio>

#include "gd32.h"

#include "emac/phy.h"

#include "debug.h"

extern void enet_gpio_config();
extern enet_descriptors_struct  txdesc_tab[ENET_TXBUF_NUM];
extern void mac_address_get(uint8_t paddr[]);

/*
 * Public function
 */

void __attribute__((cold)) emac_config() {
	DEBUG_ENTRY
#if(PHY_TYPE == LAN8700)
	puts("LAN8700");
#elif(PHY_TYPE == DP83848)
	puts("DP83848");
#elif(PHY_TYPE == RTL8201F)
	puts("RTL8201F");
#else
#error PHY_TYPE is not set
#endif

	enet_gpio_config();

	rcu_periph_clock_enable(RCU_ENET);
	rcu_periph_clock_enable(RCU_ENETTX);
	rcu_periph_clock_enable(RCU_ENETRX);

	enet_deinit(ENETx);
	enet_software_reset(ENETx);

	net::phy_config(PHY_ADDRESS);

	DEBUG_EXIT
}

void __attribute__((cold)) emac_start(uint8_t mac_address[], net::Link& link) {
	DEBUG_ENTRY
	DEBUG_PRINTF("ENET_RXBUF_NUM=%u, ENET_TXBUF_NUM=%u", ENET_RXBUF_NUM, ENET_TXBUF_NUM);

	net::PhyStatus phyStatus;
	net::phy_start(PHY_ADDRESS, phyStatus);

	link = phyStatus.link;

#ifndef NDEBUG
	{
		uint16_t phy_value;
		ErrStatus phy_state = enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BCR, &phy_value);
		printf("BCR: %.4x %s\n", phy_value, phy_state == SUCCESS ? "SUCCES" : "ERROR" );
		enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
		phy_state = enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
		printf("BSR: %.4x %s\n", phy_value & (PHY_AUTONEGO_COMPLETE | PHY_LINKED_STATUS | PHY_JABBER_DETECTION), phy_state == SUCCESS ? "SUCCES" : "ERROR" );
	}
#endif

	enet_mediamode_enum mediamode = ENET_10M_HALFDUPLEX;

	if (phyStatus.speed == net::Speed::SPEED100) {
		if (phyStatus.duplex == net::Duplex::DUPLEX_FULL) {
			mediamode = ENET_100M_FULLDUPLEX;
		} else {
			mediamode = ENET_100M_HALFDUPLEX;
		}
	} else if (phyStatus.duplex == net::Duplex::DUPLEX_FULL) {
		mediamode = ENET_10M_FULLDUPLEX;
	}

	printf("Link %s, %d, %s\n",
			phyStatus.link == net::Link::STATE_UP ? "Up" : "Down",
			phyStatus.speed == net::Speed::SPEED10 ? 10 : 100,
			phyStatus.duplex == net::Duplex::DUPLEX_HALF ? "HALF" : "FULL");

	const auto enet_init_status = enet_init(mediamode, ENET_AUTOCHECKSUM_DROP_FAILFRAMES, ENET_CUSTOM);

	if (enet_init_status != SUCCESS) {}

    DEBUG_PRINTF("enet_init_status=%s", enet_init_status == SUCCESS ? "SUCCES" : "ERROR" );

#ifndef NDEBUG
	{
		uint16_t phy_value;
		ErrStatus phy_state = enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BCR, &phy_value);
		printf("BCR: %.4x %s\n", phy_value, phy_state == SUCCESS ? "SUCCES" : "ERROR" );
		enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
		phy_state = enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, PHY_REG_BSR, &phy_value);
		printf("BSR: %.4x %s\n", phy_value & (PHY_AUTONEGO_COMPLETE | PHY_LINKED_STATUS | PHY_JABBER_DETECTION), phy_state == SUCCESS ? "SUCCES" : "ERROR" );
	}
#endif

	mac_address_get(mac_address);

	enet_mac_address_set(ENET_MAC_ADDRESS0, mac_address);

	enet_descriptors_chain_init(ENET_DMA_TX);
	enet_descriptors_chain_init(ENET_DMA_RX);

	for (unsigned i = 0; i < ENET_TXBUF_NUM; i++) {
		enet_transmit_checksum_config(&txdesc_tab[i], ENET_CHECKSUM_TCPUDPICMP_FULL);
	}

	enet_enable(ENETx);

	DEBUG_EXIT
}
