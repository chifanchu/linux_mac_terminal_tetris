#include "tetris.h"

using namespace std;

void Blocks::reset() {
    //initialize arena
    highestArena = XLIMIT+1;
    for ( int r=0; r<=XLIMIT+1; ++r ){
        arena[r][0] = BOUNDARY;
        for ( int c=1; c<=YLIMIT; ++c ) {
            if ( r==XLIMIT+1 ) {
                arena[r][c] = BOUNDARY;
            } else {
                arena[r][c] = EMPTY;
            }
        }
        arena[r][YLIMIT+1] = BOUNDARY;
    }
   
    score = 0;
    level = 0;       
    int r, c;
    r = LEVELROW+1;
    c = 4+YLIMIT*3+SCORECOL;
    oss << "\033[" << r << ";" << c << "H" << level << "  "; 
    print(oss);

    isBotton = false; 
    isDown = false;
    nextBlock = rand()%7+1; 
    swapBlock = EMPTY; 
    noProjection = true; 
   
    //get the first block 
    newBlk();

    printScore();
}


bool Blocks::newBlk() { 
    currBlock = nextBlock;
  
    //print next block in the next-block-region 
    printNextBlock(); 
   
    //reset block variable 
    rotate = 0;
    nextRotate = 0;
    layoutVector = blockLayout[ currBlock ]; 
    rotateMax = layoutVector->size()-1; 
    nextPos.first = 1;
    nextPos.second = 4;
    projectPos.first = XLIMIT+1;
    swaped = false;

    //check if new block is allocable, if not, signal to end the game 
    if ( highestArena==1 )
        return false;

    if ( !checkMove() ) {
        nextPos.first--;
        if ( !checkMove() ) {
            nextPos.first--;
            if ( !checkMove() ) {
                return false;
            }
        }
    }

    pos = nextPos; 
    projectBlock(); 
    printBlockTerminal();
    return true; 
}


int Blocks::move( int &c ) {
   
    eraseBlockTerminal(); 

    //take action accordig to the input command 
    switch (c) {
        case FALL:
            nextPos.first += 1; 
            break;

        case ROTATE:
            rotateBlock(); 
            break;

        case LEFT:
            nextPos.second--; 
            break;

        case RIGHT:
            nextPos.second++;
            break;

        case DOWN: 
            nextPos.first += 1;
            
            cmdM.lock();
            isDown = true;
            dropCV.notify_one();
            cmdM.unlock();
            break;
        
        case DROP:
            while( checkMove() ) {
                nextPos.first++;
            }
            pos.first = nextPos.first-1;
            printBlockArena();
            printBlockTerminal();
            return BOTTON;
            break;
    
        case SWAP:
            swapBlocks();
            break;

        default:
            throw runtime_error( "cmd error in move()" );
            break;
    } 
   
    if ( c!=ROTATE && c!=SWAP ) {
        if ( !checkMove() ) {
            printBlockTerminal(); 
            
            if ( c==FALL || c==DOWN ) {
                //this action touches the botton 
                printBlockArena();  
                return BOTTON;
            } else {
                // this action has no effect 
                nextPos = pos;
                return NOT_BOTTON;
            }
        }
    }
   
    //this action has effect 
    pos = nextPos;
    projectBlock();
    printBlockTerminal();
    
    return NOT_BOTTON;
}


