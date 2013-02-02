#pragma once

/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include "SampleFormat.hh"

namespace Audio
{

class PcmFormat
{
public:
	int rate;
	const SampleFormat *sample;
	int channels;

	bool canSupport(PcmFormat *p2) const
	{
		return rate >= p2->rate &&
			sample->toBits() >= p2->sample->toBits() &&
			channels >= p2->channels;
	}

	bool operator ==(PcmFormat const& rhs) const
	{
		return rate == rhs.rate && sample == rhs.sample && channels == rhs.channels;
	}

	uint framesToBytes(uint frames) const
	{
		return frames * sample->toBytes() * channels;
	}

	uint bytesToFrames(uint bytes) const
	{
		return bytes / sample->toBytes() / channels;
	}

	uint secsToBytes(uint secs) const
	{
		return (rate * sample->toBytes() * channels) * secs;
	}
};

}
