#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>

using namespace std;

class AssemblerPass2{
    vector< pair<string, int> > SymTab;
    vector< pair<string, int> > LitTab;
    string symtab_file, littab_file, IC_file; 
    
    public:
    void initTables();
    AssemblerPass2(string, string, string);
    void showTables();
    string operandMachineCode(string);
    void pass2();
};

AssemblerPass2 :: AssemblerPass2(string sym_file, string lit_file, string IC_input){
    cout<<"assembler"<<endl;
    symtab_file = sym_file + ".txt";
    littab_file = lit_file + ".txt";
    IC_file = IC_input + ".txt";
}

void AssemblerPass2 :: initTables(){
    cout<<"init tables"<<endl;
    fstream fin_symTab(symtab_file.c_str(), ios::in);
    
    while(!fin_symTab.eof()){
        string line = "";
        getline(fin_symTab, line);
        stringstream ss;    
        
        ss.str(line);
        string sym = "", lc = "";
        ss>>sym>>lc;

        pair<string, int> pr;
        pr.first = sym;
        pr.second = atoi(lc.c_str());

        SymTab.emplace_back(pr);
    }

    fstream fin_litTab(littab_file.c_str(), ios::in);

    while(!fin_litTab.eof()){
        string line = "";
        string lit = "", lc = "";
        getline(fin_litTab, line);
        stringstream ss;
        ss.str(line);
        ss>>lit>>lc;

        pair<string, int> pr;
        pr.first = lit;
        pr.second = atoi(lc.c_str());        

        LitTab.emplace_back(pr);
    }
}

void AssemblerPass2 :: showTables(){
    cout<<"show tables"<<endl;
    for(int i = 0; i < LitTab.size(); i++){
        cout<<LitTab[i].first<<" "<<LitTab[i].second<<endl;
    }
    
    for(int i = 0; i < SymTab.size(); i++){
        cout<<SymTab[i].first<<" "<<SymTab[i].second<<endl;
    }
}

string AssemblerPass2 :: operandMachineCode(string op){
    if(op.length() > 0){
        if(op[1] == 'l'){
            int ind = atoi(op.substr(3, op.length() - 4).c_str());
            cout<<ind - 1<<endl;
            return to_string(LitTab[ind - 1].second);
        }
        else if(op[1] == 's'){
            int ind = atoi(op.substr(3, op.length() - 4).c_str());
            cout<<ind - 1<<endl;
            return to_string(SymTab[ind - 1].second);
        }
        else{
            return op.substr(1, op.length() - 2);
        }
    }
    return "-1";
}

void AssemblerPass2 :: pass2(){
    fstream fin_IC("IC.txt", ios::in);
    fstream fout_machineCode("MACHINECODE.txt", ios::out);
   
    while(!fin_IC.eof()){
        string line = "";
        getline(fin_IC, line);


        string lc = "", opcode = "", op1 = "-", op2 = "-", word = "";
        vector<string> tokenized;
        //tokenize line
        for(auto c : line){
            if(c != ' ') word += c;
            else{
                tokenized.emplace_back(word);
                word = "";
            }
        }
        tokenized.emplace_back(word);

        lc = tokenized[0];
        opcode = tokenized[1];

        for(int i = 2; i < tokenized.size(); i++){
            if(i == 2) op1 = tokenized[2];
            if(i == 3) op2 = tokenized[3];
        }

        cout<<lc<<" "<<opcode<<" "<<op1<<" "<<op2<<endl;
        
        //Generate machine code
        if(lc != "-"){
            //output lc
            lc += ")";
            fout_machineCode<<lc<<" ";
            
            //output opcode depending on class
            if(opcode[1] == 'I'){
                opcode = opcode.substr(4, opcode.length() - 5);
                fout_machineCode<<opcode<<" ";
            }
            
            else if(opcode[4] == '1'){
                fout_machineCode<<"00 "<<"0 "<<op1.substr(3, op1.length() - 4)<<endl;
                continue;
            }
            else{
                fout_machineCode<<endl;
                continue;
            }
            
            //output operand machine codes
            //Both operands absent
            if(op1 == "-" && op2 == "-"){
                fout_machineCode<<"0 000"<<endl;
            }
            //1st operand absent and 2nd is present
            else if(op1 != "-" && op2 == "-"){
                fout_machineCode<<"0 "<<operandMachineCode(op1)<<endl;
            }
            //Both operands present
            else{
                fout_machineCode<<operandMachineCode(op1)<<" "<<operandMachineCode(op2)<<endl;
            }
            
        }
        
        
    }    

}

int main(){
    AssemblerPass2 obj("SYMTAB", "LITTAB", "IC");
    obj.initTables();
    obj.showTables();
    obj.pass2();
    return 0;
}