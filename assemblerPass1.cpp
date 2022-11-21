#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>

using namespace std;

class AssemblerPass1{
    //data structures
    map<string, int> registerInd;
    map<string, int> conditionInd;

    struct Mnemonic{
        string mnemonic; 
        string Class; 
        int opcode; 
    }optab[18];

    //output datastructure tables
    vector<pair<string, int>> symTab;
    vector<pair<string, int>> litTab;


    public:
    void init_datastructures();
    void updateSymTab(string, int);
    bool isOpcode(string);
    bool isSubToken(string);
    int getSymIndex(string);
    int processSubtoken(string);
    string opcodeIC(string);
    string operandIC(string);
    void pass1(string);
    void generateFiles();
};

void AssemblerPass1 :: updateSymTab(string lab, int lc){
    //Updates
    for(int i = 0; i < symTab.size(); i++){
        if(symTab[i].first == lab){
            symTab[i].second = lc;
            return;
        }
    }
    //Insertes such a symbol if not present
    pair<string, int> pr ={lab, lc};
    symTab.emplace_back(pr);
    return;
}

bool AssemblerPass1 :: isOpcode(string str){
    for(int i = 0; i < 18; i++) if(optab[i].mnemonic == str) return true;
    return false;
}

bool AssemblerPass1 :: isSubToken(string str){
    for(auto c : str) if(c == '+' || c == '-') return true;
    return false;
}

void AssemblerPass1 :: init_datastructures(){
    registerInd.emplace("areg", 1);
    registerInd.emplace("breg", 2);
    registerInd.emplace("creg", 3);
    registerInd.emplace("dreg", 4);

    conditionInd.emplace("lt", 1);
    conditionInd.emplace("le", 2);
    conditionInd.emplace("eq", 3);
    conditionInd.emplace("gt", 4);
    conditionInd.emplace("ge", 5);
    conditionInd.emplace("any", 6);

    optab[0] = {"stop", "IS", 0};
    optab[1] = {"add", "IS", 1};
    optab[2] = {"sub", "IS", 2};
    optab[3] = {"mult", "IS", 3};
    optab[4] = {"mover", "IS", 4};
    optab[5] = {"movem", "IS", 5};
    optab[6] = {"comp", "IS", 6};
    optab[7] = {"bc", "IS", 7};
    optab[8] = {"div", "IS", 8};
    optab[9] = {"read", "IS", 9};
    optab[10] = {"print", "IS", 10};
    optab[11] = {"start", "AD", 1};
    optab[12] = {"end", "AD", 2};
    optab[13] = {"origin", "AD", 3};
    optab[14] = {"equ", "AD", 4};
    optab[15] = {"ltorg", "AD", 5};
    optab[16] = {"dc", "DL", 1};
    optab[17] = {"ds", "DL", 2};
}

void AssemblerPass1 :: pass1(string fname){
    //cout<<"hello"<<endl;
    fname += ".txt";
    init_datastructures();
    fstream fin(fname.c_str(), ios::in);
    fstream fout("IC.txt", ios::out);
    fstream fout_poolTab("POOLTAB.txt", ios::out);
    string line;
    getline(fin, line);
    bool end = false;

    int LC = 0, Lit_Ind = 0;

    while(!end){
        //Required strings
        string label = "$", opcode = "", op1 = "-", op2 = "-";
        string outputLine = "";
        
        //tokenize
        vector<string> tokenized;
        string item = "";
        for(auto c : line){
            if(c != ' ') item += c;
            else{tokenized.emplace_back(item); item = "";}
        }
        tokenized.emplace_back(item);

        if(isOpcode(tokenized[0])){
            //No label exists
            //qlabel = "$";
            opcode = tokenized[0];
            for(int i = 1; i < tokenized.size(); i++){
                if(i == 1) op1 = tokenized[1];
                if(i == 2) op2 = tokenized[2];
            }
        }
        else{
            label = tokenized[0];
            opcode = tokenized[1];
            for(int i = 2; i < tokenized.size(); i++){
                if(i == 2) op1 = tokenized[2];
                if(i == 3) op2 = tokenized[3];
            }
        }


        //LC processing

        if(label != "$" && opcode != "equ"){
            //label exists
            updateSymTab(label, LC);
        }

        if(opcode == "start"){
            LC = atoi(op1.c_str());
            fout<<"- "; 
        }

        else if(opcode == "end"){
            end = true;
            fout<<"- ";
        }

        else if(opcode == "equ"){
            //update LC for label to value computed in op1
            updateSymTab(label, processSubtoken(op1));
        }

        else if(opcode == "origin"){
            fout<<"- ";
            LC = processSubtoken(op1);
        }

        else if(opcode == "ltorg"){
            fout_poolTab<<("#" + to_string(Lit_Ind + 1))<<endl; //output in pooltab file
            for(; Lit_Ind < litTab.size(); Lit_Ind++){
                litTab[Lit_Ind].second = LC;
                fout<<LC<<" "<<"(DL, 1) "<<"(c, "<<litTab[Lit_Ind].first.substr(2, litTab[Lit_Ind].first.length() - 3)<<")"<<endl;
                LC++;
            }
        }

        else if(opcode == "dc"){
            fout<<LC<<" ";
            LC++;
        }

        else if(opcode == "ds"){
            fout<<LC<<" ";
            LC = LC + atoi(op1.c_str());
        }

        else{
            //IS class operands
            fout<<LC<<" ";
            LC++;
        }

        //File output processing
        if(opcode != "equ" && opcode != "ltorg") {
            opcode = opcodeIC(opcode);
            if(op1 != "-") {op1 = operandIC(op1);} else{op1 = "";}
            if(op2 != "-") {op2 = operandIC(op2);} else{op2 = "";}
            fout<<opcode<<" "<<op1<<" "<<op2<<endl;
        }

        getline(fin, line);
    }

    //Last pool literals updated
    fout_poolTab<<("#" + to_string(Lit_Ind + 1))<<endl; //output in pooltab file
    for(; Lit_Ind < litTab.size(); Lit_Ind++){
        litTab[Lit_Ind].second = LC;
        fout<<LC<<" "<<"(DL, 1) "<<"(c, "<<litTab[Lit_Ind].first.substr(2, litTab[Lit_Ind].first.length() - 3)<<")"<<endl;
        LC++;
    }

    generateFiles();
}

