#include "tetris.h"

using namespace std;


void cmdHandlerThread() {
    int action; 
    while(1) { 
        char ch = getchar();
 
        inM.lock();
        if ( !getResult ) {
            inM.unlock();
            if ( ch==27 ) {
                getchar();
                ch = getchar();
                
                
                switch ( ch ) {
                    case 65:
                        action = ROTATE;
                        break;

                    case 66:
                        action = DOWN; 
                        break;
                    
                    case 67:
                        action = RIGHT;
                        break;
                    
                    case 68:
                        action = LEFT; 
                        break;
                
                    default:
                        continue;
                        break;
                }
            } else if ( ch==32 ) { //space key
                action = DROP; 
            } else if ( ch==115 ) { //'s'
                action = SWAP;
            } else {
                continue;
            }
        } else {
            inM.unlock(); 
            if ( ch==10 ) {
                action = REPLAY;
            } else if ( ch=='q') {
                action = QUIT;
            } else {
                continue;
            }
        }

        inM.lock(); 
        if ( !discardInput ) { 
            cmdM.lock(); 
            cmdQueue.push( action );
            cmdCV.notify_all(); 
            cmdM.unlock();
        }
        inM.unlock();
    }
}
