#include <bits/stdc++.h>

using namespace std;
vector<string> assemblyout;

vector<string> R_Inst = {"ADD", "MUL", "DIV", "AND", "OR", "XOR", "SLL", "SRL", "SUB", "SRA"};
vector<string> I_Arth_Inst = {"ADDI", "MULI", "ANDI", "ORI", "XORI", "SLLI", "SRLI", "SRAI","DIVI"};
vector<string> I_Load_Inst = {"LW", "LD", "LH", "LB", "LWU", "LHU", "LBU"};
vector<string> B_Inst = {"BEQ", "BNE", "BLT", "BGE", "BLTU", "BGEU"};
vector<string> J_Inst = {"JAL", "JALR"};
vector<string> U_Inst = {"LUI", "AUIPC"};
vector<string> S_Inst = {"SB", "SH", "SW", "SD"};

vector<string> splitInstruction(const string &instr)
{
    stringstream ss(instr);
    vector<string> parts;
    string segment;

    while (getline(ss, segment, ' '))
    {
        parts.push_back(segment);
    }

    return parts;
}

vector<int> parseOperands(const string &instr)
{
    int i = 0;
    int n = instr.size();
    vector<int> nums;
    while (i < n)
    {
        while (i < n && (!isdigit(instr[i]) && instr[i] != 'x' && instr[i] != 'X'))
            i++;
        if (i >= n)
            break;
        string temp;
        if (instr[i] == '0' && (instr[i + 1] == 'x' || instr[i + 1] == 'X'))
        {
            while (i < n && (instr[i] != ' ' && instr[i] != ',' && instr[i] != '(' && instr[i] != '[' && instr[i] != ']'))
            {
                temp.push_back(instr[i++]);
            }
            nums.push_back(stoi(temp, nullptr, 16));
        }
        else{
            if(!isdigit(instr[i])) i++;
            int start=i-1;
             while(i<n && (instr[i]!=' ' && instr[i]!=',' && instr[i] != '(' && instr[i]!='[' && instr[i]!=']')){
                temp.push_back(instr[i++]);
            }
            if(instr[start]=='-'){
                nums.push_back(-1*stoi(temp));
            }
            else
            nums.push_back(stoi(temp));
        }
    }
    return nums;
}

class Instruction
{
protected:
    vector<string> binOutput;

public:
    virtual void generateBinary() = 0;
    string displayBinary() const
    {
        string temp;
        for (const auto &bin : binOutput)
        {
            cout << bin << " ";
            temp += bin;
        }
        cout << endl;
        return temp;
    }
};

class RInstruction : public Instruction
{
    string operation;
    string opcode = "0110011";
    map<string, string> func3 = {{"ADD", "000"}, {"SLL", "001"}, {"MUL", "010"}, {"DIV", "011"}, {"XOR", "100"}, {"SRL", "101"}, {"OR", "110"}, {"AND", "111"}, {"SUB", "000"}, {"SRA", "101"}};

    map<string, string> func7 = {{"SUB", "0100000"}, {"SRA", "0100000"}};

    vector<int> operands;

public:
    RInstruction(const string &op, const vector<int> &nums) : operation(op), operands(nums)
    {
        binOutput.resize(6);
    }

    void generateBinary() override
    {
        binOutput[5] = opcode;
        binOutput[3] = func3[operation];
        binOutput[0] = func7.count(operation) ? func7[operation] : "0000000";
        binOutput[1] = bitset<5>(operands[2]).to_string();
        binOutput[2] = bitset<5>(operands[1]).to_string();
        binOutput[4] = bitset<5>(operands[0]).to_string();
    }
};

class IArthInstruction : public Instruction
{
    string operation;
    string opcode;
    map<string, string> func3 = {{"ADDI", "000"}, {"MULI", "010"}, {"ANDI", "111"}, {"ORI", "110"}, {"XORI", "100"}, {"SLLI", "001"}, {"SRLI", "101"}, {"SRAI", "101"},{"DIVI","011"}};

