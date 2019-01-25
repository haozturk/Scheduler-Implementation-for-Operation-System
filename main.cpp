#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <sstream>
#include <queue>
#include "Process.h"

using namespace std;


/*
 * This is the overwritten operator of the arrival priority queue.
 * The incoming processes are stored in this priority queue. The comparison condition is the following:
 * Prioritize the processes according to their arrival times. If the arrival times of the two processes are the same,
 * Then prioritize them according to their priorities
 */
class arrivalQueueComparator
{
public:
    int operator() (const Process* p1, const Process* p2){

        if(p1->arrivalTime == p2->arrivalTime){
            return p1->priority > p2->priority;
        }
        else{
            return p1->arrivalTime > p2->arrivalTime;
        }
    }
};

/*
 * This is the overwritten operator of the ready priority queue.
 * The available processes for the CPU are stored in this priority queue. The comparison condition is the following:
 * Prioritize the processes according to their priorities. If the priorities of the two processes are the same,
 * Then prioritize them according to arrival times(FIFO)
 */
class myComparator
{
public:
    int operator() (const Process* p1, const Process* p2){

        if(p1->getPriority() == p2->getPriority()){
            return p1->arrivalTime > p2->arrivalTime;
        }
        else{
            return p1->getPriority() > p2->getPriority();
        }

    }
};

//This is the split function for parsing inputs
template <class Container>
void split1(const string& str, Container& cont)
{
    istringstream iss(str);
    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(cont));
}

/*
 * This is the prototype of the print function.
 * It takes a ready queue and the current time and prints the content of the ready queue
 */
string printFunction(priority_queue < Process*, vector<Process*>, myComparator > readyQueue, int time);

