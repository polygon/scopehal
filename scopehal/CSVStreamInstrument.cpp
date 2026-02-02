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
	@brief Implementation of CSVStreamInstrument
	@ingroup scopedrivers
 */

#include "scopehal.h"
#include "CSVStreamInstrument.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

CSVStreamInstrument::CSVStreamInstrument(SCPITransport* transport)
	: SCPIDevice(transport, false)
	, SCPIInstrument(transport, false)
	, m_triggerArmed(false)
	, m_triggerOneShot(false)
	, m_appendingNext(false)
{
	m_vendor = "Antikernel Labs";
	m_model = "CSV Stream";
	m_serial = "N/A";
	m_fwVersion = "1.0";

	const char* defaultColors[] = {"#ffff00", "#ff6abc", "#00ffff", "#00c100"};
	for(size_t i = 0; i < 4; i++)
	{
		m_channels.push_back(new OscilloscopeChannel(
			this,
			string("CH") + to_string(i + 1),
			defaultColors[i],
			Unit(Unit::UNIT_FS),
			Unit(Unit::UNIT_VOLTS),
			Stream::STREAM_TYPE_ANALOG,
			i));
	}

	m_preloaders.push_front(sigc::mem_fun(*this, &CSVStreamInstrument::DoPreLoadConfiguration));
}

CSVStreamInstrument::~CSVStreamInstrument()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instantiation

uint32_t CSVStreamInstrument::GetInstrumentTypes() const
{
	return INST_OSCILLOSCOPE;
}

uint32_t CSVStreamInstrument::GetInstrumentTypesForChannel(size_t /*i*/) const
{
	return INST_OSCILLOSCOPE;
}

