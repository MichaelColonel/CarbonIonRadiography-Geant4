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

#include <TH1.h>
#include <TH2.h>
#include <TFile.h>

#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <functional>

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4DataVector.hh>

#include "CIR_Defines.hh"
#include "CIR_StripGeometry.hh"
#include "CIR_TrackReconstruction.hh"

namespace {

const G4double calo_x = CIR_SIZE_CALORIMETER * CLHEP::mm / 2.0;  // half size
const G4double calo_y = calo_x;  // half size
const G4double calo_z = CIR_SIZE_CALORIMETER_THICKNESS * CLHEP::mm / 2.0; // half size
const G4double calo_slice_z = CIR_SIZE_CALORIMETER_SLICE_THICKNESS * CLHEP::um / 2.0; // half size
const G4int calo_slices = calo_z / calo_slice_z;

const G4int sizeX1 = -29000;
const G4int sizeX2 = 29000;
const G4int sizeY1 = -29000;
const G4int sizeY2 = 29000;
const G4int binX = 60;
const G4int binY = 60;

} // namespace

namespace CarbonIonRadiography {

TrackReconstruction::~TrackReconstruction()
{
	if (clear_slice_) delete clear_slice_;
	if (clear_fluence_) delete clear_fluence_;
	if (clear_position_) delete clear_position_;
	if (clear_weight_) delete clear_weight_;

	if (object_slice_) delete object_slice_;
	if (object_fluence_) delete object_fluence_;
	if (object_position_) delete object_position_;
	if (object_weight_) delete object_weight_;
}

void
TrackReconstruction::save( const char* filename, const FullTracksVector& data)
{
	// dump data
	std::ofstream dump(filename);

	size_t data_size = data.size();

	dump.write( (char *)&data_size, sizeof(size_t));

	for ( FullTracksVector::const_iterator iter = data.begin();
		iter != data.end(); ++iter)
	{
		const Track& full_x = iter->first.first;
		const Track& full_y = iter->first.second;
		const G4int& position = iter->second;
		dump << full_x << full_y;
		dump.write( (char *)&position, sizeof(G4int));
	}
	dump.close();
}

void
TrackReconstruction::save( const char* filename, const std::vector<HitsPositions>& hits)
{
	// dump data
	std::ofstream dump(filename);

	size_t hits_size = hits.size();

	dump.write( (char *)&hits_size, sizeof(size_t));

	for ( std::vector<HitsPositions>::const_iterator iter = hits.begin();
		iter != hits.end(); ++iter)
	{
		dump << *iter;
	}
	dump.close();
}

void
TrackReconstruction::save( const char* filename, const MainTracksVector& data)
{
	// dump data
	std::ofstream dump(filename);

	size_t data_size = data.size();

	dump.write( (char *)&data_size, sizeof(size_t));

	for ( MainTracksVector::const_iterator iter = data.begin();
		iter != data.end(); ++iter)
	{
		const Track& main_x = iter->first;
		const Track& main_y = iter->second;
		dump << main_x << main_y;
	}
	dump.close();
}

void
TrackReconstruction::load( const char* filename, FullTracksVector& data)
{
	// dump data
	std::ifstream dump(filename);

	size_t data_size = 0;
	G4int position = -1;

	dump.read( (char *)&data_size, sizeof(size_t));

	data.resize(data_size);

	for ( FullTracksVector::iterator iter = data.begin();
		iter != data.end(); ++iter)
	{
		Track& full_x = iter->first.first;
		Track& full_y = iter->first.second;
		dump >> full_x >> full_y;
		dump.read( (char *)&position, sizeof(G4int));
		iter->second = position;
	}
	dump.close();
}

void
TrackReconstruction::load( const char* filename, MainTracksVector& data)
{
	// dump data
	std::ifstream dump(filename);

	size_t data_size = 0;

	dump.read( (char *)&data_size, sizeof(size_t));

	data.resize(data_size);

	for ( MainTracksVector::iterator iter = data.begin();
		iter != data.end(); ++iter)
	{
		Track& main_x = iter->first;
		Track& main_y = iter->second;
		dump >> main_x >> main_y;
	}
	dump.close();
}

void
TrackReconstruction::reconstruct(const char* filename)
{
	TFile* file = new TFile( filename, "RECREATE");

	TH2D* hist0 = new TH2D( "histpr", "Primary Fluence",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	TH2D* hist1 = new TH2D( "histfp", "Fluence with position",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	TH2D* hist2 = new TH2D( "histfw", "Fluence with weight",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	TH2D* hist3 = new TH2D( "histf", "Fluence",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);
	
	TH1I* slice = new TH1I( "slice", "Slice Position",
		calo_slices, 0, calo_slices - 1);

	const StripGeometry* plane_y1 = StripGeometry::strip_geometry(MSD_Y1);
	const StripGeometry* plane_x1 = StripGeometry::strip_geometry(MSD_X1);
	const StripGeometry* plane_y2 = StripGeometry::strip_geometry(MSD_Y2);
	const StripGeometry* plane_x2 = StripGeometry::strip_geometry(MSD_X2);
	const StripGeometry* plane_y3 = StripGeometry::strip_geometry(MSD_Y3);
	const StripGeometry* plane_x3 = StripGeometry::strip_geometry(MSD_X3);

	for ( size_t i = 0; i < tracks_full_.size(); ++i) {
		const G4int& position = tracks_full_[i].second;
		slice->Fill(position);
	}

	for ( size_t i = 0; i < tracks_full_.size(); ++i) {
		const Track& main_x = tracks_main_[i].first;
		const Track& main_y = tracks_main_[i].second;
		const Track& full_x = tracks_full_[i].first.first;
		const Track& full_y = tracks_full_[i].first.second;
		const G4int& position = tracks_full_[i].second;

		G4double full_x_z = (plane_x2->z + plane_x3->z) / 2.0;
		G4double full_y_z = (plane_y2->z + plane_y3->z) / 2.0;

		G4double main_x_z = (plane_x1->z + plane_x2->z) / 2.0;
		G4double main_y_z = (plane_y1->z + plane_y2->z) / 2.0;

		// main track (xy1-xy2) coordinates
		G4double mx = main_x.a() * main_x_z + main_x.b();
		G4double my = main_y.a() * main_y_z + main_y.b();

		// full track (xy1-xy2-xy3) coordinates
		G4double fx = full_x.fit(full_x_z);
		G4double fy = full_y.fit(full_y_z);

		G4double w = slice->GetBinContent(position + 1) / tracks_full_.size();

		hist0->Fill( mx, my);
		hist1->Fill( fx, fy, position);
		hist2->Fill( fx, fy, w);
		hist3->Fill( fx, fy);
	}

	hist0->Write();
	hist1->Write();
	hist2->Write();
	hist3->Write();
	slice->Write();
	file->Close();

	delete file;
}

void
TrackReconstruction::formClearTracksData(const FullTracksVector& clear_tracks)
{
	clear_slice_ = new TH1I( "slice_clear", "Slice",
		calo_slices, 0, calo_slices - 1);

	for ( size_t i = 0; i < clear_tracks.size(); ++i) {
		const G4int& position = clear_tracks[i].second;
		clear_slice_->Fill(position);
	}

	G4DataVector slices(calo_slices);
	for ( G4int i = 1; i <= calo_slices; ++i)
		slices[i - 1] = clear_slice_->GetBinContent(i);

	G4DataVector::iterator iter = std::max_element( slices.begin(), slices.end());
	
	clear_pos_max_ = iter - slices.begin() + 1;

	clear_position_ = new TH2D( "position_clear", "Position",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	clear_fluence_ = new TH2D( "fluence_clear", "Fluence",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	clear_weight_ = new TH2D( "weight_clear", "Weight",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	const StripGeometry* plane_y2 = StripGeometry::strip_geometry(MSD_Y2);
	const StripGeometry* plane_x2 = StripGeometry::strip_geometry(MSD_X2);
	const StripGeometry* plane_y3 = StripGeometry::strip_geometry(MSD_Y3);
	const StripGeometry* plane_x3 = StripGeometry::strip_geometry(MSD_X3);

	for ( size_t i = 0; i < clear_tracks.size(); ++i) {
		const Track& full_x = clear_tracks[i].first.first;
		const Track& full_y = clear_tracks[i].first.second;
		const G4int& position = clear_tracks[i].second;

//		if (position > clear_pos_max_)
//			continue;
	
		G4double w = clear_slice_->GetBinContent(position + 1) / clear_tracks.size();

		G4double full_x_z = (plane_x2->z + plane_x3->z) / 2.0;
		G4double full_y_z = (plane_y2->z + plane_y3->z) / 2.0;

		// full track (xy1-xy2-xy3) coordinates
		G4double fx = full_x.fit(full_x_z);
		G4double fy = full_y.fit(full_y_z);

		clear_position_->Fill( fx, fy, position);
		clear_fluence_->Fill( fx, fy);
		clear_weight_->Fill( fx, fy, w);
	}
}


void
TrackReconstruction::formObjectTracksData(const FullTracksVector& clear_tracks)
{
	formClearTracksData(clear_tracks);

	object_slice_ = new TH1I( "slice_object", "Slice",
		calo_slices, 0, calo_slices - 1);

	for ( size_t i = 0; i < tracks_full_.size(); ++i) {
		const G4int& position = tracks_full_[i].second;
//		if (position > clear_pos_max_ || position < object_pos_min_)
//			continue;
		object_slice_->Fill(position);
	}

	object_fluence_ = new TH2D( "fluence_object", "Fluence",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	object_position_ = new TH2D( "position_object", "Position",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	object_weight_ = new TH2D( "weight_object", "Weight",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	const StripGeometry* plane_y2 = StripGeometry::strip_geometry(MSD_Y2);
	const StripGeometry* plane_x2 = StripGeometry::strip_geometry(MSD_X2);
	const StripGeometry* plane_y3 = StripGeometry::strip_geometry(MSD_Y3);
	const StripGeometry* plane_x3 = StripGeometry::strip_geometry(MSD_X3);

	for ( size_t i = 0; i < tracks_full_.size(); ++i) {
		const Track& full_x = tracks_full_[i].first.first;
		const Track& full_y = tracks_full_[i].first.second;
		const G4int& position = tracks_full_[i].second;

		if (position > clear_pos_max_ || position < object_pos_min_)
			continue;

		G4double full_x_z = (plane_x2->z + plane_x3->z) / 2.0;
		G4double full_y_z = (plane_y2->z + plane_y3->z) / 2.0;

		G4double w = object_slice_->GetBinContent(position + 1) / tracks_full_.size();

		// full track (xy1-xy2-xy3) coordinates
		G4double fx = full_x.fit(full_x_z);
		G4double fy = full_y.fit(full_y_z);

		G4int pos = clear_pos_max_ - position;

		object_position_->Fill( fx, fy, pos);
		object_fluence_->Fill( fx, fy);
		object_weight_->Fill( fx, fy, w);
	}
}

void
TrackReconstruction::reconstruct( const FullTracksVector& clear_tracks,
	const char* filename)
{
	object_pos_min_ = 180;

	formObjectTracksData(clear_tracks);

	G4DataVector positions;
	G4DataVector fluences;
	G4double p, f, w, r, w1;
	for ( Int_t i = 1; i <= object_fluence_->GetNbinsX(); ++i) {
		for ( Int_t j = 1; j <= object_fluence_->GetNbinsY(); ++j) {

			p = object_position_->GetBinContent( i, j);
			f = object_fluence_->GetBinContent( i, j);
			
			positions.push_back(p);
			fluences.push_back(f);
		}
	}

	TH2D* hist1 = new TH2D( "pos", "Positions",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	TH2D* hist2 = new TH2D( "flu", "Fluence",
		binX, sizeX1, sizeX2, binY, sizeY1, sizeY2);

	G4double min = *std::min_element( positions.begin(), positions.end());
	G4double max = *std::max_element( positions.begin(), positions.end());

	G4double flu = std::accumulate( fluences.begin(), fluences.end(), 0.0);
	G4double average = flu / fluences.size();

	for ( G4int i = 1; i <= object_fluence_->GetNbinsX(); ++i) {
		for ( G4int j = 1; j <= object_fluence_->GetNbinsY(); ++j) {
			p = object_position_->GetBinContent( i, j);
			f = object_fluence_->GetBinContent( i, j);
			w = object_weight_->GetBinContent( i, j);

			hist1->SetBinContent( i, j, p / f);
			hist2->SetBinContent( i, j, (p / w * f) + 1);
		}
	}

	TFile* file = new TFile( filename, "RECREATE");

	object_position_->Write();
	object_fluence_->Write();
	object_slice_->Write();
	clear_slice_->Write();
	clear_fluence_->Write();
	clear_position_->Write();
	hist1->Write();
	hist2->Write();
	file->Close();

	delete file;
}
/*
void
TrackReconstruction::reconstruct( const FullTracksVector& clear_tracks,
		const char* filename)
{
	G4int clear_pos = formClearTracksData(clear_tracks);

	object_slice_ = new TH1I( "slice_object", "Slice",
		calo_slices, 0, calo_slices - 1);

	for ( size_t i = 0; i < tracks_full_.size(); ++i) {
		const G4int& position = clear_tracks[i].second;
		object_slice_->Fill(position);
	}
	
	G4DataVector slices(calo_slices);
	for ( G4int i = 1; i <= calo_slices; ++i)
		slices[i - 1] = object_slice_->GetBinContent(i);

	G4DataVector::iterator iter = std::max_element( slices.begin(), slices.end());
	
	G4int pos = iter - slices.begin() + 1;
	
	TFile* file = new TFile( filename, "RECREATE");

	object_fluence_ = new TH2D( "fluence_object", "Fluence",
		60, -30000, 30000, 60, -30000, 30000);

	object_position_ = new TH2D( "position_object", "Position",
		60, -30000, 30000, 60, -30000, 30000);
	
	const StripGeometry* plane_y2 = StripGeometry::strip_geometry(MSD_Y2);
	const StripGeometry* plane_x2 = StripGeometry::strip_geometry(MSD_X2);
	const StripGeometry* plane_y3 = StripGeometry::strip_geometry(MSD_Y3);
	const StripGeometry* plane_x3 = StripGeometry::strip_geometry(MSD_X3);

	for ( size_t i = 0; i < tracks_full_.size(); ++i) {
		const Track& full_x = tracks_full_[i].first.first;
		const Track& full_y = tracks_full_[i].first.second;
		const G4int& position = tracks_full_[i].second;

		if (position > pos)
			continue;

		G4double full_x_z = (plane_x2->z + plane_x3->z) / 2.0;
		G4double full_y_z = (plane_y2->z + plane_y3->z) / 2.0;

		// full track (xy1-xy2-xy3) coordinates
		G4double fx = full_x.fit(full_x_z);
		G4double fy = full_y.fit(full_y_z);

		object_position_->Fill( fx, fy, position);
		object_fluence_->Fill( fx, fy);
	}

	object_position_->Write();
	object_fluence_->Write();
	object_slice_->Write();
	file->Close();

	delete file;
}
*/
} // namespace CarbonIonRadiography
