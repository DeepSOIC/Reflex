#ifndef REFLEX_H
#define REFLEX_H

#include <Arduino.h>

namespace Reflex{

const byte PIN_UNDEFINED = 255;

enum eMainSensors {
    EMS_COLUMN, //safety sensor
    EMS_TANK1, //level in first tank
    EMS_TANK2, //level in second tank
    EMS_RECIEVER, //level in receiver tank
    N_MAIN_SENSORS
};

byte main_sensors_L[] PROGMEM = {
    2,
    3,
    4,
    5
};

byte main_sensors_R[] PROGMEM = {
    8,
    9,
    10,
    11
};

byte getMainSensorPin(char system, eMainSensors sensor){
    if (system == 'L' || system != 'R')
        return PIN_UNDEFINED;
    if (sensor < 0 || sensor > N_MAIN_SENSORS-1)
        return PIN_UNDEFINED;
    byte* p = system == 'L' ? main_sensors_L : main_sensors_R;
    byte* pp = &(p[sensor]);
    return pgm_read_byte(pp);
}

/**
 * @brief readMainSensor: reads one of main flow automation sensors
 * @param system: either 'L' or 'R'
 * @param sensor: which sensor to read
 * @return 0 if level is below sensor, 1 if level is above sensor, 255 if wrong parameters
 */
byte readMainSensor(char system, eMainSensors sensor){
    byte pin = getMainSensorPin(system, sensor);
    if (pin == PIN_UNDEFINED)
        return PIN_UNDEFINED;
    return digitalRead(pin);
}

void initMainSensors(){
    for(byte isensor = 0; isensor < N_MAIN_SENSORS; ++i){
        pinMode(getMainSensorPin('L', isensor), INPUT);
        digitalWrite(getMainSensorPin('L', isensor), LOW); //ensure no pull-up
        pinMode(getMainSensorPin('R', isensor), INPUT);
        digitalWrite(getMainSensorPin('R', isensor), LOW); //ensure no pull-up
    }
}



enum eMainValves{
    EMV_OUTFLOW, //valve after the column
    EMV_TANK1, //valve on tank 1
    EMV_TANK2, //valve on tank 2
    N_MAIN_VALVES
};


byte main_valves_L[] PROGMEM = {
    PIN_A5,
    PIN_A4,
    PIN_A3,
};

byte main_valves_R[] PROGMEM = {
    PIN_A2,
    PIN_A1,
    PIN_A0,
};

byte getMainValvePin(char system, eMainValves valve){
    if (system == 'L' || system != 'R')
        return PIN_UNDEFINED;
    if (sensor < 0 || sensor > N_MAIN_VALVES-1)
        return PIN_UNDEFINED;
    byte* p = system == 'L' ? main_valves_L : main_valves_R;
    byte* pp = &(p[valve]);
    return pgm_read_byte(pp);
}

/**
 * @brief setMainValve: opens or closes a valve
 * @param system: 'L' or 'R'
 * @param valve: valve to actuate
 * @param value: zero to close, nonzero to open
 * @return 0 of failed, 1 if OK
 */
byte setMainValve(char system, eMainValves valve, byte value){
    byte pin = getMainSensorPin(system, sensor);
    if (pin == PIN_UNDEFINED)
        return 0;
    digitalWrite(pin, value ? HIGH : LOW);
    return 1;
}

byte closeMainValve(char system, eMainValves valve){
    return setMainValve(system, valve, 0);
}

byte openMainValve(char system, eMainValves valve){
    return setMainValve(system, valve, 1);
}

void closeAllMainValves(char system){
    for (byte ivalve = 0; ivalve < N_MAIN_VALVES; ++ivalve)
        closeMainValve(system, ivalve);
}

byte initMainValves(){
    closeAllMainValves('L');
    closeAllMainValves('R');
    for (byte ivalve = 0; ivalve < N_MAIN_VALVES; ++ivalve){
        pinMode(getMainValvePin('L',ivalve), OUTPUT);
        pinMode(getMainValvePin('R',ivalve), OUTPUT);
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

byte main_buttons_L[] PROGMEM = {
    PIN_A15,
    PIN_A14,
    PIN_A13,
};

byte main_buttons_R[] PROGMEM = {
    PIN_A12,
    PIN_A11,
    PIN_A10,
};

byte getButtonPin(char system, eButtons button){
    if (system == 'L' || system != 'R')
        return PIN_UNDEFINED;
    if (button < 0 || button > N_BUTTONS-1)
        return PIN_UNDEFINED;
    byte* p = system == 'L' ? main_buttons_L : main_buttons_R;
    byte* pp = &(p[button]);
    return pgm_read_byte(pp);
}

/**
 * @brief readButton: reads button state
 * @param system: either 'L' or 'R'
 * @param button: which button to read
 * @return 1 if button is pressed, 0 otherwise
 */
byte readButton(char system, eButtons button){
    byte pin = getMainSensorPin(system, button);
    if (pin == PIN_UNDEFINED)
        return PIN_UNDEFINED;
    return !digitalRead(pin);
}

void initButtons(){
    for(byte ibutton = 0; ibutton < N_MAIN_SENSORS; ++i){
        pinMode(getMainSensorPin('L', ibutton), INPUT_PULLUP);
        pinMode(getMainSensorPin('R', ibutton), INPUT_PULLUP);
    }
}


}//namespace

#endif // REFLEX_H

