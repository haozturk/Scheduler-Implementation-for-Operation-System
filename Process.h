#ifndef PROJECT1_PROCESS_H
#define PROJECT1_PROCESS_H

#include <string>
#include <vector>

using namespace std;

class Process {

public:

    vector<int> instTime; // holds the instruction durations for each process

    string name;          //name of the process
    string codeFile;      //name of the code file
    int priority;         //priority of the process
    int arrivalTime;      //arrival time of the process

    long nextInst;        //Line number of the next instruction to be executed
    long lastInst;        //Line number of the last instruction

    int waitingTime;      //waiting time of the process
    int turnaroundTime;   //turnaround time of the process
    int finishTime;       //finishing time of the process
    int processingTime;   //sum of the instruction durations of the process

    //Constructor prototype
    Process(string _name, int _priority, string _codeFile , int _arrivalTime);

    int getPriority() const;

};


#endif //PROJECT1_PROCESS_H