    vector<int> operands;

public:
    IArthInstruction(const string &op, const vector<int> &nums) : operation(op), operands(nums)
    {
        binOutput.resize(5);
        opcode = "0010011";
    }

    void generateBinary() override
    {
        binOutput[4] = opcode;
        binOutput[2] = func3[operation];
        binOutput[1] = bitset<5>(operands[1]).to_string();
        binOutput[3] = bitset<5>(operands[0]).to_string();
        binOutput[0] = bitset<12>(operands[2]).to_string();
    }
};

class ILoadInstruction : public Instruction
{
    string operation;
    string opcode = "0000011";
    map<string, string> func3 = {{"LB", "000"}, {"LH", "001"}, {"LW", "010"}, {"LBU", "100"}, {"LHU", "101"}, {"LWU", "110"}, {"LD", "011"}};

    vector<int> operands;

public:
    ILoadInstruction(const string &op, const vector<int> &nums) : operation(op), operands(nums)
    {
        binOutput.resize(5);
    }

    void generateBinary() override
    {
        binOutput[4] = opcode;
        binOutput[2] = func3[operation];
        binOutput[1] = bitset<5>(operands[1]).to_string();
        binOutput[3] = bitset<5>(operands[0]).to_string();
        binOutput[0] = bitset<12>(operands[2]).to_string();
    }
};

class SInstruction : public Instruction
{
    string operation;
    string opcode = "0100011";
    map<string, string> func3 = {{"SB", "000"}, {"SH", "001"}, {"SW", "010"}, {"SD", "01NE 1"}};

    vector<int> operands;

public:
    SInstruction(const string &op, const vector<int> &nums) : operation(op), operands(nums)
    {
        binOutput.resize(6);
    }
    string numToBinary(int &num1, int size)
    {
        string num=to_string(num1);
        if (num.size() > 2 && (num[1] == 'x' || num[1] == 'X'))
        {
            // hexadecimal
            unsigned int x = stoul(num, nullptr, 16);
            if (size == 12)
                return bitset<12>(x).to_string();
            else
                return bitset<20>(x).to_string();
        }
        else
        {
            // num to binary
            if (size == 12)
                return bitset<12>(stoi(num)).to_string();
            else
                return bitset<20>(stoi(num)).to_string();
        }
    }
    void generateBinary() override
    {
        int imm=operands[1];
        string immbinary=numToBinary(imm,12);
        binOutput[5] = opcode;
        binOutput[4]=immbinary.substr(7,5);
        binOutput[3] = func3[operation];
        binOutput[1] = bitset<5>(operands[0]).to_string();
        binOutput[2] = bitset<5>(operands[2]).to_string();
        binOutput[0] = immbinary.substr(0,7);

        //  result[0] = immBinary.substr(0, 7);      // imm[11:5]
        // result[1] = regToBinary(instruction[1]); // rs2
        // result[2] = regToBinary(rs1);            // rs1
        // result[3] = s_type[func];                // funct3 (operation type)
        // result[4] = immBinary.substr(7, 5);      // imm[4:0] (5 bits)
        // result[5] = opcode;                      // opcode
    }
};

class BInstruction : public Instruction {
    string operation;
    string opcode = "1100011";
    map<string, string> func3 = {{"BEQ", "000"}, {"BNE", "001"}, {"BLT", "100"},
                                 {"BGE", "101"}, {"BLTU", "110"}, {"BGEU", "111"}};

