#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>

namespace Log{

void init(){
    Serial.begin(9600);
}

void _printlnPGM(Print &p, const char* pgm_s){
  p.println(
    reinterpret_cast<const __FlashStringHelper *>(   //hack!!
      pgm_s
    )
  );
}

void _printPGM(Print &p, const char* pgm_s){
  p.print(
    reinterpret_cast<const __FlashStringHelper *>(   //hack!!
      pgm_s
    )
  );
}

void log(const __FlashStringHelper* str){
    Serial.print(str);
}

void logLn(const __FlashStringHelper* str){
    Serial.println(str);
}

void log_pgm(const char* pgm_s){
    _printPGM(Serial, pgm_s);
}

void logLn_pgm(const char* pgm_s){
    _printlnPGM(Serial, pgm_s);
}

void log(byte number){
    Serial.print(number);
}

void logLn(byte number){
    Serial.println(number);
}

void log_ram(const char* ram_s){
    Serial.print(ram_s);
}

void logLn_ram(const char* ram_s){
    Serial.println(ram_s);
}

} //namespace

#endif // LOGGING_H

