/**
 * @file h3_usb.cpp
 *
 */
/* Copyright (C) 2020-2023 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#include "h3.h"
#include "h3_ccu.h"

void __attribute__((cold)) h3_usb_end() {
	uint32_t value = H3_CCU->BUS_CLK_GATING0;
	value &= ~CCU_BUS_CLK_GATING0_USB_OTG;
	value &= ~CCU_BUS_CLK_GATING0_USB_OTG_EHCI0;
	value &= ~CCU_BUS_CLK_GATING0_USB_EHCI1;
	value &= ~CCU_BUS_CLK_GATING0_USB_EHCI2;
	value &= ~CCU_BUS_CLK_GATING0_USB_EHCI3;
	value &= ~CCU_BUS_CLK_GATING0_USB_OTG_OHCI0;
	value &= ~CCU_BUS_CLK_GATING0_USB_OHCI1;
	value &= ~CCU_BUS_CLK_GATING0_USB_OHCI2;
	value &= ~CCU_BUS_CLK_GATING0_USB_OHCI3;
	H3_CCU->BUS_CLK_GATING0 = value;

	value = H3_CCU->BUS_SOFT_RESET0;
	value &= ~CCU_BUS_SOFT_RESET0_USB_OTG;
	value &= ~CCU_BUS_SOFT_RESET0_USB_OTG_EHCI0;
	value &= ~CCU_BUS_SOFT_RESET0_USB_EHCI1;
	value &= ~CCU_BUS_SOFT_RESET0_USB_EHCI2;
	value &= ~CCU_BUS_SOFT_RESET0_USB_EHCI3;
	value &= ~CCU_BUS_SOFT_RESET0_USB_OTG_OHCI0;
	value &= ~CCU_BUS_SOFT_RESET0_USB_OHCI1;
	value &= ~CCU_BUS_SOFT_RESET0_USB_OHCI2;
	value &= ~CCU_BUS_SOFT_RESET0_USB_OHCI3;
	H3_CCU->BUS_SOFT_RESET0 = value;

	H3_CCU->USBPHY_CFG = 0;
}
