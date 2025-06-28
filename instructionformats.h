#ifndef INSTRUCTIONFORMATS_H
#define INSTRUCTIONFORMATS_H
#include <bits/stdc++.h>
using namespace std;

class InstructionFormats
{
protected:
    // Method to convert register number to a 5-bit binary string
    string regToBinary(const string &reg) const
    {
        string regnumber = reg.substr(1); // Remove leading character (e.g., 'x')
        int num = stoi(regnumber);
        return bitset<5>(num).to_string();
    }
    string numToBinary(const string &num, int size)
    {

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

    pair<string, string> immRegForSAndL(string instructionchild)
    {

        string imm, rs1;
        int i = 0;
        while (i < instructionchild.size())
        {
            char c = instructionchild[i];
            if (c == '(')
            {
                break;
            }
            else if (c == ' ')
            {
                i++;
                continue;
            }
            imm += c;
            i++;
        }
        i++;
        while (i < instructionchild.size())
        {
            char c = instructionchild[i];
            if (c == ')')
                break;
            else if (c == ' ')
            {
                i++;
                continue;
            }
            else
            {
                rs1 += c;
            }
            i++;
        }
        return make_pair(imm, rs1);
    }
};

class RType : public InstructionFormats
{
private:
    string opcode;
    vector<string> instruction;
    struct RTypeFields
    {
        string funct3;
        string funct7;
    };

    map<string, RTypeFields> r_type = {
        {"add", {"000", "0000000"}},
        {"sub", {"000", "0100000"}},
        {"sll", {"001", "0000000"}},
        {"mul", {"010", "0000000"}},
        {"div", {"011", "0000000"}},
        {"xor", {"100", "0000000"}},
        {"srl", {"101", "0000000"}},
        {"or", {"110", "0000000"}},
        {"and", {"111", "0000000"}},
        {"sra", {"101", "0100000"}}};

public:
    RType(const vector<string> &instruction, const string &opcode)
        : instruction(instruction), opcode(opcode) {}

    string instructionToBinary()
    {
        // instruction - func rd,rs1,rs2
        // funct7 rs2 rs1 funct3 rd opcode
        vector<string> result(6);

        string func = instruction[0]; // Operation type

        result[0] = r_type[func].funct7;         // funct7
        result[1] = regToBinary(instruction[3]); // rs2
        result[2] = regToBinary(instruction[2]); // rs1
        result[3] = r_type[func].funct3;         // funct3
        result[4] = regToBinary(instruction[1]); // rd
        result[5] = opcode;                      // opcode

        string resultstr = accumulate(result.begin(), result.end(), string());

        return resultstr;
    }
};

class IType : public InstructionFormats
{
private:
    string opcode;
    vector<string> instruction;

    map<string, string> i_type = {
        {"addi", "000"},
        {"xori", "100"},
        {"ori", "110"},
        {"andi", "111"},
        {"slli", "001"},
        {"srli", "101"},
        {"srai", "101"},
        {"muli", "010"},
        {"divi", "011"},
        {"lb", "000"},
        {"lh", "001"},
        {"lw", "010"},
        {"lbu", "100"},
        {"lhu", "101"},
        {"jalr", "000"}};

public:
    IType(const vector<string> &instruction, const string &opcode)
        : instruction(instruction), opcode(opcode) {}

    string instructionToBinary()
    {
        // instruction - func rd,rs1,imm
        // imm[11:0] rs1 funct3 rd opcode
        if (opcode == "0010011")
        {
            vector<string> result(5);

            string func = instruction[0];                // Operation type
            result[0] = numToBinary(instruction[3], 12); // imm
            result[1] = regToBinary(instruction[2]);     // rs1
            result[2] = i_type[func];                    // funct3
            result[3] = regToBinary(instruction[1]);     // rd
            result[4] = opcode;                          // opcode

            string resultstr = accumulate(result.begin(), result.end(), string());
            return resultstr;
        }
        else
        {
            vector<string> result(6);
            auto res = immRegForSAndL(instruction[2]);
            string imm = res.first, rs1 = res.second;
            string func = instruction[0];            // Operation type
            result[0] = numToBinary(imm, 12);        // imm
            result[1] = regToBinary(rs1);            // rs1
            result[2] = i_type[func];                // funct3
            result[3] = regToBinary(instruction[1]); // rd
            result[4] = opcode;                      // opcode

            string resultstr = accumulate(result.begin(), result.end(), string());
            return resultstr;
        }
    }
};

class SType : public InstructionFormats
{
private:
    string opcode;
    vector<string> instruction;

