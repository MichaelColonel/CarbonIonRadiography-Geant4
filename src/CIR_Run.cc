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

#include <G4Event.hh>
#include <G4HCofThisEvent.hh>
#include <G4SDManager.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4DigiManager.hh>
#include <G4THitsMap.hh>

#include "CIR_EventAction.hh"
#include "CIR_TrackCoordinates.hh"
#include "CIR_Run.hh"

namespace CarbonIonRadiography {

Run::Run(EventAction* fEventAction)
	:
	G4Run(),
	eventAction(fEventAction)
{ 
}

Run::~Run()
{
}

void
Run::RecordEvent(const G4Event* /* event */)
{
	TREC::HitsPositions pos = eventAction->getPositions();
	hits_positions.push_back(pos);
}

void
Run::Merge(const G4Run* aRun)
{
	const Run* run = dynamic_cast<const Run*>(aRun);
	Run* local_run = const_cast<Run*>(run);

	const TREC::HitsPositionsVector& local_hits = local_run->hitsPositions();

	TREC::HitsPositionsVector new_hits(this->hits_positions.size() + local_hits.size());

	// merge hits positions
	std::merge(
		this->hits_positions.begin(),
		this->hits_positions.end(),
		local_hits.begin(),
		local_hits.end(),
		new_hits.begin());

	this->hits_positions = new_hits;

	G4cout << G4endl << "--- Merge --- " << G4endl;

	G4Run::Merge(run);
}

} // namespace CarbonIonRadiography
