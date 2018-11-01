#ifndef MAINCONTROL_H
#define MAINCONTROL_H

#include "reflex_rstatemachine.h"
#include "reflex.h"
#include "reflex_events.h"



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
        EM_Fault_Receiver_Full = 8,
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
        void onEnter(State* from_state, byte message) {
            closeAllMainValves();
        }

        byte event(byte event_type, byte param1, byte param2){
            if(event_type == Reflex::EE_BUTTONDOWN){
                switch(param2){
                case Reflex::EB_PLAY:{
                    this->machine->changeState(EMS_Tank1);
                    return 1;
                }break;
                case Reflex::EB_PAUSE:{
                    this->machine->changeState(EMS_Suspend);
                    return 1;
                }break;
                case Reflex::EB_STOP:{
                    this->machine->changeState(EMS_Idle);
                    return 1;
                }break;
                };
            }
            return 0;
        }

        void loop(){
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

        void onEnter(State* from_state, byte message){
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

        byte event(byte event_type, byte param1, byte param2){
            if(event_type == Reflex::EE_BUTTONDOWN){
                switch(param2){
                case Reflex::EB_PLAY:{
                    if (!is_last) {
                        this->machine->changeState(next_state, EM_Skipped);
                        return 1;
                    }
                }break;
                case Reflex::EB_PAUSE:{
                    this->machine->changeState(EMS_Suspend);
                    return 1;
                }break;
                case Reflex::EB_STOP:{
                    this->machine->changeState(EMS_Idle);
                    return 1;
                }break;
                };
            }
            return 0;
        }

        void loop(){
            if(readMainSensor(sensor) == 0){
                if(is_last)
                    this->machine->changeState(EMS_Idle, EM_Finished);
                else
                    this->machine->changeState(next_state, EM_LastTank);
            }
            if(readMainSensor(Reflex::eMainSensors::EMS_COLUMN) == 0){
                this->machine->changeState(EMS_Fault, EM_Fault_ColumnLevel);
            }
            if (readMainSensor(Reflex::eMainSensors::EMS_RECEIVER) == 0){
                this->machine->changeState(EMS_Fault, EM_Fault_Receiver_Full);
            }
        }

    };

    class SuspendState: public ReflexState {
    public:
        byte last_state;

        virtual void onEnter(State* from_state, byte message){
            closeAllMainValves();
            last_state = from_state->state_number;
        }

        byte event(byte event_type, byte param1, byte param2){
            if(event_type == Reflex::EE_BUTTONDOWN){
                switch(param2){
                case Reflex::EB_PLAY:{
                    this->machine->changeState(last_state, EM_Resume);
                    return 1;
                }break;
                case Reflex::EB_STOP:{
                    this->machine->changeState(EMS_Idle);
                    return 1;
                }break;
                };
            }
            return 0;
        }

        void loop(){}
    };

    class FaultState: public SuspendState {
        void onEnter(State* from_state, byte message){
            SuspendState::onEnter(from_state, message);

            if(message == EM_Fault_ColumnLevel)
                Log::logLn(F("Fault: column level low"));

            if(message == EM_Fault_EmptyTank)
                Log::logLn(F("Fault: tank empty"));

            if(message == EM_Fault_Receiver_Full)
                Log::logLn(F("Fault: receiver full"));
        }
    };



public:
    virtual init(byte system, __FlashStringHelper* pgm_name) /*override*/ {
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
    virtual byte event(byte event, byte param1, byte param2){
        if(param1 == this->system)
            return ReflexStateMachine::event(event, param1, param2);
        else
            return 0;
    }
};

#endif // MAINCONTROL_H
