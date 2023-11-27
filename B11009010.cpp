#include <iostream>
#include "stateclass.h"
#include <vector>
#include <set>
#include <algorithm>
#include <map>
stateMachine readFile(std::string &filename);
implicantTable Build_implicationTable(stateMachine state_machine);
implicantElement find_stateRelation(std::string state1, std::string state2, std::vector<stateRelation> state_relations);
implicantTable testvaild(implicantTable implicationtable);
implicantTable reduction_implicantTable(implicantTable implicationtable);
implicantTable merge_state(implicantTable implicationtable);
std::vector<implicantElement> rearrange_implicantTable(implicantTable implicationtable);
void outputkissfile(std::string outputfile_name, std::vector<implicantElement> new_implicantelements, stateMachine state_machine, std::vector<std::string> statechar);
void outputdotfile(std::string outputfile_name, std::vector<implicantElement> new_implicantelements, stateMachine state_machine, std::vector<std::string> statechar);
int main(int argc, char *filename[])
{
    if (argc < 4)
    {
        std::cerr << "please enter input_kissfile_name output_kissfile_name dotfile_name";
        return 1;
    }
    std::string input_kiss(filename[1]);
    std::string output_kiss(filename[2]);
    std::string dotfile_name(filename[3]);
    std::cout
        << "readfile: " << input_kiss << std::endl;
    stateMachine state_machine = readFile(input_kiss);
    if (!state_machine.readable)
    {
        return 0;
    }
    implicantTable implicationtable = Build_implicationTable(state_machine);
    // implicationtable.show();
    std::cout << "testvaild" << std::endl;
    implicationtable = testvaild(implicationtable);
    implicationtable.show();
    std::cout << "reduction" << std::endl;
    implicationtable = reduction_implicantTable(implicationtable);
    implicationtable.show();
    std::cout << "merge state" << std::endl;
    implicationtable = merge_state(implicationtable);
    std::vector<implicantElement> new_implicantelements = rearrange_implicantTable(implicationtable);
    for (int i = 0; i < new_implicantelements.size(); i++)
    {
        std::cout << new_implicantelements[i];
    }
    outputkissfile(output_kiss, new_implicantelements, state_machine, implicationtable.statechar);
    outputdotfile(dotfile_name, new_implicantelements, state_machine, implicationtable.statechar);
    std::cout << "no error test" << std::endl;
    return 0;
}
implicantTable Build_implicationTable(stateMachine state_machine)
{
    std::vector<stateRelation> state_relations = state_machine.state_relation;
    // create all state symbol
    std::vector<std::string> statechar;
    // using set to fast find member
    std::set<std::string> statecharset;
    // add init state, keep it in first element
    std::string initstate = state_machine.initstate;
    statecharset.insert(initstate);
    statechar.push_back(initstate);
    for (int i = 0; i < state_relations.size(); i++)
    {
        //  if can't find state, add state into set
        if (statecharset.find(state_relations[i].present_state) == statecharset.end())
        {
            statecharset.insert(state_relations[i].present_state);
            statechar.push_back(state_relations[i].present_state);
        }
    }
    statecharset.clear();
    // create implicantTable
    implicantTable implicanttable;
    std::vector<std::vector<implicantElement>> implicantelements;
    std::vector<int> implicantrow_size;
    std::cout << "make state" << std::endl;
    for (int i = 0; i < statechar.size() - 1; i++)
    {
        std::vector<implicantElement> implicantTable_row;
        int tempimplicantrow_size = 0;
        for (int j = i + 1; j < statechar.size(); j++)
        {
            tempimplicantrow_size++;
            implicantElement implicantelement = find_stateRelation(statechar[i], statechar[j], state_relations);
            implicantTable_row.push_back(implicantelement);
        }
        implicantrow_size.push_back(tempimplicantrow_size);
        implicantelements.push_back(implicantTable_row);
    }
    implicanttable.implicantelements = implicantelements;
    implicanttable.implicantcolumn_size = implicantrow_size;
    implicanttable.statechar = statechar;
    implicanttable.statenumber = state_machine.state_number;
    return implicanttable;
}
implicantElement find_stateRelation(std::string state1, std::string state2, std::vector<stateRelation> state_relations)
{
    implicantElement implicantelement;
    implicantelement.state_major = state1;
    implicantelement.state_minor = state2;
    std::map<std::string, simplfystate> majorstate_element;
    std::map<std::string, simplfystate> minorstate_element;
    for (int i = 0; i < state_relations.size(); i++)
    {
        simplfystate tempsimplfystate;
        tempsimplfystate.input = state_relations[i].input;
        tempsimplfystate.output = state_relations[i].output;
        tempsimplfystate.next_state = state_relations[i].next_state;
        if (state_relations[i].present_state == state1)
        {
            majorstate_element.insert(std::make_pair(state_relations[i].input, tempsimplfystate));
        }
        if (state_relations[i].present_state == state2)
        {
            minorstate_element.insert(std::make_pair(state_relations[i].input, tempsimplfystate));
        }
    }
    implicantelement.majorstate_element = majorstate_element;
    implicantelement.minorstate_element = minorstate_element;
    return implicantelement;
}
implicantTable testvaild(implicantTable implicationtable)
{
    for (int i = 0; i < implicationtable.statenumber - 1; i++)
    {
        for (int j = 0; j < implicationtable.implicantcolumn_size[i]; j++)
        {
            implicationtable.implicantelements[i][j].testvaild();
        }
    }
    return implicationtable;
}
implicantTable reduction_implicantTable(implicantTable implicationtable)
{
    bool modify = true;
    while (modify)
    {
        modify = false;
        for (int i = 0; i < implicationtable.statenumber - 1; i++)
        {
            for (int j = 0; j < implicationtable.implicantcolumn_size[i]; j++)
            {
                if (implicationtable.implicantelements[i][j].vaild == false)
                {
                    continue;
                }
                for (auto &majorstate_mapiterator : implicationtable.implicantelements[i][j].majorstate_element)
                {
                    // to find nextstate location first find state1's input and nextstate
                    std::string state1_input = majorstate_mapiterator.first;
                    std::string state1_nextstate = majorstate_mapiterator.second.next_state;
                    // use state1's input map state2's input find state2's nextstate
                    auto state2_simplfystate_iterator = implicationtable.implicantelements[i][j].minorstate_element.find(state1_input);
                    std::string state2_nextstate = (state2_simplfystate_iterator->second).next_state;
                    auto iterator1 = std::find(implicationtable.statechar.begin(), implicationtable.statechar.end(), state1_nextstate);
                    int state1index = std::distance(implicationtable.statechar.begin(), iterator1);
                    auto iterator2 = std::find(implicationtable.statechar.begin(), implicationtable.statechar.end(), state2_nextstate);
                    int state2index = std::distance(implicationtable.statechar.begin(), iterator2);
                    if (state1index < state2index)
                    {
                        if (implicationtable.implicantelements[state1index][state2index - 1 - state1index].vaild == false)
                        {
                            implicationtable.implicantelements[i][j].vaild = false;
                            modify = true;
                        }
                    }
                    if (state1index > state2index)
                    {
                        if (implicationtable.implicantelements[state2index][state1index - 1 - state2index].vaild == false)
                        {
                            implicationtable.implicantelements[i][j].vaild = false;
                            modify = true;
                        }
                    }
                }
            }
        }
    }
    return implicationtable;
}
implicantTable merge_state(implicantTable implicationtable)
{
    std::vector<implicantElement> merge_state;
    for (int i = 0; i < implicationtable.statenumber - 1; i++)
    {
        for (int j = 0; j < implicationtable.implicantcolumn_size[i]; j++)
        {
            if (implicationtable.implicantelements[i][j].vaild == 1)
            {
                merge_state.push_back(implicationtable.implicantelements[i][j]);
            }
        }
    }
    std::cout << "delete state: " << std::endl;
    for (int i = 0; i < merge_state.size(); i++)
    {
        std::cout << merge_state[i].state_minor << std::endl;
    }
    // i, j find the implicant table location
    for (int i = 0; i < implicationtable.statenumber - 1; i++)
    {
        for (int j = 0; j < implicationtable.implicantcolumn_size[i]; j++)
        {
            // m is to check the delete state
            for (int m = 0; m < merge_state.size(); m++)
            {
                // this is  to mark the delete implicant_element
                if (implicationtable.implicantelements[i][j].state_major == merge_state[m].state_minor)
                {
                    implicationtable.implicantelements[i][j].redutcion_delete = true;
                }
                if (implicationtable.implicantelements[i][j].state_minor == merge_state[m].state_minor)
                {
                    implicationtable.implicantelements[i][j].redutcion_delete = true;
                }
                //  want to check transaction merge the old state to new state
                for (auto &majorstate_mapiterator : implicationtable.implicantelements[i][j].majorstate_element)
                {
                    if (majorstate_mapiterator.second.next_state == merge_state[m].state_minor)
                    {
                        majorstate_mapiterator.second.next_state = merge_state[m].state_major;
                    }
                }
                for (auto &minorstate_mapiterator : implicationtable.implicantelements[i][j].minorstate_element)
                {
                    if (minorstate_mapiterator.second.next_state == merge_state[m].state_minor)
                    {
                        minorstate_mapiterator.second.next_state = merge_state[m].state_major;
                    }
                }
            }
        }
    }
    // delete state  implement
    for (int i = 0; i < merge_state.size(); i++)
    {
        auto iterator = std::find(implicationtable.statechar.begin(), implicationtable.statechar.end(), merge_state[i].state_minor);
        if (iterator != implicationtable.statechar.end())
        {
            int state1index = std::distance(implicationtable.statechar.begin(), iterator);
            implicationtable.statechar.erase(implicationtable.statechar.begin() + state1index);
        }
    }
    return implicationtable;
}
std::vector<implicantElement> rearrange_implicantTable(implicantTable implicationtable)
{
    std::vector<implicantElement> implicantelements;
    for (int i = 0; i < implicationtable.statenumber - 1; i++)
    {
        for (int j = 0; j < implicationtable.implicantcolumn_size[i]; j++)
        {
            // std::cout << implicationtable.implicantelements[i][j].redutcion_delete << std::endl;
            if (!(implicationtable.implicantelements[i][j].redutcion_delete))
            {
                implicantelements.push_back(implicationtable.implicantelements[i][j]);
            }
        }
    }
    return implicantelements;
}