int main() {

    ifstream infile("definition.txt");
    //ifstream infile(argv[1]);

    vector<pair<int, string>> tempVec;

    string line;
    vector<Process*> processVec; //stores pointers to the processes inside the definition file
    vector<string> words;  //This is for parsing the input lines
    Process* p;

    //For writing to output file
    FILE *myfile;
    myfile = fopen("output.txt","w");

    /*
     * This loop reads the definition file line by line and creates Process objects.
     * Then it stores references to the created processes in a vector.
     */
    while(getline(infile, line)){
        split1(line, words);
        p = new Process(words[0],stoi(words[1]), words[2], stoi(words[3]));
        processVec.push_back(p);
        words.clear();
    }
    infile.close();


    /*
     * This piece of code reads code files for each process,
     * Then stores the instruction durations for each process in their instTime vector.
     * Besides it calculates the total CPU burst for each process.
     * Line number of the last instruction for each process is also calculated here
     */
    for(int i=0;i<processVec.size();i++){
        ifstream codeFiles((*(processVec[i])).codeFile + ".txt");
        while(getline(codeFiles,line)){
            split1(line,words);
            (*processVec[i]).instTime.push_back(stoi(words[1]));  //Take care
            processVec[i]->processingTime += stoi(words[1]);
            if(words[0] == "exit")
                processVec[i]->lastInst = processVec[i]->instTime.size();  //Specify the line number of the last instruction
            words.clear();
        }
        codeFiles.close();
    }


    /*
     * Processes which are available for the execution are stored in this priority queue
     * They are prioritized according to their priorities. If there are multiple processes whose priorities are the same,
     * Then they are prioritized according to their arrival times(FIFO)
     */
    priority_queue < Process*, vector<Process*>, myComparator > readyQueue;


    /*
     * Arrange the processes according to their arrival times.
     * Put them in a priority queue.
     * If there are multiple processes whose arrival time are the same,
     * Then sort them according their priorities
     */
    priority_queue < Process*, vector<Process*>, arrivalQueueComparator > arrivalQueue;
    for(int i=0;i<processVec.size();i++){
        arrivalQueue.push(processVec[i]);
    }

    //If there is no process which comes at time 0, then it writes the file the following
    if(arrivalQueue.top()->arrivalTime != 0)
        fprintf(myfile,"0:HEAD--TAIL\n");

    //True if the CPU stays idle
    bool isIdle = false;

    Process* currentProcess = arrivalQueue.top(); //The first process came in
    readyQueue.push(currentProcess);  //Push the current process to ready queue for execution
    arrivalQueue.pop(); //pop the process from the arrival queue

    int pastTime = currentProcess->arrivalTime; //pastTime stores current time.
    //Current time and ready queue is stored in the vector, here I stored the first process came in manually
    tempVec.push_back(make_pair(pastTime,printFunction(readyQueue,pastTime).c_str()));


    /*
     * The scheduling is performed in this while loop.
     * If there are processes either in the ready queue or the arrival queue it continues
     */
    while(!arrivalQueue.empty() || !readyQueue.empty()){

        //if there are processes to come
        if(!arrivalQueue.empty()){

            /*
            * When a new process comes, I first check the previous processes and update their states,
            * Then I add the incoming process to the system.
            */
            Process* incomingProcess = arrivalQueue.top();
            //timeLeft stores the available time for processes to execute
            int timeLeft = incomingProcess->arrivalTime - pastTime;
            /*
             * This variable stores the duration of the next instruction to be executed.
             * -1 is for indexing(vector starts from 0, whereas line numbers start from 1)
             */
            int  nextInstTime = currentProcess->instTime[currentProcess->nextInst-1];

            //This while loop handles the states of the processes before the next process comes
            while(timeLeft > 0){

                //If it is the "exit" instruction
                if(currentProcess->nextInst == currentProcess->lastInst) {
                    timeLeft = timeLeft - nextInstTime;
                    pastTime += nextInstTime;
                    readyQueue.pop();
                    // In this if block processes terminates. When this occurs I record the finishing time of each process
                    currentProcess->finishTime = pastTime;

                    /*
                     * By this time, we've popped the terminated process. Now we should take the next process from the ready queue
                     * if it is not empty. If the ready queue is empty, then CPU stays idle.
                     */
                    if (!readyQueue.empty()) {

                        currentProcess = readyQueue.top(); //Take the next process from the ready queue
                        nextInstTime = currentProcess->instTime[currentProcess->nextInst-1]; // duration of the next instruction

                        //State of the ready queue is changed, Print the content of it.
                        //Current time and ready queue is stored in the vector
                        tempVec.push_back(make_pair(pastTime,printFunction(readyQueue,pastTime).c_str()));

                    }//if the ready queue is empty
                    else {
                        /////    CPU IDLE

                        //State of the ready queue is changed, Print the empty ready queue.
                        //Current time and ready queue is stored in the vector
                        tempVec.push_back(make_pair(pastTime,printFunction(readyQueue,pastTime).c_str()));

                        //This variable used outside of the loop.
                        isIdle = true;
                        break;
                    }
                }
                /*
                 * If the instruction is not the exit instruction, then execute it.
                 * Increase the time, decrease the timeLeft.
                 * Specify in which line is to be executed.
                 */
                else{
                    timeLeft = timeLeft - nextInstTime;
                    pastTime += nextInstTime;
                    currentProcess->nextInst++;
                    nextInstTime = currentProcess->instTime[currentProcess->nextInst-1]; // duration of the next instruction
                }
            }

            //If the CPU is idle make the incoming process current.
            if(isIdle){
                isIdle = false;
                currentProcess = incomingProcess;
                readyQueue.push(incomingProcess);
                arrivalQueue.pop();

                pastTime = currentProcess->arrivalTime; ///// IMPORTANT
                //Current time and ready queue is stored in the vector
                tempVec.push_back(make_pair(pastTime,printFunction(readyQueue,pastTime).c_str()));
            }
            /*
            * In the previous while loop I handled the states of the processes before the next process comes.
            * There are 2 possibilities
            * 1. Incoming process may have higher priority than the current one -> preempt
            * 2. Incoming process may have lower priority than the current one -> push to the ready queue
            */
            else{
                if(incomingProcess->priority >= currentProcess->priority){
                    readyQueue.push(incomingProcess);
                    arrivalQueue.pop();

                    //Current time and ready queue is stored in the vector
                    tempVec.push_back(make_pair(pastTime,printFunction(readyQueue,pastTime).c_str()));
                }
                else{
                    currentProcess = incomingProcess;
                    readyQueue.push(incomingProcess);
                    arrivalQueue.pop();

                    //Current time and ready queue is stored in the vector
                    tempVec.push_back(make_pair(pastTime,printFunction(readyQueue,pastTime).c_str()));
                }
            }

        }
        //If there are no processes to come, finish the ready queue
        else{

            int nextInstTime = currentProcess->instTime[currentProcess->nextInst-1];

            while(!readyQueue.empty()){

                //if the next instruction is "exit"
                if(currentProcess->nextInst == currentProcess->lastInst){
                    pastTime += nextInstTime;
                    readyQueue.pop();
                    currentProcess->finishTime = pastTime; // This is the 2nd place where a process terminates

                    //Current time and ready queue is stored in the vector
                    tempVec.push_back(make_pair(pastTime,printFunction(readyQueue,pastTime).c_str()));

                    //if the ready queue is not empty go on
                    if(!readyQueue.empty()){
                        currentProcess = readyQueue.top();
                        nextInstTime = currentProcess->instTime[currentProcess->nextInst-1]; // duration of the next instruction
                    }
                    //if the ready is empty, halt the program
                    else{
                        cout << "END OF PROGRAM" << endl;
                        break;
                    }
                }
                //if the next instruction is not "exit"
                else{
                    pastTime += nextInstTime;
                    currentProcess->nextInst++;
                    nextInstTime = currentProcess->instTime[currentProcess->nextInst-1]; // duration of the next instruction

                }
            }
        }
    }

    /* My code prints multiple lines if multiple events occur at the same time.
    *  The following 3 for loops removes those duplicate lines.
    */
    vector<int> indexVec;
    for(int i=0;i<tempVec.size()-1;i++){
        if(tempVec[i].first != tempVec[i+1].first){
            indexVec.push_back(i);
        }
    }
    indexVec.push_back(tempVec.size()-1); //push last one in any case

    vector<pair<int,string>> finalVec;
    for(int i=0;i<indexVec.size();i++){
        finalVec.push_back(tempVec[indexVec[i]]);
    }

    for(int i=0;i<finalVec.size();i++){
        //cout << finalVec[i].second << endl;
        fprintf(myfile, "%s\n",finalVec[i].second.c_str());
    }


    /*
     * Print turnaround and waiting times
     * Turnaround Time = Finish Time - Arrival Time
     * Waiting Time = Turnaround Time - Total Processing Time
     */
    fprintf(myfile, "\n");
    for(int i=0;i<processVec.size();i++){
        processVec[i]->turnaroundTime = processVec[i]->finishTime - processVec[i]->arrivalTime;
        fprintf(myfile, "Turnaround time for %s = %d ms\n",processVec[i]->name.c_str(),processVec[i]->finishTime - processVec[i]->arrivalTime);
        if(i == processVec.size()-1){
            fprintf(myfile, "Waiting time for %s = %d",processVec[i]->name.c_str(),processVec[i]->turnaroundTime - processVec[i]->processingTime);
        }
        else{
            fprintf(myfile, "Waiting time for %s = %d\n",processVec[i]->name.c_str(),processVec[i]->turnaroundTime - processVec[i]->processingTime);
        }
    }
    fclose(myfile);

    //Free the dynamically allocated space
    for(int i=0;i<processVec.size();i++){
        delete processVec[i];
    }

    return 0;
}

//This function returns the content of the ready queue at the given time.
string printFunction(priority_queue < Process*, vector<Process*>, myComparator > readyQueue, int time){

    bool inWhile = false;
    string line = to_string(time) + ":HEAD-";
    while(!readyQueue.empty()){
        inWhile = true;
        line = line + readyQueue.top()->name + "[" + to_string(readyQueue.top()->nextInst) + "]-";
        readyQueue.pop();
    }
    if(inWhile){
        line = line + "TAIL";
    }
    else{
        line = line + "-TAIL";
    }

    return line;
}