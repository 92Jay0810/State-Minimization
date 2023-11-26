#include <iostream>
#include <vector>
#include <map>
class stateRelation;
class stateMachine
{
public:
    int input_number;
    int output_number;
    int state_number;
    int relation_number;
    char initstate;
    std::vector<stateRelation> state_relation;
    bool readable = true;
    stateMachine(int inputNum, int outputNum, int stateNum, int relationNum, char init_state, std::vector<stateRelation> staterelation) : input_number(inputNum), output_number(outputNum), state_number(stateNum), relation_number(relationNum), initstate(init_state), state_relation(staterelation) {}
    stateMachine(bool readable) : readable(readable) {}
};
class stateRelation
{
public:
    std::string input;
    char present_state;
    char next_state;
    int output;
    stateRelation(std::string input, char presentstate, char nextstate, int output) : input(input), present_state(presentstate), next_state(nextstate), output(output) {}
    stateRelation() {}
};
class simplfystate
{
public:
    std::string input;
    char next_state;
    int output;
    simplfystate() {}
    friend std::ostream &operator<<(std::ostream &os, const simplfystate &state)
    {
        os << "input: " << state.input << ", nextState: " << state.next_state << ", output: " << state.output << std::endl;
        return os;
    }
    bool operator==(const simplfystate &other)
    {
        return (input == other.input) && (output == other.output);
    }
};
class implicantElement
{
public:
    char state_major;
    char state_minor;
    std::map<std::string, simplfystate> majorstate_element;
    std::map<std::string, simplfystate> minorstate_element;
    bool vaild = true;
    bool redutcion_delete = false;
    implicantElement() {}
    friend std::ostream &operator<<(std::ostream &os, const implicantElement &element)
    {
        os << "major state: " << element.state_major << std::endl;
        for (auto &state : element.majorstate_element)
        {
            os << state.second;
        }
        os << "minor state: " << element.state_minor << std::endl;
        for (auto &state : element.minorstate_element)
        {
            os << state.second;
        }
        os << "vaild :" << element.vaild << std::endl;
        return os;
    }
    void testvaild()
    {
        for (auto &major_pair : majorstate_element)
        {
            std::string major_key = major_pair.first;
            simplfystate major_simplfystate = major_pair.second;
            auto minor_simplfystate_iterator = minorstate_element.find(major_key);
            if (minor_simplfystate_iterator != minorstate_element.end())
            {
                simplfystate minor_simplfystate = minor_simplfystate_iterator->second;
                //  check input and output is same or not
                if (!(major_simplfystate == minor_simplfystate))
                {
                    vaild = false;
                    break;
                }
            }
            else
            {
                // if can't find key in  minorstate_element
                vaild = false;
                break;
            }
        }
    }
};
class implicantTable
{
public:
    std::vector<std::vector<implicantElement>> implicantelements;
    int statenumber;
    std::vector<char> statechar;
    std::vector<int> implicantcolumn_size;
    implicantTable() {}
    void show()
    {
        for (int i = 0; i < statenumber - 1; i++)
        {
            for (int j = 0; j < implicantcolumn_size[i]; j++)
            {
                std::cout << implicantelements[i][j];
            }
        }
    }
};