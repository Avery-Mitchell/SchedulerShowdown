#include "schedulers.h"

//Round Robin scheduler implementation. In general, this function maintains a double ended queue
//of processes that are candidates for scheduling (the ready variable) and always schedules
//the first process on that list, if available (i.e., if the list has members)
int RoundRobin(const int& curTime, const vector<Process>& procList, const int& timeQuantum)
{
    static int timeToNextSched = timeQuantum;  //keeps track of when we should actually schedule a new process
    static deque<int> ready;  //keeps track of the processes that are ready to be scheduled

    int idx = -1;

    // first look through the process list and find any processes that are newly ready and
    // add them to the back of the ready queue
    for(int i = 0, i_end = procList.size(); i < i_end; ++i)
    {
        if(procList[i].startTime == curTime)
        {
            ready.push_back(i);
        }
    }

    // now take a look the head of the ready queue, and update if needed
    // (i.e., if we are supposed to schedule now or the process is done)
    if(timeToNextSched == 0 || procList[ready[0]].isDone)
    {
        // the process at the start of the ready queue is being taken off of the
        // processor

        // if the process isn't done, add it to the back of the ready queue
        if(!procList[ready[0]].isDone)
        {
            ready.push_back(ready[0]);
        }

        // remove the process from the front of the ready queue and reset the time until
        // the next scheduling
        ready.pop_front();
        timeToNextSched = timeQuantum;
    }

    // if the ready queue has any processes on it
    if(ready.size() > 0)
    {
        // grab the front process and decrement the time to next scheduling
        idx = ready[0];
        --timeToNextSched;
    }
    // if the ready queue has no processes on it
    else
    {
        // send back an invalid process index and set the time to next scheduling
        // value so that we try again next time step
        idx = -1;
        timeToNextSched = 0;
    }

    // return back the index of the process to schedule next
    return idx;
}

//Shortest Process Next implementation. The function is nonpreemptive (it continues to run until it terminates or blocks itself).
//It works by selecting the process with the least amount of time needed on the processor next.
int ShortestProcessNext(const int& curTime, const vector<Process>& procList){

    static int index = 0;       //Keeps track of what process index to return
    static int runTime = 0;     //Keeps track if the there is a process running

    //if the process is done, it resets the time 
    if(procList[index].isDone){
        runTime = 0;
    }

    //finds the first process in the procList that is not done
    for(long unsigned int y=0; y < procList.size(); y++){
        if((procList[y].isDone == 0) && (procList[y].startTime <= curTime) && (runTime == 0)){
            index = y;
            break;
        }
    }

    //finds the next process to run -- the process with the shortest totalTimeNeeded
    for(long unsigned int x=0; x < procList.size(); x++){
        if((procList[x].startTime <= curTime) && (procList[x].totalTimeNeeded < procList[index].totalTimeNeeded) && (procList[x].isDone != 1) && (runTime == 0)){
            index = x;
        }
    }
    runTime++;

    //need to check if all the processes are done, if so return -1
    long unsigned int count = 0;
    for(long unsigned int z=0; z < procList.size(); z++){
        if(procList[z].isDone){
            count++;
        }
    }
    if(count == procList.size()){
        return -1; 
    }

    return index; 
}

//Shortest Remaining Time implementation. The function is the preemptive version of Shortest Process Next.
//It works by running the process for the length of the provided time quantum, and then selecting
//The process with the least amount of time needed on the processor.
int ShortestRemainingTime(const int& curTime, const vector<Process>& procList, const int& timeQuantum){

    static int index = 0;       //Keeps track of what process index to return
    static int runTime = 0;     //Keeps track if the there is a process running

    //if the process is done, it resets the time 
    if(procList[index].isDone){
        runTime = 0;
    }

    //finds the first process in the procList that is not done
    for(long unsigned int y=0; y < procList.size(); y++){
        if((procList[y].isDone == 0) && (procList[y].startTime <= curTime) && (runTime == 0)){
            index = y;
            break;
        }
    }

    //finds the next process to run -- the process with the shortest time remaining
    for(long unsigned int x=0; x < procList.size(); x++){
        float indexTimeRemaining = procList[index].totalTimeNeeded - procList[index].timeScheduled;
        float xTimeRemaining = procList[x].totalTimeNeeded - procList[x].timeScheduled; 
        if((procList[x].startTime <= curTime) && (xTimeRemaining < indexTimeRemaining) && (procList[x].isDone != 1) && (runTime == 0)){
            index = x;
        }
    }
    //runTime++;

    //need to check if all the processes are done, if so return -1
    long unsigned int count = 0;
    for(long unsigned int z=0; z < procList.size(); z++){
        if(procList[z].isDone){
            count++;
        }
    }
    if(count == procList.size()){
        return -1; 
    }

    return index; 
}

//Highest Response Ratio Next implementation. The function is nonpreemptive (it continues to run until it terminates of blocks itself).
//It works by selecting the process with the best ratio to run next. The ratio is calcualted as such:
// Ratio = (time spent waiting + service time) / (service time)
int HighestResponseRatioNext(const int& curTime, const vector<Process>& procList){

    static int index = 0;       //Keeps track of what process index to return
    static int runTime = 0;     //Keeps track of how much time the processes has been running

    //if the process is done, it resets the time 
    if(procList[index].isDone){
        runTime = 0;
    }

    //finds the first process in the procList that is not done
    for(long unsigned int y=0; y < procList.size(); y++){
        if((procList[y].isDone == 0) && (procList[y].startTime <= curTime) && (runTime == 0)){
            index = y;
            break;
        }
    }

    //finds the next process to run -- the process with the shortest totalTimeNeeded
    for(long unsigned int x=0; x < procList.size(); x++){
        float xTimeSpentWaiting = curTime - procList[x].startTime - procList[x].timeScheduled;
        float indexTimeSpentWaiting = curTime - procList[index].startTime - procList[index].timeScheduled;
        float xRatio = (xTimeSpentWaiting + procList[x].totalTimeNeeded) / procList[x].totalTimeNeeded;
        float indexRatio = (indexTimeSpentWaiting + procList[index].totalTimeNeeded) / procList[index].totalTimeNeeded;

        if((procList[x].startTime <= curTime) && (xRatio > indexRatio) && (procList[x].isDone != 1) && (runTime == 0)){
            index = x;
        }
    }
    runTime++;

    //need to check if all the processes are done, if so return -1
    long unsigned int count = 0;
    for(long unsigned int z=0; z < procList.size(); z++){
        if(procList[z].isDone){
            count++;
        }
    }
    if(count == procList.size()){
        return -1; 
    }

    return index; 
}