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

#include <gsl/gsl_fit.h>
#include <trec_ccmath.h>
//#include "CIR_ccmath.h"
#include "CIR_HitCoordinates.hh"

#define SIZE 2

namespace {

const G4bool border_down[SIZE] = { true, false };
const G4bool border_up[SIZE] = { false, true };

const G4double threshold_si = 4.0 * CLHEP::MeV;
const G4double threshold_calo = 150.0 * CLHEP::MeV;

const std::pair< G4bool, G4bool> pair_ok( true, true);

// sigma^2 = pitch^2 / 12
const G4double sigma_xy1 = CIR_SIZE_SILICON_STRIP / sqrt(12.0); // sigma on 1 module (Y1-X1 planes) in (um) 
const G4double sigma_xy2 = 94.0; // sigma on 2 module (Y2-X2 planes) in (um) 
const G4double sigma_xy3 = 1000.0; // sigma on 3 module (Y3-X3 planes) in (um) 

void
fit_track( double* mean_z, double* x, double* y, double* w, int n,
	double* a, double* b, double* da, double* db, double* e, double* u)
{
	double W2, W2X, WW, WWF, WWFI, W2Y, W2XY, W2X2, W2Y2;
	int i;

	if (n <= 1) {
		*a = 0.0;
		*b = 0.0;
		*da = 0.0;
		*db = -1.0;
		*e = 4.9;
		return;
	}

	W2 = W2X = W2Y = W2XY = W2X2 = W2Y2 = 0.0;
	*mean_z = 0.0;
	*e = 0.0;
	*u = 0.0;
	for ( i = 0; i < n; ++i) {
		WW = 1.0 / w[i];
		W2 += WW;
		WWF = WW * x[i];
		W2X += WWF;
		W2X2 += WWF * x[i];
		W2XY += WWF * y[i];
		WWFI = WW * y[i];
		W2Y += WWFI;
		W2Y2 += WWFI * y[i];
	}
	
	*mean_z = W2X / W2;
	/* fit parameters */
	*a = (W2XY - W2X * W2Y / W2) / (W2X2 - W2X * W2X / W2);
	*b = (W2Y - (*a) * W2X) / W2;

	for ( i = 0; i < n; ++i)
		*e += pow((y[i] - (*a * x[i] + *b)), 2) / w[i];

	*da = 1.0 / (W2X2 - W2X * W2X / W2);
	*db = 1.0 / W2;
}

void
output_test(G4double v)
{
	G4cout << v << " ";
}

} // namespace

