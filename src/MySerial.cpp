/*
 * MySerial.cpp
 *
 *  Created on: 01.11.2022
 *      Author: Alain
 */
#include "MySerial.h"

MySerial::MySerial(int speed, uint32_t config,int rxd, int txd){
  Serial2.begin(speed, config, rxd, txd);
};

 bool MySerial::isAvailable(){
   return(Serial2.available());
   };

int MySerial::writeIt(std::string cmdFormat2){
  Serial2.write(cmdFormat2.c_str());
  
  return(0);
};
 
int MySerial::readIt(char *data, int len){
  return(Serial2.readBytes(data, len));
  
};