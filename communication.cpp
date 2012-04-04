#include <iostream>
#include "example.h"
using namespace std;
 
// Print Hello World! op het scherm.
 
// In C++ krijgen functies een type mee, dit is meestal void of int.
int random(void)
{   
    // endl staat voor end line en doet dus hetzelfde als \n.
    // betekenis code: verplaats naar cout eerst "Hello World!" en daarna endl.
    // cout plaatst de uitvoer vervolgens op het scherm.
    cout <<  "Hello World!" << endl;
 
    // dit betekent: programma afgesloten zonder problemen 
    return 0;     
}
