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
#include <G4PhysListFactory.hh>
#include <G4VPhysicsConstructor.hh>
#include <G4RunManager.hh>

// Physic lists (contained inside the Geant4 source code, in the 'physicslists folder')
#include <G4HadronPhysicsFTFP_BERT.hh>
#include <G4HadronPhysicsQGSP_FTFP_BERT.hh>
#include <G4HadronPhysicsQGSP_BERT.hh>
#include <G4HadronPhysicsQGSP_BERT_HP.hh>
#include <G4HadronPhysicsQGSP_BIC.hh>
#include <G4HadronPhysicsQGSP_BIC_HP.hh>
#include <G4IonINCLXXPhysics.hh>
#include <G4EmStandardPhysics_option3.hh>
#include <G4EmLivermorePhysics.hh>
#include <G4EmPenelopePhysics.hh>
#include <G4EmExtraPhysics.hh>
#include <G4StoppingPhysics.hh>
#include <G4DecayPhysics.hh>
#include <G4HadronElasticPhysics.hh>
#include <G4IonElasticPhysics.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <G4IonBinaryCascadePhysics.hh>
#include <G4DecayPhysics.hh>
#include <G4NeutronTrackingCut.hh>
#include <G4LossTableManager.hh>
#include <G4UnitsTable.hh>
#include <G4ProcessManager.hh>
#include <G4IonFluctuations.hh>
#include <G4IonParametrisedLossModel.hh>
#include <G4EmProcessOptions.hh>
#include <G4ParallelWorldScoringProcess.hh>

#include <G4IonConstructor.hh>

#include "CIR_GlobalStrings.hh"
#include "CIR_PhysicsList.hh"

namespace CarbonIonRadiography {

/////////////////////////////////////////////////////////////////////////////
PhysicsList::PhysicsList()
	:
	G4VModularPhysicsList()
{
	G4LossTableManager::Instance();

	SetVerboseLevel(1);

	// ******     Definition of some defaults for the physics     *****
	// ******     in case no physics is called by the macro file  *****
	// EM physics
	emPhysicsList = new G4EmStandardPhysics_option3();
	emName = G4String("standard_opt3");

	// Decay physics and all particles
	decPhysicsList = new G4DecayPhysics();
	raddecayList = new G4RadioactiveDecayPhysics();
}

/////////////////////////////////////////////////////////////////////////////
PhysicsList::~PhysicsList()
{
	delete emPhysicsList;
	delete decPhysicsList;
	delete raddecayList;

	for ( size_t i = 0; i < hadronPhys.size(); i++) {
		delete hadronPhys[i];
	}
}

/////////////////////////////////////////////////////////////////////////////
void
PhysicsList::ConstructParticle()
{
	decPhysicsList->ConstructParticle();
}

/////////////////////////////////////////////////////////////////////////////
void
PhysicsList::ConstructProcess()
{
	// transportation
	AddTransportation();

	// electromagnetic physics list
	emPhysicsList->ConstructProcess();
	em_config.AddModels();

	// decay physics list
	decPhysicsList->ConstructProcess();
	raddecayList->ConstructProcess();

	// hadronic physics lists
	for ( size_t i = 0; i < hadronPhys.size(); i++) {
		hadronPhys[i]->ConstructProcess();
	}

	// Add Decay Process
	G4Decay* theDecayProcess = new G4Decay();


	// Add parallel world scoring process
	G4ParallelWorldScoringProcess* theParallelWorldScoringProcess =
		new G4ParallelWorldScoringProcess(ParallelWorldScorringProcessStr);

	theParallelWorldScoringProcess->SetParallelWorld(ParallelWorldStr);

//	theParticleIterator->reset();
	G4ParticleTable::G4PTblDicIterator* theParticleIterator = GetParticleIterator();

	while( (*theParticleIterator)() ) {
    
		G4ParticleDefinition* particle = theParticleIterator->value();
		G4ProcessManager* pmanager = particle->GetProcessManager();

		if (theDecayProcess->IsApplicable(*particle)) { 
			pmanager->AddProcess(theDecayProcess);
			// set ordering for PostStepDoIt and AtRestDoIt
			pmanager->SetProcessOrdering( theDecayProcess, idxPostStep);
			pmanager->SetProcessOrdering( theDecayProcess, idxAtRest);
		}

		if (theParallelWorldScoringProcess->IsApplicable(*particle)) {
			pmanager->AddProcess(theParallelWorldScoringProcess);
			pmanager->SetProcessOrderingToLast( theParallelWorldScoringProcess, idxAtRest);
			pmanager->SetProcessOrdering( theParallelWorldScoringProcess, idxAlongStep, 1);
			pmanager->SetProcessOrderingToLast( theParallelWorldScoringProcess, idxPostStep);
		}

	}
}

void
PhysicsList::AddPhysicsList(const G4String& name)
{
	if (verboseLevel > 1) {
		G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
	}
	if (name == emName) return;


	/////////////////////////////////////////////////////////////////////////////
	//   ELECTROMAGNETIC MODELS
	/////////////////////////////////////////////////////////////////////////////

	bool hadron_models_selected = 0;
	if (name == "standard_opt3") {
		emName = name;
		delete emPhysicsList;
		emPhysicsList = new G4EmStandardPhysics_option3();
//		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmStandardPhysics_option3" << G4endl;

	}
	else if (name == "LowE_Livermore") {
		emName = name;
		delete emPhysicsList;
    
		emPhysicsList = new G4EmLivermorePhysics();
//		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmLivermorePhysics" << G4endl;

	}
	else if (name == "LowE_Penelope") {
		emName = name;
		delete emPhysicsList;
		emPhysicsList = new G4EmPenelopePhysics();
//		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
		G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmPenelopePhysics" << G4endl;
	}
	else if (name == "QGSP_FTFP_BERT") {
		hadronPhys.push_back( new G4HadronPhysicsQGSP_FTFP_BERT());
		hadron_models_selected = 1;
	}
	else if (name == "FTFP_BERT") {
		hadronPhys.push_back( new G4HadronPhysicsFTFP_BERT());
		hadron_models_selected = 1;
	}
	else if (name == "QGSP_BERT") {
		hadronPhys.push_back( new G4HadronPhysicsQGSP_BERT());
		hadron_models_selected = 1;
	}
	else if (name == "QGSP_BERT_HP") {
		hadronPhys.push_back( new G4HadronPhysicsQGSP_BERT_HP());
		hadron_models_selected = 1;
	}
	else if (name == "QGSP_BIC") {
		hadronPhys.push_back( new G4HadronPhysicsQGSP_BIC());
		hadron_models_selected = 1;
	}
	else if (name == "QGSP_BIC_HP") {
		hadronPhys.push_back( new G4HadronPhysicsQGSP_BIC_HP());
		hadron_models_selected = 1;
	}
	else { 
		G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
			<< " is not defined" << G4endl;
	}

	if (hadron_models_selected) {
		G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
		AddPhysicsList("standard_opt3");
		hadronPhys.push_back( new G4EmExtraPhysics());
		hadronPhys.push_back( new G4HadronElasticPhysics());
		hadronPhys.push_back( new G4StoppingPhysics());
		hadronPhys.push_back( new G4IonBinaryCascadePhysics());
		hadronPhys.push_back( new G4NeutronTrackingCut());
//		G4RunManager::GetRunManager()->PhysicsHasBeenModified();
	}
}

/////////////////////////////////////////////////////////////////////////////
void
PhysicsList::SetCuts()
{
	SetCutsWithDefault();
}

} // namespace CarbonIonRadiography
