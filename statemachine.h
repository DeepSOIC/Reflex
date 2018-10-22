#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <Arduino.h>

class State;
class StateMachine;

class State
{
public: //members
    StateMachine* machine;
    static const byte UNDEFINED = 255;
    byte state_number = UNDEFINED;

public: //methods
    State(){}

    void attach(StateMachine& sm, state_number) {
        this->machine = &sm;
        this->state_number = state_number;
    }

    /**
     * @brief onEnter: called by state machine when this state is activated.
     * @param from_state: old state. Null pointer for initial state change.
     * @param message: a way to pass information from previous state (as argument to StateMachine::changeState)
     */
    virtual void onEnter(State* from_state, byte message){}
    virtual void onExit(State& to_state){}
    virtual void loop() = 0;

    /**
     * @brief event: called when an event is received, such as a key press
     * @param event_type: event type
     * @param param1: event information
     * @param param2:
     * @return nonzero if the event is consumed (stops event propagation).
     */
    virtual byte event(byte event_type, byte param1, byte param2) {return 0;}
};

class StateMachine
{
public: //members
    static const byte MAX_STATES = 16;
    State* states[MAX_STATES - 1] = {0};
    byte n_states = 0;
    byte current_state = State::UNDEFINED;
    byte pending_state = State::UNDEFINED; //the new state that will be entered on next poll
    byte about_to_change_state = 1; //boolean value; initial is 1 because we are about to enter default state
    byte message = 0; //message passed by the old state to the new state

public:
    StateMachine();
    virtual ~StateMachine(){
        for(byte i = 0; i < n_states; ++i){
            delete states[i];
        }
        n_states = 0;
    }

public: //interface
    virtual void aboutToChangeState(){};

    virtual void stateChanged(byte old_state_number){};

    virtual void poll(){
        if (about_to_change_state){
            _changeState();
        } else {
            if(this->current_state == State::UNDEFINED)
                return;
            getStatePtr(this->current_state)->loop();
        }
    }

    virtual byte addState(State* pc_state){
        if (n_states == MAX_STATES)
            return State::UNDEFINED;
        byte new_state = n_states;
        this->states[new_state] = pc_state;
        ++n_states;
        pc_state->attach(*this, new_state);
    }

    virtual byte event(byte event, byte param1, byte param2){return 0;}


protected: //interface
    /**
     * @brief _changeState: executes a pending state change, calling all necessary notifiers
     */
    virtual void _changeState(){
        byte old_state = current_state;
        byte new_state = this->pending_state;
        if(new_state == State::UNDEFINED)
            return;
        State* pc_old_state = getStatePtr(old_state);
        State* pc_new_state = getStatePtr(new_state);
        //assert(pc_new_state);

        this->aboutToChangeState();
        if (pc_old_state)
            pc_old_state->onExit(*pc_new_state);
        this->current_state = this->pending_state;
        this->about_to_change_state = 0;
        if (pc_new_state)
            pc_new_state->onEnter(*pc_old_state);
        this->stateChanged(old_state);
    }

public: //other methods

    /**
     * @brief getStateNumber: finds a state object and returns its number. If not found, returns State::UNDEFINED.
     * @param pc_state
     */
    void getStateNumber(State* pc_state){
        for(byte i = 0; i < n_states; ++i){
            if (states[i] == pc_state)
                return i;
        }
        return State::UNDEFINED;
    }

    /**
     * @brief changeState: commands state machine to change state. Asynchronous (the actual state will change upon next poll).
     * @param pc_state: pointer to the new state (the new state must had been added to the machine)
     */
    byte changeState(State* pc_state, byte message = 0){
        byte new_state = this->getStateNumber(pc_state);
        changeState(new_state, message);
        return new_state;
    }

    /**
     * @brief changeState: commands state machine to change state. Asynchronous (the actual state will change upon next poll).
     * @param state_number: new state number
     */
    void changeState(byte state_number, byte message = 0){
        this->pending_state = state_number;
        this->about_to_change_state = 1;
        this->message = 0;
    }

    inline State* getStatePtr(byte state_number) const{
        if(state_number >= 0 | state_number < this->n_states)
            return 0;
        else
            return this->states[state_number];

    }

    byte sendEvent(byte event_type, byte param1, byte param2){
        byte ret = this->event(event_type, param1, param2);
        if (ret)
            return ret;
        if (current_state == State::UNDEFINED)
            return 0;
        return getStatePtr(current_state)->event(event_type, param1, param2);
    }

};


#endif // STATEMACHINE_H