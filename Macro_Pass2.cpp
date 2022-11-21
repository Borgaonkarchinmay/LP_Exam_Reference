#include<iostream>
#include<algorithm>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<map>

using namespace std;

class macropreprocessor{
    vector<vector<string>> mnt;
    vector<vector<string>> kpdt;
    vector<vector<string>> pntab;
    string file_mnt, file_mdt, file_pntab, file_kpdt, file_calls;
public:
    macropreprocessor(string, string, string, string, string);
    void init_structures();
    int pIndex(string, int);
    int macroIndex(string);
    int kpIndex(string, int, int);
    void showPass1_Tables();
    void pass2();
};

macropreprocessor :: macropreprocessor(string file_mnt, string file_mdt, string file_pntab, string file_kpdt, string file_calls){
    this->file_mnt = file_mnt + ".txt";
    this->file_mdt = file_mdt + ".txt";
    this->file_pntab = file_pntab + ".txt";
    this->file_kpdt = file_kpdt + ".txt";
    this->file_calls = file_calls + ".txt";
}

void macropreprocessor :: init_structures(){
    fstream f_mnt(file_mnt.c_str(), ios::in);
    while(!f_mnt.eof()){ //read mnt and populate mnt table
        string line;
        getline(f_mnt, line);
        
        vector<string> row(5);
        //string name, pp, kp, mdtp, kpdtp;
        stringstream ss;
        ss.str(line);
        for(int i = 0; i < 5; i++){
            ss>>row[i];
        }

        mnt.emplace_back(row);
    }
    

    fstream f_pntab(file_pntab.c_str(), ios::in);
    while(!f_pntab.eof()){ //read mnt and populate mnt table
        string line;
        getline(f_pntab, line);
        
        vector<string> row;
        string word  = "";
        for(auto c : line){
            if(c == ' '){row.emplace_back(word); word = "";}
            else word += c;
        }
        row.emplace_back(word);

        pntab.emplace_back(row);
    }
    
    fstream f_kpdt(file_kpdt.c_str(), ios::in);
    while(!f_kpdt.eof()){ //read mnt and populate mnt table
        string line;
        getline(f_kpdt, line);
        
        vector<string> row(2);
        //string name, pp, kp, mdtp, kpdtp;
        stringstream ss;
        ss.str(line);
        for(int i = 0; i < 2; i++){
            ss>>row[i];
        }

        kpdt.emplace_back(row);
    }
}

int macropreprocessor :: pIndex(string param, int macroNo){
    for(int i = 0 ; i < pntab[macroNo].size(); i++){
        if(pntab[macroNo][i] == param) return i + 1;
    }

    return -1;
}

int macropreprocessor :: macroIndex(string name){
    for(int i = 0; i < mnt.size(); i++){
        if(mnt[i][0] == name) return i;
    }
    return -1;
}

int macropreprocessor :: kpIndex(string kp, int begin, int end){
    begin--;
    end--;
    for(int i = begin; i < end; i++){
        if(kpdt[i][0] == kp) {return i + 1;}
    }
    return -1;
}

void macropreprocessor :: showPass1_Tables(){
    
    //MNT
    for(int mn = 0; mn < mnt.size(); mn++){
        string line = "";
        line += mnt[mn][0];
        for(int item = 1; item < mnt[mn].size(); item++){
            line += " ";
            line += mnt[mn][item];
        }
        cout<<line<<endl;
    }

    //PNTAB
    cout<<pntab.size()<<endl;
    for(int pn = 0; pn < pntab.size(); pn++){
        cout<<pntab[pn][0];
        for(int item = 1; item < pntab[pn].size(); item++){
            cout<<" "<<pntab[pn][item];
        }
        cout<<endl;
    }

    //KPDTAB
    for(int kp = 0; kp < kpdt.size(); kp++){
        string line = "";
        line += kpdt[kp][0];
        for(int item = 1; item < kpdt[kp].size(); item++){
            line += " ";
            line += kpdt[kp][item];
        }
        cout<<line<<endl;
    }

    
}

