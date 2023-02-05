#ifndef PROCESS_H
#define PROCESS_H

#include "cpplibraries.h"

class Process {
    public:
        Process();
        Process(
            int pid,
            int init_time,
            int priority,
            int exec_time,
            int alloc_mem_blocks,
            int printer_code,
            int scan_req,
            int modem_req,
            int disk_num,
            std::queue<Operation> operations
        );
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
        bool getIO(int io);
        bool freeIO(int io);
        Operation run();
        void incrementWait();
        void updateWait(int new_wait);
        void setPriority(int new_priority);
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
};

#endif
