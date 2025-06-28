#include <bits/stdc++.h>
#include "addressing.h"
#include "cpu.cpp"
using namespace std;

class parsing
{
    vector<string> instructionSet;
    vector<vector<string>> result;

public:
    parsing(string input)
    {
        string temp;
        for (char c : input)
        {
            if (c == '\n')
            {
                instructionSet.push_back(temp);
                temp = "";
            }
            else
            {
                temp += c;
            }
        }
        if (!temp.empty())
            instructionSet.push_back(temp);
    }
    vector<vector<string>> parse()
    {
        for (auto instruction : instructionSet)
        {
            string temp;
            vector<string> subresult;
            for (char c : instruction)
            {

                if ((c == ' ' || c == ',') && !temp.empty())
                {
                    subresult.push_back(temp);
                    temp = "";
                }
                else if (c != ' ' && c != ',')
                {
                    temp = temp + (char)tolower(c);
                }
            }
            if (!temp.empty())
            {
                subresult.push_back(temp);
            }
            result.push_back(subresult);
        }
        return result;
    }
};
int main()
{
    int count = 1;
    vector<string> question = {"Question: is prime or not (bool value stored in a0)",
                               "Question: sum of n natural numbers(answer will be stored in a2)"};
    vector<string> assemblecode = {"\nsw x1,1(x0)\n addi x6,x0,1\n addi x2,x0,2\n divi x3,x1,2\n blt,x3,x2,8\n div x4,x1,x2\n mul x5,x4,x2\n bne,x1,x5,3\n addi x6,x6,-1\n beq x0,x0,3 \n addi x2,x2,1\n beq x0,x0,-7\n sw x6,0(x0)",
                                   "\n sw x1,1(x0) \n addi x2,x0,1 \n addi x3,x0,0\n blt,x1,x2,4\n add x3,x3,x2\n addi x2,x2,1\n beq,x0,x0,-3 \n sw x3,2(x0)"};
    while (count--)
    {
        string input;
        int n;
        cout << question[count] << endl;
        cout << "Give input for n: ";
        cin >> n;
        stringstream ss;
        ss << "addi x1,x0," << n << assemblecode[count];
        input = ss.str();
        input="ADDI x1,x0,5\nADDI x2,x0,0\nADDI x3,x0,1\nADD x2,x2,x3\nSUB x4,x3,x1\nADDI x3,x3,1\nBEQ x4,x0,2\nBEQ x0,x0,-4";
        parsing output(input);
        vector<vector<string>> instructionSet = output.parse();

        Tagging address(instructionSet);
        vector<string> result = address.instructionSetToBinarySet();
        for(auto it:result){
            cout<<it<<endl;
        }
        CPU process(result);
        process.processing();
    }
}
// sum of 5 integers
// input = "addi x1,x0,6\n sw x1,1(x0) \n addi x2,x0,1 \n addi x3,x0,0\n blt,x1,x2,4\n add x3,x3,x2\n addi x2,x2,1\n beq,x0,x0,-3 \n sw x3,2(x0)";

//

// "addi x1,x0,5\n addi x2,x0,7 \n addi x3,x0,1\n BEQ x1,x2,3\n add x1,x1,x3 \n BEQ x2,x2,-2\n sw x2,1(x0) \n lw x5,1(x0)";
// "00000000010100000000000010010011",
//     "00000000011100000000000100010011",
//     "00000000000100000000000110010011",
//     "00000000000100010000001101100011",
//     "00000000001100001000000010110011",
//     "11111110001000010000111011100011",
//     "00000000001000000010000010100011",
//     "00000000000100000010001010000011"
// "addi x1,x0,5\n addi x2,x0,7 \n mux x3,x1,x2 \n mod x4,x1,x2 \n muxi x5,x2,10 \n modi x6,x1,10";
//  "00000000010100000000000010010011","00000000011100000000000100010011","00000000001000001010000110110011","00000000001000001011001000110011",
//"00000000101000010010001010010011", "00000000101000001011001100010011"
