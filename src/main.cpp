#include "tetris.h"

#include <iostream>
#include <time.h>
#include <stdlib.h>

//using namespace std;

int main() {

    //initialize random number seed
    srand(time(NULL));   
    for(int i=0;i<20;++i )
        rand();
    

    setEnvVar();
        
    gameInit(); 
    // loop in gamePlay(); 
    gameEnd();

    restoreEnvVar();
}
