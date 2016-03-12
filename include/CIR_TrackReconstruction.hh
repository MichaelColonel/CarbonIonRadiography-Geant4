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

#include "CIR_Track.hh"
#include "CIR_HitsPositions.hh"

class TH1I;
class TH2D;

namespace CarbonIonRadiography {

class TrackReconstruction {

public:
	TrackReconstruction( const MainTracksVector& main,
		const FullTracksVector& full);
	virtual ~TrackReconstruction();
	void formClearTracksData(const FullTracksVector& clear_tracks);
	void formObjectTracksData(const FullTracksVector& clear_tracks);

	void reconstruct(const char* filename = "reconstruct.root");
	void reconstruct( const FullTracksVector& clear_tracks,
		const char* filename = "reconstruct.root");

	const MainTracksVector& main_tracks() const { return tracks_main_; }
	const FullTracksVector& full_tracks() const { return tracks_full_; }

	static void save( const char* filename, const FullTracksVector&);
	static void save( const char* filename, const MainTracksVector&);
	static void save( const char* filename, const std::vector<HitsPositions>&);
	static void load( const char* filename, FullTracksVector&);
	static void load( const char* filename, MainTracksVector&);
	static void load( const char* filename, std::vector<HitsPositions>&);

private:
	const MainTracksVector& tracks_main_;
	const FullTracksVector& tracks_full_;
	TH1I* clear_slice_;
	TH1I* object_slice_;
	TH2D* clear_position_;
	TH2D* object_position_;
	TH2D* clear_fluence_;
	TH2D* object_fluence_;
	TH2D* clear_weight_;
	TH2D* object_weight_;
	G4int clear_pos_min_, clear_pos_max_;
	G4int object_pos_min_, object_pos_max_;
};

inline
TrackReconstruction::TrackReconstruction( const MainTracksVector& main,
	const FullTracksVector& full)
	:
	tracks_main_(main),
	tracks_full_(full),
	clear_slice_(0),
	object_slice_(0),
	clear_position_(0),
	object_position_(0),
	clear_fluence_(0),
	object_fluence_(0),
	clear_weight_(0),
	object_weight_(0),
	clear_pos_min_(-1),
	clear_pos_max_(-1),
	object_pos_min_(-1),
	object_pos_max_(-1)
{
}

} // namespace CarbonIonRadiography
