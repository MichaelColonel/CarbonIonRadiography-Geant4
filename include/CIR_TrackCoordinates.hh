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

#include "CIR_Track.hh"
#include "CIR_StripGeometry.hh"
#include "CIR_HitsPositions.hh"
#include "CIR_Defines.hh"

namespace CarbonIonRadiography {

class TrackCoordinates {
public:
	TrackCoordinates(HitsPositions& hits_data);
	TrackCoordinates(const TrackCoordinates& src);
	TrackCoordinates& operator=(const TrackCoordinates& src);
	void calculate_coordinates();
	void calculate_tracks( G4bool& main, G4bool& full);
	void get_tracks( TrackXYPair& track_main, TrackXYPair& track_full) const;
	TrackXYPair get_track(G4bool type) const;
private:
	G4int check_one_cluster( const HitsVector& si_plane_hits,
		HitsVector::const_iterator& begin,
		HitsVector::const_iterator& end) const;
	G4bool check_tracks_within_trajectory();
	G4double multistrip_cluster_sigma( StripGeometryType,
		HitsVector::const_iterator& begin,
		HitsVector::const_iterator& end);
	G4double multistrip_cluster_coordinate( StripGeometryType,
		HitsVector::const_iterator& begin,
		HitsVector::const_iterator& end);

	G4int find_coordinate( StripGeometryType, const HitsVector& si_plane_hits);
	void calculate_main_track(G4bool);
	void calculate_full_track(G4bool);

	std::pair< G4double, G4double> xy1; // coordinate
	std::pair< G4bool, G4bool> xy1_ok; // state
	std::pair< G4double, G4double> xy2; // coordinate
	std::pair< G4bool, G4bool> xy2_ok; // state
	std::pair< G4double, G4double> xy3; // coordinate
	std::pair< G4bool, G4bool> xy3_ok; // state

	HitsPositions& hits; // not save
	TrackXYPair main_track;
	TrackXYPair full_track;
};

inline
void
TrackCoordinates::get_tracks( TrackXYPair& track_main,
	TrackXYPair& track_full) const
{
	track_main = main_track;
	track_full = full_track;
}

inline
TrackXYPair
TrackCoordinates::get_track(G4bool type) const
{
	return type ? full_track : main_track;
}

} // namespace CarbonIonRadiography