namespace CarbonIonRadiography {

FinalHitCoordinates::FinalHitCoordinates(RawHitCoordinates& raw_hits)
	:
	xy1(std::make_pair( 0.0, 0.0)),
	xy1_ok(std::make_pair( false, false)),
	xy2(std::make_pair( 0.0, 0.0)),
	xy2_ok(std::make_pair( false, false)),
	xy3(std::make_pair( 0.0, 0.0)),
	xy3_ok(std::make_pair( false, false)),
	total_energy(0.0),
	max_slice_energy(0.0),
	max_slice_index(-1),
	hits(raw_hits)
{
/*	G4DataVector& calo = hits.calo;
	total_energy = std::accumulate( calo.begin(), calo.end(), 0.0);
	
	G4DataVector::iterator it = std::max_element( calo.begin(), calo.end());
	if (it != calo.end()) {
		max_slice_energy = *it;
		max_slice_index = it - calo.begin();
	}
*/
	G4DataVector& calo = hits.calo;
	std::vector<G4bool> res( calo.size(), false);

	total_energy = std::accumulate( calo.begin(), calo.end(), 0.0);
	
	std::transform( calo.begin(), calo.end(), res.begin(),
		std::bind2nd(std::greater_equal<G4double>(), threshold_calo));

	int values = std::accumulate( res.begin(), res.end(), 0);
	if (values) {
		std::vector<G4bool>::iterator p = std::find_end(
			res.begin(), res.end(), border_down, border_down + SIZE);
		if (p != res.end()) {
//			max_slice_index = p - res.begin();
			max_slice_index = std::distance( res.begin(), p);
			max_slice_energy = calo.at(max_slice_index);
		}
		else {
			max_slice_index = -1;
			max_slice_energy = 0.0;
		}
	}
}

void
FinalHitCoordinates::calculateCoordinates()
{
	StripsEnergyMap& si = hits.energy;

	for ( StripsEnergyMap::iterator it = si.begin(); it != si.end(); ++it) {
		G4int res = findCoordinate( it->first, it->second);
		if (res == -1) {
			; // Can't finding coordinates in silicon detector
		}
	}
}

G4bool
FinalHitCoordinates::checkCalorimeterData() const
{
	G4DataVector& data = hits.calo;
	std::vector<G4bool> res( data.size(), false);

	std::transform( data.begin(), data.end(), res.begin(),
		std::bind2nd(std::greater_equal<G4double>(), threshold_calo));

	int values = std::accumulate( res.begin(), res.end(), 0);

	return static_cast<G4bool>(values);
}

G4int
FinalHitCoordinates::findCoordinate( TREC::StripGeometryType type, G4DataVector& ene)
{
	const TREC::StripGeometry* geom = TREC::StripGeometry::get(type);
	G4double v = 0;
	G4int res = 0;

	G4DataVector::iterator begin, end, it;
	res = checkOneCluster( ene, begin, end);
	if (!res) {
		if (end == ene.begin()) {
			// one strip cluster
			G4int pos = begin - ene.begin();
			v = -(geom->x * CLHEP::um) // half on detector size
				+ pos * (geom->pitch * CLHEP::um) // strips shift
				+ (geom->pitch * CLHEP::um / 2.0) // half strip offset
				+ (geom->dx * CLHEP::um); // detector offset
		}
		else {
			// one multistrip cluster
			for ( it = begin; it != end; ++it) {
				G4int pos = it - ene.begin();
				v += -(geom->x * CLHEP::um) // half on detector size
					+ pos * (geom->pitch * CLHEP::um) // strips shift
					+ (geom->pitch * CLHEP::um / 2.0) // half strip offset
					+ (geom->dx * CLHEP::um); // detector offset
			}
			v /= (end - begin);
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
		case TREC::MSD_X1:
			xy1.first = v;
			xy1_ok.first = true;
			break;
		case TREC::MSD_Y1:
			xy1.second = -v;
			xy1_ok.second = true;
			break;
		case TREC::MSD_X2:
			xy2.first = v;
			xy2_ok.first = true;
			break;
		case TREC::MSD_Y2:
			xy2.second = -v;
			xy2_ok.second = true;
			break;
		case TREC::MSD_X3:
			xy3.first = v;
			xy3_ok.first = true;
			break;
		case TREC::MSD_Y3:
			xy3.second = -v;
			xy3_ok.second = true;
			break;
		case TREC::MSD__U:
		case TREC::MSD__V:
		default:
			res = -1;
			break;
		}
	}
	return res;
}

G4int
FinalHitCoordinates::checkOneCluster( G4DataVector& ene,
	G4DataVector::iterator& begin, G4DataVector::iterator& end)
{
	std::vector<G4bool> res;
	res.resize(ene.size());

	std::transform( ene.begin(), ene.end(), res.begin(),
		std::bind2nd(std::greater_equal<G4double>(), threshold_si));

	int values = std::accumulate( res.begin(), res.end(), 0);

	if (!values) {
		// no energy bigger than threshold
		return 1;
	}
	else if (values == 1) {
		// one strip cluster
		std::vector<G4bool>::iterator it = std::find( res.begin(), res.end(), true);
		if (it != res.end()) {
			begin = ene.begin() + std::distance( res.begin(), it);
			end = ene.begin();
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
		
		std::vector<G4bool>::iterator pos, up, down;

		int ups = 0, downs = 0;
		
		pos = res.begin();
		while (pos != res.end()) {
			pos = std::search( pos, res.end(), border_up, border_up + SIZE);
			if (pos != res.end()) {
				up = pos + 1;
				pos++;
				ups++;
			}
		}

		pos = res.begin();
		while (pos != res.end()) {
			pos = std::search( pos, res.end(), border_down, border_down + SIZE);
			if (pos != res.end()) {
				down = pos + 1;
				pos++;
				downs++;
			}
		}
		
		// one multistrip cluster
		if (ups == 1 && downs == 1) {
			begin = ene.begin() + std::distance( res.begin(), up);
			end = ene.begin() + std::distance( res.begin(), down);
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
FinalHitCoordinates::calculateTracks( G4bool& track_main,
	G4bool& track_full)
{
	track_main = false;
	track_full = false;

	if (xy1_ok == pair_ok && xy2_ok == pair_ok) {
		track_main = true;
		calculateMainTrack(true); // Y track
		calculateMainTrack(false); // X track
	}
	if (xy1_ok == pair_ok && xy2_ok == pair_ok && xy3_ok == pair_ok) {
		calculateFullTrack(true); // Y track
		calculateFullTrack(false); // X track
		
		if (checkTracksWithinTrajectory()) {
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
FinalHitCoordinates::calculateMainTrack(G4bool type)
{
	Track& main_x = main_track.first;
	Track& main_y = main_track.second;

	G4double f[2] = {}; // x coord for "true", y for "false"
	G4double z[2] = {};
	const TREC::StripGeometry* f1 = 0;
	const TREC::StripGeometry* f2 = 0;

	if (type) { // x coordinate (um)
		f1 = TREC::StripGeometry::get(TREC::MSD_X1);
		f2 = TREC::StripGeometry::get(TREC::MSD_X2);
		f[0] = xy1.first / CLHEP::um;
		f[1] = xy2.first / CLHEP::um;
	}
	else { // y coordinate (um)
		f1 = TREC::StripGeometry::get(TREC::MSD_Y1);
		f2 = TREC::StripGeometry::get(TREC::MSD_Y2);
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
FinalHitCoordinates::calculateFullTrack(G4bool type)
{
	Track& full_x = full_track.first;
	Track& full_y = full_track.second;

	G4double f[3] = {}; // x coord for "true", y for "false"
	G4double z[3] = {}; // z coord
	G4double w[3] = { 58., 94., 1000. }; // sigma
	const TREC::StripGeometry* f1 = 0;
	const TREC::StripGeometry* f2 = 0;
	const TREC::StripGeometry* f3 = 0;

	if (type) { // x coordinate (um)
		f1 = TREC::StripGeometry::get(TREC::MSD_X1);
		f2 = TREC::StripGeometry::get(TREC::MSD_X2);
		f3 = TREC::StripGeometry::get(TREC::MSD_X3);
		f[0] = xy1.first / CLHEP::um;
		f[1] = xy2.first / CLHEP::um;
		f[2] = xy3.first / CLHEP::um;
	}
	else { // y coordinate (um)
		f1 = TREC::StripGeometry::get(TREC::MSD_Y1);
		f2 = TREC::StripGeometry::get(TREC::MSD_Y2);
		f3 = TREC::StripGeometry::get(TREC::MSD_Y3);
		f[0] = xy1.second / CLHEP::um;
		f[1] = xy2.second / CLHEP::um;
		f[2] = xy3.second / CLHEP::um;
	}
	
	if (f1 && f2 && f3) {
		z[0] = f1->z;
		z[1] = f2->z;
		z[2] = f3->z;
		
		// Builtin fit_track function
/*
		double mz, a, b, da, db, e, u;
		fit_track( &mz, z, f, w, 3, &a, &b, &da, &db, &e, &u);
		switch (type) {
		case true: // x coordinate
			full_x.a = a;
			full_x.b = b;
			break;
		case false: // y coordinate
			full_y.a = a;
			full_y.b = b;
			break;
		default:
			break;
		}
*/

		// GSL least squares fit function
/*
		double c0, c1, cov00, cov01, cov11, chisq;
		gsl_fit_wlinear( z, 1, w, 1, f, 1, 3, 
			&c0, &c1, &cov00, &cov01, &cov11, &chisq);
*/
		if (type) // x coordinate
//			full_x.a = c1;
//			full_x.b = c0;
			full_x = Track::create( z, f, w, 3);
		else // y coordinate
//			full_y.a = c1;
//			full_y.b = c0;
			full_y = Track::create( z, f, w, 3);

		// CCMATH Compute least squares coefficients via QR reduction
/*
		const G4int N = 3, FIT = 2;
		G4int id;
		G4double* tmp = new G4double[N * FIT];

		for ( G4int i = 0; i < N; ++i) {
			for ( G4int j = 0; j < FIT; ++j)
				tmp[i * FIT + j] = pow( z[i], j);
		}

		G4double t = ccm_qrlsq( tmp, f, N, FIT, &id);
		switch (type) {
		case true: // x coordinate
//			full_x.first = f[1];
//			full_x.second = f[0];
			full_x = Track( f[1], f[0]);
			break;
		case false: // y coordinate
//			full_y.first = f[1];
//			full_y.second = f[0];
			full_y = Track( f[1], f[0]);
			break;
		default:
			break;
		}
		delete [] tmp;
		tmp = 0;
*/

	}
}

G4bool
FinalHitCoordinates::checkTracksWithinTrajectory()
{
	const TREC::StripGeometry* x3 = TREC::StripGeometry::get(TREC::MSD_X3);
	const TREC::StripGeometry* y3 = TREC::StripGeometry::get(TREC::MSD_Y3);

	Track& main_x = main_track.first;
	Track& main_y = main_track.second;

	// x, y positions in plane XY3 by track (um)
//	G4double main_x3 = main_x.first * x3->z + main_x.second;
//	G4double main_y3 = main_y.first * y3->z + main_y.second;

//	G4double main_x3 = main_x.a() * x3->z + main_x.b();
//	G4double main_y3 = main_y.a() * y3->z + main_y.b();

	G4double main_x3 = main_x.fit(x3->z);
	G4double main_y3 = main_y.fit(y3->z);

	// x, y positions in plane XY3 by hit (um)
	G4double mx3 = xy3.first / CLHEP::um;
	G4double my3 = xy3.second / CLHEP::um;
	
	G4double dist_x3 = sqrt( (mx3 - main_x3) * (mx3 - main_x3) +
		(my3 - main_y3) * (my3 - main_y3));

	return (dist_x3 <= 2 * sigma_xy3);
}

TREC::HitsPositions
FinalHitCoordinates::getPositions()
{
	G4DataVector& calo = hits.calo;
	std::vector<G4bool> res( calo.size(), false);

	total_energy = std::accumulate( calo.begin(), calo.end(), 0.0);
	
	std::transform( calo.begin(), calo.end(), res.begin(),
		std::bind2nd(std::greater_equal<G4double>(), threshold_calo));

	TREC::HitsPositions poss(res);

	StripsEnergyMap& si = hits.energy;

	for ( StripsEnergyMap::iterator it = si.begin(); it != si.end(); ++it) {

		G4DataVector& ene = it->second;
//		G4cout << "Plane: " << StripGeometry::index(it->first) << G4endl;
		std::vector<G4bool> si_res( ene.size(), false);

		std::transform( ene.begin(), ene.end(), si_res.begin(),
			std::bind2nd(std::greater_equal<G4double>(), threshold_si));

//		std::for_each( si_res.begin(), si_res.end(), output_test);
//		G4cout << G4endl;

		poss.add_plane_hits( it->first, si_res);
	}
	
	return poss;
}

} // namespace CarbonIonRadiography
