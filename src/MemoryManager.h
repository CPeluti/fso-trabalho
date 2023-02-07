#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "cpplibraries.h"
#include "Process.h"

class MemoryManager {
    private:
        class Segment;
        int blocksCount;
        int realTimeBlocksCount;
        int lastPid;
        Segment* segmentsBegin,* segmentsUserBegin;
        std::map<int, std::pair<int, int>> processesTable;

        int GetFreeSegment(Segment* firstSegment, int blocksCount);

    public:
        MemoryManager();
        ~MemoryManager();
        int Allocate(Process* process);
        bool Deallocate(int pid);
        int GetSize();
        int GetRealTimeSize();
        int GetUserSize();
        std::map<int, std::pair<int, int>> GetProcessesTable();
};

class MemoryManager::Segment {
    public:
        bool filled;
        int address;
        int size;
        Segment* nextSegment;
    
        Segment(bool filled, int address, int size, Segment* nextSegment=nullptr);
};

#endif