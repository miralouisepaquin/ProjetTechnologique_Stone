/*
 * MyStone.cpp
 *
 *  Created on: 01.11.2022
 *      Author: Alain
 */
#include "MyStone.h"

MyStone::MyStone(int speed, uint32_t config, int rxd, int txd){
  mySerial = new MySerial(speed, config, rxd, txd);
};

//Lire le contenu disponible sur le port série
//Retourne les données dans la structure local datasRead
//Si dataRead.id == 0 alors il n'y a pas de données lues sur le port
//Exemple de données qui peuvent être lues sur le port:
//  Basic format(Voir la documentation de l'écran pour plus de détails):
//    Frame head  : ST<
//    Command     : 0x1068 (exemple)
//    Longeur     : 0x0004 (exemple)
//    Data        : ctew   (exemple)
//    Key Value   : 0x01   (exemple)
//    Frame Tail  : >ET
//    CRC         : AB 24  (exemple)
datasRead  MyStone::getValidsDatasIfExists() {
    int n;
    datasRead rd;

    int commande, longeur;
    char data[2048];
    struct {
        union {  //Commande
          unsigned short hexaShort;
          struct  {
            char c1;
            char c2;
            } c;
        } shortDataCommand;
        union { //Longeur
          unsigned short hexaShort;
          struct  {
            char c1;
            char c2;
            } c;
        } shortDataLongeur;

      } shortData;

    //Initialisation
    rd.id = 0;            //0 : Pas de données
    rd.line[0] = 0x00;
    rd.command[0] = 0x00;
    rd.name[0] = 0x00;
    rd.type = 0;

    if(!mySerial->isAvailable()) return (rd);

    //Essai de trouver un S
    n = mySerial->readIt( (char*)data, 1);
    while((n==1) && (data[0] != 'S'))  {
      n = mySerial->readIt( (char*)data, 1);
      };
    if(n<=0) return(rd);
    //std::cout << "S FOUND\n";

    //Essai de trouver un T
    n = mySerial->readIt( (char*)data, 1);
    while((n==1) && (data[0] != 'T'))  {
      n = mySerial->readIt( (char*)data, 1);
      };
    if(n<=0) return(rd);
    //std::cout << "T FOUND\n";

    //Essai de trouver un <T>
    n = mySerial->readIt( (char*)data, 1);
    while((n==1) && (data[0] != '<'))  {
      n = mySerial->readIt( (char*)data, 1);
      };
    if(n<=0) return(rd);
     //std::cout << "< FOUND\n";

    //Lecture de la commande et de la longeur de la donnee
    n = mySerial->readIt( &shortData.shortDataCommand.c.c1, 4);

    //Inverser certaines données
    std::swap(shortData.shortDataCommand.c.c2, shortData.shortDataCommand.c.c1);
    std::swap(shortData.shortDataLongeur.c.c2, shortData.shortDataLongeur.c.c1);

    commande  = shortData.shortDataCommand.hexaShort;
    longeur   = shortData.shortDataLongeur.hexaShort;

    //Lecture Data
     n = mySerial->readIt( (char*)data, longeur);

    switch (commande){

          case 0x0002: { //Version
            int keyValue = (int) data[longeur-1];
            data[longeur-1] = 0x00;

            //Lire les données suivantes : TAIL (3 char ">ET") et CRC (Hexa16)
            char TailDatas[5];
             n = mySerial->readIt( TailDatas, 5);
            //Check if TAIL (>ET) is OK
            if ((n!=5) || (TailDatas[0]!='>') || (TailDatas[1]!='E') || (TailDatas[2]!='T')) return (rd);
            //Nous ne vérifions pas le CRC pour plus de rapidité mais ce serait mieux...
              //Traitement du CRC
              //int crc = TailDatas[4]; crc <<= 8; crc |= TailDatas[3];
              //std::cout << "Crc: " << intToString(crc, "%4X") << "\n";

            rd.id = commande;
            strcpy(rd.command, "Version");
            strcpy(rd.name, data);
            rd.type = keyValue;

            return (rd);
            break;
            };

          case 0x1001: { //Version
            int keyValue = (int) data[longeur-1];
            data[longeur-1] = 0x00;

            //Lire les données suivantes : TAIL (3 char ">ET") et CRC (Hexa16)
            char TailDatas[5];
             n = mySerial->readIt( TailDatas, 5);
            //Check if TAIL (>ET) is OK
            if ((n!=5) || (TailDatas[0]!='>') || (TailDatas[1]!='E') || (TailDatas[2]!='T')) return (rd);
            //Nous ne vérifions pas le CRC pour plus de rapidité mais ce serait mieux...
              //Traitement du CRC
              //int crc = TailDatas[4]; crc <<= 8; crc |= TailDatas[3];
              //std::cout << "Crc: " << intToString(crc, "%4X") << "\n";

            rd.id = commande;
            strcpy(rd.command, "button");
            strcpy(rd.name, data);
            rd.type = keyValue;

            return (rd);
            break;
            };

        default:{

            int keyValue = (int) data[longeur-1];
            data[longeur-1] = 0x00;
            //std::cout << "Key Value: " << keyValue << "\n";
            //std::cout << "Data: " << data << "\n";

            //Lire les données suivantes : TAIL (3 char ">ET") et CRC (Hexa16)
            char TailDatas[5];
             n = mySerial->readIt( TailDatas, 5);

            //Check if TAIL (>ET) is OK
            if ((n!=5) || (TailDatas[0]!='>') || (TailDatas[1]!='E') || (TailDatas[2]!='T')) return (rd);
            //Nous ne vérifions pas le CRC pour plus de rapidité mais ce serait mieux...
              //Traitement du CRC
              //int crc = TailDatas[4]; crc <<= 8; crc |= TailDatas[3];
              //std::cout << "Crc: " << intToString(crc, "%4X") << "\n";

            rd.id = -commande;
            strcpy(rd.command, "???????");
            strcpy(rd.name, data);
            rd.type = keyValue;

            return (rd);
            break;

            };
        };

  return (rd);
};

