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
#include <G4ParticleDefinition.hh>
#include <G4Geantino.hh>
#include <G4IonTable.hh>

#include <Randomize.hh>

#include "CIR_Defines.hh"
#include "CIR_PrimaryGeneratorAction.hh"

namespace {

// Carbon-12
G4int Z = 6, A = 12;
G4double ionCharge = 6. * CLHEP::eplus;
G4double ee = 0. * CLHEP::keV; // excit. energy
G4double energy = CIR_ENERGY_PER_NUCLEON * A * CLHEP::MeV;

} // namespace

namespace CarbonIonRadiography {

PrimaryGeneratorAction::PrimaryGeneratorAction()
	:
	gun(0)
{
	// Definition of the General particle Source
	gun = new G4ParticleGun();

	SetDefaultPrimaryParticle();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	delete gun;
}

void
PrimaryGeneratorAction::SetDefaultPrimaryParticle()
{
	// Geantino dummy particle gun
	gun->SetParticleDefinition(G4Geantino::Geantino());
}

void
PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
	G4ParticleDefinition* particle = gun->GetParticleDefinition();
	if (particle == G4Geantino::Geantino()) {
		G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon( Z, A, ee);

		gun->SetParticleDefinition(ion);
		gun->SetParticleCharge(ionCharge);
		gun->SetParticleMomentumDirection(G4ThreeVector( 0., 0., 1.));
		gun->SetParticleEnergy(energy);
	}

	G4double x0 = 60.0 * (G4UniformRand() - 0.5) * CLHEP::mm;
//	G4double y0 = 0.0 * CLHEP::mm;
	G4double y0 = 60.0 * (G4UniformRand() - 0.5) * CLHEP::mm;

	gun->SetParticlePosition(G4ThreeVector( x0, y0,
		-CIR_DISTANCE_BEAM_Y1 * CLHEP::mm));

//	gun->SetParticlePosition(G4ThreeVector( 0.28 * CLHEP::cm, 0.30 * CLHEP::cm,
//		-CIR_DISTANCE_BEAM_Y1 * CLHEP::mm));

//	gun->SetParticlePosition(G4ThreeVector( 0.0 * CLHEP::cm, 0.0 * CLHEP::cm,
//		-CIR_DISTANCE_BEAM_Y1 * CLHEP::mm));

	gun->GeneratePrimaryVertex(event);
}

} // namespace CarbonIonRadiography
