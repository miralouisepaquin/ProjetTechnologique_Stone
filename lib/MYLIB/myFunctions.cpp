/* Copyright (C) 2021 Alain Dube
 * All rights reserved.
 *
 * Fonctions utilitaires pour le projet
 *
 * Projet Sac
 * Ecole du Web
 * Cours Objets connectés (c)2021
 *  
 *
 * bool isEqualString(std::string line1, std::string line2)             //Pour vérifier plus simplement que deux chaines sont identiques
 * std::string getValue(std::string data, char separator, int index)    //Pour extraire une partie d'une chaine de caractères avec l'aide d'un index
 * bool replaceAll(std::string& source, const std::string& from, const std::string& to) //Pour remplacer plus facilement une sous chaine
 * std::string get_random_string(unsigned int len)                      //Pour obtenir un chaine aléatoire d'une certaine longeur
 *
*/
#ifndef MYFUNCTIONS_CPP
#define MYFUNCTIONS_CPP

#include <string>
#include <Arduino.h>

//Pour vérifier plus simplement que deux chaines sont identiques
bool isEqualString(std::string line1, std::string line2) {
    return (std::string(line1.c_str()).compare(std::string(line2)) == 0)   ;
    }

//Pour extraire une partie d'une chaine de caractères avec l'aide d'un index
std::string getValue(std::string data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
      if (data.at(i) == separator || i == maxIndex) {
        found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
        }
    return found > index ? data.substr(strIndex[0], strIndex[1]-strIndex[0]) : "";
    }

//Pour remplacer plus facilement une sous chaine
//Retourne 0: Pas trouvé la chaine  1: A eu une modification
bool replaceAll(std::string& source, const std::string& from, const std::string& to) {
    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while(std::string::npos != (findPos = source.find(from, lastPos))) {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
        }

    // Care for the rest after last occurrence
    newString += source.substr(lastPos);
    
    bool modification = (source.compare(newString) != 0);
    source.swap(newString);
    return(modification);
    }

//Pour obtenir un chaine aléatoire d'une certaine longeur
// Generate random string of length "len" chars
// Ne pas oublier d'inclure srand(time(NULL)); dans le Setup
std::string get_random_string(unsigned int len) {
    unsigned int i;
    std::string retValue;
    for (i = 0; i < len; i++) {
        // Add random printable ASCII char
        retValue.push_back(char((esp_random() % ('Z' - 'A')) + 'A'));
        }
    return(retValue);
    }

#endif