    vector<int> operands;

public:
    BInstruction(const string& op, const vector<int>& nums) : operation(op), operands(nums) {
        binOutput.resize(6);
    }
    string numToBinary(int &num1, int size)
    {
        string num=to_string(num1);
        if (num.size() > 2 && (num[1] == 'x' || num[1] == 'X'))
        {
            // hexadecimal
            unsigned int x = stoul(num, nullptr, 16);
            if (size == 12)
                return bitset<12>(x).to_string();
            else
                return bitset<20>(x).to_string();
        }
        else
        {
            // num to binary
            if (size == 12)
                return bitset<12>(stoi(num)).to_string();
            else
                return bitset<20>(stoi(num)).to_string();
        }
    }
    void generateBinary() override {
        int imm = operands[2];

        // binOutput[0] = bitset<1>(imm >> 11).to_string() + bitset<6>((imm >> 5) & 0x3F).to_string() + bitset<4>(imm & 0xF).to_string() + bitset<1>((imm >> 10) & 0x1).to_string();
        string immBinary = numToBinary(imm, 12); // Convert immediate to binary
        binOutput[0] = immBinary.substr(0, 7);      // imm[11:5]
        binOutput[1] = bitset<5>(operands[1]).to_string(); // rs2
        binOutput[2] = bitset<5>(operands[0]).to_string();           // rs1
        binOutput[3] = func3[operation];                // funct3 (operation type)
        binOutput[4] = immBinary.substr(7, 5);      // imm[4:0] (5 bits)
        binOutput[5] = opcode;                      // opcode
    }
};

class JInstruction : public Instruction
{
    string operation;
    string opcode = "1101111";

    vector<int> operands;

public:
    JInstruction(const string &op, const vector<int> &nums) : operation(op), operands(nums)
    {
        binOutput.resize(3);
    }

    void generateBinary() override
    {

        binOutput[2] = opcode;
        binOutput[1] = bitset<5>(operands[0]).to_string();
        int imm = operands[1];
        binOutput[0] = bitset<1>(imm >> 20).to_string() + bitset<8>((imm >> 12) & 0xFF).to_string() + bitset<1>((imm >> 11) & 0x1).to_string() + bitset<10>((imm >> 1) & 0x3FF).to_string();
    }
};

class UInstruction : public Instruction
{
    string operation;
    string opcode;
    vector<int> operands;

public:
    UInstruction(const string &op, const vector<int> &nums) : operation(op), operands(nums)
    {
        binOutput.resize(2);
        opcode = (op == "LUI") ? "0110111" : "0010111";
    }

    void generateBinary() override
    {
        binOutput[1] = opcode;
        binOutput[0] = bitset<5>(operands[0]).to_string() + bitset<20>(operands[1]).to_string();
    }
};

struct CW
{
    bool RegRead;
    bool AluSrc;
    bool RegWrite;
    bool MemRead;
    bool MemWrite;
    bool Mem2Reg;
    bool Branch;
    bool Jump;
    int AluOp;

    // Default constructor
    CW() : RegRead(false), AluSrc(false), RegWrite(false), MemRead(false), MemWrite(false), Mem2Reg(false), Branch(false), Jump(false), AluOp(0)
    {
    }

    // Parameterized constructor
    CW(bool aluSrc, bool regRead, bool regWrite, int aluOp, bool mem2Reg, bool memRead, bool memWrite, bool branch, bool jump)
        : AluSrc(aluSrc), RegRead(regRead), RegWrite(regWrite), Mem2Reg(mem2Reg), MemRead(memRead), MemWrite(memWrite), Branch(branch), Jump(jump), AluOp(aluOp)
    {
    }
};

struct MOWB
{
    CW cw;
    int LDOUT;
    int ALLOUT;
    int rdl;
    MOWB() : rdl(-1)
    {
    }
};

struct EXMO
{
    CW cw;
    int rs2;
    int ALLOUT;
    int rdl;
    EXMO() : rdl(-1)
    {
    }
};

struct IDEX
{
    CW cw;
    int JPC;
    int DPC;
    int imm;
    int rs1;
    int rs2;
    int rsl1;
    int rsl2;
    int rdl;
    int func;
    IDEX() : rdl(-1)
    {
    }
};

struct IFID
{
    string IR;
    int DPC;
    IFID() : IR("") {}
};

