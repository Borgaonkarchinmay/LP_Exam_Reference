#include<iostream>
#include<algorithm>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<map>

using namespace std;

class macropreprocessor{
    //Pass1 data structures
    vector<vector<string>> mnt;
    vector<vector<string>> kpdt;
    vector<vector<string>> pntab;
    string mdt;
    //Pass2 data structures
public:
    void pass1(string);
    int pType(string);
    void showPass1_Tables();
    int pIndex(string, int);
    int apIndex(string, int);
    int macroIndex(string);
    int kpIndex(string, int, int);
    void storeKP(string, vector<string>&);
    void processPrototype(string);
    void pass2();
};

void macropreprocessor :: processPrototype(string line){
    string word;
    int pos = 0, keywrd = 0;
    vector<string> mnt_row;
    vector<string> pntab_row;

    for(auto c : line){
        if(c != ' ' && c != ','){
            word += c;
        }
        else if(c == ' '){
            //update mnt
            mnt_row.emplace_back(word);
            word = "";
        }
        else{
            //update pn tab
            int i = pType(word);
            if(i == 2) {
                pntab_row.emplace_back(word.substr(1, word.length() - 1));
                pos++;
            }
            else if(i = 0){
                pntab_row.emplace_back(word.substr(1, word.length() - 2));            
                keywrd++;
            }
            else {
                int k = 0;
                while(word[k] != '=')k++;
                pntab_row.emplace_back(word.substr(1, k - 1));
                keywrd++;
            }
            word = "";
        }
    }

    //update pn tab
    int i = pType(word);
    if(i == 2){
        pntab_row.emplace_back(word.substr(1, word.length() - 1));
        pos++;
    }
    else if(i = 0){
        pntab_row.emplace_back(word.substr(1, word.length() - 2));            
        keywrd++;
    }
    else{
        int k = 0;
        while(word[k] != '=')k++;
        pntab_row.emplace_back(word.substr(1, k - 1));
        keywrd++;
    }

    //trace mnt row
    mnt_row.emplace_back(to_string(pos));
    mnt_row.emplace_back(to_string(keywrd));
    
    //update mnt, pntab
    mnt.emplace_back(mnt_row);
    pntab.emplace_back(pntab_row);
}

int macropreprocessor :: pType(string param){
    
    if(param[param.length() - 1] == '='){
        //keyword param
        //update kpdt
        vector<string> kpdt_row;
        kpdt_row.emplace_back(param.substr(1, param.length() - 2));
        kpdt_row.emplace_back("---");
        kpdt.emplace_back(kpdt_row);
        return 0;
    }

    for(int i = 0; i < param.length(); i++){
        if(param[i] == '='){
            //default param
            //update kpdt
            vector<string> kpdt_row;
            kpdt_row.emplace_back(param.substr(1, i-1));
            kpdt_row.emplace_back(param.substr(i + 1, param.length() - i - 1));
            kpdt.emplace_back(kpdt_row);
            return 1;
        }
    }

    //positional param
    return 2;
}

void macropreprocessor :: pass1(string src){
    
    src += ".txt";
    
    fstream fin(src.c_str(), ios::in);
    fstream fout("MDT.txt", ios::out);
    
    int MdtInd = 1;
    int MacroCount = -1;
    
    string line;
    
    
    while(!fin.eof()){
        getline(fin, line);
        if(line == "MACRO"){
            //process prototpye
            MacroCount++;
            getline(fin, line);
            processPrototype(line);
            mnt[MacroCount].emplace_back(to_string(MdtInd));
            if(kpdt.size() != 0){
                int index = kpdt.size() - atoi(mnt[MacroCount][1].c_str()) + 1; 
                mnt[MacroCount].emplace_back(to_string(index));
            }
        }

        else if(line == "MEND"){
            //output in mdt file
            fout<<to_string(MdtInd)<<" "<<"MEND"<<endl;
            //maintain mdtIndex for mdt file
            MdtInd++;
        }

        else{
            //output in mdt file
            string opcode, op1, op2, op;
            int ind = 0;
            
            stringstream ss;
            ss.str(line);
            ss>>opcode>>op1>>op2;
            
            //push to file left to right
            fout<<to_string(MdtInd)<<" ";
            fout<<opcode<<" ";

            if(op1 != ""){
                ind = pIndex(op1.substr(1, op1.length() - 2), MacroCount); //process formal parameter references using pntab
                if(op1[0] == '&'){
                    op = "(P,";
                    op += to_string(ind);
                    op += ")";
                    fout<<op<<" ";
                }
                else{
                    fout<<op1<<" ";
                }
            }

            if(op2 != ""){
                op = "";
                ind = pIndex(op2.substr(1, op2.length() - 1), MacroCount); //process formal parameter references using pntab
                if(op2[0] == '&'){
                    op = "(P,";
                    op += to_string(ind);
                    op += ")";
                    fout<<op<<" ";
                }
                else{
                    fout<<op2<<" ";
                }
            }

            fout<<endl;

            //maintain index to be mentioned for mdt file 
            MdtInd++;
        }
    }
    showPass1_Tables();
}

void macropreprocessor :: showPass1_Tables(){
    fstream fmnt("MNT.txt", ios::out);
    fstream fpntab("PNTAB.txt", ios::out);
    fstream fkpdt("KPDTAB.txt", ios::out);

    //MNT
    for(int mn = 0; mn < mnt.size(); mn++){
        string line = "";
        line += to_string(mn+1);
        for(int item = 0; item < mnt[mn].size(); item++){
            line += " ";
            line += mnt[mn][item];
        }
        fmnt<<line<<endl;
    }

    //PNTAB
    for(int pn = 0; pn < pntab.size(); pn++){
        for(int item = 0; item < pntab[pn].size(); item++){
            fpntab<<to_string(item + 1)<<" "<<pntab[pn][item]<<endl;
        }
    }

    //KPDTAB
    for(int kp = 0; kp < kpdt.size(); kp++){
        string line = "";
        line += to_string(kp+1);
        for(int item = 0; item < kpdt[kp].size(); item++){
            line += " ";
            line += kpdt[kp][item];
        }
        fkpdt<<line<<endl;
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

void macropreprocessor :: pass2(){
    fstream fin("calls.txt", ios :: in);
    fstream fout("expandedCode.txt", ios::out);

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
                aptab[i] = kpdt[i + 1 - pp + kpdtp - 1][1]; //Set default value 
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
    }

}

int main(){
    macropreprocessor obj;
    string src, fcall_name;
    cout<<"enter src name: ";
    cin>>src;
    obj.pass1(src);
    cout<<"Pass1 successfull!!"<<endl;
    obj.pass2();

    return 0;
}