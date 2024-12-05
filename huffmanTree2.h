// Write the definition of the huffmanTree class here.
#include<bits/stdc++.h>
using namespace std;

struct minHeap{
    char symbol;
    int freq;
    int prio;
    minHeap *left;
    minHeap *right;
    minHeap(char symbol, int freq, int prio){
        left = right = nullptr;
        this->symbol = symbol;
        this->freq = freq;
        this->prio = prio;
    }
};

struct compare{
    bool operator()(minHeap *left, minHeap *right){
        if(left->freq == right->freq){
            if(left->symbol == right->symbol){
                return left->prio < right->prio;
            }
            return (left->symbol > right->symbol);
        }
        return (left->freq > right->freq);
    }
};

void printTree(struct minHeap* root, string st){
    if(! root) return;

    if(root->symbol != '\0') //not internal
        cout << "Symbol: " << root->symbol << ", Frequency: " << root->freq << ", Code: " << st << endl;

    printTree(root->left, st + "0"); //left as 0
    printTree(root->right, st + "1"); //right as 1
}

minHeap* huffMan(vector<char> symb, vector<int> frequen, int size){
    struct minHeap *left;
    struct minHeap *right;
    struct minHeap *top;

    priority_queue<minHeap *, vector<minHeap *>, compare> minHeapPrio;

    //leaf node
    for(int i=0; i<size; i++)
        minHeapPrio.push(new minHeap(symb[i], frequen[i], i));
    
    while(minHeapPrio.size() != 1){
        left = minHeapPrio.top();
            minHeapPrio.pop();
        right = minHeapPrio.top();
            minHeapPrio.pop();
        
        //sum of two nodes for huffman
        struct minHeap* temp = new minHeap('\0', (left->freq + right->freq), (left->prio + right->prio));
            temp->left = left;
            temp->right = right;
        minHeapPrio.push(temp);
    }

    printTree(minHeapPrio.top(), "");
    return minHeapPrio.top();
}

char findChar(minHeap* root, string code) {
    minHeap* curr = root;
    for (int i=0; i<code.size(); i++) {
        if (code[i] == '0') {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }
    return curr->symbol;
}

