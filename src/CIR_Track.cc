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

#include <cmath>
#include <fstream>

#include <gsl/gsl_fit.h>

#include "CIR_ccmath.h"
#include "CIR_Track.hh"

namespace {

const G4int FIT = 2;

} // namespace

namespace CarbonIonRadiography {

Track
Track::create( G4double* z, G4double* f, G4double* w, G4int n)
{
	G4double c0, c1, cov00, cov01, cov11, chisq;
	gsl_fit_wlinear( z, 1, w, 1, f, 1, n, 
		&c0, &c1, &cov00, &cov01, &cov11, &chisq);
	
	return Track( c1, c0, cov00, cov01, cov11);
}

Track
Track::create( G4double* z, G4double* f, G4int n)
{
	G4int id;
	G4double* tmp = new G4double[n * FIT];
	G4double* ff = new G4double[n];

	for ( G4int i = 0; i < n; ++i) {
		ff[i] = f[i]; // just copy
		for ( G4int j = 0; j < FIT; ++j)
			tmp[i * FIT + j] = pow( z[i], j);
	}

	/* G4double t = */ cir_ccmath_qrlsq( tmp, ff, n, FIT, &id);

	Track res = (id == -1) ? Track( ff[1], ff[0]) : Track();
	
	delete [] tmp;
	delete [] ff;
	
	return res;
}

G4double
Track::fit(G4double z) const
{
	G4double v, v_err;
	gsl_fit_linear_est( z, b_, a_, cov00_, cov01_, cov11_, &v, &v_err);

	return v;
}

std::pair< G4double, G4double>
Track::fit_error(G4double z) const
{
	G4double v, v_err;
	gsl_fit_linear_est( z, b_, a_, cov00_, cov01_, cov11_, &v, &v_err);

	return std::make_pair( v, v_err);
}

std::ostream&
operator<<( std::ostream& s, const Track& obj)
{
	s.write( (char *)&obj.a_, sizeof(G4double));
	s.write( (char *)&obj.b_, sizeof(G4double));
	s.write( (char *)&obj.cov00_, sizeof(G4double));
	s.write( (char *)&obj.cov01_, sizeof(G4double));
	s.write( (char *)&obj.cov11_, sizeof(G4double));

	return s;
}

std::istream&
operator>>( std::istream& s, Track& obj)
{
	s.read( (char *)&obj.a_, sizeof(G4double));
	s.read( (char *)&obj.b_, sizeof(G4double));
	s.read( (char *)&obj.cov00_, sizeof(G4double));
	s.read( (char *)&obj.cov01_, sizeof(G4double));
	s.read( (char *)&obj.cov11_, sizeof(G4double));

	return s;
}

} // namespace CarbonIonRadiography
