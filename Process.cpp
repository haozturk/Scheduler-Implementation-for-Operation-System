//
// Created by tony on 16.11.2018.
//

#include "Process.h"

//Constructor
Process::Process(string _name, int _priority, string _codeFile, int _arrivalTime) {

    this->name = _name;
    this->codeFile = _codeFile;
    this->priority = _priority;
    this->arrivalTime = _arrivalTime;
    this->nextInst = 1;			//When a process is created, it's next instruction to be execution is 1 by default
    this->processingTime = 0;	//When a process is created, it's has never executed yet. Therefore it's processing time is 0 by default

}

int Process::getPriority() const {
    return priority;
}