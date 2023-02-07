#ifndef OPERATION_H
#define OPERATION_H

#include "cpplibraries.h"

class Operation {
    public:
        Operation();
        Operation(
            int id, 
            int pid, 
            int opcode, 
            std::string file_name, 
            int file_size);
        int getId();
        int getPid();
        int getOpcode();
        std::string getFileName();
        int getFileSize();
        enum operation_status{
            NONE        = (1 << 0),
            WAITING     = (1 << 1),
            EXECUTING   = (1 << 2),
            FAILED      = (1 << 3),
            SUCCESS     = (1 << 4)
        };
        operation_status status;

    private:
        int id;
        int pid;
        int opcode;
        std::string file_name;
        int file_size;
};

#endif