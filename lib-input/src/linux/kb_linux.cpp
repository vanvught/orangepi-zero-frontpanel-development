/**
 * @file kb_linux.cpp
 *
 */
/* Copyright (C) 2017-2022 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#include <cstddef>
#include <termios.h>
#include <cstdio>
#include <sys/ioctl.h>
#ifndef FIONREAD    // cygwin defines FIONREAD in  socket.h  instead of  ioctl.h
#include <sys/socket.h>
#endif

#include "kb_linux.h"

#define STDIN	0

KbLinux::KbLinux() {
}

KbLinux::~KbLinux() {
}

bool KbLinux::Start() {
	struct termios term;

	tcgetattr(STDIN, &term);
    term.c_lflag &= static_cast<unsigned>(~ICANON);
    tcsetattr(STDIN, TCSANOW, &term);
    setbuf(stdin, nullptr);

    return true;
}

bool KbLinux::IsAvailable() {
    ioctl(STDIN, FIONREAD, &m_nBytesWaiting);
    return m_nBytesWaiting > 0 ? true : false;
}

int KbLinux::GetChar() {
	if (IsAvailable()) {
//printf("%d:%d:%d\n", __LINE__, m_nState, m_nBytesWaiting);
		int c = getchar();
		if ((m_nState == 0) && (c == 27) && (m_nBytesWaiting-- > 1)) {
			m_nState++;
			if (m_nBytesWaiting-- > 0) {
				c = getchar();
				if ((m_nState == 1) && (c == 91)) {
					m_nState++;
					if (m_nBytesWaiting > 0) {
						m_nState = 0;
						return -getchar();
					}
				}
			}
		}
//printf("%d:%d:%d:{%d}\n", __LINE__, m_nState, m_nBytesWaiting, c);
		return c;
	} else {
		return 0;
	}
}
