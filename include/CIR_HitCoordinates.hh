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

#include <G4DataVector.hh>
#include <trec_hits_positions.hh>

#include "CIR_Track.hh"
//#include "CIR_HitsPositions.hh"
//#include "CIR_StripGeometry.hh"
#include "CIR_Defines.hh"

namespace CarbonIonRadiography {

typedef std::map< TREC::StripGeometryType, G4DataVector> StripsEnergyMap;

struct RawHitCoordinates {
	StripsEnergyMap energy; // energy in strips planes
	G4DataVector calo; // energy in calorimeter
};

class FinalHitCoordinates {
public:
	FinalHitCoordinates(RawHitCoordinates& raw_hits);
//	FinalHitCoordinates(const FinalHitCoordinates& src);
//	FinalHitCoordinates& operator=(const FinalHitCoordinates& src);
	
	G4double energy() const { return total_energy; }
	G4int slice() const { return max_slice_index; }
	G4double sliceEnergy() const { return max_slice_energy; }
	void getTracks( TrackXYPair& track_main, TrackXYPair& track_full) const;
	TrackXYPair getTrack(G4bool type) const;
	void calculateCoordinates();
	void calculateTracks( G4bool& main, G4bool& full);
	G4bool checkCalorimeterData() const;
	TREC::HitsPositions getPositions();

private:
	G4int checkOneCluster( G4DataVector& ene,
		G4DataVector::iterator& begin,
		G4DataVector::iterator& end);

	G4int findCoordinate( TREC::StripGeometryType, G4DataVector& ene);
	G4bool checkTracksWithinTrajectory();
	void calculateMainTrack(G4bool);
	void calculateFullTrack(G4bool);

	std::pair< G4double, G4double> xy1; // coordinate
	std::pair< G4bool, G4bool> xy1_ok; // state
	std::pair< G4double, G4double> xy2; // coordinate
	std::pair< G4bool, G4bool> xy2_ok; // state
	std::pair< G4double, G4double> xy3; // coordinate
	std::pair< G4bool, G4bool> xy3_ok; // state

	G4double total_energy; // total energy deposits in calorimeter
	G4double max_slice_energy; // maximim energy deposition in slice
	G4int max_slice_index; // index of the slice with maximim energy deposition

	RawHitCoordinates& hits; // not save
	TrackXYPair main_track;
	TrackXYPair full_track;
};

inline
void
FinalHitCoordinates::getTracks( TrackXYPair& track_main,
	TrackXYPair& track_full) const
{
	track_main = main_track;
	track_full = full_track;
}

inline
TrackXYPair
FinalHitCoordinates::getTrack(G4bool type) const
{
	return type ? full_track : main_track;
}

} // namespace CarbonIonRadiography
