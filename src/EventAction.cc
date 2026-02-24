#include "EventAction.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

EventAction::EventAction() : G4UserEventAction()
{
    auto am = G4AnalysisManager::Instance();
    am->SetDefaultFileType("root");

    // Tabella 0: Sommario Elettrone
    am->CreateNtuple("Eventi", "Dati Elettrone");
    am->CreateNtupleDColumn("E_init_MeV");      // 0
    am->CreateNtupleDColumn("TrackL_water_mm"); // 1
    am->CreateNtupleDColumn("Edep_water_MeV");  // 2
    am->CreateNtupleIColumn("N_Cerenkov_prod"); // 3
    am->CreateNtupleIColumn("N_Cerenkov_entrati"); // 4
    //am->CreateNtupleIColumn("N_Reflected_entrati"); // 5
    am->CreateNtupleDColumn("T_FirstHit_ns");   // 5
    am->CreateNtupleIColumn("Hits_L_Bot");      // 6
    am->CreateNtupleIColumn("Hits_R_Bot");      // 7
    am->CreateNtupleIColumn("Hits_L_Mid");      // 8
    am->CreateNtupleIColumn("Hits_R_Mid");      // 9
    am->CreateNtupleIColumn("Hits_L_Top");      // 10
    am->CreateNtupleIColumn("Hits_R_Top");      // 11
    am->FinishNtuple(0);

    // Tabella 1: Fotoni (Ottimizzata)
    am->CreateNtuple("Fotoni", "Dati Fotoni");
    am->CreateNtupleIColumn("EventID");           // 0
    am->CreateNtupleIColumn("TrackID");           // 1
    am->CreateNtupleDColumn("E_Hit_eV");          // 2 (Solo energia di arrivo)
    am->CreateNtupleDColumn("Arrival_Time_ns");   // 3
    am->CreateNtupleIColumn("PMT_ID");            // 4
    am->FinishNtuple(1);
}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event*) {
    fCountedPhotons.clear();
    fEinit = 0.; fEdep = 0.; fTrackLength = 0.; 
    fNCerenkovProd = 0; fNCerenkovEntrati = 0; //fNReflectedEntrati = 0;
    for(int i=0; i<6; i++) fPMTHits[i] = 0;
    fFirstHitTime = 999999.*ns;
}

void EventAction::RegisterPMTHit(G4double time) {
    if (time < fFirstHitTime) fFirstHitTime = time;
}

void EventAction::EndOfEventAction(const G4Event*) {
    auto am = G4AnalysisManager::Instance();

    am->FillNtupleDColumn(0, 0, fEinit/MeV);
    am->FillNtupleDColumn(0, 1, fTrackLength/mm);
    am->FillNtupleDColumn(0, 2, fEdep/MeV);
    am->FillNtupleIColumn(0, 3, fNCerenkovProd);
    am->FillNtupleIColumn(0, 4, fNCerenkovEntrati);
    //am->FillNtupleIColumn(0, 5, fNReflectedEntrati);
    am->FillNtupleDColumn(0, 5, (fFirstHitTime > 900000.*ns) ? -1. : fFirstHitTime/ns);
    
    // Riempe le colonne da 6 a 11 con il ciclo for (Super pulito!)
    for(int i=0; i<6; i++) {
        am->FillNtupleIColumn(0, 6+i, fPMTHits[i]);
    }
    am->AddNtupleRow(0);
}
