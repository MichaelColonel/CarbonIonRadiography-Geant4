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

#include <G4THitsMap.hh>
#include <G4UserEventAction.hh>

#include "CIR_Track.hh"
#include "CIR_HitCoordinates.hh"

namespace CarbonIonRadiography {

class EventActionMessenger;

class EventAction : public G4UserEventAction {
public:
	EventAction();
	virtual ~EventAction();
	virtual void BeginOfEventAction(const G4Event*);
	virtual void EndOfEventAction(const G4Event*);
	void setCaloSliceThres(G4double thres) { threshold_energy_calo_slice = thres; }
	void setSiStripsThres(G4double thres) { threshold_energy_si_strips = thres; }
	void update();
	TREC::HitsPositions getPositions() const { return positions; }

private:
	G4bool fillEnergyCoordinates( G4int pos, G4THitsMap<G4double>* energy);
	EventActionMessenger* event_action_messenger;
	G4int calorimeterID;
	std::vector<G4int> siliconID;

	RawHitCoordinates coordinates;
	TREC::HitsPositions positions;

	G4double threshold_energy_calo_slice;
	G4double threshold_energy_si_strips;
	G4int mod;
};

} // namespace CarbonIonRadiography
