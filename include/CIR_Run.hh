/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#pragma once

#include <vector>
#include <G4Run.hh>

#include <trec_hits_positions.hh>

//#include "CIR_HitsPositions.hh"
#include "CIR_Track.hh"

class G4Event;

namespace CarbonIonRadiography {

class EventAction;

class Run : public G4Run {
public:
	Run(EventAction* eventAction);
	virtual ~Run();
	virtual void RecordEvent(const G4Event*);
	virtual void Merge(const G4Run*);
	const TREC::HitsPositionsVector& hitsPositions() const { return hits_positions; }

private:
	EventAction* eventAction;
	TREC::HitsPositionsVector hits_positions;
};

} // namespace CarbonIonRadiography
