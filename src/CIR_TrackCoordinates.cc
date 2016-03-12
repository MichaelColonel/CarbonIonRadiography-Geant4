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
#include <algorithm>
#include <functional>
#include <numeric>

#include "CIR_Constants.hh"
#include "CIR_TrackCoordinates.hh"

namespace {

const std::pair< G4bool, G4bool> pair_ok( true, true);

// sigma^2 = pitch^2 / 12
const G4double sigma_xy1 = CIR_SIZE_SILICON_STRIP / sqrt(12.0); // sigma on 1 module (Y1-X1 planes) in (um) 
const G4double sigma_xy2 = 94.0; // sigma on 2 module (Y2-X2 planes) in (um) 
const G4double sigma_xy3 = 1000.0; // sigma on 3 module (Y3-X3 planes) in (um) 

void
output_test(G4bool v)
{
	G4cout << v << " ";
}

} // namespace

namespace CarbonIonRadiography {

TrackCoordinates::TrackCoordinates(HitsPositions& hits_data)
	:
	xy1(std::make_pair( 0.0, 0.0)),
	xy1_ok(std::make_pair( false, false)),
	xy2(std::make_pair( 0.0, 0.0)),
	xy2_ok(std::make_pair( false, false)),
	xy3(std::make_pair( 0.0, 0.0)),
	xy3_ok(std::make_pair( false, false)),
	hits(hits_data)
{
}

void
TrackCoordinates::calculate_coordinates()
{
	StripsNumbersMap& si = hits.strips_numbers_;

	for ( StripsNumbersMap::iterator it = si.begin(); it != si.end(); ++it) {
		
		HitsVector si_plane_hits = hits.numbers_2_hits(it->first);

		G4cout << "Plane1: " << StripGeometry::index(it->first) << G4endl;
		std::for_each( si_plane_hits.begin(), si_plane_hits.end(), output_test);
		G4cout << G4endl;

		G4int res = find_coordinate( it->first, si_plane_hits);
		if (res == -1) {
			; // Can't finding coordinates in silicon detector
		}
	}
}

G4int
TrackCoordinates::find_coordinate( StripGeometryType type,
	const HitsVector& plane_hits)
{
	const StripGeometry* geom = StripGeometry::strip_geometry(type);
	G4double v = 0;
	G4int res = 0;

	HitsVector::const_iterator begin, end, it;
	res = check_one_cluster( plane_hits, begin, end);
	if (!res) {
		if (end == plane_hits.begin()) {
			// one strip cluster
			G4int pos = std::distance( plane_hits.begin(), begin);
			v = -(geom->x * CLHEP::um) // half on detector size
				+ pos * (geom->pitch * CLHEP::um) // strips shift
				+ (geom->pitch * CLHEP::um / 2.0) // half strip offset
				+ (geom->dx * CLHEP::um); // detector offset
		}
		else {
			// one multistrip cluster
			for ( it = begin; it != end; ++it) {
				G4int pos = std::distance( plane_hits.begin(), it);
				v += -(geom->x * CLHEP::um) // half on detector size
					+ pos * (geom->pitch * CLHEP::um) // strips shift
					+ (geom->pitch * CLHEP::um / 2.0) // half strip offset
					+ (geom->dx * CLHEP::um); // detector offset
			}
			v /= std::distance( begin, end);
		}
	}
	else if (res == 1) {
		// no energy in detector bigger than threshold
		// just skip, exit from function
		;
	}
	else if (res == -1) {
		// error: something went completely wrong
		;
	}

	if (!res) {
		switch (type) {
		case MSD_X1:
			xy1.first = v;
			xy1_ok.first = true;
			break;
		case MSD_Y1:
			xy1.second = -v;
			xy1_ok.second = true;
			break;
		case MSD_X2:
			xy2.first = v;
			xy2_ok.first = true;
			break;
		case MSD_Y2:
			xy2.second = -v;
			xy2_ok.second = true;
			break;
		case MSD_X3:
			xy3.first = v;
			xy3_ok.first = true;
			break;
		case MSD_Y3:
			xy3.second = -v;
			xy3_ok.second = true;
			break;
		case MSD__U:
		case MSD__V:
		default:
			res = -1;
			break;
		}
	}
	return res;
}

G4int
TrackCoordinates::check_one_cluster( const HitsVector& plane_hits,
	HitsVector::const_iterator& begin, HitsVector::const_iterator& end) const
{
	G4int values = std::accumulate( plane_hits.begin(), plane_hits.end(), 0);

	if (!values) {
		// no energy bigger than threshold
		return 1;
	}
	else if (values == 1) {
		// one strip cluster
		begin = std::find( plane_hits.begin(),
			plane_hits.end(), true);
		if (begin != plane_hits.end()) {
			end = plane_hits.begin();
			return 0;
		}
		else {
			// impossible energy value;
			return -1;
		}
	}
	else if (values > 1) {
		// two or more strips cluster / clusters
		// find if its only one multistips cluster or two and more clusters 
		
		HitsVector::const_iterator pos, up, down;

		G4int ups = 0, downs = 0;
		
		pos = plane_hits.begin();
		while (pos != plane_hits.end()) {
			pos = std::search( pos, plane_hits.end(),
				border_up, border_up + border_size);
			if (pos != plane_hits.end()) {
				up = pos + 1;
				pos++;
				ups++;
			}
		}

		pos = plane_hits.begin();
		while (pos != plane_hits.end()) {
			pos = std::search( pos, plane_hits.end(),
				border_down, border_down + border_size);
			if (pos != plane_hits.end()) {
				down = pos + 1;
				pos++;
				downs++;
			}
		}
		
		// one multistrip cluster
		if (ups == 1 && downs == 1) {
			begin = up;
			end = down;
			return 0;
		}
		else {
			// two or more clusters
			return -1;
		}
	}
	else {
		// impossible
		return -1;
	}

	return -1;
}

void
TrackCoordinates::calculate_tracks( G4bool& track_main,
	G4bool& track_full)
{
	track_main = false;
	track_full = false;

	if (xy1_ok == pair_ok && xy2_ok == pair_ok) {
		track_main = true;
		calculate_main_track(true); // Y track
		calculate_main_track(false); // X track
	}
	if (xy1_ok == pair_ok && xy2_ok == pair_ok && xy3_ok == pair_ok) {
		calculate_full_track(true); // Y track
		calculate_full_track(false); // X track
		
		if (check_tracks_within_trajectory()) {
			// Track within initial trajectory
			track_full = true;
		}
		else {
			// Track away from initial trajectory
			;
		}
	}
}

void
TrackCoordinates::calculate_main_track(G4bool type)
{
	Track& main_x = main_track.first;
	Track& main_y = main_track.second;

	G4double f[2] = {}; // x coord for "true", y for "false"
	G4double z[2] = {};
	const StripGeometry* f1 = 0;
	const StripGeometry* f2 = 0;

	if (type) { // x coordinate (um)
		f1 = StripGeometry::strip_geometry(MSD_X1);
		f2 = StripGeometry::strip_geometry(MSD_X2);
		f[0] = xy1.first / CLHEP::um;
		f[1] = xy2.first / CLHEP::um;
	}
	else { // y coordinate (um)
		f1 = StripGeometry::strip_geometry(MSD_Y1);
		f2 = StripGeometry::strip_geometry(MSD_Y2);
		f[0] = xy1.second / CLHEP::um;
		f[1] = xy2.second / CLHEP::um;
	}

	if (f1 && f2) {
		z[0] = f1->z;
		z[1] = f2->z;
		
		G4double a = (f[0] - f[1]) / (z[0] - z[1]);
		G4double b = f[0] - a * z[0];

		if (type) // x coordinate
			main_x = Track( a, b);
		else // y coordinate
			main_y = Track( a, b);
	}
}

void
TrackCoordinates::calculate_full_track(G4bool type)
{
	Track& full_x = full_track.first;
	Track& full_y = full_track.second;

	G4double f[3] = {}; // x coord for "true", y for "false"
	G4double z[3] = {}; // z coord
	G4double w[3] = { 58., 94., 1000. }; // sigma
	const StripGeometry* f1 = 0;
	const StripGeometry* f2 = 0;
	const StripGeometry* f3 = 0;

	if (type) { // x coordinate (um)
		f1 = StripGeometry::strip_geometry(MSD_X1);
		f2 = StripGeometry::strip_geometry(MSD_X2);
		f3 = StripGeometry::strip_geometry(MSD_X3);
		f[0] = xy1.first / CLHEP::um;
		f[1] = xy2.first / CLHEP::um;
		f[2] = xy3.first / CLHEP::um;
	}
	else { // y coordinate (um)
		f1 = StripGeometry::strip_geometry(MSD_Y1);
		f2 = StripGeometry::strip_geometry(MSD_Y2);
		f3 = StripGeometry::strip_geometry(MSD_Y3);
		f[0] = xy1.second / CLHEP::um;
		f[1] = xy2.second / CLHEP::um;
		f[2] = xy3.second / CLHEP::um;
	}
	
	if (f1 && f2 && f3) {
		z[0] = f1->z;
		z[1] = f2->z;
		z[2] = f3->z;

		// GSL least squares fit function
		if (type) // x coordinate
			full_x = Track::create( z, f, w, 3);
		else // y coordinate
			full_y = Track::create( z, f, w, 3);
	}
}

G4bool
TrackCoordinates::check_tracks_within_trajectory()
{
	const StripGeometry* x3 = StripGeometry::strip_geometry(MSD_X3);
	const StripGeometry* y3 = StripGeometry::strip_geometry(MSD_Y3);

	Track& main_x = main_track.first;
	Track& main_y = main_track.second;

	// x, y positions in plane XY3 by track (um)
	G4double main_x3 = main_x.fit(x3->z);
	G4double main_y3 = main_y.fit(y3->z);

	// x, y positions in plane XY3 by hit (um)
	G4double mx3 = xy3.first / CLHEP::um;
	G4double my3 = xy3.second / CLHEP::um;
	
	G4double dist_x3 = sqrt( (mx3 - main_x3) * (mx3 - main_x3) +
		(my3 - main_y3) * (my3 - main_y3));

	return (dist_x3 <= 2 * sigma_xy3);
}

} // namespace CarbonIonRadiography