void macropreprocessor :: pass2(){
    fstream fin("calls.txt", ios :: in);
    fstream fout("expandedCode.txt", ios::out);
    fstream fout_aptab("aptab.txt", ios::out);

    //process a call
    //generate aptab
    while(!fin.eof()){
        string line = "";
        getline(fin, line);

        //process a call at a time
        string temp = "";
        vector<string> call;
        for(auto c : line){
            if(c != ' '){
                temp += c; 
            }
            else{
                call.emplace_back(temp);
                temp = "";
            }
        }
        call.emplace_back(temp);

        for(auto w : call) cout<<w<<" ";
        cout<<endl;

        int mnt_i = macroIndex(call[0]);
        //cout<<call[0]<<endl;

        int kp_i = 0;
        int pp = atoi(mnt[mnt_i][1].c_str());
        int kp = atoi(mnt[mnt_i][2].c_str());
        int mdtp = atoi(mnt[mnt_i][3].c_str());
        int kpdtp = atoi(mnt[mnt_i][4].c_str());

        vector<string> aptab(pp + kp, "-");
        
        //store pp values
        for(int i = 0; i < pp; i++) aptab[i] = call[i + 1];
        
        //store kp values
        for(int i = pp + 1; i < call.size(); i++){
            string kparam = ""; //ketword param
            string temp = "";
            cout<<call[i]<<endl;
            for(auto c : call[i]){
                if(c != '=') temp += c;
                else{
                    kparam = temp;
                    temp = ""; 
                }
            }

            //temp holds the value

            kp_i = kpIndex(kparam, kpdtp, kpdtp + kp) + pp - kpdtp + 1 - 1;
            cout<<kp_i<<endl;
            aptab[kp_i] = temp;
        }

        for(int i = 0; i < aptab.size(); i++){
            if(aptab[i] == "-"){
                cout<<"doubt kpdt index: "<<kpdtp<<endl;
                aptab[i] = kpdt[i + 1 - pp + kpdtp - 1 - 1][1]; //Set default value 
            }
        }
        
        
        //Generate expanded code
        fstream finExpCode("mdt.txt", ios::in);
       
        int mdt_i = atoi(mnt[mnt_i][3].c_str());

        while(!finExpCode.eof()){
            string line = "";
            string ind, opcode, op1, op2;
            getline(finExpCode, line);
            stringstream ss;
            ss.str(line);
            ss>>ind>>opcode>>op1>>op2;

            if(atoi(ind.c_str()) < mdt_i){
                continue;
            }
            
            else{
                if(opcode != "MEND"){
                    string outLine = "";
                    outLine += "+ ";
                    outLine += opcode;

                    outLine += " ";
                    if(op1[0] == '(')
                        outLine += aptab[atoi(op1.substr(3, op1.length() - 4).c_str()) - 1];
                    else
                        outLine += op1;

                    outLine += " ";
                    if(op2[0] == '(')
                        outLine += aptab[atoi(op2.substr(3, op2.length() - 4).c_str()) - 1];
                    else
                        outLine += op2;

                    fout<<outLine<<endl;
                }
                else{
                    break;
                }
            }
        }

        for(int item = 0; item < aptab.size(); item++){
            fout_aptab<<to_string(item + 1)<<" "<<aptab[item]<<endl;
        }
        cout<<endl;

        showPass1_Tables();
    }

}

int main(){
    string mnt, mdt, pntab, kpdt, calls;
    cout<<"MNT MDT PNTAB KPDT CALLS- File name: ";
    cin>>mnt>>mdt>>pntab>>kpdt>>calls;
    macropreprocessor obj(mnt, mdt, pntab, kpdt, calls);
    obj.init_structures();
    obj.pass2();
    cout<<"Pass2 executed successfully!!"<<endl;
    return 0;
}