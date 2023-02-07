#ifndef PROCESS_H
#define PROCESS_H

#include "cpplibraries.h"
#include "IOManager.h"
#include "FileSystem.h"
#include "Operation.h"

class Process {
    public:
        Process();
        Process(
            int init_time,
            int priority,
            int exec_time,
            int alloc_mem_blocks,
            int printer_code,
            int scan_req,
            int modem_req,
            int disk_num
        );
        int remainingOperations();
        Operation getRunningOp();
        int getRemainingTime();
        int getPid();
        int getInitTime();
        int getPriority();
        int getExecTime();
        int getRunTime();
        int getAllocMemBlocks();
        int getPrinterCode();
        int getScanReq();
        int getModemReq();
        int getDiskNum();
        int getWait();
        bool getIO(IO& io);
        void freeIO(IO& io);
        Operation run(IO io, FileSystem& fs);
        void incrementWait();
        void updateWait(int new_wait);
        void setPriority(int new_priority);
        void insertOperation(Operation operation);
        bool isFinished();
    private:
        void resetRunTime();
        void updateRunTime(int executed_time);
        std::queue<Operation> operations;
        int pid;
        int init_time;
        int priority;
        int exec_time;
        int run_time;
        int alloc_mem_blocks;
        int printer_code;
        int scan_req;
        int modem_req;
        int disk_num;
        int wait;
        Operation running_op;
    friend class MemoryManager;
};

#endif