int MyStone::writeIt(std::string cmdFormat2){
  if(mySerial) mySerial->writeIt(cmdFormat2);
  return(0);
};
 
int MyStone::readIt(char *data, int len){
  if(mySerial) mySerial->readIt(data, len);
  return(0);
};

//Format: ST<{"cmd_code":"set_text","type":"label","widget":"label1","text":"HelloStone"}>ET
void MyStone::setLabel(const char *labelName, const char *value){
    char cmdFormat2[1024];
    sprintf(cmdFormat2, "ST<{\"cmd_code\":\"set_text\",\"type\":\"label\",\"widget\":\"%s\",\"text\":\"%s\"}>ET", labelName, value);
    if(mySerial) mySerial->writeIt(cmdFormat2);
    };

//Format: ST<{"cmd_code":"get_value","type":"progress_bar","widget":"progress_bar"}>ET
void MyStone::getLabel(const char *labelName, const char *value){
    char cmdFormat2[1024];
    sprintf(cmdFormat2, "ST<{\"cmd_code\":\"get_value\",\"type\":\"label\",\"widget\":\"%s\",\"text\":\"%s\"}>ET", labelName, value);
    if(mySerial) mySerial->writeIt(cmdFormat2);
    };


//Format: ST<{\"cmd_code\":\"open_win\",\"widget\":\"window1\"}>ET
//If pageName is empty, use home_page
void MyStone::changePage(const char *pageName) {
    char cmdFormat2[99];
    sprintf(cmdFormat2, "ST<{\"cmd_code\":\"open_win\",\"widget\":\"%s\"}>ET", strlen(pageName) ? pageName : "home_page");
    if(mySerial) mySerial->writeIt(cmdFormat2);
  };


//Format: ST<{"cmd_code":"sys_version","type":"system"}>ET
void MyStone::getVersion() {
    char cmdFormat2[99];
    strcpy(cmdFormat2, "ST<{\"cmd_code\":\"sys_version\",\"type\":\"system\"}>ET");
	if(mySerial) mySerial->writeIt(cmdFormat2);
};
//Format: ST<{"cmd_code":"set_text","type":"button","widget":"button1","text":"HelloStone"}>ET
void MyStone::setTextButton(const char *buttonName, const char *value){
    char cmdFormat2[1024];
    sprintf(cmdFormat2, "ST<{\"cmd_code\":\"set_text\",\"type\":\"button\",\"widget\":\"%s\",\"text\":\"%s\"}>ET", buttonName, value);
    if(mySerial) mySerial->writeIt(cmdFormat2);
    };