void Blocks::eraseRow() { 
    //turn off input 
    inM.lock();
    discardInput = true;
    inM.unlock();
   
    //clear projection; 
    projectPos.first = XLIMIT+1; 

    //check if any row is erasable 
    queue<int> deletedRow;
    for ( int r=XLIMIT; r>=1; --r ) {
        bool del = true;
        for ( int c=1; c<=YLIMIT; ++c ) {
            if ( arena[r][c]==EMPTY ) {
                del = false;
                break;
            }
        }
        
        if ( del ) {
            score += 5; 
            deletedRow.push(r); 
            pair<int,int> b(r,1);
            setCursorRC( b );
            for ( int c=1; c<=YLIMIT*3/2; ++c ) {  
                oss << "==";
            }
        }
    }
   
    //erase rows 
    if ( deletedRow.size()!=0 ) {
        print( oss ); 
        delayMilliSecond( 300 );
        
        int copyTo = deletedRow.front();
        int beCopied = copyTo-1;
        deletedRow.pop();
        while ( beCopied>=highestArena ) {  
        
            if ( deletedRow.size()!=0 && beCopied==deletedRow.front() ) { 
                deletedRow.pop();
                beCopied--;
                continue; 
            }

            pair<int,int> b(copyTo,1);
            setCursorRC(b);
            for ( int c=1; c<=YLIMIT; ++c ) {
                arena[copyTo][c] = arena[beCopied][c];
                if ( arena[copyTo][c]==EMPTY ) {
                    oss << "   "; 
                } else {
                    oss << color[arena[copyTo][c]] << ANSI_COLOR_BLACK << "[ ]" << ANSI_COLOR_RESET;
                }
            }
            
            copyTo--;
            beCopied--;
        }
        
        for ( int r=copyTo; r>beCopied; --r ) {
            pair<int,int> b(r,1);
            setCursorRC(b);
            for ( int c=1; c<=YLIMIT; ++c ) {
                arena[r][c] = EMPTY;
                oss << "   ";
            }
        }

        highestArena = copyTo+1;
    }
   
    //update score 
    if ( score/50>level && speed>100 ) {
        speed -= 110;
        level++;

        int r, c;
        r = LEVELROW+1;
        c = 4+YLIMIT*3+SCORECOL;
    
        oss << "\033[" << r << ";" << c << "H" << level; 
    }

    print(oss); 
    printScore();
   
    //signal dropTimeoutThread to reset the timer 
    cmdM.lock(); 
    isBotton = true;
    while( cmdQueue.size()>0 ) {
        cmdQueue.pop();
    }
    dropCV.notify_one(); 
    cmdM.unlock();
    
    //turn on input 
    inM.lock();
    discardInput = false;
    inM.unlock();
}


void Blocks::setCursorRC( const pair<int,int> &b ){
    int r = b.first + 1;
    int c = 2 + (b.second-1)*3 + 1;    
    oss << "\033[" << r << ";" << c << "H";
}


void Blocks::printBlockArena() {
    vector< pair<int,int> > *layout = (*layoutVector)[rotate]; 

    pair<int,int> b;
    for ( int i=0; i<layout->size(); ++i ) {
        b.first = pos.first + (*layout)[i].first;
        b.second = pos.second + (*layout)[i].second;
 
        if ( b.first>=1 ) {  
            arena[b.first][b.second] = currBlock;
            if ( b.first<highestArena ) {
                highestArena = b.first;
            }
        } 
    }
}


void Blocks::printBlockTerminal() {
    vector< pair<int,int> > *layout = (*layoutVector)[rotate]; 

    pair<int,int> b;
    for ( int i=0; i<layout->size(); ++i ) {
        b.first = pos.first + (*layout)[i].first;
        b.second = pos.second + (*layout)[i].second;
 
        if ( b.first>=1 ) { 
            setCursorRC( b );
            oss << color[currBlock] << ANSI_COLOR_BLACK << "[ ]" << ANSI_COLOR_RESET;
        }
    }
    
    print(oss); 
}


void Blocks::printNextBlock() {
    
    vector< pair<int,int> > *layout = (*blockLayout[nextBlock])[0]; 

    pair<int,int> b;
    for ( int i=0; i<layout->size(); ++i ) {
        b.first = NEXTROW + 3 + (*layout)[i].first;
        b.second = 13 + (*layout)[i].second; 
     
        if ( b.first>=1 ) { 
            setCursorRC( b );
            oss << "   ";
        }
    }
    
    nextBlock = rand()%7+1; 

    layout = (*blockLayout[nextBlock])[0];

    for ( int i=0; i<layout->size(); ++i ) {
        b.first = NEXTROW + 3 + (*layout)[i].first;
        b.second = 13 + (*layout)[i].second;
 
        setCursorRC( b );
        oss << color[nextBlock] << ANSI_COLOR_BLACK << "[ ]" << ANSI_COLOR_RESET;
    }
    
    print(oss); 
}

