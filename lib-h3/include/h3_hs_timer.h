/**
 * @file h3_hs_timer.h
 *
 */
/* Copyright (C) 2018-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef H3_HS_TIMER_H_
#define H3_HS_TIMER_H_

#include "h3.h"

#ifdef __cplusplus
extern "C" {
#endif

inline static uint32_t h3_hs_timer_lo_us() {
	return ~(H3_HS_TIMER->CURNT_LO / 100);
}

inline static void h3_hs_timer_delay(uint32_t d) {
	const uint32_t t1 = H3_HS_TIMER->CURNT_LO;

	do {
	} while (t1 - H3_HS_TIMER->CURNT_LO < d);
}

extern void h3_hs_timer_udelay(uint32_t d);

#ifdef __cplusplus
}
#endif

#endif /* H3_HS_TIMER_H_ */