class CPU
{
public:
    vector<int> GPR;
    vector<int> memory;
    vector<string> instructionSet;
    int NPC, PC, wait;
    EXMO exmo;
    IDEX idex;
    IFID ifid;
    MOWB mowb;
    MOWB prevmowb;
    vector<int> state;
    unordered_set<int> usingReg;

    CPU(vector<string> instructionSet)
    {
        GPR.resize(32, 0);
        memory.resize(20, 0);
        state.resize(5, 0);
        this->instructionSet = instructionSet;
        PC = 0;
        NPC = 0;
        wait = 0;
    }

private:
    int extractBits(int number, int start, int end)
    {
        int mask = ((1 << (start - end + 1)) - 1) << end;
        return (number & mask) >> end;
    }

    int binaryStrToInt(string &binaryStr)
    {
        return stoi(binaryStr, nullptr, 2);
    }

    string numToBinary(const string &num, int size)
    {
        if (size == 4)
            return bitset<12>(stoi(num)).to_string();
        else
            return bitset<20>(stoi(num)).to_string();
    }

    int signExtend(int imm, int bitLength)
    {
        int mask = 1 << (bitLength - 1);
        return (imm ^ mask) - mask;
    }

    int immGen(string I, string format)
    {
        string imm;
        int bitLength = 0;

        if (format == "I" || format == "L")
        {
            imm = I.substr(0, 12); // 12-bit immediate
            bitLength = 12;
        }
        else if (format == "S")
        {
            imm = I.substr(0, 7) + I.substr(20, 5); // S-type 12-bit immediate
            bitLength = 12;
        }
        else if (format == "B")
        {
            imm =  I.substr(0, 7) + I.substr(20, 5); // B-type 12-bit immediate
            bitLength = 12;
        }
        else if (format == "U")
        {
            imm = I.substr(0, 20); // U-type 20-bit immediate
            return binaryStrToInt(imm) << 12;
        }
        else if (format == "J")
        {
            imm = string(1, I[0]) + I.substr(12, 8) + string(1, I[11]) + I.substr(1, 10); // J-type 21-bit immediate
            bitLength = 21;
        }

        int immValue = binaryStrToInt(imm);
        return signExtend(immValue, bitLength);
    }

    pair<CW, string> controller(string opcode)
    {
        bool aluSrc = 0, regRead = 1, regWrite = 0;
        int aluOP = 0;
        bool mem2Reg = 0, memRead = 0, memWrite = 0, branch = 0, jump = 0;
        string format;
        if (opcode == "0110011") // R-type
        {
            regWrite = 1;
            aluOP = 2;
            format = "R";
            // return {CW(false, true, true, 2, false, false, false, false, false), "R"};
        }
        else if (opcode == "0010011") // I-Type
        {
            aluSrc = 1;
            regWrite = 1;
            aluOP = 2;
            format = "I";
            // return {CW(true, true, true, 2, false, false, false, false, false), "I"};
        }
        else if (opcode == "0000011") // L-type
        {
            aluSrc = 1;
            regWrite = 1;
            mem2Reg = 1;
            memRead = 1;
            format = "L";
            // return {CW(true, true, true, 0, true, true, false, false, false), "L"};
        }
        else if (opcode == "0100011") // S-type
        {
            aluSrc = 1;
            mem2Reg = 1;
            memWrite = 1;
            format = "S";
            // return {CW(true, true, false, 0, true, false, true, false, false), "S"};
        }
        else if (opcode == "1100011") // B-type
        {
            aluOP = 1;
            branch = 1;
            format = "B";
            // return {CW(false, true, false, 1, false, false, false, true, false), "B"};
        }
        else
        {
            cout << "error in CW" << endl;
        }
        return {CW(aluSrc, regRead, regWrite, aluOP, mem2Reg, memRead, memWrite, branch, jump), format};
    }

