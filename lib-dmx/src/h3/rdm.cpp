/**
 * @file rdm.cpp
 *
 */
/* Copyright (C) 2018-2023 by Arjan van Vught mailto:info@orangepi-dmx.nl
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
#include <cassert>

#include "rdm.h"

#include "dmx.h"
#include "dmxconst.h"
#include "dmx_internal.h"

#include "h3.h"
#include "h3_uart.h"

#include "debug.h"

extern volatile uint32_t gv_RdmDataReceiveEnd;

void Rdm::SendDiscoveryRespondMessage(const uint32_t nPortIndex, const uint8_t *pRdmData, uint32_t nLength) {
	DEBUG_PRINTF("nPort=%u, pRdmData=%p, nLength=%u", nPort, pRdmData, nLength);
	assert(nPortIndex < dmx::config::max::OUT);
	assert(pRdmData != nullptr);
	assert(nLength != 0);

	// 3.2.2 Responder Packet spacing
	udelay(RDM_RESPONDER_PACKET_SPACING, gv_RdmDataReceiveEnd);

	Dmx::Get()->SetPortDirection(nPortIndex, dmx::PortDirection::OUTP, false);

	auto *p = _port_to_uart(nPortIndex);

	p->LCR = UART_LCR_8_N_2;

	for (uint32_t i = 0; i < nLength; i++) {
		while (!(p->LSR & UART_LSR_THRE))
			;
		p->O00.THR = pRdmData[i];
	}

	while (!((p->LSR & UART_LSR_TEMT) == UART_LSR_TEMT))
		;

	udelay(RDM_RESPONDER_DATA_DIRECTION_DELAY);

	Dmx::Get()->SetPortDirection(nPortIndex, dmx::PortDirection::INP, true);

	DEBUG_EXIT
}
