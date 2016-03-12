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

#include <G4Track.hh>
#include <G4Gamma.hh>
#include <G4Neutron.hh>
#include <G4Proton.hh>
#include <G4Deuteron.hh>
#include <G4Electron.hh>
#include <G4NeutrinoE.hh>
#include <G4Triton.hh>
#include <G4Alpha.hh>
#include <G4GenericIon.hh>

#include "CIR_StackingAction.hh"

namespace CarbonIonRadiography {

StackingAction::StackingAction()
{
}

StackingAction::~StackingAction()
{
}

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
	G4ParticleDefinition* particleDef = aTrack->GetDefinition();

	// keep primary particle
	if (aTrack->GetParentID() == 0)
		return fUrgent;

	// kill secondary neutrino
	if (particleDef == G4NeutrinoE::NeutrinoE())
		return fKill;
	else
		return fWaiting;

}

} // namespace CarbonIonRadiography
