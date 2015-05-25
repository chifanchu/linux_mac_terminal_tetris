#ifndef _TETRIS_H_
#define _TETRIS_H_

#include <sstream>
#include <queue>
#include <vector>
#include <unordered_map>
#include <utility>
#include <mutex>
#include <condition_variable>

using namespace std;


#define XLIMIT      24
#define YLIMIT      10
#define SCOREROW    13
#define SCORECOL    12
#define SWAPROW     17
#define NEXTROW     2
#define LEVELROW    11

#define INIT_SPEED  1200


#define EMPTY       0
#define SHAPE_L     1
#define SHAPE_RL    2
#define SHAPE_T     3
#define SHAPE_N     4
#define SHAPE_RN    5
#define SHAPE_I     6
#define SHAPE_O     7
#define BOUNDARY    8

#define FALL        0
#define ROTATE      1
#define LEFT        2
#define RIGHT       3
#define DOWN        4
#define DROP        5
#define SWAP        6
#define REPLAY      7
#define QUIT        8

#define BOTTON      0
#define NOT_BOTTON  1

#define ANSI_COLOR_BLACK        "\x1b[30m"
#define ANSI_COLOR_RED          "\x1b[31m"
#define ANSI_COLOR_GREEN        "\x1b[32m"
#define ANSI_COLOR_YELLOW       "\x1b[33m"
#define ANSI_COLOR_BLUE         "\x1b[34m"
#define ANSI_COLOR_MAGENTA      "\x1b[35m"
#define ANSI_COLOR_CYAN         "\x1b[36m"
#define ANSI_COLOR_WHITE        "\x1b[37m"
#define ANSI_COLOR_RESET        "\x1b[0m"

#define ANSI_COLOR_RED_BG       "\x1b[41m"
#define ANSI_COLOR_GREEN_BG     "\x1b[42m"
#define ANSI_COLOR_YELLOW_BG    "\x1b[43m"
#define ANSI_COLOR_BLUE_BG      "\x1b[44m"
#define ANSI_COLOR_PURPLE_BG    "\x1b[45m"
#define ANSI_COLOR_CYAN_BG      "\x1b[46m"
#define ANSI_COLOR_WHITE_BG     "\x1b[47m"



extern unordered_map< int, vector<vector<pair<int,int> >* >* > blockLayout;

extern mutex cmdM;
extern condition_variable cmdCV;
extern queue<int> cmdQueue;

extern bool isDown;
extern bool isBotton;
extern condition_variable dropCV;

extern mutex inM;
extern bool discardInput;
extern bool getResult;

extern mutex resetM;
extern bool reset;

extern int speed;

/*
 * configure terminal setting
 * create output control thread
 */ 
void setEnvVar();

/*
 * restore terminal setting
 * terminate output control thread
 */
void restoreEnvVar();

/* 
 * Initialize game variable, basic layout
 * to be game main thread
 * create drop-timeout control thread
 * create command listener and handler thread
 */ 
void gameInit();

/*
 * main function of game
 * be called for each game 
 * return true to start new game
 */
bool gamePlay();

/*
 * output game terminating prompt
 * terminate game main thread
 * terminate drop-timeout control thread
 * terminate command listener and handler thread
 */ 
void gameEnd();


/*
 * invoke output
 * reset and clear input oss
 */
void print( ostringstream &oss );

/*
 * output Control thread
 */
void outputControlThread();


/*
 * drop-timeout control thread
 */
void dropTimeoutThread();


/*
 * command listener and handler thread
 */
void cmdHandlerThread();


/*
 * pause current thread for t millisecond
 */ 
void delayMilliSecond( int t );


/*
 * show result promt
 * return true if restart
 */
bool result();


/*
 * define layout of blocks
 */
void createBlockLayout();


/*
 * class blocks
 * control all blocks in the arena
 */
class Blocks {
    public:
        Blocks(){            
            color[SHAPE_I]  = ANSI_COLOR_RED_BG;
            color[SHAPE_L]  = ANSI_COLOR_GREEN_BG;
            color[SHAPE_N]  = ANSI_COLOR_YELLOW_BG;
            color[SHAPE_O]  = ANSI_COLOR_BLUE_BG;
            color[SHAPE_T]  = ANSI_COLOR_PURPLE_BG;
            color[SHAPE_RL] = ANSI_COLOR_CYAN_BG;
            color[SHAPE_RN] = ANSI_COLOR_WHITE_BG;
            
            nextBlock = rand()%7+1; 
        };       

        /*
         * initialize arena
         * get the first block
         * set the next block
         */
        void reset();
       
        /*
         * set currBlock to nextBlock
         * get a new nextBlock
         * reset block register
         * return false if false to allocate new block
         */ 
        bool newBlk();


        /*
         * move block according to command c
         * return BOTTON if this block is placed to the botton
         */
        int move( int &c );

        /*
         * erase rows if applicable
         */
        void eraseRow(); 


    private:
        /*
         * set cursor to row r, column c
         */
        void setCursorRC( const pair<int,int> &b );
     
        /*
         * erase block on the arena
         */
        void eraseBlockArena();

        /*
         * erase block on the terminal
         */
        void eraseBlockTerminal();

        /* 
         * print block on the arena 
         */
        void printBlockArena();

        /* 
         * print block on the terminal
         */
        void printBlockTerminal();

        /*
         * project block to botton
         */
        void projectBlock();

        /*
         * print next block on the terminal
         */
        void printNextBlock();

        /*
         * print score
         */
        void printScore(); 

        /*
         * check if moveable or placable
         */
        bool checkMove( );
         
        /*
         * rotate the block
         */
        void rotateBlock();
      
        /*
         * swap block
         */
        void swapBlocks();

        int score; 
        int level;
        int currBlock;
        int nextBlock;
        int swapBlock;
        int rotate;
        int nextRotate;
        int rotateMax;
        vector< vector<pair<int,int> >* > *layoutVector;
        pair<int,int> pos; 
        pair<int,int> nextPos;
        pair<int,int> projectPos;
        int projectRotate; 
        bool noProjection;
        int highestArena;            
        bool swaped;

        unordered_map<int,string> color;        

        ostringstream oss;
        int arena[XLIMIT+2][YLIMIT+2];
};


#endif /*TETR_S_H_*/
