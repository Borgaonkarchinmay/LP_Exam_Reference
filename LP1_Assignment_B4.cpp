#include<iostream>
#include<algorithm>
#include<queue>
#include<vector>
#include<map>

using namespace std;

class Main_Memory{
    int pageFrames;
    int pageFault;
    int* pframe;
public:
    Main_Memory();
    Main_Memory(int);
    int  get_pageFault(){return pageFault;}
    void LRU_pgRep(int*, int);
    void FIFO_pgRep(int*, int);
    void Optimal_pgRep(int*, int);
    void get_pframe();
    void alloc_frame_space();
    ~Main_Memory();
};

Main_Memory :: Main_Memory(){
    pageFrames = 0;
    pageFault = 0;
    pframe = nullptr;
}

Main_Memory :: Main_Memory(int pgFrames){
    pageFrames = pgFrames;
    pageFault = 0;
    int* pframe = nullptr;
    //for(int i = 0; i < pageFrames; i++){ pframe[i] = -1;}
}

Main_Memory :: ~Main_Memory(){
    delete []pframe;
}

void Main_Memory :: LRU_pgRep(int* inpStr, int len){

    auto cmp = [](pair<int, int> pr1, pair<int, int> pr2){
        return pr1.second < pr2.second;
    };

    alloc_frame_space();

    
    vector<pair<int, int>> pageHistory;
    int time_elapsed = 1;
    pageFault = 0;
    

    for(int i = 0; i < len; i++){ //traverse input string
        bool replace = true;
        //check for vacancy or page hit
        for(int k = 0; k < pageFrames; k++){
            if(pframe[k] == inpStr[i] || pframe[k] == -1){
                if(pframe[k] == -1){
                    pframe[k] = inpStr[i];
                    pair<int, int> pr = {inpStr[i], time_elapsed};
                    pageHistory.emplace_back(pr);
                    sort(pageHistory.begin(), pageHistory.end(), cmp); //sort according to least recently used (ascending order of use time of frame)
                    pageFault++;
                }
                else{//Page Hit
                    for(auto &pr : pageHistory){
                        if(pr.first == inpStr[i]){
                            pr.second = time_elapsed; //update latest use time
                            sort(pageHistory.begin(), pageHistory.end(), cmp);
                            break;
                        }
                    }
                }
                replace = false;
                break;
            }
        }

        if(replace){
            int replace_page_string = pageHistory[0].first;
            pageHistory.erase(pageHistory.begin());

            for(int k = 0; k < pageFrames; k++){
                if(pframe[k] == replace_page_string){
                    pframe[k] = inpStr[i];
                    pair<int, int> pr = {inpStr[i], time_elapsed};
                    pageHistory.emplace_back(pr);
                    sort(pageHistory.begin(), pageHistory.end(), cmp);
                    pageFault++;
                    break;
                }
            }
        }

        time_elapsed++;
    }
}

void Main_Memory :: alloc_frame_space(){
    pframe = new int[pageFrames];
    for(int i = 0; i < pageFrames; i++){ pframe[i] = -1;}
}

void Main_Memory :: FIFO_pgRep(int* inpStr, int len){
    queue<int> pgQueue;
    alloc_frame_space();
    pageFault = 0;

    for(int i = 0; i < len; i++){
        bool present = false;
        
        for(int j = 0; j < pageFrames; j++){
            //cout<<pframe[j]<<" ";
            if((pframe[j] == inpStr[i])  || (pframe[j] == -1)) {
                if(pframe[j] == -1){
                    pframe[j] = inpStr[i];
                    pgQueue.push(inpStr[i]);
                    pageFault++;
                }
                present = true;
                break;
            }
        }
        
        if(!present){
            int pgNo = pgQueue.front();
            for(int j = 0; j < pageFrames; j++){
                if(pframe[j] == pgNo){ pframe[j] = inpStr[i]; pgQueue.push(inpStr[i]); pageFault++; break;}
            }
            pgQueue.pop();  
        }
    }
}

void Main_Memory :: get_pframe(){
    for(int i = 0; i < pageFrames; i++){
        cout<<pframe[i]<<" ";
    }
    cout<<endl;
}

void Main_Memory :: Optimal_pgRep(int* inpStr, int len){
    alloc_frame_space();
    pageFault = 0;

    for(int k = 0; k < len; k++){
        //replace flag
        bool replace = true;

        //Check physical memory space is vacant or page is available in page frame
        for(int i = 0; i < pageFrames; i++){
            if(pframe[i] == inpStr[k] || pframe[i] == -1){
                if(pframe[i] == -1){
                    pframe[i] = inpStr[k];
                    pageFault++;
                }
                replace = false;
                break;
            }
        }

        //If replacement required
        if(replace){
            int replace_ind = 0;
            int future_use_count = INT32_MAX;

            for(int i = 0; i < pageFrames; i++){
                int temp_count = 0;
                for(int j = k + 1; j < len; j++){ //check use of each page frame field in future(i.e. from k+1 till end)
                    if(pframe[i] == inpStr[j]) temp_count++;
                }
                if(temp_count <= future_use_count) {replace_ind = i; future_use_count = temp_count;}
            }
            pframe[replace_ind] = inpStr[k];
            pageFault++;
        } 
    }
}

int main(){
    int size = 0;
    int pageFrames = 0;
    cout<<"Enter no of pages in sequence: ";
    cin>>size;
    cout<<"Enter no of page frames in sequence: ";
    cin>>pageFrames;
    int arr[size];
    cout<<"Enter page reference string: ";
    for(int i = 0; i < size; i++) cin>>arr[i];

    Main_Memory p1(pageFrames);
    
    p1.LRU_pgRep(arr, size);
    cout<<"Page fault by LRU algorithm: "<<p1.get_pageFault()<<endl;

    p1.FIFO_pgRep(arr, size);
    cout<<"Page fault by FIFO algorithm: "<<p1.get_pageFault()<<endl;


    p1.Optimal_pgRep(arr, size);
    cout<<"Page fault by Optimal algorithm: "<<p1.get_pageFault()<<endl;
    
    return 0;
}


