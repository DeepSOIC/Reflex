#ifndef MAINCONTROL_H
#define MAINCONTROL_H

#include "reflexstatemachine.h"
#include "reflex.h"



class MainControlMachine: public ReflexStateMachine
{
public: //states
    enum eMessages{
        EM_LastTank = 1,
        EM_Finished = 2,
        EM_Resume = 3,
        EM_Skipped = 4,
        EM_TankEmptied = 5,
        EM_Fault_ColumnLevel = 6,
        EM_Fault_EmptyTank = 7,
    };

    enum eMainStates{
        EMS_Idle,
        EMS_Tank1,
        EMS_Tank2,
        EMS_Suspend,
        EMS_Fault
    };

    class IdleState: public ReflexState {
    public:
        void onEnter(State& from_state, byte message) {
            closeAllMainValves();
        }

        void loop(){
            if(readButton(Reflex::eButtons::EB_PLAY)){
                this->machine->changeState(EMS_Tank1);
            }
        }
    };


    class TankState: public ReflexState {
    public:
        eMainStates next_state = 0;
        byte is_last = 0;
        byte tanknum = 0;
        byte sensor = 0;
        byte tank_valve = 0;

        TankState(Reflex::eMainSensors sensor, Reflex::eMainValves valve, eMainStates next_state){
            this->sensor = sensor;
            this->tank_valve = valve;
            this->next_state = next_state;
        }

        void onEnter(State& from_state, byte message){
            closeAllMainValves();
            if(readMainSensor(sensor) == 0){
                this->machine->changeState(EMS_Fault, EM_Fault_EmptyTank);
                return;
            }
            openMainValve(tank_valve);
            openMainValve(Reflex::eMainValves::EMV_OUTFLOW);
            if(message != EM_Resume)
                is_last = (message == EM_LastTank);
        }

        void loop(){
            if(readButton(Reflex::eButtons::EB_STOP)){
                this->machine->changeState(EMS_Idle);
            }
            if(readButton(Reflex::eButtons::EB_PAUSE)){
                this->machine->changeState(EMS_Suspend);
            }
            if(readButton(Reflex::eButtons::EB_PLAY) && !is_last){
                this->machine->changeState(next_state, EM_Skipped);
            }
            if(readMainSensor(sensor) == 0){
                if(is_last)
                    this->machine->changeState(EMS_Idle, EM_Finished);
                else
                    this->machine->changeState(next_state, EM_LastTank);
            }
            if(readMainSensor(Reflex::eMainSensors::EMS_COLUMN) == 0){
                this->machine->changeState(EMS_Fault, EM_Fault_ColumnLevel);
            }
        }

    };

    class SuspendState: public ReflexState {
    public:
        byte last_state;

        virtual void onEnter(State& from_state, byte message){
            closeAllMainValves();
            last_state = from_state.state_number;
        }

        virtual void loop(){
            if(readButton(Reflex::eButtons::EB_STOP)){
                this->machine->changeState(EMS_Idle);
            }
            if(readButton(Reflex::eButtons::EB_PLAY) && !no_next){
                this->machine->changeState(last_state, EM_Resume);
            }
        }
    };

    class FaultState: public SuspendState {
        void onEnter(State& from_state, byte message){
            SuspendState::onEnter(from_state, message);

            static const char* spgm_columnlevel [] PROGMEM = "Fault: column level low";
            if(message == EM_Fault_ColumnLevel)
                Log::logln_pgm(spgm_columnlevel);

            static const char* spgm_emptytank [] PROGMEM = "Fault: tank empty";
            if(message == EM_Fault_EmptyTank)
                Log::logln_pgm(spgm_emptytank);
        }
    };



public:
    virtual init(char system, char *pgm_name) /*override*/ {
        ReflexStateMachine::init(system, pgm_name);

        addState(new IdleState);
        addState(new TankState(
            Reflex::eMainSensors::EMS_TANK1,
            Reflex::eMainValves::EMV_TANK1,
            EMS_Tank2
        ));
        addState(new TankState(
            Reflex::eMainSensors::EMS_TANK2,
            Reflex::eMainValves::EMV_TANK2,
            EMS_Tank1
        ));
        addState(new SuspendState());
        addState(new FaultState());
    }
};

#endif // MAINCONTROL_H

