/* Copyright (C) 2022 Alain Dube
 * All rights reserved.
 *
 * Projet Domotique
 * Ecole du Web
* Cours Systèmes embarqués (c)2022
 *  
    @file     MySerial.h
    @author   Alain Dubé
    @version  1.1 22/08/15 
    @description
      Démonstration comment utiliser le PORT SERIE RS232 UART ESP32

    platform = ESP32
    OS = Arduino
    Langage : C++


    Historique des versions
        Version    Date       Auteur       Description
        1.1        22/08/15  Alain       Première version du logiciel

**/
#ifndef MYSERIAL_H
#define MYSERIAL_H

#include <Arduino.h>
#include <string>

class MySerial {
    private:
       
    public:
        MySerial(int speed, uint32_t config, int rxd, int txd);
        ~MySerial() { };

        bool isAvailable();
        int writeIt(std::string cmdFormat2);
        int readIt(char *data, int len);
};
#endif
