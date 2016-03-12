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

#include <TH1.h>
#include <TH2.h>
#include <TFile.h>

#include "CIR_Run.hh"
//#include "CIR_TrackReconstruction.hh"
#include "CIR_RunAction.hh"

namespace CarbonIonRadiography {

RunAction::RunAction(EventAction* fEventAction)
	:
	G4UserRunAction(),
    eventAction(fEventAction)
{
}

RunAction::~RunAction()
{
}

G4Run*
RunAction::GenerateRun()
{
	return new Run(eventAction);
}

void
RunAction::BeginOfRunAction(const G4Run*)
{
	// Initiate run parameters
}

void
RunAction::EndOfRunAction(const G4Run* run)
{
	// Finish run and save the results

	const Run* theRun = dynamic_cast<const Run*>(run);
	 
	if(IsMaster()) {
		G4cout << "Global result with " << theRun->GetNumberOfEvent() << G4endl;
		
		const TREC::HitsPositionsVector& track_hits = theRun->hitsPositions();

		TREC::HitsPositions::save( "hits.dat", track_hits);

	}
	else {
		G4cout << "Local thread result with " << theRun->GetNumberOfEvent() << G4endl;
	}
}

} // namespace CarbonIonRadiography
