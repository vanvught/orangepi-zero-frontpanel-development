/**
 * @file ws28xxdmxmulti.h
 *
 */
/* Copyright (C) 2019-2023 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef WS28XXDMXMULTI_H_
#define WS28XXDMXMULTI_H_

#include <cstdint>
#include <cassert>

#include "lightset.h"
#include "lightsetdata.h"

#include "ws28xxmulti.h"

#include "pixeldmxconfiguration.h"
#include "pixelpatterns.h"

#include "pixeldmxhandler.h"

namespace ws28xxdmxmulti {
#if !defined (CONFIG_PIXELDMX_MAX_PORTS)
# define CONFIG_PIXELDMX_MAX_PORTS	8
#endif
static constexpr auto MAX_PORTS = CONFIG_PIXELDMX_MAX_PORTS;
}  // namespace ws28xxdmxmulti

class WS28xxDmxMulti final: public LightSet {
public:
	WS28xxDmxMulti(PixelDmxConfiguration& pixelDmxConfiguration);
	~WS28xxDmxMulti() override;

	void Start(const uint32_t nPortIndex) override;
	void Stop(const uint32_t nPortIndex) override;

	void SetData(uint32_t nPortIndex, __attribute__((unused)) const uint8_t *pData, __attribute__((unused)) uint32_t nLength, const bool doUpdate) override {
		if (!doUpdate) {
			return;
		}

		if (nPortIndex == m_PortInfo.nProtocolPortIndexLast) {
			while (m_pWS28xxMulti->IsUpdating()) {
				// wait for completion
			}

			for (uint32_t nPortIndex =0 ; nPortIndex < m_PortInfo.nProtocolPortIndexLast;nPortIndex++) {
				SetData(nPortIndex, lightset::Data::Backup(nPortIndex), lightset::Data::GetLength(nPortIndex));
			}

			m_pWS28xxMulti->Update();
		}
	}

	void Sync(const uint32_t nPortIndex) override {
		SetData(nPortIndex, lightset::Data::Backup(nPortIndex), lightset::Data::GetLength(nPortIndex));
	}

	void Sync(const bool doForce) override {
		if (__builtin_expect((!doForce), 1)) {
			assert(m_pWS28xxMulti != nullptr);
			while (m_pWS28xxMulti->IsUpdating()) {
				// wait for completion
			}
			m_pWS28xxMulti->Update();
		}
	}

#if defined (OUTPUT_HAVE_STYLESWITCH)
	void SetOutputStyle(__attribute__((unused)) const uint32_t nPortIndex, __attribute__((unused)) const lightset::OutputStyle outputStyle) override {}
	lightset::OutputStyle GetOutputStyle(__attribute__((unused)) const uint32_t nPortIndex) const override {
		return lightset::OutputStyle::DELTA;
	}
#endif

	void Blackout(bool bBlackout) override;
	void FullOn() override;

	void Print() override {
		m_pixelDmxConfiguration.Print();
	}

	pixel::Type GetType() const {
		return m_pixelDmxConfiguration.GetType();
	}

	pixel::Map GetMap() const {
		return m_pixelDmxConfiguration.GetMap();
	}

	uint32_t GetCount() const {
		return m_pixelDmxConfiguration.GetCount();
	}

	uint32_t GetGroups() const {
		return m_pixelDmxConfiguration.GetGroups();
	}

	uint32_t GetGroupingCount() const {
		return m_pixelDmxConfiguration.GetGroupingCount();
	}

	uint32_t GetUniverses() const {
		return m_pixelDmxConfiguration.GetUniverses();
	}

	uint32_t GetOutputPorts() const {
		return m_pixelDmxConfiguration.GetOutputPorts();
	}

	uint32_t GetChannelsPerPixel() const {
		return m_nChannelsPerPixel;
	}

	void SetPixelDmxHandler(PixelDmxHandler *pPixelDmxHandler) {
		m_pPixelDmxHandler = pPixelDmxHandler;
	}

	// RDMNet LLRP Device Only
	bool SetDmxStartAddress(__attribute__((unused)) uint16_t nDmxStartAddress) override {
		return false;
	}

	uint16_t GetDmxStartAddress() override {
		return lightset::dmx::ADDRESS_INVALID;
	}

	uint16_t GetDmxFootprint() override {
		return 0;
	}

private:
	void SetData(uint32_t nPortIndex, const uint8_t *pData, uint32_t nLength);

private:
	PixelDmxConfiguration m_pixelDmxConfiguration;
	pixeldmxconfiguration::PortInfo m_PortInfo;
	uint32_t m_nChannelsPerPixel;

	WS28xxMulti *m_pWS28xxMulti { nullptr };
	PixelDmxHandler *m_pPixelDmxHandler { nullptr };

	uint32_t m_bIsStarted { 0 };
	bool m_bBlackout { false };
};

#endif /* WS28XXDMXMULTI_H_ */
