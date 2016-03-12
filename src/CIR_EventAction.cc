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

#include <G4SystemOfUnits.hh>
#include <G4Event.hh>
#include <G4EventManager.hh>
#include <G4HCofThisEvent.hh>
#include <G4VHitsCollection.hh>
#include <G4SDManager.hh>
#include <G4VVisManager.hh>
#include <G4UnitsTable.hh>

#include <cmath>
#include <algorithm>

#include "CIR_Defines.hh"
#include "CIR_Track.hh"
#include "CIR_GlobalStrings.hh"
//#include "CIR_StripGeometry.hh"
#include "CIR_EventActionMessenger.hh"
#include "CIR_EventAction.hh"

namespace {

G4double calo_z = CIR_SIZE_CALORIMETER_THICKNESS * CLHEP::mm;
G4double calo_slice_z = CIR_SIZE_CALORIMETER_SLICE_THICKNESS * CLHEP::um;
G4int calo_slices = calo_z / calo_slice_z;

}

namespace CarbonIonRadiography {

EventAction::EventAction()
	:
	G4UserEventAction(),
	event_action_messenger(0),
	calorimeterID(-1),
	siliconID( CIR_NUMBER_OF_SILICON_DETECTORS, -1),
	positions(),
	threshold_energy_calo_slice(150.0 * CLHEP::MeV),
	threshold_energy_si_strips(4.0 * CLHEP::MeV),
	mod(100)
{
	event_action_messenger = new EventActionMessenger(this);
}

EventAction::~EventAction()
{
	delete event_action_messenger; 
}

void
EventAction::BeginOfEventAction(const G4Event* event)
{
	G4int number = event->GetEventID();
  
	if (number % mod == 0)
		G4cout << G4endl << "---> Begin of Event: " << number << G4endl;

	G4SDManager* pSDManager = G4SDManager::GetSDMpointer();

	// Init calorimeter (only once)
	if (calorimeterID == -1) {
		G4String collection_id = CalorimeterStr + G4String("/EnergyDeposit");
		calorimeterID = pSDManager->GetCollectionID(collection_id);
		coordinates.calo.resize(calo_slices);
	}

	// Init silicon planes (only once)
	if (siliconID[0] == -1) {
		TREC::StripNamesMap map = TREC::StripGeometry::create_names();
		for ( TREC::StripNamesMap::iterator it = map.begin();
			it != map.end(); ++it) {
			G4int pos = TREC::StripGeometry::index(it->first);
			TREC::StripGeometryNames& names = it->second;

			std::string& functional = names.functional_detector_name;
			G4String collection_id = functional + G4String("/EnergyDeposit");
			siliconID[pos] = pSDManager->GetCollectionID(collection_id);

			TREC::StripGeometryType plane = TREC::StripGeometry::index(pos);
			G4DataVector& energy = coordinates.energy[plane];
			energy.resize(CIR_NUMBER_OF_STRIPS_PER_SILICON);
		}
	}

	// clear energy deposition in silicon planes
	for ( StripsEnergyMap::iterator it = coordinates.energy.begin();
		it != coordinates.energy.end(); ++it) {
		std::fill( it->second.begin(), it->second.end(), 0.0);
	}

	// clear energy deposition in calorimeter
	std::fill( coordinates.calo.begin(), coordinates.calo.end(), 0.0);
}

void
EventAction::EndOfEventAction(const G4Event* event)
{
	G4HCofThisEvent* HCE = event->GetHCofThisEvent();
	G4THitsMap<G4double>* energy = 0;
	if (!HCE)
		return;

	for ( G4int i = 0; i < CIR_NUMBER_OF_SILICON_DETECTORS; ++i) {
		energy = dynamic_cast< G4THitsMap<G4double>* >(HCE->GetHC(siliconID[i]));
		G4bool res = fillEnergyCoordinates( i, energy);
		if (!res) {
			G4cerr << "Error in energy deposition in silicon detector:" << i;
			G4cerr << G4endl;
		}
	}

	energy = dynamic_cast< G4THitsMap<G4double>* >(HCE->GetHC(calorimeterID));
	if (energy) {
		std::map< G4int, G4double*>::iterator it = energy->GetMap()->begin();
		for( ; it != energy->GetMap()->end(); ++it) {
			coordinates.calo[it->first] += *it->second;
		}
	}
	
	FinalHitCoordinates final_hit(coordinates);

	positions = final_hit.getPositions();
}

G4bool
EventAction::fillEnergyCoordinates( G4int pos, G4THitsMap<G4double>* energy)
{
	bool res = false;
	TREC::StripGeometryType type = TREC::StripGeometry::index(pos);
	if (energy) {
		std::map< G4int, G4double*>::iterator it = energy->GetMap()->begin();
		for( ; it != energy->GetMap()->end(); ++it) {
			coordinates.energy[type][it->first] += *it->second;
		}
		res = true;
	}
	return res;
}

void
EventAction::update()
{
	G4cout << "Calorimeter slice threshold: " <<
		G4BestUnit( threshold_energy_calo_slice, "Energy") << G4endl;
	G4cout << "Silicon strips threshold: " <<
		G4BestUnit( threshold_energy_si_strips, "Energy") << G4endl;
}

} // namespace CarbonIonRadiography
