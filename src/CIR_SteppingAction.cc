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

#include <G4SteppingManager.hh>
#include <G4Track.hh>
#include <G4Step.hh>
#include <G4StepPoint.hh>
#include <G4TrackStatus.hh>
#include <G4VPhysicalVolume.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTypes.hh>

#include "CIR_GlobalStrings.hh"

#include "CIR_SteppingAction.hh"
#include "CIR_EventAction.hh"

namespace CarbonIonRadiography {

SteppingAction::SteppingAction(EventAction* fEventAction)
	:
	G4UserSteppingAction(),
    eventAction(fEventAction)
{
}

SteppingAction::~SteppingAction()
{
}

void
SteppingAction::UserSteppingAction(const G4Step* /* theStep */)
{
	// write data to event actions
	// in case of stepping
}

} // namespace CarbonIonRadiography
