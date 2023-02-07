#include "ProcessesManager.h"

// executa um processo da fila de processos e se precisar, realimenta a fila 
Process execProcess(std::vector<std::vector<Process>>& readyProcesses, int queue, IO io, FileSystem& fs){
    Process* tempProcess = &(readyProcesses[queue].front());
    Process processCopy;
    Operation op;
    if(tempProcess->getPriority()){
        processCopy = *tempProcess;
        readyProcesses[queue].erase(readyProcesses[queue].begin());
        // se ainda falta executar, aumenta o tempo de exec e realimenta a fila zerando o wait
        op = processCopy.run(io,fs);
        if(op.status == op.EXECUTING || op.status == op.WAITING || processCopy.remainingOperations()){
            readyProcesses[processCopy.getPriority()].push_back(processCopy);
        }
    } else {
        op = tempProcess->run(io,fs);
        processCopy = *tempProcess;
        if(op.status != op.EXECUTING && op.status != op.WAITING){
            if(!tempProcess->remainingOperations() && (tempProcess->getRunningOp().status != op.WAITING && tempProcess->getRunningOp().status != op.EXECUTING)){
                readyProcesses[queue].erase(readyProcesses[queue].begin());
            }
        }
    }
    return processCopy;
}
bool ProcessesManager::queuesAreEmpty(){
    bool flag = true;
    for(auto& v : this->readyProcesses){
        if(v.size()){
            flag = false;
        }
    }
    return flag;
}
ProcessesManager::ProcessesManager(int max_wait){
    readyProcesses = std::vector<std::vector<Process>>(4);
    this->max_wait = max_wait;
    this->readyProcesses = std::vector<std::vector<Process>>(4);
}
bool ProcessesManager::insertProcess(Process process){
    if(this->readyProcesses[process.getPriority()].size()<1000){
        this->readyProcesses[process.getPriority()].push_back(process); 
        return true;
    } else {
        return false;
    }
}
// Escolhe um processo para realizar
Process ProcessesManager::cycleQueues(IO io, FileSystem& fs){
    // Verifica fila de tempo real
    this->updateWaits();
    if(this->readyProcesses[0].size()){
        return execProcess(this->readyProcesses, 0, io, fs);
    } else {
        this->checkWait();
        for(int i = 1; i<this->readyProcesses.size(); i++){
            if(this->readyProcesses[i].size()){
                return execProcess(this->readyProcesses, i, io, fs);
            }
        }
    }
    return Process();
}
Process ProcessesManager::run(IO io, FileSystem& fs){
    return this->cycleQueues(io, fs);
}
void ProcessesManager::updateWaits(){
    // começa em 1 para ignorar fila de tempo real
    for(int i = 1; i<this->readyProcesses.size(); i++){
        for(int j = 0; j<this->readyProcesses[i].size(); j++){
            // atualiza o wait de todos que estão na fila
            this->readyProcesses[i][j].incrementWait();
        }
    }
}
// Itera nas filas e gerencia as prioridades
void ProcessesManager::checkWait(){
    for(int i = 1; i <= 3; i++){
        for(int j = 0; j < this->readyProcesses[i].size(); j++){
            // aumenta a prioridade se o tempo de espera do processo for maior que o max_wait definido
            if(this->readyProcesses[i][j].getWait() > this->max_wait){    
                if(i>1){
                    // zera wait antes de mudar de fila
                    this->readyProcesses[i][j].updateWait(0);
                    this->readyProcesses[i-1].push_back(this->readyProcesses[i][j]);
                    //remove o processo da fila
                    this->readyProcesses[i].erase(this->readyProcesses[i].begin()+j);
                }
            }
        }
    }
}