#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction();
    virtual ~SteppingAction();

    // Questa funzione viene chiamata a ogni passo di ogni particella
    virtual void UserSteppingAction(const G4Step*);
};

#endif
