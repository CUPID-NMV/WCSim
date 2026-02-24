#include "PMTSD.hh"
#include "EventAction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4SystemOfUnits.hh"

PMTSD::PMTSD(G4String name) : G4VSensitiveDetector(name) 
{}

PMTSD::~PMTSD() 
{}

void PMTSD::Initialize(G4HCofThisEvent*) 
{}

G4bool PMTSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    
    // Controlliamo che sia un fotone ottico
    if (aStep->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {

        G4String name = GetName();
        auto eventAction = (EventAction*)G4RunManager::GetRunManager()->GetUserEventAction();

        // 1. Mappiamo il nome del Sensore al suo ID numerico (da 0 a 5)
        G4int pmtID = -1;
        if      (name == "PMT_SD_L0") pmtID = 0; 
        else if (name == "PMT_SD_R0") pmtID = 1;
        else if (name == "PMT_SD_L1") pmtID = 2; 
        else if (name == "PMT_SD_R1") pmtID = 3;
        else if (name == "PMT_SD_L2") pmtID = 4; 
        else if (name == "PMT_SD_R2") pmtID = 5;

        if (pmtID >= 0) {
            
            // 2. Estraiamo i dati fisici
            G4double hitTime = aStep->GetPostStepPoint()->GetGlobalTime();
            G4double energy  = aStep->GetTrack()->GetKineticEnergy();
            G4int trackID    = aStep->GetTrack()->GetTrackID();
            G4int evtID      = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

            // 3. Aggiorniamo i contatori globali in EventAction (Ntuple 0)
            eventAction->AddPMTHit(pmtID); // Nuova funzione ottimizzata!
            eventAction->RegisterPMTHit(hitTime);

            // 4. Salviamo i dettagli di questo fotone su ROOT (Ntuple 1)
            auto am = G4AnalysisManager::Instance();
            am->FillNtupleIColumn(1, 0, evtID);
            am->FillNtupleIColumn(1, 1, trackID);
            am->FillNtupleDColumn(1, 2, energy/eV);   // Energia di arrivo
            am->FillNtupleDColumn(1, 3, hitTime/ns);  // Tempo di arrivo
            am->FillNtupleIColumn(1, 4, pmtID);       // ID del PMT colpito
            am->AddNtupleRow(1);

            // 5. Uccidiamo il fotone così non rimbalza all'infinito dentro al vetro
            aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        }
      
        return true;
    }
    return false;
}

void PMTSD::EndOfEvent(G4HCofThisEvent*) 
{}
