#include <Arduino.h>

#include "reflex.h"
#include "thread.h"
#include "maincontrol.h"
#include "keyboard.h"

Host thehost;
MainControlMachine l_column_control;
MainControlMachine r_column_control;
ButtonWatcher kbd;

void setup(){
    reflex::initMainSensors();
    Reflex::initMainValves();
    Reflex::initButtons();
    thehost.add(l_column_control);
    thehost.add(r_column_control);
    l_column_control.init(Reflex::ES_L, F("Left Column"));
    r_column_control.init(Reflex::ES_R, F("Right Column"));
    thehost.add(kbd);
}

void loop(){
    thehost.loop();
}
