#include "tetris.h"

#include <unordered_map>
#include <vector>
#include <utility>

using namespace std;

void createBlockLayout() {
    vector< vector<pair<int,int> >* > *thisKindBlock; 
    vector< pair<int, int> > *layout;
    pair<int, int> b;   

//SHAPE_L
    thisKindBlock = new vector< vector<pair<int,int> >* >;
        
    /*
     * [ ][ ][ ]---
     * [ ]---------
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=0;  layout->push_back(b); 
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=1;  b.second=0;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    /*
     * ---[ ][ ]---
     * ------[ ]---
     * ------[ ]---
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    b.first=2;  b.second=2;  layout->push_back(b); 
    thisKindBlock->push_back( layout );
    
    /*
     * ------[ ]---
     * [ ][ ][ ]---
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=1;  b.second=0;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    thisKindBlock->push_back( layout );
  
    /*
     * ---[ ]------
     * ---[ ]------
     * ---[ ][ ]---
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=2;  b.second=1;  layout->push_back(b); 
    b.first=2;  b.second=2;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    blockLayout[SHAPE_L] = thisKindBlock; 
   

//SHAPE_RL
    thisKindBlock = new vector< vector<pair<int,int> >* >;
    
    /*
     * [ ][ ][ ]---
     * ------[ ]---
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=0;  layout->push_back(b); 
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    thisKindBlock->push_back( layout );
    
    /*
     * ------[ ]---
     * ------[ ]---
     * ---[ ][ ]---
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    b.first=2;  b.second=1;  layout->push_back(b); 
    b.first=2;  b.second=2;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    /*
     * [ ]---------
     * [ ][ ][ ]
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=0;  layout->push_back(b); 
    b.first=1;  b.second=0;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    /*
     * ---[ ][ ]---
     * ---[ ]------
     * ---[ ]------
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=2;  b.second=1;  layout->push_back(b); 
    thisKindBlock->push_back( layout );
 
    blockLayout[SHAPE_RL] = thisKindBlock; 
   


//SHAPE_T
    thisKindBlock = new vector< vector<pair<int,int> >* >;
    /*
     * [ ][ ][ ]---
     * ---[ ]------
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=0;  layout->push_back(b); 
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    /*
     * ---[ ]------
     * [ ][ ]------
     * ---[ ]------
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=0;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=2;  b.second=1;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    /*
     * ---[ ]------
     * [ ][ ][ ]---
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=0;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    /*
     * ---[ ]------
     * ---[ ][ ]---
     * ---[ ]------
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    b.first=2;  b.second=1;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    blockLayout[SHAPE_T] = thisKindBlock; 
   


//SHAPE_N
    thisKindBlock = new vector< vector<pair<int,int> >* >;

    /*
     * [ ][ ]------
     * ---[ ][ ]---
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=0;  layout->push_back(b); 
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    /*
     * ------[ ]---
     * ---[ ][ ]---
     * ---[ ]------
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    b.first=2;  b.second=1;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    blockLayout[SHAPE_N] = thisKindBlock; 
   


//SHAPE_RN 
    thisKindBlock = new vector< vector<pair<int,int> >* >;
    
    /*
     * ---[ ][ ]---
     * [ ][ ]------
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=1;  b.second=0;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    thisKindBlock->push_back( layout );
 
    /*
     * ---[ ]------
     * ---[ ][ ]---
     * ------[ ]---
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    b.first=2;  b.second=2;  layout->push_back(b); 
    thisKindBlock->push_back( layout );
 
    blockLayout[SHAPE_RN] = thisKindBlock; 
   


//SHAPE_I
    thisKindBlock = new vector< vector<pair<int,int> >* >;
    /*
     * [ ][ ][ ][ ]
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=0;  layout->push_back(b); 
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=0;  b.second=3;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    /*
     * ---[ ]------
     * ---[ ]------
     * ---[ ]------
     * ---[ ]------
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=2;  b.second=1;  layout->push_back(b); 
    b.first=3;  b.second=1;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    blockLayout[SHAPE_I] = thisKindBlock; 
   

//SHAPE_O
    thisKindBlock = new vector< vector<pair<int,int> >* >;
    /*
     * ---[ ][ ]---
     * ---[ ][ ]---
     */
    layout = new vector< pair<int, int> >;    
    b.first=0;  b.second=1;  layout->push_back(b); 
    b.first=0;  b.second=2;  layout->push_back(b); 
    b.first=1;  b.second=1;  layout->push_back(b); 
    b.first=1;  b.second=2;  layout->push_back(b); 
    thisKindBlock->push_back( layout );

    blockLayout[SHAPE_O] = thisKindBlock; 
   
}
