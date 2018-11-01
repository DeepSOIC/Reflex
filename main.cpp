#include <Arduino.h>

#include "reflex.h"
#include "reflex_thread.h"
#include "reflex_maincontrol.h"
#include "reflex_keyboard.h"
#include "reflex_logging.h"

Host thehost;
MainControlMachine l_column_control;
MainControlMachine r_column_control;
ButtonWatcher kbd;

void setup(){
    Log::init();
    Reflex::initMainSensors();
    Reflex::initMainValves();
    Reflex::initButtons();
    thehost.add(&l_column_control);
    thehost.add(&r_column_control);
    l_column_control.init(Reflex::ES_L, F("Left Column"));
    r_column_control.init(Reflex::ES_R, F("Right Column"));
    thehost.add(&kbd);
}

void loop(){
    thehost.loop();
}