void Blocks::eraseBlockArena() {
    vector< pair<int,int> > *layout = (*layoutVector)[rotate]; 

    pair<int,int> b;
    for ( int i=0; i<layout->size(); ++i ) {
        b.first = pos.first + (*layout)[i].first;
        b.second = pos.second + (*layout)[i].second;
        
        if ( b.first>=1 )  
            arena[b.first][b.second] = EMPTY;
    } 
}


void Blocks::eraseBlockTerminal() {
    vector< pair<int,int> > *layout = (*layoutVector)[rotate]; 

    pair<int,int> b;
    for ( int i=0; i<layout->size(); ++i ) {
        b.first = pos.first + (*layout)[i].first;
        b.second = pos.second + (*layout)[i].second; 
     
        if ( b.first>=1 ) { 
            setCursorRC( b );
            oss << "   ";
        }
    }
    print(oss); 
}

void Blocks::printScore() {
    int r, c;
    r = SCOREROW+1;
    c = 4+YLIMIT*3+SCORECOL;
    
    oss << "\033[" << r << ";" << c << "H" << score;
    print( oss ); 
}


bool Blocks::checkMove() {
    //return true; 
    vector< pair<int,int> > *layout = (*layoutVector)[nextRotate]; 
    pair<int,int> b;
    for ( int i=0; i<layout->size(); ++i ) {
        b.first = nextPos.first + (*layout)[i].first;
        b.second = nextPos.second + (*layout)[i].second;
        
        if ( arena[b.first][b.second]!=EMPTY )
            return false;
    }
    return true;
}


void Blocks::projectBlock() {
    pair<int,int> nextProjectPos = pos; 
    vector< pair<int,int> > *lastLayout = (*layoutVector)[projectRotate];  
    vector< pair<int,int> > *layout = (*layoutVector)[rotate];  
    pair<int,int> b; 
    bool found = false; 
    
    //find to place to project
    //search from the top to the bottom
    while( nextProjectPos.first<=XLIMIT+1 ) {
        for ( int i=0; i<layout->size(); ++i ) {
            b.first = nextProjectPos.first + (*layout)[i].first;
            b.second = nextProjectPos.second + (*layout)[i].second;

            if ( b.first>XLIMIT || arena[b.first][b.second]!=EMPTY ) {
                nextProjectPos.first--; 
                found = true; 
                break;
            }
        }
        if ( found )
            break;

        nextProjectPos.first++;
    }
 

    //erase old projection 
    if ( !noProjection ) {
        if ( projectPos.first<XLIMIT+1 ) {
             for ( int i=0; i<lastLayout->size(); ++i ) {
                b.first = projectPos.first + (*lastLayout)[i].first;
                b.second = projectPos.second + (*lastLayout)[i].second;
         
                if ( b.first>=1 ) { 
                    setCursorRC( b );
                    oss << "   ";
                }
            }  
        }
    }

    //create new projection 
    if (found) { 
        for ( int i=0; i<layout->size(); ++i ) {
            b.first = nextProjectPos.first + (*layout)[i].first;
            b.second = nextProjectPos.second + (*layout)[i].second;
     
            if ( b.first>=1 ) { 
                setCursorRC( b );
                oss << "[ ]";
            }
        }
    }
   
    projectPos = nextProjectPos; 
    projectRotate = rotate; 
    noProjection = false; 

    print(oss);
}


void Blocks::rotateBlock() {
    switch ( currBlock ) {
        case SHAPE_O:
            break;

        case SHAPE_L:
        case SHAPE_RL:
        case SHAPE_T:
        case SHAPE_N:
        case SHAPE_RN: 
            nextRotate++;
            if ( nextRotate>rotateMax )
                nextRotate = 0;
            
            if ( !checkMove() ) {
                nextPos.second++;
                if ( !checkMove() ) {
                    nextPos.second -= 2;
                    if ( !checkMove() ) {
                        nextPos = pos; 
                        nextRotate = rotate;
                        break;
                    }
                }
            }
            pos = nextPos;
            rotate = nextRotate;
            break;

        case SHAPE_I:
            nextRotate++;
            if ( nextRotate>rotateMax )
                nextRotate = 0;
            
            if ( !checkMove() ) {
                nextPos.second++;
                if ( !checkMove() ) {
                    nextPos.second -= 2;
                    if ( !checkMove() ) {
                        nextPos.second -= 1;
                        if ( !checkMove() ) {
                            nextPos = pos;
                            nextRotate = rotate;
                            break;
                        }
                    }
                }
            }
            pos = nextPos;
            rotate = nextRotate;
            break;

        default:
            throw runtime_error("error in rotateBlock");
            break;
    }
}


