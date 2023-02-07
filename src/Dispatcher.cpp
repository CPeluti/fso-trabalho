#include "globaldefinitions.h"

int Dispatcher::clock = 0;
std::vector<Process> Dispatcher::instantiatedProcesses;
std::vector<Operation> Dispatcher::instantiatedOperations;
std::vector<std::tuple<int, Process, int>> Dispatcher::logProcesses;
std::vector<std::pair<int, Operation>> Dispatcher::logOperations;
FileSystem* Dispatcher::fileSystemPtr = nullptr;

void Dispatcher::Start (
    MemoryManager* memoryManager, ProcessesManager* processesManager, FileSystem* fileSystem
) {
    Parser::ProcessInstantiator(Parser::processesDescriptor);
    std::sort(instantiatedProcesses.begin(), instantiatedProcesses.end(),
        [&](Process A, Process B){ return (A.getInitTime() < B.getInitTime()); });

    for (int i=0; i < Parser::operationDescriptor.size(); i++) {
        Operation opRaw(
            i,
            stoi(Parser::operationDescriptor[i][0]),
            stoi(Parser::operationDescriptor[i][1]),
            Parser::operationDescriptor[i][2],
            stoi(Parser::operationDescriptor[i][3])
        );
        instantiatedOperations.push_back(opRaw);
    }

    while (true) {
        // se nao ha mais processos para serem criados e todas as operacoes foram concluidas, finalizar
        if (instantiatedProcesses.empty()) {
            if (processesManager->queuesAreEmpty())
                break;

        // se nova instancia de processo chegou, tenta alocar memoria
        } else {
            std::vector<Process>::iterator it = instantiatedProcesses.begin();
            do {
                Process newProcess = *it;
                if (newProcess.getInitTime() <= clock) {
                    int newProcessAddress = memoryManager->Allocate(&newProcess);

                    // se conseguiu alocar memoria, processo eh criado
                    if (newProcess.getPid() != -1){
                        for (int i=0; i < instantiatedOperations.size(); i++)
                            if (instantiatedOperations[i].getPid() == newProcess.getPid()) {
                                instantiatedOperations[i].status = Operation::WAITING;
                                newProcess.insertOperation(instantiatedOperations[i]);
                            }
                        processesManager->insertProcess(newProcess);

                        // faz o log dos processos
                        logProcesses.push_back(std::make_tuple(clock, newProcess, newProcessAddress));

                        it = instantiatedProcesses.erase(instantiatedProcesses.begin());
                    } else it++;
                } else break;
            } while (it != instantiatedProcesses.end());
        }
        // continua o processamento
        Process process = processesManager->run(Hardware::IOmanager, *fileSystem);
        if (process.isFinished())
            memoryManager->Deallocate(process.getPid());

        // faz o log das operacoes
        Operation operation = process.getRunningOp();
        if (operation.status & (Operation::SUCCESS | Operation::FAILED))
            logOperations.push_back(std::make_pair(clock, operation));

        clock++;
    }
    fileSystemPtr = fileSystem;
}

void Dispatcher::PrintLog () {
    std::cout << std::endl;
    for (auto& log_p: logProcesses) {
        Process& process = std::get<1>(log_p);
        std::cout << "dispatcher at time " << std::get<0>(log_p) << " =>" << std::endl;
        std::cout << "\tPID: " << process.getPid() << std::endl;
        std::cout << "\toffset: " << std::get<2>(log_p) << std::endl;
        std::cout << "\tblocks: " << process.getAllocMemBlocks() << std::endl;
        std::cout << "\tpriority: " << process.getPriority() << std::endl;
        std::cout << "\ttime: " << process.getExecTime() << std::endl;
        std::cout << "\tprinter: " << process.getPrinterCode() << std::endl;
        std::cout << "\tscanner: " << process.getScanReq() << std::endl;
        std::cout << "\tmodem: " << process.getModemReq() << std::endl;
        std::cout << "\tdrive: " << process.getDiskNum() << std::endl << std::endl;

        int processPid = process.getPid();
        std::cout << "process " << processPid << " =>" << std::endl;
        std::cout << "\tP" << process.getPid() << " STARTED" << std::endl;
        std::sort(logOperations.begin(), logOperations.end(),
        [&](std::pair<int, Operation> A, std::pair<int, Operation> B) {
            return (A.second.getId() < B.second.getId());
        });
        int index = 0;
        for (auto& log_o: logOperations)
            if (log_o.second.getPid() == processPid)
                std::cout << "\tP" << process.getPid() << " instruction " << index++ << " made at time " << log_o.first << std::endl;
        std::cout << "\tP" << process.getPid() << " return SIGINT" << std::endl << std::endl;
    }

    std::vector<std::tuple<int, int, int, int, bool>> fileSystemLog = fileSystemPtr->getLog();
    std::cout << "Sistema de Arquivos =>";
    for (auto& inst_op: instantiatedOperations) {
        std::tuple<int, int, int, int, bool> log_op;

        int i, logSize = fileSystemLog.size();
        for (i=0; i < logSize; i++) {
            if (std::get<0>(fileSystemLog[i]) == inst_op.getPid()) {
                log_op = fileSystemLog[i];
                fileSystemLog.erase(fileSystemLog.begin()+i);
                break;
            }
        } if (i == logSize) {
            log_op = std::make_tuple(inst_op.getPid(), inst_op.getOpcode(), 0, -1, false);
        }
        std::cout << "\n\nOperacao " << (inst_op.getId()+1) << " => " << (std::get<4>(log_op)? "Sucesso" : "Falha") << std::endl;
        std::cout << "O processo " << std::get<0>(log_op) << " ";
        /*
            int0 = pid (erro se pid nao existir)
            int1 = opcode (erro se nao existir)
            int2 = tipo do erro
                -1 = sem erro
                0 = erro se pid nao existir // automatico do dispatcher
                1 = processo sem permissao de delecao
                2 = arquivo nao existe
                3 = falta de espaco // fs editado depois
            int3 = primeiro bloco do arquivo (criado)
            bool4 = falha (false) ou sucesso (true)
        */
        if ((std::get<1>(log_op) > 1) or (std::get<1>(log_op) < 0)) {
            std::cout << "tentou realizar uma operacao que nao existe" << std::endl << std::endl;
        } else if (std::get<4>(log_op)) {
            if (std::get<1>(log_op)) {
                std::cout << "deletou o arquivo " << inst_op.getFileName();
            } else {
                std::cout << "criou o arquivo " << inst_op.getFileName() << " ( blocos: ";
                for (int j=0; j < inst_op.getFileSize(); j++)
                    std::cout << (std::get<3>(log_op)+j) << " ";
                std::cout << ")";
            }
        } else {
            switch (std::get<2>(log_op)) {
                case 1:
                    std::cout << "nao tem permissao para deletar o arquivo " << inst_op.getFileName();
                    break;
                case 2:
                    std::cout << "nao pode deletar o arquivo " << inst_op.getFileName() << " porque ele nao existe";
                    break;
                case 3:
                    std::cout << "nao pode criar o arquivo " << inst_op.getFileName() << " por falta de espaco";
                    break;
                default:
                    std::cout << "nao existe";
                    break;
            }
        }
    }
    std::cout << "\n\nMapa de Ocupacao do disco:\n|";
    std::vector<std::pair<std::string, int>> fsDisk = fileSystemPtr->getDisk();
    for (auto& file: fsDisk)
        if (file.first == "0") std::cout << "   |";
        else std::cout << " " << file.first << " |";
    std::cout << std::endl << std::endl;
}