/**
 * @file input.h
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

#ifndef INPUT_H_
#define INPUT_H_

namespace input {
static constexpr int KEY_NOT_DEFINED = 0;
static constexpr int KEY_ENTER = 10;
static constexpr int KEY_ESC = 27;
static constexpr int KEY_DEC = '+';
static constexpr int KEY_INC = '-';
static constexpr int KEY_NUMERIC_0 = 48;
static constexpr int KEY_NUMERIC_1 = 49;
static constexpr int KEY_NUMERIC_2 = 50;
static constexpr int KEY_NUMERIC_3 = 51;
static constexpr int KEY_NUMERIC_4 = 52;
static constexpr int KEY_NUMERIC_5 = 53;
static constexpr int KEY_NUMERIC_6 = 54;
static constexpr int KEY_NUMERIC_7 = 55;
static constexpr int KEY_NUMERIC_8 = 56;
static constexpr int KEY_NUMERIC_9 = 57;
static constexpr int KEY_UP = -65;
static constexpr int KEY_DOWN = -66;
static constexpr int KEY_RIGHT = -67;
static constexpr int KEY_LEFT = -68;
}  // namespace input

#endif /* INPUT_H_ */
