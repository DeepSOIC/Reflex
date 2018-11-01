#ifndef KEYBOARD
#define KEYBOARD

#include <Arduino.h>
#include "reflex_thread.h"
#include "reflex.h"
#include "reflex_events.h"
#include "reflex_logging.h"

class ButtonWatcher: public Thread
{
public:
    struct _Data {
        byte stable_state;
        byte current_state;
        unsigned long last_change_time;
        _Data(): stable_state(0), current_state(0), last_change_time(0) {}
    };

    _Data data[Reflex::N_SYSTEMS];

    byte active_system = 0;

    unsigned long debounce_time = 20; //in milliseconds

    virtual void loop(){
        ++active_system;
        active_system = active_system % Reflex::N_SYSTEMS;
        Reflex::selectSystemMux(active_system);

        _Data &d = this->data[active_system];

        byte new_state = getState();
        unsigned long time = millis();
        if (new_state == d.current_state){
            if (time - d.last_change_time > debounce_time){
                if (new_state != d.stable_state){
                    keyStateChanged(d.stable_state, new_state);
                }
                d.stable_state = new_state;
            }
        } else if (new_state != d.current_state) {
            d.current_state = new_state;
            d.last_change_time = time;
        }
    }

    void keyStateChanged(byte old_state, byte new_state){
        for(byte ibut = 0; ibut < Reflex::N_BUTTONS; ++ibut){
            byte mask = 1 << ibut;
            if ((old_state & mask) && !(new_state & mask)){
                this->host->event(Reflex::EE_BUTTONUP, active_system, ibut);
            } else if (!(old_state & mask) && (new_state & mask)) {
                Log::log(F("Button pressed: "));
                Log::log(active_system); Log::log_ram("."); Log::logLn(ibut);
                this->host->fireEvent(Reflex::EE_BUTTONDOWN, active_system, ibut);
            }
        }
    }

    static byte getState(){
        byte s = 0;
        for(byte ibut = 0; ibut < Reflex::N_BUTTONS; ++ibut){
            s = s << 1 & Reflex::readButton(ibut);
        }
        return s;
    }
};

#endif // KEYBOARD