string AssemblerPass1 :: opcodeIC(string opc){
    int i = 0;
    for(; i < 18; i++){
        if(optab[i].mnemonic == opc)break;
    }

    string result = "(";
    result += optab[i].Class;
    result += ", ";
    result += to_string(optab[i].opcode);
    result += ")";

    return result;
}

string AssemblerPass1 :: operandIC(string op){
    string result = "-1";

    if(op == "areg" || op == "breg" || op == "creg" || op == "dreg"){
        //operand is a register
        result = "(";
        result += to_string(registerInd.find(op)->second);
        result += ")";
    }

    else if(op == "lt" || op == "le" || op == "eq" || op == "gt" || op == "ge" || op == "any"){
        //operand is a register
        result = "(";
        result += to_string(conditionInd.find(op)->second);
        result += ")";
    }

    else if(op[0] == '='){
        pair<string, int> pr = {op, 0};
        litTab.emplace_back(pr);
        result = "(l, ";
        result += to_string(litTab.size());
        result += ")";
    }

    else if(op[0] == '\''){
        result = "(c, ";
        result += op.substr(1, op.length() - 2);
        result += ")";
    }

    else if(isSubToken(op)){
        string op1, op2, opr; // A+2 or A+B

        for(auto c : op){
            if(c == '+' || c == '-') {op1 = op2; opr = c; op2="";}
            else{op2 += c;}
        }

        cout<<op1<<" "<<op2<<" "<<opr<<endl;

        result = "(s, ";
        result += to_string(getSymIndex(op1));
        result += ")";
        result += opr;

        if(atoi(op2.c_str())){
            //op2 is number
            result += op2;
        }
        else{
            //op2 is variable
            result += "(s, ";
            result += to_string(getSymIndex(op2));
            result += ")";
        }

        return result;
    }

     //numeric constants
    else if(atoi(op.c_str())){
        result = "(c,";
        result +=  op;
        result +=  ")";
    }

    else{
        int index = getSymIndex(op);
        result = "(s, ";
        result += to_string(index);
        result += ")";
    }

    return result;
}

int AssemblerPass1 :: getSymIndex(string sym){
    for(int i = 0; i < symTab.size(); i++){
        if(symTab[i].first == sym){
            return i + 1;
        }
    }
    //Insertes such a symbol if not present
    pair<string, int> pr ={sym, 0};
    symTab.emplace_back(pr);
    return symTab.size();
}

int AssemblerPass1 :: processSubtoken(string str){
    char opr = '!';
    string op1, op2;

    for(auto c : str){
        if(c == '+' || c == '-'){op1 = op2; op2 = ""; opr = c;}
        else {op2 += c;}
    }
    cout<<op1<<" "<<opr<<" "<<op2<<endl;

    if(opr == '!'){
        return symTab[getSymIndex(str) - 1].second;
    }
    else if(opr == '+'){
        return symTab[getSymIndex(op1) - 1].second + atoi(op2.c_str());
    }
    else{
        return symTab[getSymIndex(op1) - 1].second - atoi(op2.c_str());
    }
}

void AssemblerPass1 :: generateFiles(){
    fstream fout_SymTab("SYMTAB.txt", ios::out);
    fstream fout_LitTab("LITTAB.txt", ios::out);

    for(int i = 0; i < symTab.size(); i++){
        fout_SymTab<<symTab[i].first<<" "<<symTab[i].second<<endl;
    }

    for(int i = 0; i < litTab.size(); i++){
        fout_LitTab<<litTab[i].first<<" "<<litTab[i].second<<endl;
    }

}

int main(){
    AssemblerPass1 obj;
    obj.pass1("testcaseFinal");
    
    return 0;
}
