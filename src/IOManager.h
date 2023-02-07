#ifndef IO_H
#define IO_H
#include "cpplibraries.h"
class IO {
    public:
        IO();
        bool useScanner(int pid);
        bool usePrinter(int pid,int id);
        bool useModem(int pid);
        bool useSATA(int pid,int id);
        void freeScanner(int pid);
        void freePrinter(int pid,int id);
        void freeModem(int pid);
        void freeSATA(int pid,int id);

    private:
        std::queue<int> printq1;
        std::queue<int> printq2;
        std::queue<int> scannerq;
        std::queue<int> sataq1;
        std::queue<int> sataq2;
        std::queue<int> modemq;
        std::vector<int> printers;
        std::vector<int> SATAs;
        int modemPID;
        int scannerPID;
};
#endif