    int forwordingUnit(int rsl, int rs)
    {
        int res = 0, input;
        cout << " exmo.rdl=" << exmo.rdl << " rsl=" << rsl << endl;
        if (exmo.rdl == rsl)
        {
            if (exmo.cw.MemRead)
            {
                res = 1;
            }
            else if (!exmo.cw.MemRead && !exmo.cw.MemWrite && exmo.cw.RegWrite)
            {
                res = 2;
            }
        }
        else if (prevmowb.rdl == rsl && prevmowb.cw.RegWrite)
        {
            res = 3;
        }
        cout << " res=" << res << endl;
        switch (res)
        {
        case 0:
            input = rs;
            break;
        case 1:
            input = mowb.LDOUT; // memory to register
            break;
        case 2:
            input = mowb.ALLOUT;
            break;
        case 3:
            input = GPR[rsl];
            break;
        default:
            cout << "error in forwarding unit" << endl;
            break;
        }
        return input;
    }

    int ALUControl(int ALUop, int func)
    {
        // changed slt -->mul, sltu-->div
        map<string, int> Aluaction = {{"and", 0b0000}, {"or", 0b0001}, {"add", 0b0010}, {"sub", 0b0110}, {"xor", 0b0011}, {"sll", 0b0100}, {"srl", 0b0101}, {"sra", 0b0111}, {"mul", 0b1000}, {"div", 0b1001}};
        map<int, string> funcToOp = {{0b000, "add"}, {0b1000, "sub"}, {0b001, "sll"}, {0b010, "mul"}, {0b011, "div"}, {0b100, "xor"}, {0b101, "srl"}, {0b110, "or"}, {0b111, "and"}, {0b1101, "sra"}};
        switch (ALUop)
        {
        case 0: // load, store
            return Aluaction["add"];
            break;
        case 1: // branch
            return Aluaction["sub"];
            break;
        case 2: // r-type ,I-type
            return Aluaction[funcToOp[func]];
            break;
        default:
            cout << "Aluop error" << endl;
            break;
        }
        return 0;
    }

    int ALU(int Aluselect, int rs1, int rs2)
    {
        switch (Aluselect)
        {
        case 0:
            return rs1 & rs2;
            break;
        case 1:
            return rs1 | rs2;
            break;
        case 2:
            return rs1 + rs2;
            break;
        case 3:
            return rs1 ^ rs2;
            break;
        case 4:
            return rs1 << rs2;
            break;
        case 5:
            return rs1 >> rs2;
            break;
        case 6:
            return rs1 - rs2;
            break;
        case 8:
            return rs1 * rs2;
            break;
        case 9:
            return rs1 / rs2;
            break;
        default:
            cout << "alu error" << endl;
            break;
        }
        return 0;
    }

    void fetch()
    {
        cout << "Fetch:" << endl;
        if (state[0] == 0 || PC >= instructionSet.size())
        {
            state[1] = 0;
            return;
        }
        else
        {
            string instruction = instructionSet[PC];
            ifid.IR = instruction;
            state[1] = 1;
        }
        ifid.DPC = PC;
        NPC = PC + 1;
        cout << " IR = " << ifid.IR << endl;
        cout << " DPC = " << ifid.DPC << endl;
        cout << endl;
        return;
    }