    map<string, string> s_type = {
        {"sb", "000"},
        {"sh", "001"},
        {"sw", "010"}};

public:
    SType(const vector<string> &instruction, const string &opcode)
        : instruction(instruction), opcode(opcode) {}

    string instructionToBinary()
    {
        // instruction format: func rs2, imm(rs1)
        // Binary format: imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode
        vector<string> result(6);
        auto res = immRegForSAndL(instruction[2]);
        string imm = res.first, rs1 = res.second;
        string func = instruction[0];
        string immBinary = numToBinary(imm, 12); // Convert immediate to binary
        result[0] = immBinary.substr(0, 7);      // imm[11:5]
        result[1] = regToBinary(instruction[1]); // rs2
        result[2] = regToBinary(rs1);            // rs1
        result[3] = s_type[func];                // funct3 (operation type)
        result[4] = immBinary.substr(7, 5);      // imm[4:0] (5 bits)
        result[5] = opcode;                      // opcode

        string resultstr = accumulate(result.begin(), result.end(), string());
        return resultstr;
    }
};

// B-Type Instructions
class SBType : public InstructionFormats
{
private:
    string opcode;
    vector<string> instruction;

    map<string, string> sb_type = {
        {"beq", "000"},
        {"bne", "001"},
        {"blt", "100"},
        {"bge", "101"},
        {"bltu", "110"},
        {"bgeu", "111"}};

public:
    SBType(const vector<string> &instruction, const string &opcode)
        : instruction(instruction), opcode(opcode) {}

    string instructionToBinary()
    {
        // instruction - func rs1, rs2, imm
        // imm[12] | imm[10:5] | rs2 | rs1 | funct3 | imm[4:1] | imm[11] | opcode
        vector<string> result(8);

        string func = instruction[0]; // Operation type
        string imm = numToBinary(instruction[3], 12);
        result[0] = imm[0];                      // imm[12]
        result[1] = imm.substr(2, 6);            // imm[10:5]
        result[2] = regToBinary(instruction[2]); // rs2
        result[3] = regToBinary(instruction[1]); // rs1
        result[4] = sb_type[func];               // funct3
        result[5] = imm.substr(8, 4);            // imm[4:1]
        result[6] = imm[1];                      // imm[11]
        result[7] = opcode;                      // opcode

        string resultstr = accumulate(result.begin(), result.end(), string());
        return resultstr;
    }
};

// J-Type Instructions
class UJType : public InstructionFormats
{
private:
    string opcode;
    vector<string> instruction;

public:
    UJType(const vector<string> &instruction, const string &opcode)
        : instruction(instruction), opcode(opcode) {}

    string instructionToBinary()
    {
        // instruction - func rd, imm
        // imm[20] | imm[10:1] | imm[11] | imm[19:12] | rd | opcode
        vector<string> result(6);

        // immediate value for J-Type
        string imm = numToBinary(instruction[2], 20);
        result[0] = imm[0];                      // imm[20]
        result[1] = imm.substr(10, 10);          // imm[10:1]
        result[2] = imm[9];                      // imm[11]
        result[3] = imm.substr(1, 8);            // imm[19:12]
        result[4] = regToBinary(instruction[1]); // rd
        result[5] = opcode;                      // opcode
        string resultstr = accumulate(result.begin(), result.end(), string());

        return resultstr;
    }
};

// U-Type Instructions
class UType : public InstructionFormats
{
private:
    string opcode;
    vector<string> instruction;

public:
    UType(const vector<string> &instruction, const string &opcode)
        : instruction(instruction), opcode(opcode) {}

    string instructionToBinary()
    {
        // instruction - func rd, imm
        // imm[31:12] | rd | opcode
        vector<string> result(3);

        string imm = numToBinary(instruction[2], 20);
        result[0] = imm.substr(0, 20);           // imm[31:12]
        result[1] = regToBinary(instruction[1]); // rd
        result[2] = opcode;                      // opcode
        string resultstr = accumulate(result.begin(), result.end(), string());

        return resultstr;
    }
};

#endif // INSTRUCTIONFORMATS_H