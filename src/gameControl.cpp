#include "tetris.h"

#include <time.h>
#include <stdlib.h>

#include <iostream>
#include <termios.h>
#include <unistd.h>

#include <sstream>
#include <deque>
#include <vector>
#include <utility>
#include <string>
#include <unordered_map>
#include <queue>

#include <thread>
#include <condition_variable>
#include <mutex>


using namespace std;

bool terminating = false;
termios oldt, newt;

thread *output;
condition_variable outputReadyCV;
bool ready = false;
mutex outM;
condition_variable outCV;
deque<string> outQueue;

thread *cmd;
thread *t;

unordered_map< int, vector<vector<pair<int,int> >* >* > blockLayout;

mutex cmdM;
condition_variable cmdCV;
queue<int> cmdQueue;
Blocks *blk;

bool isDown = false;
bool isBotton = false;
condition_variable dropCV;

mutex inM;
bool discardInput = false;

int speed = INIT_SPEED;
mutex resetM;
bool reset = false;

bool getResult = false;



void setEnvVar() {
    
    //configure terminal echo
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    //hide cursor 
    cout << "\e[?25l\n";

    //create output control thread  
    unique_lock<mutex> lck( outM );
    output = new thread( outputControlThread ); 
    outputReadyCV.wait( lck ); 
    outM.unlock(); 
}

void restoreEnvVar() { 
    //restore terminal echo 
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    //terminate output control thread 
    outM.lock();
    terminating = true;
    outCV.notify_one();
    outM.unlock();
    output->join();
    delete output; 

    //restore cursor 
    cout << "\e[?25h\n";
}


void gameInit() {
    static ostringstream oss;
    blk = new Blocks;
    cmd = new thread( cmdHandlerThread );
    t = new thread( dropTimeoutThread );
    
    createBlockLayout(); 

    //this loop between different games 
    do {    

    //reset speed 
    speed = INIT_SPEED; 
    resetM.lock();
    reset = true;
    resetM.unlock();

    //clear screen
    oss << "\033[2J"; //clear screen
    oss << "\033[1;1H"; //go back to first row
  
    //print initial layout
    for ( int r=0; r<XLIMIT+2; ++r ) {
        oss << ANSI_COLOR_CYAN_BG << "  " << ANSI_COLOR_RESET;  
        for ( int c=1; c<=YLIMIT; ++c ) {
            if ( r==0 || r==XLIMIT+1 ) {
                oss << ANSI_COLOR_CYAN_BG << "   " << ANSI_COLOR_RESET;
            } else {
                oss << "   ";
            }
        }
        oss << ANSI_COLOR_CYAN_BG << "  " << ANSI_COLOR_RESET; 
    
        if ( r==NEXTROW ) {
            oss << "    NEXT";
        } else if ( r==LEVELROW ) { 
            oss << "    LEVEL: 0    ";
        } else if ( r==SCOREROW ) {
            oss << "    SCORE:      ";
        } else if ( r==SWAPROW ) {
            oss << "    SWAP";
        }
        oss << '\n';
    }
    print( oss );

    //reset block setting 
    blk->reset();

    } while ( gamePlay() ); 
}


bool gamePlay() {

    static ostringstream oss;     
    int action;

    unique_lock<mutex> lck(cmdM);  
    
    while(1) {
        while ( cmdQueue.size()==0 ) {
            cmdCV.wait( lck );
        }
    
        do {
            action = cmdQueue.front(); 
            cmdQueue.pop();   
            cmdM.unlock();
            
            if ( BOTTON==blk->move( action )) {
                blk->eraseRow();  
                if ( true!=blk->newBlk() ) {
                    //ending this game 
                    return result(); 
                } 
            }   

            cmdM.lock(); 
        } while( cmdQueue.size()>0 );     
    } 
}


void gameEnd() {
    delete blk; 
}


void print( ostringstream &oss ) {
    outM.lock();
    outQueue.push_back( oss.str() );
    outCV.notify_one();
    oss.str("");
    oss.clear();
    outM.unlock();
}


void outputControlThread() {
    while(1) {
        unique_lock<mutex> lock(outM);
       
        //signal the main thread "outputControlThread is working" 
        if ( !ready ) {
            outputReadyCV.notify_one();
            ready = true;
        }
        
        while ( outQueue.size()==0 ) {
            if ( terminating ) {
                return;
            }
            outCV.wait( lock );
        }
        
        while ( outQueue.size()>0 ) {
            string str = outQueue.front();
            outQueue.pop_front();
            cout << str;
        }
        
        cout.flush();
    }
}



void delayMilliSecond( int t ) {
    mutex dummy;
    condition_variable cv; 
    unique_lock<mutex> lck(dummy);  
    cv.wait_for( lck, chrono::milliseconds( t ) );
}



bool result() {
    static ostringstream oss; 
    int r, c;
    r = 4+XLIMIT;
    c = 1;
    oss << "\033[" << r << ";" << c << "H";
    
    oss << "GOOD GAME !!!" << endl;

    oss << endl << "Restart(Enter)" << endl;
    oss << "Quit(q)" << endl;

    print( oss );
  

    inM.lock();
    getResult = true;
    inM.unlock();

    //get user command to end/continue the game 
    int a = 0;
    while(1) {
        cmdM.lock();
        
        while ( cmdQueue.size()!=0 ) {
            if ( a==REPLAY || a==QUIT ) {
                cmdQueue.pop();
            } else {
                a = cmdQueue.front();
                cmdQueue.pop();
            }
        }
       
        if ( a==REPLAY || a==QUIT )
            break;
        
        cmdM.unlock();
    }

    inM.lock();
    getResult = false;
    inM.unlock();

    if ( a==REPLAY )
        return true;
    else if ( a==QUIT )
        return false; 
    else
        throw runtime_error( "error in getting user's answer" );
}
