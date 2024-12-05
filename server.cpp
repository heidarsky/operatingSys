// Write your code here

/*
    NOTE: lecture/class code was used to assist
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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include "huffmanTree.h"
using namespace std;

bool freqSort(const pair<char,int> &a, const pair<char,int> &b){ //by frequency
    return (a.second < b.second);
}

bool asciiSort(const pair<char,int> &a, const pair<char,int> &b){ //by ascii
    return ( (int(a.first) < int(b.first)) && (a.second == b.second) );
}

void fireman(int){
   while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char* argv[]){
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr , clie_addr;
    struct hostent *server;
    int g;
    char buffer[255];
    int req = 0;

    signal(SIGCHLD, fireman);
    if (argc < 2){
        cerr << "ERROR, no port provided" << endl;
        exit(1);
    }

    char a; // char or symbol 
    int n; // freq
    string w; string line; // for strings
    vector<pair<char, int>> myVec; //store input

    // READ INPUT (input redirection)
        while(getline(cin, line) && line.size()!=0){
            istringstream st1(line);
                a = line[0]; //CHAR/SYMBOL
                w = line.substr(1); //FREQ
                n = stoi(w); //MAKE IT INT
            myVec.push_back(make_pair(a, n));
            req++;
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
    

    sockfd = socket(AF_INET, SOCK_STREAM, 0); //TCP 
    if (sockfd < 0){
        cerr << "ERROR opening socket" << endl;
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    int r = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(r < 0){
        cerr << "BIND error"<< endl;
        exit(1);
    }

    listen(sockfd, 50); //
    clilen = sizeof(clie_addr);

    for(int z = 0; z<req; z++){ // requests
        newsockfd = accept(sockfd, (struct sockaddr *)&clie_addr, &clilen);
        if(fork() == 0){ // child process per request
            if (newsockfd < 0){
                cerr << "ERROR on accept"<< endl;
                exit(1);
            }
            //receive binary from client
                string c = "";
                g = read(newsockfd, buffer, 255);
                if (g < 0){
                    cerr << "ERROR reading from socket"<< endl;
                    exit(1);
                }
                for(int t=0; t<strlen(buffer); t++){
                    if(!isdigit(buffer[t]))
                        break;
                    else
                        c += buffer[t];
                }
                cout << endl;
                //cout << "code from client: " << c << endl;
            //use tree to decode
                char res = findChar(tree, c);
            //send char to client
                string hold = ""; hold += res;
                const char* k = hold.c_str();
                int len = strlen(k);
                g = write(newsockfd, k, len); // k holds the character
                if (g < 0){
                    cerr << "ERROR writing to socket"<< endl;
                    exit(1);
                }
            close(newsockfd);
            _exit(0);
        }
    }
    for(int e =0; e<req; e++){
        wait(NULL);
    }
    close(newsockfd);
    close(sockfd);    

    return 0;
};