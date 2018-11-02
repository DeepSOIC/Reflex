#ifndef REFLEXSTATEMACHINE_H
#define REFLEXSTATEMACHINE_H

#include "reflex_statemachine.h"
#include "reflex_logging.h"
#include "reflex.h"

class ReflexStateMachine : public StateMachine
{
public: //members
    byte system = 0;
    const __FlashStringHelper* name;

public: //methods
    ReflexStateMachine(){}

    virtual void init(byte system, const __FlashStringHelper* pgm_name){
        this->system = system;
        this->name = pgm_name;
    }

    virtual void stateChanged(byte old_state_number) /*override*/ {
        StateMachine::stateChanged(old_state_number);

        Log::log(this->name);
        Log::log(F(" changed state "));
        Log::log(old_state_number);
        Log::log_ram("->");
        Log::logLn(this->current_state);
    }
};

class ReflexState: public State
{
public:
    ReflexStateMachine* myMachine();

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
    byte setLED(Reflex::eButtons button, byte val){
        return Reflex::setLED(this->myMachine()->system, button, val);
    }
};

ReflexStateMachine* ReflexState::myMachine(){
    return static_cast<ReflexStateMachine*>(this->machine);
}

#endif // REFLEXSTATEMACHINE_H