string CSVStreamInstrument::GetDriverNameInternal()
{
	return "csvstream";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Device info

void CSVStreamInstrument::FlushConfigCache()
{
}

OscilloscopeChannel* CSVStreamInstrument::GetExternalTrigger()
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Channel configuration

bool CSVStreamInstrument::IsChannelEnabled(size_t /*i*/)
{
	return true;
}

void CSVStreamInstrument::EnableChannel(size_t /*i*/)
{
}

void CSVStreamInstrument::DisableChannel(size_t /*i*/)
{
}

OscilloscopeChannel::CouplingType CSVStreamInstrument::GetChannelCoupling(size_t /*i*/)
{
	return OscilloscopeChannel::COUPLE_DC_50;
}

void CSVStreamInstrument::SetChannelCoupling(size_t /*i*/, OscilloscopeChannel::CouplingType /*type*/)
{
}

vector<OscilloscopeChannel::CouplingType> CSVStreamInstrument::GetAvailableCouplings(size_t /*i*/)
{
	return {};
}

double CSVStreamInstrument::GetChannelAttenuation(size_t /*i*/)
{
	return 1;
}

void CSVStreamInstrument::SetChannelAttenuation(size_t /*i*/, double /*atten*/)
{
}

unsigned int CSVStreamInstrument::GetChannelBandwidthLimit(size_t /*i*/)
{
	return 0;
}

void CSVStreamInstrument::SetChannelBandwidthLimit(size_t /*i*/, unsigned int /*limit_mhz*/)
{
}

vector<unsigned int> CSVStreamInstrument::GetChannelBandwidthLimiters(size_t /*i*/)
{
	return {};
}

float CSVStreamInstrument::GetChannelVoltageRange(size_t /*i*/, size_t /*stream*/)
{
	return 5;
}

void CSVStreamInstrument::SetChannelVoltageRange(size_t /*i*/, size_t /*stream*/, float /*range*/)
{
}

float CSVStreamInstrument::GetChannelOffset(size_t /*i*/, size_t /*stream*/)
{
	return 0;
}

void CSVStreamInstrument::SetChannelOffset(size_t /*i*/, size_t /*stream*/, float /*offset*/)
{
}

string CSVStreamInstrument::GetProbeName(size_t /*i*/)
{
	return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Triggering

Oscilloscope::TriggerMode CSVStreamInstrument::PollTrigger()
{
	if(!m_triggerArmed)
		return TRIGGER_MODE_STOP;
	return TRIGGER_MODE_TRIGGERED;
}

void CSVStreamInstrument::Start()
{
	m_triggerArmed = true;
	m_triggerOneShot = false;
}

void CSVStreamInstrument::StartSingleTrigger()
{
	m_triggerArmed = true;
	m_triggerOneShot = true;
}

void CSVStreamInstrument::Stop()
{
	m_triggerArmed = false;
}

void CSVStreamInstrument::ForceTrigger()
{
	m_triggerArmed = true;
}

bool CSVStreamInstrument::IsTriggerArmed()
{
	return m_triggerArmed;
}

void CSVStreamInstrument::PushTrigger()
{
}

void CSVStreamInstrument::PullTrigger()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Timebase

vector<uint64_t> CSVStreamInstrument::GetSampleRatesNonInterleaved()
{
	return {1};
}

vector<uint64_t> CSVStreamInstrument::GetSampleRatesInterleaved()
{
	return {};
}

set<Oscilloscope::InterleaveConflict> CSVStreamInstrument::GetInterleaveConflicts()
{
	return {};
}

vector<uint64_t> CSVStreamInstrument::GetSampleDepthsNonInterleaved()
{
	return {1};
}

vector<uint64_t> CSVStreamInstrument::GetSampleDepthsInterleaved()
{
	return {};
}

uint64_t CSVStreamInstrument::GetSampleRate()
{
	return 1;
}

uint64_t CSVStreamInstrument::GetSampleDepth()
{
	return 1;
}

void CSVStreamInstrument::SetSampleDepth(uint64_t /*depth*/)
{
}

void CSVStreamInstrument::SetSampleRate(uint64_t /*rate*/)
{
}

void CSVStreamInstrument::SetTriggerOffset(int64_t /*offset*/)
{
}

int64_t CSVStreamInstrument::GetTriggerOffset()
{
	return 0;
}

bool CSVStreamInstrument::IsInterleaving()
{
	return false;
}

bool CSVStreamInstrument::SetInterleaving(bool /*combine*/)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Waveform management

bool CSVStreamInstrument::IsAppendingToWaveform()
{
	return m_appendingNext;
}

bool CSVStreamInstrument::PopPendingWaveform()
{
	lock_guard<mutex> lock(m_pendingWaveformsMutex);
	if(m_pendingWaveforms.empty())
		return false;

	auto& set = m_pendingWaveforms.front();

	for(auto it : set)
	{
		auto chan = it.first.m_channel;
		auto data = dynamic_cast<SparseAnalogWaveform*>(it.second);
		auto nstream = it.first.m_stream;

		auto oldWaveform = dynamic_cast<SparseAnalogWaveform*>(chan->GetData(nstream));
		if(oldWaveform && data && m_appendingNext)
		{
			size_t len = data->size();
			oldWaveform->PrepareForCpuAccess();
			data->PrepareForCpuAccess();
			for(size_t i=0; i<len; i++)
			{
				oldWaveform->m_samples.push_back(data->m_samples[i]);
				oldWaveform->m_offsets.push_back(data->m_offsets[i]);
				oldWaveform->m_durations.push_back(data->m_durations[i]);
			}
			oldWaveform->m_revision++;
			oldWaveform->MarkModifiedFromCpu();
		}
		else
			chan->SetData(data, nstream);
	}
	m_pendingWaveforms.pop_front();

	m_appendingNext = true;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization

void CSVStreamInstrument::DoPreLoadConfiguration(
	[[maybe_unused]] int version,
	const YAML::Node& node,
	IDTable& idmap,
	[[maybe_unused]] ConfigWarningList& list)
{
	m_channels.clear();

	auto& chans = node["channels"];
	for(auto it : chans)
	{
		auto& cnode = it.second;
		auto index = cnode["index"].as<int>();

		while(m_channels.size() <= (size_t)index)
		{
			m_channels.push_back(new OscilloscopeChannel(
				this,
				string("CH") + to_string(index),
				"#808080",
				Unit(Unit::UNIT_FS),
				Unit(Unit::UNIT_VOLTS),
				Stream::STREAM_TYPE_ANALOG,
				m_channels.size()));
		}

		auto chan = m_channels[index];
		idmap.emplace(cnode["id"].as<int>(), chan);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Acquisition

bool CSVStreamInstrument::AcquireData()
{
	auto line = Trim(m_transport->ReadReply(false));

	if(line.empty())
		return true;

	auto start = line.find("CSV-");
	if(start == string::npos)
		return true;
	line = line.substr(start);

	auto fields = explode(line, ',');

	if(fields[0] == "CSV-NAME")
	{
		for(size_t i=1; i<fields.size() && i <= m_channels.size(); i++)
			m_channels[i-1]->SetDisplayName(fields[i]);
	}

	else if(fields[0] == "CSV-UNIT")
	{
		for(size_t i=1; i<fields.size() && i <= m_channels.size(); i++)
		{
			Unit yunit(fields[i]);
			m_channels[i-1]->SetYAxisUnits(yunit, 0);
		}
	}

	else if(fields[0] == "CSV-DATA")
	{
		if(fields.size() < 3)
			return true;

		int64_t timestamp = stoll(fields[1]);

		SequenceSet s;

		for(size_t i=2; i<fields.size(); i++)
		{
			size_t chanIndex = i - 2;

			if(chanIndex >= m_channels.size())
				break;

			auto chan = dynamic_cast<OscilloscopeChannel*>(m_channels[chanIndex]);
			if(!chan)
				continue;

			auto value = chan->GetYAxisUnits(0).ParseString(fields[i]);

			auto wfm = new SparseAnalogWaveform;
			wfm->m_timescale = 1;
			wfm->m_triggerPhase = 0;
			wfm->m_startTimestamp = time(nullptr);
			wfm->m_startFemtoseconds = 0;
			wfm->PrepareForCpuAccess();
			wfm->Resize(1);
			wfm->m_offsets[0] = timestamp;
			wfm->m_durations[0] = 1;
			wfm->m_samples[0] = value;
			wfm->MarkModifiedFromCpu();

			s[StreamDescriptor(chan, 0)] = wfm;
		}

		if(!s.empty())
		{
			lock_guard<mutex> lock(m_pendingWaveformsMutex);
			m_pendingWaveforms.push_back(s);
		}
	}

	return true;
}
