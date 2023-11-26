#include <fstream>
#include <iostream>
#include "stateclass.h"
#include <set>
#include <algorithm>
stateMachine readFile(std::string &filename)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string line;
        int input_number;
        int output_number;
        int state_number;
        int relation_number;
        std::string initstate;
        std::vector<stateRelation> state_relation;
        while (std::getline(file, line))
        {
            std::cout << line << std::endl;
            if (line.substr(0, 2) == ".i")
            {
                input_number = std::stoi(line.substr(line.find(" ") + 1, line.length()));
            }
            if (line.substr(0, 2) == ".o")
            {
                output_number = std::stoi(line.substr(line.find(" ") + 1, line.length()));
            }
            if (line.substr(0, 2) == ".p")
            {
                relation_number = std::stoi(line.substr(line.find(" ") + 1, line.length()));
            }
            if (line.substr(0, 2) == ".s")
            {
                if (line != ".start_kiss")
                {
                    state_number = std::stoi(line.substr(line.find(" ") + 1, line.length()));
                }
            }
            if (line.substr(0, 2) == ".r")
            {
                initstate = line.substr(line.find(" ") + 1, line.length()); // space in right
                while (std::getline(file, line))
                {
                    if (line == ".end_kiss")
                    {
                        break;
                    }
                    if (line != ".end_kiss")
                    {
                        std::cout << line << std::endl;
                        std::vector<std::string> split_space;
                        while (true)
                        {
                            if (line.substr(0, line.find(" ")) != "\0")
                            {
                                split_space.push_back(line.substr(0, line.find(" "))); // space in left
                            }
                            line = line.substr(line.find(" ") + 1, line.length()); // space in right
                            if (line.find(" ") == -1)                              // final no string push left in vector
                            {
                                split_space.push_back(line);
                                break;
                            }
                        }
                        stateRelation temp_state_relation(split_space[0], split_space[1], split_space[2], std::stoi(split_space[3]));
                        state_relation.push_back(temp_state_relation);
                    }
                }
            }
        }
        file.close();
        stateMachine statemachine(input_number, output_number, state_number, relation_number, initstate, state_relation);
        return statemachine;
    }
    else
    {
        std::vector<stateRelation> staterelation;
        stateMachine statemachine(false);
        std::cerr << "Unable to open file: " << filename << std::endl;
        return statemachine;
    }
}
void outputkissfile(std::string outputfile_name, std::vector<implicantElement> new_implicantelements, stateMachine state_machine, std::vector<std::string> statechar)
{
    std::ofstream outputFile(outputfile_name);
    if (outputFile.is_open())
    {
        outputFile << ".start_kiss" << std::endl;
        outputFile << ".i " << state_machine.input_number << std::endl;
        outputFile << ".o " << state_machine.output_number << std::endl;
        outputFile << ".p " << statechar.size() * state_machine.input_number * 2 << std::endl;
        outputFile << ".s " << statechar.size() << std::endl;
        outputFile << ".r " << state_machine.initstate << std::endl;
        std::set<std::string> include_state;
        for (int i = 0; i < new_implicantelements.size(); i++)
        {
            if (include_state.find(new_implicantelements[i].state_major) == include_state.end())
            {
                for (auto &majorstate_mapiterator : new_implicantelements[i].majorstate_element)
                {
                    outputFile << majorstate_mapiterator.second.input << " " << new_implicantelements[i].state_major << " ";
                    outputFile << majorstate_mapiterator.second.next_state << " " << majorstate_mapiterator.second.output << std::endl;
                }
                include_state.insert(new_implicantelements[i].state_major);
            }
            if (include_state.find(new_implicantelements[i].state_minor) == include_state.end())
            {
                for (auto &minorstate_mapiterator : new_implicantelements[i].minorstate_element)
                {
                    outputFile << minorstate_mapiterator.second.input << " " << new_implicantelements[i].state_minor << " ";
                    outputFile << minorstate_mapiterator.second.next_state << " " << minorstate_mapiterator.second.output << std::endl;
                }
                include_state.insert(new_implicantelements[i].state_minor);
            }
        }
        outputFile << ".end_kiss" << std::endl;
        // close file
        outputFile.close();
        std::cout << "kiss file is written " << std::endl;
    }
    else
    {
        std::cout << "can't open file" << std::endl;
    }
}
void outputdotfile(std::string outputfile_name, std::vector<implicantElement> new_implicantelements, stateMachine state_machine, std::vector<std::string> statechar)
{
    std::ofstream outputFile(outputfile_name);
    if (outputFile.is_open())
    {
        outputFile << "digraph STG {" << std::endl;
        outputFile << "\t rankdir=LR;" << std::endl;
        outputFile << "\t INIT [shape=point];" << std::endl;
        for (int i = 0; i < statechar.size(); i++)
        {
            outputFile << "\t " << statechar[i] << " [label=\"" << statechar[i] << "\"];  " << std::endl;
        }
        outputFile << std::endl;
        outputFile << "\t INIT -> " << state_machine.initstate << ";" << std::endl;
        std::set<std::string> include_state;
        for (int i = 0; i < new_implicantelements.size(); i++)
        {
            if (include_state.find(new_implicantelements[i].state_major) == include_state.end())
            {
                for (auto &majorstate_mapiterator : new_implicantelements[i].majorstate_element)
                {
                    outputFile << "\t " << new_implicantelements[i].state_major << " -> " << majorstate_mapiterator.second.next_state;
                    outputFile << "[label=\"" << majorstate_mapiterator.second.input << "/" << majorstate_mapiterator.second.output << "\"];" << std::endl;
                }
                include_state.insert(new_implicantelements[i].state_major);
            }
            if (include_state.find(new_implicantelements[i].state_minor) == include_state.end())
            {
                for (auto &minorstate_mapiterator : new_implicantelements[i].minorstate_element)
                {
                    outputFile << "\t " << new_implicantelements[i].state_minor << " -> " << minorstate_mapiterator.second.next_state;
                    outputFile << "[label=\"" << minorstate_mapiterator.second.input << "/" << minorstate_mapiterator.second.output << "\"];" << std::endl;
                }
                include_state.insert(new_implicantelements[i].state_minor);
            }
        }
        outputFile << "}" << std::endl;
        // close file
        outputFile.close();
        std::cout << "dot file is written " << std::endl;
    }
    else
    {
        std::cout << "can't open file" << std::endl;
    }
}