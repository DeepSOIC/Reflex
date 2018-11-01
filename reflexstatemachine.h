#ifndef REFLEXSTATEMACHINE_H
#define REFLEXSTATEMACHINE_H

#include "statemachine.h"
#include "logging.h"
#include "reflex.h"

class ReflexState: public State
{
public:
    ReflexStateMachine* myMachine(){
        return static_cast<ReflexStateMachine*>(this->machine);
    }

    byte readButton(Reflex::eButtons button) const {
        return Reflex::readButton(this->myMachine()->system, button);
    }

    byte readMainSensor(Reflex::eMainSensors sensor){
        return Reflex::readMainSensor(this->myMachine()->system, sensor);
    }

    byte openMainValve(Reflex::eMainValves valve) {
        return Reflex::openMainValve(this->myMachine()->system, valve);
    }
    byte closeMainValve(Reflex::eMainValves valve) {
        return Reflex::closeMainValve(this->myMachine()->system, valve);
    }
    byte closeAllMainValves() {
        return Reflex::closeAllMainValves(this->myMachine()->system);
    }
};

class ReflexStateMachine : public StateMachine
{
public: //members
    char system = 0;
    const __FlashStringHelper* pgm_name;

public: //methods
    ReflexStateMachine();

    virtual void init(char system, const __FlashStringHelper* pgm_name){
        this->system = system;
        this->pgm_name = pgm_name;
    }

    virtual void stateChanged(byte old_state_number) /*override*/ {
        StateMachine::stateChanged(old_state_number);

        Log::log_pgm(this->pgm_name);
        Log::log(F(" changed state "));
        Log::log(old_state_number);
        Log::log_ram("->");
        Log::logLn(this->current_state);
    }


};

#endif // REFLEXSTATEMACHINE_H