    void decode()
    {
        cout << "Decode:" << endl;
        if (state[1] == 0)
        {
            state[2] = 0;
            return;
        }
        state[2] = 1;
        idex.DPC = ifid.DPC;
        string opcode = ifid.IR.substr(25, 7);
        string format;
        // control unit
        auto res = controller(opcode);
        idex.cw = res.first;
        format = res.second;
        cout << " Opcode = " << opcode << endl;
        cout << " Format = " << format << endl;

        // imm gen
        if (format != "R")
            idex.imm = immGen(ifid.IR, format);

        // func
        string funct7, funct3;
        if (format == "R")
        {
            funct7 = ifid.IR.substr(0, 7);
            funct3 = ifid.IR.substr(17, 3);
            string res = string(1, funct7[1]) + funct3;
            idex.func = binaryStrToInt(res);
        }
        else
        {
            funct3 = ifid.IR.substr(17, 3);
            idex.func = binaryStrToInt(funct3);
        }

        if (idex.cw.RegRead)
        {
            string strReg = ifid.IR.substr(12, 5);
            idex.rsl1 = binaryStrToInt(strReg);
            idex.rs1 = GPR[idex.rsl1];
        }
        cout << " rsl1 = " << idex.rsl1 << endl;
        cout << " rs1 = " << idex.rs1 << endl;

        string strrs2 = ifid.IR.substr(7, 5);
        idex.rsl2 = binaryStrToInt(strrs2);
        idex.rs2 = GPR[idex.rsl2];
        cout << " rsl2 = " << idex.rsl2 << endl;
        cout << " rs2 = " << idex.rs2 << endl;

        string rdl = ifid.IR.substr(20, 5);
        idex.rdl = binaryStrToInt(rdl);
        cout << " rdl = " << idex.rdl << endl;

        // Debug output for Stage 2
        cout << " imm = " << idex.imm << endl;
        cout << " func = " << idex.func << endl;
        return;
    }

    void execute()
    {
        cout << "Execute:" << endl;
        if (state[2] == 0)
        {
            state[3] = 0;
            PC = NPC;
            return;
        }
        state[3] = 1;
        int input1, input2;
        if (idex.cw.RegRead)
        {
            input1 = forwordingUnit(idex.rsl1, idex.rs1);
        }

        if (idex.cw.AluSrc)
        {
            input2 = idex.imm;
        }
        else
        {
            input2 = forwordingUnit(idex.rsl2, idex.rs2);
        }

        if (idex.cw.MemWrite)
            exmo.rs2 = forwordingUnit(idex.rsl2, idex.rs2);

        int Aluselect = ALUControl(idex.cw.AluOp, idex.func);

        // ALU
        exmo.ALLOUT = ALU(Aluselect, input1, input2);
        exmo.cw = idex.cw;
        exmo.rdl = idex.rdl;

        // branch type
        bool branchTaken = false;
        if (idex.cw.Branch)
        {
            int value = exmo.ALLOUT;
            switch (idex.func)
            {
            case 0b000: // BEQ (Branch if Equal)
                branchTaken = value == 0 ? 1 : 0;
                break;
            case 0b001: // BNQ (Branch if Not Equal)
                branchTaken = value != 0 ? 1 : 0;
                break;
            case 0b101: // BGE (Branch if Greater or Equal)
                branchTaken = value >= 0 ? 1 : 0;
                break;
            case 0b100: // BLT (Branch if Less Than)
                branchTaken = value < 0 ? 1 : 0;
                break;

            default:
                cout << "Unknown branch function code: " << idex.func << endl;
                break;
            }

            if (branchTaken)
            {
                state[1] = 0;
                state[0] = 0;
                wait = 1;
                NPC = idex.imm + idex.DPC;
            }
            else
            {
                PC = NPC;
            }
        }
        else
        {
            PC = NPC;
        }

        if (idex.cw.Jump)
            PC = idex.JPC;

        // Debug output for Stage 3
        cout << " input1 = " << input1 << endl;
        cout << " input2 = " << input2 << endl;
        cout << " ALLOUT = " << exmo.ALLOUT << endl;
        cout << " PC = " << PC << endl;
        return;
    }

    void memoryOperation()
    {
        cout << "Memory Operation:" << endl;
        prevmowb = mowb;
        if (state[3] == 0)
        {
            state[4] = 0;
            return;
        }
        state[4] = 1;
        if (exmo.cw.MemRead)
        {
            mowb.LDOUT = memory[exmo.ALLOUT];
        }
        if (exmo.cw.MemWrite)
        {
            memory[exmo.ALLOUT] = exmo.rs2;
            cout << " rs2:" << exmo.rs2 << endl;
        }
        mowb.ALLOUT = exmo.ALLOUT;
        mowb.cw = exmo.cw;
        mowb.rdl = exmo.rdl;
        // Debug output for Stage 4

        cout << " LDOUT = " << mowb.LDOUT << endl;
        cout << " ALLOUT = " << mowb.ALLOUT << endl;
        return;
    }

