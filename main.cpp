// Write your program here

/*
note: Professor code & AI were used to assist.
*/

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <utility>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include "huffmanTree.h"
using namespace std;

struct symInfo{
    string code;
    string line;
    vector<char> *msg;
    minHeap* tree; //to search for char later
    int id;
    pthread_mutex_t *mut;
    pthread_cond_t *waitTurn;
    int *tur;
    pthread_mutex_t *mut2;
};

bool freqSort(const pair<char,int> &a, const pair<char,int> &b){ //by frequency
    return (a.second < b.second);
}

bool asciiSort(const pair<char,int> &a, const pair<char,int> &b){ //by ascii
    return ( (int(a.first) < int(b.first)) && (a.second == b.second) );
}

void * deCompress(void* myData){
    symInfo* thrData = (symInfo*) myData; //arg values
        string code = thrData->code;
        string line = thrData -> line;
        vector<char> *mS = thrData -> msg;
        minHeap* tree = thrData -> tree;
        int id = thrData->id;
        //int tur = *thrData->tur;
        pthread_mutex_t * mut = thrData->mut;
        pthread_mutex_t * mut2 = thrData->mut2;
        pthread_cond_t * waitTurn = thrData->waitTurn;
    pthread_mutex_unlock(mut);

    char c = findChar(tree, code); //letter
    int fr = findFre(tree, code); //frequency
    string w; int h;
    istringstream st(line); //process line indexes/positions
    while(st >> w){ 
        h = stoi(w);
        (*mS)[h] = c; //into map
    }
    
    pthread_mutex_lock(mut2); //wait for proper turn
    while(*thrData->tur != id){
        pthread_cond_wait(waitTurn, mut2);
    }
    pthread_mutex_unlock(mut2);
     
    cout << "Symbol: " << c << ", Frequency: " << fr << ", Code: " << code << endl;
    
    pthread_mutex_lock(mut2); //next
        *thrData->tur =  *thrData->tur + 1;
        pthread_cond_broadcast(waitTurn);
    pthread_mutex_unlock(mut2);
    
    
    return NULL;
}

int main(){
    char a; // char or symbol 
    int n; // freq
    string w; string line; // for strings
    vector<pair<char, int>> myVec; //store input
    int al; //num of alpha
    cin >> al;
    cin.ignore(256, '\n');
    static const int alp = al;
    int vecSize = 0;

// READ INPUT (input redirection)
    for(int i=0; i<alp; i++){ 
        getline(cin, line);
        istringstream st1(line);
            a = line[0]; //CHAR/SYMBOL
            w = line.substr(1); //FREQ
            n = stoi(w); //MAKE IT INT
            vecSize += n;
        myVec.push_back(make_pair(a, n));
    }
    
// SORT INPUT BY FREQ AND ASCII
    sort(myVec.begin(), myVec.end(), freqSort);
    sort(myVec.begin(), myVec.end(), asciiSort);

// Split vector then Huffman
    vector<char> vecC; //symbols
    vector<int> vecI; //freq
    for(int i=0; i<myVec.size(); i++){
        vecC.push_back(myVec[i].first);
        vecI.push_back(myVec[i].second);
    }
    minHeap* tree = huffMan(vecC, vecI, myVec.size()); //store tree 
    

// PROCESS
    vector<char> msg(vecSize);
    static struct symInfo myData;
    pthread_t tid[alp]; //array of threads

    
    //code and positions
    vector<string> veCode;
    vector<string> vePos;
    for(int i=0; i<alp; i++){
        getline(cin, line);
        string code;
            istringstream st(line); //line into binary
            st >> code;
        string positions = line.substr(code.size()+1);
        veCode.push_back(code);
        vePos.push_back(positions);
    }
    
    static pthread_mutex_t mute;
        pthread_mutex_init(&mute, NULL);
    static pthread_cond_t waitT = PTHREAD_COND_INITIALIZER;
    int turn = 0;

    static pthread_mutex_t mut2;
        pthread_mutex_init(&mut2, NULL);

// THREADS
    for(int i=0; i<alp; i++){ 
        pthread_mutex_lock(&mute);

        myData.code = veCode[i]; //send code to thread
        myData.line = vePos[i]; //send positions to thread
        myData.tree = tree; //send tree pointer to thread
        myData.id = i;
        myData.mut = &mute;
        myData.mut2 = &mut2;
        myData.waitTurn = &waitT;
        myData.tur = &turn;
        myData.msg = &msg;
        
        if(pthread_create(&tid[i], NULL, deCompress, (void *)&myData)){
            return 1; //error making thread
        }
    }

//Wait
    for(int i=0; i<alp; i++)
        pthread_join(tid[i], NULL);

//Print
    cout << "Original message: ";
    for(int i = 0; i < msg.size(); i++)
        cout << msg[i];

    return 0;
};