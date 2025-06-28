#include <bits/stdc++.h>
using namespace std;

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
            imm = string(1, I[0]) + string(1, I[24]) + I.substr(1, 6) + I.substr(20, 4); // B-type 12-bit immediate
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
        return;
    }

    void decode()
    {
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

        string strrs2 = ifid.IR.substr(7, 5);
        idex.rsl2 = binaryStrToInt(strrs2);
        idex.rs2 = GPR[idex.rsl2];

        string rdl = ifid.IR.substr(20, 5);
        idex.rdl = binaryStrToInt(rdl);

        return;
    }

    void execute()
    {
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

        return;
    }

    void memoryOperation()
    {
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
        }
        mowb.ALLOUT = exmo.ALLOUT;
        mowb.cw = exmo.cw;
        mowb.rdl = exmo.rdl;
        return;
    }

    void writeBack()
    {
        if (state[4] == 0)
            return;
        if (mowb.cw.RegWrite)
            if (mowb.cw.Mem2Reg)
                GPR[mowb.rdl] = mowb.LDOUT;
            else
                GPR[mowb.rdl] = mowb.ALLOUT;
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
        return 0;
    }
};
