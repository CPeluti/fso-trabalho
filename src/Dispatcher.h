#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "cpplibraries.h"
#include "MemoryManager.h"
#include "ProcessesManager.h"
#include "FileSystem.h"
#include "Process.h"
#include "Operation.h"

class Dispatcher {
    private:
        static std::vector<Process> instantiatedProcesses;
        static std::vector<Operation> instantiatedOperations;
        static int clock;

        // logs
        static std::vector<std::tuple<int, Process, int>> logProcesses;
        static std::vector<std::pair<int, Operation>> logOperations;

        // ponteiros
        static FileSystem* fileSystemPtr;

    public:
        static void Start(
            MemoryManager* memoryManager,
            ProcessesManager* processesManager,
            FileSystem* fileSystem
        );
        static void PrintLog();

    friend class Parser;
};

#endif