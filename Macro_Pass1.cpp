#include<iostream>
#include<fstream>
#include<vector>
#include<sstream>

using namespace std;

class macroPass1{
    vector<vector<string>> mnt;
    vector<vector<string>> kpdt;
    vector<vector<string>> pntab;
    public:
    void processPrototype(string, int);
    int locateEqual(string);
    void generateFiles();
    void pass1(string);
    int getIndex(string, int);

};

void macroPass1 :: processPrototype(string line, int m_ind){
    vector<string> tokenized;
    string item = "";

    for(auto c : line){
        if(c == ' ') {tokenized.emplace_back(item); item="";}
        else {item += c;}
    }
    tokenized.emplace_back(item);

    vector<string> mnt_row(5);
    vector<string> kpdt_row(2);
    vector<string> pntab_row;

    int pp = 0, kp = 0, kpdtp = kpdt.size() + 1;
    
    for(int i = 1; i < tokenized.size(); i++){
        int ind = locateEqual(tokenized[i]);

        //Positional
        if(ind == -1){
            pp++;
            pntab_row.emplace_back(tokenized[i].substr(1));
        }

        //Keyword
        else if(ind == tokenized[i].length() - 1){
            kp++;
            pntab_row.emplace_back(tokenized[i].substr(1, tokenized[i].length() - 2));
            kpdt_row[0] = tokenized[i].substr(1, tokenized[i].length() - 2);
            kpdt_row[1] = "---";
            kpdt.emplace_back(kpdt_row);
        }

        //Default
        else{
            kp++;
            pntab_row.emplace_back(tokenized[i].substr(1, ind - 1));
            kpdt_row[0] = tokenized[i].substr(1, ind - 1);
            kpdt_row[1] = tokenized[i].substr(ind + 1, tokenized[i].length() - ind - 1);
            kpdt.emplace_back(kpdt_row);
        }
    }
    
    //Mdt row generated
    mnt_row[0] = tokenized[0];
    mnt_row[1] = to_string(pp);
    mnt_row[2] = to_string(kp);
    mnt_row[3] = to_string(m_ind);
    mnt_row[4] = to_string(kpdtp);
    mnt.emplace_back(mnt_row);
    
    //Append pntab row
    pntab.emplace_back(pntab_row);
}

int macroPass1 ::  locateEqual(string parameter){
    for(int i = 0; i < parameter.length(); i++){
        if(parameter[i] == '=') return i;
    }
    return -1;
}

void macroPass1 :: generateFiles(){
    fstream f_mnt("MNT.txt", ios::out);
    for(auto row : mnt){
        string line = row[0];
        for(int i = 1; i < row.size(); i++){
            line += " " + row[i];
        }
        f_mnt<<line<<endl;    
    }

    fstream f_kpdt("KPDTAB.txt", ios::out);
    for(auto row : kpdt){
        string line = row[0];
        for(int i = 1; i < row.size(); i++){
            line += " " + row[i];
        }
        f_kpdt<<line<<endl;    
    }

    fstream f_pntab("PNTAB.txt", ios::out);
    for(auto row : pntab){
        string line = row[0];
        for(int i = 1; i < row.size(); i++){
            line += " " + row[i];
        }
        f_pntab<<line<<endl;    
    }
}

void macroPass1 :: pass1(string inpFile){
    inpFile += ".txt";

    fstream f_input(inpFile.c_str(), ios::in);
    fstream mdt_output("MDT.txt", ios::out);

    int mdt_ind = 1;
    int curr_macro_ind = -1;
    int count = 0;
    while(!f_input.eof()){
        count++;
        string line;
        getline(f_input, line);

        if(line == "MACRO"){
            getline(f_input, line);
            cout<<line<<endl;
            curr_macro_ind++;
            processPrototype(line, mdt_ind);
        }

        else if(line == "MEND"){
            mdt_output<<to_string(mdt_ind)<<" "<<line<<endl;
            mdt_ind++;
        }
        
        else{//macro body
            string opcode, op1, op2;
            stringstream ss;
            ss.str(line);
            ss>>opcode>>op1>>op2;
            
            int op1_ind = getIndex(op1.substr(1), curr_macro_ind);
            op1 = "(p," + to_string(op1_ind + 1) + ")";

            if(op2[0] == '&'){
                int op2_ind = getIndex(op2.substr(1), curr_macro_ind);
                op2 = "(p," + to_string(op2_ind + 1) + ")";
            }
 
            mdt_output<<to_string(mdt_ind)<<" "<<opcode<<" "<<op1<<" "<<op2<<endl;
            
            mdt_ind++;
        }
    }

    generateFiles();
    cout<<count<<endl;
}

int macroPass1 :: getIndex(string param, int macro_ind){
    for(int i = 0; i < pntab[macro_ind].size(); i++)
        if(pntab[macro_ind][i] == param) return i;
    return -1;
}

int main(){
    macroPass1 obj;
    obj.pass1("macro_tc");
    /*obj.processPrototype("M1 &X &Y &B= &A=AREG", 1);
    obj.processPrototype("M2 &P &Q &U=CREG &V=DREG", 7);
    obj.generateFiles();*/
    
    return 0;
}