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

#include <G4TrackingManager.hh>
#include <G4UnitsTable.hh>
#include <G4Track.hh>
#include <G4Gamma.hh>
#include <G4Neutron.hh>
#include <G4Proton.hh>
#include <G4Deuteron.hh>
#include <G4Electron.hh>
#include <G4Triton.hh>
#include <G4Alpha.hh>

#include "CIR_TrackingAction.hh"

namespace CarbonIonRadiography {

void
TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
	G4int id = aTrack->GetParentID();

	// only primaries	
	if (id == 0)
		fpTrackingManager->SetStoreTrajectory(true);
	else
		fpTrackingManager->SetStoreTrajectory(false);
}

} // namespace CarbonIonRadiography
