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
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include "huffmanTree.h"
using namespace std;


struct symInfo{
    string line;
    map<int, char> msg;
    int port;
    char * u;
};

void * deCompress(void* myData){
    symInfo* thrData = (symInfo*) myData;
        string line = thrData -> line;
        map<int, char> mS = thrData -> msg;
        int port = thrData->port;
        char * u = thrData->u;

    string code, w; int h;
    istringstream st(line); //process line into binary and indexes
        st >> code;
    // send code to server
        int sockfd = socket(AF_INET, SOCK_STREAM, 0); //TCP 
        if (sockfd < 0){
            cerr << "ERROR opening socket" << endl;
            exit(1);
        }
        struct sockaddr_in serv_addr;
        struct hostent *server;
        int g;
        char buffer[255];
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        int e = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        if(e < 0){
            cerr << "connection error" << endl;
            exit(1);
        }
        server = gethostbyname(u);
        if(server == NULL){
            cerr << "host error" << endl;
            exit(1);
        }
    // send code
        const char *temp = code.c_str();
        int len = strlen(temp);
        g = write(sockfd, temp, len); // temp sends the binary code
        if(g < 0){
            cerr << "Error on writing" << endl;
            exit(1);
        }
    // receive char
        string serverMsg = "";
        g = read(sockfd, buffer, 255); // buffer holds the character from server
        if(g < 0){
            cerr << "Error on reading" << endl;
            exit(1);
        }
        serverMsg = buffer;
        //cout << "char from server: " << serverMsg << endl;
    //indexes
    while(st >> w){ 
        h = stoi(w);
        mS.insert(make_pair(h, serverMsg[0])); //into map
    }
    thrData->msg = mS;
    close(sockfd);
    return NULL;
}

int main(int argc, char* argv[]){
    int portno = atoi(argv[2]); //port
    char * u = argv[1]; //host

    if (argc < 2){
        cerr << "ERROR, no port provided" << endl;
        exit(1);
    }

    // READ COMPRESSED
        map<int, char> msg;
        static const int thrSize = 26; //symbols alphabet - max threads
    //array of threads
        pthread_t tid[thrSize];
        static struct symInfo myData[thrSize];

        int index = 0; string line;
        while(getline(cin, line)){
            myData[index].line = line;
            myData[index].port = portno;
            myData[index].u = u;

            if(pthread_create(&tid[index], NULL, deCompress, (void *)&myData[index])){
                return 1; //error making thread
            }
            index++;
        }
    //wait
        for(int i=0; i<index; i++)
            pthread_join(tid[i], NULL);
    
    //Print
        map<int,char> final;
        cout << "Original message: ";
        for(int j=0; j<index; j++)
            for(auto it = myData[j].msg.begin(); it!=myData[j].msg.end(); it++)
                final.insert(make_pair(it->first, it->second)); //into string
        for(auto it = final.begin(); it!=final.end();it++)
            cout << it->second;
    
    return 0;
};