#include "globaldefinitions.h"
#include "IOManager.h"
#include "FileSystem.h"

Process::Process(int init_time, int priority, int exec_time, int alloc_mem_blocks, int printer_code, int scan_req, int modem_req, int disk_num)
{
    this->pid = -1;
    this->run_time = 0;
    this->init_time = init_time;
    this->priority = priority;
    this->exec_time = exec_time;
    this->alloc_mem_blocks = alloc_mem_blocks;
    this->printer_code = printer_code;
    this->scan_req = scan_req;
    this->modem_req = modem_req;
    this->disk_num = disk_num;
    this->wait = 0;
    this->running_op = Operation(-1,-1,-1,"",-1);
    this->running_op.status = this->running_op.NONE;
}
Process::Process()
{
    this->pid = -1;
    this->run_time = -1;
    this->init_time = -1;
    this->priority = -1;
    this->exec_time = -1;
    this->alloc_mem_blocks = -1;
    this->printer_code = -1;
    this->scan_req = -1;
    this->modem_req = -1;
    this->disk_num = -1;
    this->wait = -1;
    this->running_op = Operation(-1,-1,-1,"",-1);
    this->running_op.status = this->running_op.NONE;
}
int Process::remainingOperations(){
    return this->operations.size();
}
// zera run_time
void Process::resetRunTime()
{
    this->run_time = 0;
}
// retorna o tempo de de processador restante (exec_time - run_time)
int Process::getRemainingTime()
{
    return this->exec_time-this->run_time;
}
// aumenta wait em 1
void Process::incrementWait(){
    this->wait++;
}
// soma executed_time no run_time
void Process::updateRunTime(int executed_time)
{
    this->run_time += executed_time;
}
// setta wait como new_wait
void Process::updateWait(int new_wait){
    this->wait = new_wait;
}
// seta priority como new_priority
void Process::setPriority(int new_priority){
    this->priority = new_priority;
}

void Process::insertOperation(Operation operation) {
    operations.push(operation);
}

bool Process::isFinished () {
    return ((remainingOperations() == 0) and (not (getRunningOp().status & (Operation::WAITING | Operation::EXECUTING))));
}

Operation Process::getRunningOp(){
    return this->running_op;
}
int Process::getWait(){
    return this->wait;
}
int Process::getPid(){
   return this->pid;
}
int Process::getInitTime(){
    return this->init_time;
}
int Process::getPriority(){
    return this->priority;
}
int Process::getExecTime(){
    return this->exec_time;
}
int Process::getRunTime(){
    return this->run_time;
}
int Process::getAllocMemBlocks(){
    return this->alloc_mem_blocks;
}
int Process::getPrinterCode(){
    return this->printer_code;
}
int Process::getScanReq(){
    return this->scan_req;
}
int Process::getModemReq(){
    return this->modem_req;
}
int Process::getDiskNum(){
    return this->disk_num;
}
bool Process::getIO(IO& io){
    if(this->modem_req){
        return io.useModem(this->pid);
    } 
    if(this->printer_code){
        return io.usePrinter(this->pid, this->printer_code);

    }
    if(this->scan_req){
        return io.useScanner(this->pid);
    }
    if(this->disk_num){
        return io.useSATA(this->pid, this->disk_num);
    }
    return true;
}
void Process::freeIO(IO& io){
    if(this->modem_req){
        io.freeModem(this->pid);
    } 
    if(this->printer_code){
        io.freePrinter(this->pid, this->printer_code);
    }
    if(this->scan_req){
        io.freeScanner(this->pid);
    }
    if(this->disk_num){
        io.freeSATA(this->pid, this->disk_num);
    }
}
Operation Process::run(IO io, FileSystem& fs){
    if(this->running_op.status != this->running_op.WAITING && this->running_op.status != this->running_op.EXECUTING){
        if(this->operations.size()){
            this->running_op = this->operations.front();
            this->operations.pop();
            this->resetRunTime();
        }
    }
    if(this->getRemainingTime()-1 >= 0){
        if(!getIO(io))
            return this->running_op;
        this->updateRunTime(1);
        this->updateWait(0);
        if(this->running_op.status == this->running_op.WAITING){
            bool res = fs.doOperation(this->running_op, this->priority);
            if(!res)
                this->running_op.status = this->running_op.FAILED;
            else
                this->running_op.status = this->running_op.EXECUTING;
        }
        return this->running_op;
    } else {
        // separa tarefa
        
        // this->running_op = this->operations.front();
        // this->operations.pop();
        if(this->running_op.status != this->running_op.FAILED && this->running_op.status != this->running_op.NONE)
            this->running_op.status = this->running_op.SUCCESS;
        this->freeIO(io);
        return this->running_op;
    }
    
}