    void writeBack()
    {
        cout << "Write Back:" << endl;
        if (state[4] == 0)
            return;
        if (mowb.cw.RegWrite)
            if (mowb.cw.Mem2Reg)
                GPR[mowb.rdl] = mowb.LDOUT;
            else
                GPR[mowb.rdl] = mowb.ALLOUT;

        cout << " GPR[" << mowb.rdl << "] = " << GPR[mowb.rdl] << endl;
        return;
    }

public:
    int processing()
    {
        int clocks = 0;
        state[0] = 1; // state=1 (valid) state=0 (invalid)
        while (state[0] == 1 || state[1] == 1 || state[2] == 1 || state[3] == 1 || state[4] == 1)
        {
            state[0] = 1;
            writeBack();
            memoryOperation();
            execute();
            decode();
            fetch();
            state[0] = 0;
            clocks++;
        }
        // Print final GPR state
        cout << "Final GPR State: ";
        for (int i = 0; i < GPR.size(); i++)
        {
            cout << GPR[i] << " ";
        }
        cout << endl;
        cout << "Final Memory Values: ";
        for (int i = 0; i < memory.size(); i++)
        {
            cout << memory[i] << " ";
        }
        cout << endl;
        cout << "clocks=" << clocks << endl;
        return 0;
    }
};

int main()
{
    int a;
    cout << "Enter number of Instructions: ";
    cin >> a;
    cin.ignore();

    vector<string> input(a);
    for (int i = 0; i < a; i++)
    {
        getline(cin, input[i]);
    }
    for (int i = 0; i < a; i++)
    {
        vector<string> tokens = splitInstruction(input[i]);
        string opcode = tokens[0];
        vector<int> operands = parseOperands(input[i]);
        string temp;

        if (find(R_Inst.begin(), R_Inst.end(), opcode) != R_Inst.end())
        {
            RInstruction rInst(opcode, operands);
            rInst.generateBinary();
            temp = rInst.displayBinary();
            assemblyout.push_back(temp);
        }
        else if (find(I_Arth_Inst.begin(), I_Arth_Inst.end(), opcode) != I_Arth_Inst.end())
        {
            IArthInstruction iArthInst(opcode, operands);
            iArthInst.generateBinary();
            temp = iArthInst.displayBinary();
            assemblyout.push_back(temp);
        }
        else if (find(I_Load_Inst.begin(), I_Load_Inst.end(), opcode) != I_Load_Inst.end())
        {
            ILoadInstruction iLoadInst(opcode, operands);
            iLoadInst.generateBinary();
            temp = iLoadInst.displayBinary();
            assemblyout.push_back(temp);
        }
        else if (find(S_Inst.begin(), S_Inst.end(), opcode) != S_Inst.end())
        {
            SInstruction sInst(opcode, operands);
            sInst.generateBinary();
            temp = sInst.displayBinary();
            assemblyout.push_back(temp);
        }
        else if (find(B_Inst.begin(), B_Inst.end(), opcode) != B_Inst.end())
        {
            BInstruction bInst(opcode, operands);
            bInst.generateBinary();
            temp = bInst.displayBinary();
            assemblyout.push_back(temp);
        }
        else if (find(J_Inst.begin(), J_Inst.end(), opcode) != J_Inst.end())
        {
            JInstruction jInst(opcode, operands);
            jInst.generateBinary();
            temp = jInst.displayBinary();
            assemblyout.push_back(temp);
        }
        else if (find(U_Inst.begin(), U_Inst.end(), opcode) != U_Inst.end())
        {
            UInstruction uInst(opcode, operands);
            uInst.generateBinary();
            temp = uInst.displayBinary();
            assemblyout.push_back(temp);
        }
        else
        {
            cerr << "Error: Unsupported instruction." << endl;
        }
    }
    CPU process(assemblyout);
    process.processing();

    return 0;
}
