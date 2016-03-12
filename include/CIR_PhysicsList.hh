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

#include <G4VModularPhysicsList.hh>
#include <G4EmConfigurator.hh>

class G4VPhysicsConstructor;

namespace CarbonIonRadiography {

class PhysicsList : public G4VModularPhysicsList {
public:
	PhysicsList();
	virtual ~PhysicsList();

	virtual void ConstructParticle();
	virtual void ConstructProcess();
	virtual void SetCuts();
	void AddPhysicsList(const G4String& name);
private:
	void AddStepMax();

	G4EmConfigurator em_config;

	G4String                             emName;
	G4VPhysicsConstructor*               emPhysicsList;
	G4VPhysicsConstructor*               decPhysicsList;
	G4VPhysicsConstructor*               raddecayList;

	std::vector<G4VPhysicsConstructor*>  hadronPhys;
};

} // namespace CarbonIonRadiography
