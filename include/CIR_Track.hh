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

#include <G4Types.hh>

#include <vector>
#include <utility>

namespace CarbonIonRadiography {

// Track.first -- the "a" parameter in y = a*x + b
// Track.second -- the "b" parameter in y = a*x + b

//typedef std::pair< G4double, G4double> Track;

class Track;
typedef std::pair< Track, Track > TrackXYPair;
typedef std::pair< TrackXYPair, G4double > TracksEnergyPair;
typedef std::pair< TrackXYPair, G4int > TracksPositionPair;
typedef std::vector<TrackXYPair> MainTracksVector;
typedef std::vector<TracksPositionPair> FullTracksVector;

class Track {
	
friend std::ostream& operator<<( std::ostream&, const Track&);
friend std::istream& operator>>( std::istream&, Track&);

public:
	Track( G4double aa = 0.0, G4double bb = 0.0, G4double cov00 = 0.0,
		G4double cov01 = 0.0, G4double cov11 = 0.0); // GSL
	Track(const Track& src);
	virtual ~Track() {};
	Track& operator=(const Track& src);
	G4bool operator==(const Track& src) const;
	G4bool operator<(const Track& src) const;

	G4double a() const { return a_; }
	G4double b() const { return b_; }
	G4double fit(G4double z) const; // GSL
	std::pair< G4double, G4double> fit_error(G4double z) const; // GSL

	static Track create( G4double* z, G4double* f, G4int n);
	static Track create( G4double* z, G4double* f, G4double* w, G4int n);
private:
	G4double a_;
	G4double b_;
	G4double cov00_;
	G4double cov01_;
	G4double cov11_;
};

inline
Track::Track( G4double aa, G4double bb, G4double cov00,
	G4double cov01, G4double cov11)
	:
	a_(aa),
	b_(bb),
	cov00_(cov00),
	cov01_(cov01),
	cov11_(cov11)
{
}

inline
Track::Track(const Track& src)
	:
	a_(src.a_),
	b_(src.b_),
	cov00_(src.cov00_),
	cov01_(src.cov01_),
	cov11_(src.cov11_)
{
} 

inline
Track&
Track::operator=(const Track& src)
{
	a_ = src.a_;
	b_ = src.b_;
	cov00_ = src.cov00_;
	cov01_ = src.cov01_;
	cov11_ = src.cov11_;
	return *this;
}

inline
G4bool
Track::operator==(const Track& src) const
{
	G4bool a1 = (a_ == src.a_);
	G4bool a2 = (b_ == src.b_);
	G4bool a3 = (cov00_ == src.cov00_);
	G4bool a4 = (cov01_ == src.cov01_);
	G4bool a5 = (cov11_ == src.cov11_);
	return (a1 && a2 && a3 && a4 && a5);
}

inline
G4bool
Track::operator<(const Track& src) const
{
	G4bool a1 = (a_ < src.a_);
	G4bool a2 = (b_ < src.b_);

	return (a1 && a2);
}

} // namespace CarbonIonRadiography
