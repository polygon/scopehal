/***********************************************************************************************************************
*                                                                                                                      *
* libscopehal                                                                                                          *
*                                                                                                                      *
* Copyright (c) 2012-2024 Andrew D. Zonenberg and contributors                                                         *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

/**
	@file
	@author Andrew D. Zonenberg
	@brief Declaration of CSVStreamInstrument
	@ingroup scopedrivers
 */

#ifndef CSVStreamInstrument_h
#define CSVStreamInstrument_h

/**
	@brief An oscilloscope driver which streams time-series data over CSV

	Typically used to plot real time system state (voltages, temperatures, ADC values, etc) from a microcontroller over
	a UART or SWO trace interface.

	The instrument protocol is a unidirectional stream of line oriented comma-separated-value (CSV) rather than SCPI.

	At any time, the instrument may send lines with one or more of the following formats, separated by \n characters.
	Lines not starting with these magic keywords are ignored.

	* CSV-NAME,ch1name,ch2name, ... : assign human readable names to channels
	* CSV-UNIT,V,A, ... : specify unit associated with each channel
	* CSV-DATA,timestamp_fs,1.23,3.14, ... : specify timestamp (in femtoseconds) and measurement values for each channel.
	  The first value after CSV-DATA is the timestamp, followed by one value per channel.
	  It is not possible to perform partial updates of a single channel without updating the others.

	@ingroup scopedrivers
 */
class CSVStreamInstrument
	: public virtual SCPIOscilloscope
{
public:
	CSVStreamInstrument(SCPITransport* transport);
	virtual ~CSVStreamInstrument();

	CSVStreamInstrument(const CSVStreamInstrument& rhs) =delete;
	CSVStreamInstrument& operator=(const CSVStreamInstrument& rhs) =delete;

	//Device information
	virtual uint32_t GetInstrumentTypes() const override;
	virtual uint32_t GetInstrumentTypesForChannel(size_t i) const override;

	virtual void FlushConfigCache() override;
	virtual OscilloscopeChannel* GetExternalTrigger() override;

	//Channel configuration
	virtual bool IsChannelEnabled(size_t i) override;
	virtual void EnableChannel(size_t i) override;
	virtual void DisableChannel(size_t i) override;
	virtual OscilloscopeChannel::CouplingType GetChannelCoupling(size_t i) override;
	virtual void SetChannelCoupling(size_t i, OscilloscopeChannel::CouplingType type) override;
	virtual std::vector<OscilloscopeChannel::CouplingType> GetAvailableCouplings(size_t i) override;
	virtual double GetChannelAttenuation(size_t i) override;
	virtual void SetChannelAttenuation(size_t i, double atten) override;
	virtual unsigned int GetChannelBandwidthLimit(size_t i) override;
	virtual void SetChannelBandwidthLimit(size_t i, unsigned int limit_mhz) override;
	virtual std::vector<unsigned int> GetChannelBandwidthLimiters(size_t i) override;
	virtual float GetChannelVoltageRange(size_t i, size_t stream) override;
	virtual void SetChannelVoltageRange(size_t i, size_t stream, float range) override;
	virtual float GetChannelOffset(size_t i, size_t stream) override;
	virtual void SetChannelOffset(size_t i, size_t stream, float offset) override;
	virtual std::string GetProbeName(size_t i) override;

	//Triggering
	virtual Oscilloscope::TriggerMode PollTrigger() override;
	virtual bool AcquireData() override;
	virtual void Start() override;
	virtual void StartSingleTrigger() override;
	virtual void Stop() override;
	virtual void ForceTrigger() override;
	virtual bool IsTriggerArmed() override;
	virtual void PushTrigger() override;
	virtual void PullTrigger() override;

	//Timebase
	virtual std::vector<uint64_t> GetSampleRatesNonInterleaved() override;
	virtual std::vector<uint64_t> GetSampleRatesInterleaved() override;
	virtual std::set<InterleaveConflict> GetInterleaveConflicts() override;
	virtual std::vector<uint64_t> GetSampleDepthsNonInterleaved() override;
	virtual std::vector<uint64_t> GetSampleDepthsInterleaved() override;
	virtual uint64_t GetSampleRate() override;
	virtual uint64_t GetSampleDepth() override;
	virtual void SetSampleDepth(uint64_t depth) override;
	virtual void SetSampleRate(uint64_t rate) override;
	virtual void SetTriggerOffset(int64_t offset) override;
	virtual int64_t GetTriggerOffset() override;
	virtual bool IsInterleaving() override;
	virtual bool SetInterleaving(bool combine) override;

	virtual bool PopPendingWaveform() override;
	virtual bool IsAppendingToWaveform() override;

protected:
	void DoPreLoadConfiguration(int version, const YAML::Node& node, IDTable& idmap, ConfigWarningList& list);

	bool m_triggerArmed;
	bool m_triggerOneShot;
	bool m_appendingNext;

public:
	static std::string GetDriverNameInternal();
	OSCILLOSCOPE_INITPROC(CSVStreamInstrument)
};

#endif
