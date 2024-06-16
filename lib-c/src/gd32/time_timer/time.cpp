/**
 * @file time.c
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

#pragma GCC push_options
#pragma GCC optimize ("O2")

#include <cstddef>
#include <sys/time.h>
#include <cstdint>
#include <cassert>

#include "gd32.h"

volatile static uint32_t sv_nSeconds;

void time_timer_init() {
	rcu_periph_clock_enable(RCU_TIMER6);
	timer_deinit(TIMER6);

	timer_parameter_struct timer_initpara;
	timer_struct_para_init(&timer_initpara);

	timer_initpara.prescaler = TIMER_PSC_10KHZ;
	timer_initpara.period = (10000 - 1);		// 1 second
	timer_init(TIMER6, &timer_initpara);

	timer_flag_clear(TIMER6, ~0);
	timer_interrupt_flag_clear(TIMER6, ~0);

	timer_interrupt_enable(TIMER6, TIMER_INT_UP);

	NVIC_SetPriority(TIMER6_IRQn, (1UL<<__NVIC_PRIO_BITS)-1UL); // Lowest priority
	NVIC_EnableIRQ(TIMER6_IRQn);

	timer_enable(TIMER6);
}

extern "C" {
void TIMER6_IRQHandler() {
	const auto nIntFlag = TIMER_INTF(TIMER6);

	if ((nIntFlag & TIMER_INT_FLAG_UP) == TIMER_INT_FLAG_UP) {
		sv_nSeconds++;
	}

	TIMER_INTF(TIMER6) = static_cast<uint32_t>(~nIntFlag);
}

int gettimeofday(struct timeval *tv, __attribute__((unused))    struct timezone *tz) {
	assert(tv != 0);

	tv->tv_sec = sv_nSeconds;
	tv->tv_usec = TIMER_CNT(TIMER6) * 100U;

	return 0;
}

int settimeofday(const struct timeval *tv, __attribute__((unused)) const struct timezone *tz) {
    assert(tv != 0);

    // Disable the timer interrupt to prevent it from triggering while we adjust the counter
    timer_interrupt_disable(TIMER6, TIMER_INT_UP);
    timer_disable(TIMER6);

    sv_nSeconds = tv->tv_sec;
    TIMER_CNT(TIMER6) = (tv->tv_usec / 100U) % 10000;

    timer_flag_clear(TIMER6, ~0);
    timer_interrupt_flag_clear(TIMER6, ~0);
    timer_interrupt_enable(TIMER6, TIMER_INT_UP);
    timer_enable(TIMER6);

    return 0;
}

time_t time(time_t *__timer) {
	struct timeval tv;
	gettimeofday(&tv, nullptr);

	if (__timer != nullptr) {
		*__timer = tv.tv_sec;
	}

	return tv.tv_sec;
}
}
