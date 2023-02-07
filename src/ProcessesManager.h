#ifndef PROCESSES_MANAGER_H
#define PROCESSES_MANAGER_H

#include "cpplibraries.h"

#include "Process.h"

class ProcessesManager {
    private:
        std::vector<std::vector<Process>> readyProcesses;
        void updateWaits();
        void checkWait();
        int max_wait;
        Process cycleQueues(IO io, FileSystem& fs);

    public:
        bool queuesAreEmpty();
        // executa ciclo das filas
        Process run(IO io, FileSystem& fs);
        // max wait é o tempo de espera maximo de um processo antes dele aumentar a prioridade
        ProcessesManager(int max_wait);
        // Adiciona processa em alguma das filas
        bool insertProcess(Process process);
};

#endif