void Blocks::swapBlocks() {
    //if there is currently no block in the swap-region
    if ( !swaped ) { 
        vector< pair<int,int> > *swapLayout; 
        pair<int,int> b;
        
        vector< vector<pair<int,int> >* > *layoutVectorBackup = layoutVector;
        if ( swapBlock!=EMPTY ) 
            layoutVector = blockLayout[swapBlock];
        int rotateBackup = rotate;
        int nextRotateBackup = nextRotate;
        int rotateMaxBackup = rotateMax;
        rotate = 0;
        nextRotate = 0;
        rotateMax = layoutVector->size()-1;
        int nextPosSecondBackup = nextPos.second; 
        
        //check whether swap is applicable   
        if ( swapBlock!=EMPTY ) {  
            //swap blocks
            int swapBackup = swapBlock;
            swapBlock = currBlock;
            currBlock = swapBackup;  
            
            if ( !checkMove() ) {
                nextPos.second++;
                if ( !checkMove() ) {
                    nextPos.second -= 2;
                    if ( !checkMove() ) {
                        nextPos.second -= 1;
                        if ( !checkMove() ) {
                            //swap failed, swap back 
                            nextPos.second = nextPosSecondBackup; 
                            currBlock = swapBlock;
                            swapBlock = swapBackup;
                            layoutVector = layoutVectorBackup; 
                            rotate = rotateBackup;
                            nextRotate = nextRotateBackup;
                            rotateMax = rotateMaxBackup;
                            return;
                        }
                    }
                }
            }
     
            //erase current swap block 
            swapLayout = (*blockLayout[swapBackup])[0];
            
            for ( int i=0; i<swapLayout->size(); ++i ) {
                b.first = SWAPROW + 3 + (*swapLayout)[i].first;
                b.second = 13 + (*swapLayout)[i].second; 
                
                setCursorRC(b);
                oss << "   ";
            }
       
            //erase old projection
            vector< pair<int,int> > *lastProjectLayout = (*layoutVectorBackup)[projectRotate]; 
            for ( int i=0; i<lastProjectLayout->size(); ++i ) {
                b.first = projectPos.first + (*lastProjectLayout)[i].first;
                b.second = projectPos.second + (*lastProjectLayout)[i].second;
     
                if ( b.first>=1 ) { 
                    setCursorRC( b );
                    oss << "   ";
                }
            }  
            noProjection = true; 

        } else {
            swapBlock = currBlock;
            //erase old projection
            vector< pair<int,int> > *lastProjectLayout = (*layoutVectorBackup)[projectRotate]; 
            for ( int i=0; i<lastProjectLayout->size(); ++i ) {
                b.first = projectPos.first + (*lastProjectLayout)[i].first;
                b.second = projectPos.second + (*lastProjectLayout)[i].second;
         
                if ( b.first>=1 ) { 
                    setCursorRC( b );
                    oss << "   ";
                }
            }  
            noProjection = true; 

            newBlk(); 
        }

        //print new swap block
        swapLayout = (*blockLayout[swapBlock])[0];
        
        for ( int i=0; i<swapLayout->size(); ++i ) {
            b.first = SWAPROW + 3 + (*swapLayout)[i].first;
            b.second = 13 + (*swapLayout)[i].second; 
            
            setCursorRC(b);
            oss << color[swapBlock] << ANSI_COLOR_BLACK << "[ ]" << ANSI_COLOR_RESET;
        }
      
        swaped = true; 
        print( oss ); 
    }
}
