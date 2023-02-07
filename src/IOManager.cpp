#include "globaldefinitions.h"

IO::IO(){
    printers = std::vector<int>(2,-1);
    SATAs = std::vector<int>(2,-1);
    modemPID = -1;
    scannerPID = -1;
}
bool IO::useModem(int pid){
    if(this->modemPID == -1 || this->modemPID == pid){
        this->modemPID = pid;
        return true;
    } else
        this->modemq.push(pid);
    return false;
}
bool IO::usePrinter(int pid,int id){
    switch(id){
        case 1:
            if(this->printers[0] == -1 || this->printers[0] == pid){
                this->printers[0] = pid;
                return true;
            } else
                this->printq1.push(pid);
            break;
        case 2:
            if(this->printers[1] == -1  || this->printers[1] == pid){
                this->printers[1] = pid;
                return true;
            } else
                this->printq2.push(pid);
            break;
        default:
            break;
    }
    return false;
}
bool IO::useSATA(int pid,int id){
    switch(id){
        case 1:
            if(this->SATAs[0] == -1 || this->SATAs[0] == pid){
                this->SATAs[0] = pid;
                return true;
            } else
                this->sataq1.push(pid);
            break;
        case 2:
            if(this->SATAs[1] == -1 || this->SATAs[1] == pid){
                this->SATAs[1] = pid;
                return true;
            } else
                this->sataq2.push(pid);
            break;
        default:
            break;
    }
    return false;
}
bool IO::useScanner(int pid){
    if(this->scannerPID == -1 || this->scannerPID == pid){
        this->scannerPID = pid;
        return true;
    } else
        this->scannerq.push(pid);
    return false;
}
void IO::freeScanner(int pid){
    if(this->scannerPID == pid){
        if(this->scannerq.size()){
            this->scannerPID = this->scannerq.front();
            this->scannerq.pop();
        } else
            this->scannerPID = -1;
    }
}
void IO::freeModem(int pid){
    if(this->modemPID == pid){
        if(this->modemq.size()){
            this->modemPID = this->modemq.front();
            this->modemq.pop();
        } else
            this->modemPID = -1;
    }
}
void IO::freePrinter(int pid, int id){
    switch(id){
        case 1:
            if(this->printers[0] == pid){
                if(this->printq1.size()){
                    this->printers[0] = this->printq1.front();
                    this->printq1.pop();
                } else
                    this->printers[0] = -1;
            }
            break;
        case 2:
            if(this->printers[1] == pid){
                if(this->printq2.size()){
                    this->printers[1] = this->printq2.front();
                    this->printq2.pop();
                } else
                    this->printers[1] = -1;
            }
            break;
        default:
            break;
    }
}
void IO::freeSATA(int pid,int id){
     switch(id){
        case 1:
            if(this->SATAs[0] == pid){
                if(this->sataq1.size()){
                    this->SATAs[0] = this->sataq1.front();
                    this->sataq1.pop();
                } else
                    this->SATAs[0] = -1;
            }
            break;
        case 2:
            if(this->SATAs[1] == pid){
                if(this->sataq2.size()){
                    this->SATAs[1] = this->sataq2.front();
                    this->sataq2.pop();
                } else
                    this->SATAs[1] = -1;
            }
            break;
        default:
            break;
    }
}