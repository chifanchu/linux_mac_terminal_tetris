#include "tetris.h"

#include <mutex>
#include <condition_variable>

using namespace std;

void dropTimeoutThread() { 
    unique_lock<mutex> lck(cmdM);  
    int currSpeed = speed; 
    while(1) {    
        resetM.lock(); 
            if ( reset ) {
                reset = false;
                currSpeed = speed;
            }
        resetM.unlock();
        
        dropCV.wait_for( lck, chrono::milliseconds( currSpeed ) ); 
        //wether or not reset the timer 
        while ( isBotton || isDown ) { 
            isBotton = false;  
            isDown = false;
            currSpeed = speed; 
            dropCV.wait_for( lck, chrono::milliseconds( currSpeed ) );
        }
        cmdQueue.push( FALL );
        cmdCV.notify_one();
    }
}

