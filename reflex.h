#ifndef REFLEX_H
#define REFLEX_H

#include <Arduino.h>

namespace Reflex{

const byte PIN_UNDEFINED = 255;

enum eSystems {
    ES_L, //left column system
    ES_R, //right column system
    //ES_COLLECTOR_L,
    //ES_COLLECTOR_R,
    N_SYSTEMS,
};


enum eMainSensors {
    EMS_COLUMN, //safety sensor
    EMS_TANK1, //level in first tank
    EMS_TANK2, //level in second tank
    EMS_RECEIVER, //level in receiver tank
    N_MAIN_SENSORS
};

const byte main_sensors_L[] PROGMEM = {
    2,
    3,
    4,
    5
};

const byte main_sensors_R[] PROGMEM = {
    8,
    9,
    10,
    11
};

byte getMainSensorPin(eSystems sys, eMainSensors sensor){
    if (sys == ES_L || sys != ES_R)
        return PIN_UNDEFINED;
    if (sensor < 0 || sensor > N_MAIN_SENSORS-1)
        return PIN_UNDEFINED;
    byte* p = sys == ES_L ? main_sensors_L : main_sensors_R;
    byte* pp = &(p[sensor]);
    return pgm_read_byte(pp);
}

/**
 * @brief readMainSensor: reads one of main flow automation sensors
 * @param sys: ES_L or ES_R, left or right column system
 * @param sensor: which sensor to read
 * @return 0 if level is below sensor, 1 if level is above sensor, 255 if wrong parameters
 */
byte readMainSensor(eSystems sys, eMainSensors sensor){
    byte pin = getMainSensorPin(sys, sensor);
    if (pin == PIN_UNDEFINED)
        return PIN_UNDEFINED;
    return digitalRead(pin);
}

void initMainSensors(){
    for(byte isensor = 0; isensor < N_MAIN_SENSORS; ++isensor){
        pinMode(getMainSensorPin(ES_L, isensor), INPUT);
        digitalWrite(getMainSensorPin(ES_L, isensor), LOW); //ensure no pull-up
        pinMode(getMainSensorPin(ES_R, isensor), INPUT);
        digitalWrite(getMainSensorPin(ES_R, isensor), LOW); //ensure no pull-up
    }
}



enum eMainValves{
    EMV_OUTFLOW, //valve after the column
    EMV_TANK1, //valve on tank 1
    EMV_TANK2, //valve on tank 2
    N_MAIN_VALVES
};


const byte main_valves_L[] PROGMEM = {
    PIN_A5,
    PIN_A4,
    PIN_A3,
};

const byte main_valves_R[] PROGMEM = {
    PIN_A2,
    PIN_A1,
    PIN_A0,
};

byte getMainValvePin(eSystems sys, eMainValves valve){
    if (sys == ES_L || sys != ES_R)
        return PIN_UNDEFINED;
    if (valve < 0 || valve > N_MAIN_VALVES-1)
        return PIN_UNDEFINED;
    byte* p = sys == ES_L ? main_valves_L : main_valves_R;
    byte* pp = &(p[valve]);
    return pgm_read_byte(pp);
}

/**
 * @brief setMainValve: opens or closes a valve
 * @param system: ES_L or ES_R
 * @param valve: valve to actuate
 * @param value: zero to close, nonzero to open
 * @return 0 of failed, 1 if OK
 */
byte setMainValve(eSystems sys, eMainValves valve, byte value){
    byte pin = getMainValvePin(sys, valve);
    if (pin == PIN_UNDEFINED)
        return 0;
    digitalWrite(pin, value ? HIGH : LOW);
    return 1;
}

byte closeMainValve(eSystems sys, eMainValves valve){
    return setMainValve(sys, valve, 0);
}

byte openMainValve(eSystems sys, eMainValves valve){
    return setMainValve(sys, valve, 1);
}

byte closeAllMainValves(eSystems sys){
    if (sys > N_SYSTEMS)
        return 0;
    for (byte ivalve = 0; ivalve < N_MAIN_VALVES; ++ivalve)
        closeMainValve(sys, ivalve);
    return 1;
}

byte initMainValves(){
    closeAllMainValves(ES_L);
    closeAllMainValves(ES_R);
    for (byte ivalve = 0; ivalve < N_MAIN_VALVES; ++ivalve){
        pinMode(getMainValvePin(ES_L, ivalve), OUTPUT);
        pinMode(getMainValvePin(ES_R, ivalve), OUTPUT);
    }
}


/*
const byte N_COLLECTOR_SENSORS = 8;

byte collector_sensors_L[] PROGMEM = {
    22,
    24,
    26,
    28,
    30,
    32,
    34,
    36,
};

byte collector_sensors_R PROGMEM = {
    38,
    40,
    42,
    44,
    46,
    48,
    50,
    52,
};
*/


enum eButtons {
    EB_PLAY,
    EB_PAUSE,
    EB_STOP,
    N_BUTTONS
};

const byte button_pins[] PROGMEM = {
    14,
    15,
    16,
};

const byte led_pins[] PROGMEM = {
    PIN_A8,
    PIN_A9,
    PIN_A10,
};

static byte led_state[4] = {0};

byte getButtonPin(eButtons button){
    if (button < 0 || button > N_BUTTONS-1)
        return PIN_UNDEFINED;
    byte* p =  button_pins;
    byte* pp = &(p[button]);
    return pgm_read_byte(pp);
}

byte getSystemMuxPin(eSystems sys){
    if(sys == ES_L)
        return 17;
    else if (sys == ES_R)
        return 18;
    else
        return PIN_UNDEFINED;
}

byte getLEDPin(eButtons button){
    if (button < 0 || button > N_BUTTONS-1)
        return PIN_UNDEFINED;
    byte* p =  led_pins;
    byte* pp = &(p[button]);
    return pgm_read_byte(pp);
}

byte updateLEDs(eSystems system){
    byte state = 0;
    state = led_state[system];
    for(byte iled = 0; iled < N_BUTTONS; ++iled){
        digitalWrite(getLEDPin(iled), (state >> iled) & 1 ? LOW : HIGH);
    }
    return 1;
}

byte selectSystemMux(eSystems sys){
    if (sys >= N_SYSTEMS)
        return 0;
    for(byte isys = 0; isys < N_SYSTEMS; isys++){
        digitalWrite(getSystemMuxPin(isys), isys == sys ? HIGH : LOW);
    }
    updateLEDs(sys);
    return 1;
}

/**
 * @brief setLED: sets status led. It doesn't actually switch the led on, it
 * only changes state variable. The led will light up when the corresponding
 * system mux is activated.
 * @param sys
 * @param button
 * @param state: 1 for on, 0 for off
 * @return 1 if successful, 0 if error.
 */
byte setLED(eSystems sys, eButtons button, byte state){
    byte* ptr = &(led_state[sys]);
    byte mask = (byte)(1) << button;
    *ptr = *ptr & ~mask | (state ? B111 : 0) & mask;
    return 1;
}

/**
 * @brief readButton: reads button state. The buttons from currently selected system are read. Select the system by calling selectSystemMux() first.
 * @param button: which button to read
 * @return 1 if button is pressed, 0 otherwise
 */
byte readButton(eButtons button){
    byte pin = getButtonPin(button);
    if (pin == PIN_UNDEFINED)
        return PIN_UNDEFINED;
    return digitalRead(pin);
}

void initButtons(){
    for(byte ibutton = 0; ibutton < N_BUTTONS; ++ibutton){
        pinMode(getButtonPin(ibutton), INPUT);
        pinMode(getLEDPin(ibutton), INPUT);
    }
    for(byte isys = 0; isys<N_SYSTEMS; ++isys){
        pinMode(getSystemMuxPin(isys), OUTPUT);
    }
}



}//namespace

#endif // REFLEX_H

