#include "globaldefinitions.h"

MemoryManager::MemoryManager () {
    blocksCount = 1024;
    realTimeBlocksCount = 64;
    segmentsBegin = new Segment(false, 0, 64);
    segmentsUserBegin = new Segment(false, 64, 960);
    lastPid = -1;
}

MemoryManager::Segment::Segment (bool filled, int address, int size, Segment* nextSegment) {
    this->filled = filled;
    this->address = address;
    this->size = size;
    this->nextSegment = nextSegment;
}

MemoryManager::~MemoryManager () {
    std::array<Segment*, 2> segmentsList = {segmentsBegin, segmentsUserBegin};
    Segment* currentSegment,* nextSegment;

    for (auto& firstSegment: segmentsList) {
        currentSegment = firstSegment;
        do {
            nextSegment = currentSegment->nextSegment;
            delete(currentSegment);
        } while (nextSegment != nullptr);
    }
    segmentsBegin = segmentsUserBegin = nullptr;
}

int MemoryManager::GetFreeSegment (Segment* firstSegment, int blocksCount) {
    Segment* currentSegment = firstSegment;

    // busca por um segmento livre para alocacao, se encontrar cria processo e o poe numa tabela
    while (currentSegment != nullptr) {
        if ((not currentSegment->filled) and (currentSegment->size >= blocksCount)) {
            processesTable[++lastPid] = std::make_pair(currentSegment->address, blocksCount);

            // preenche o espaco de memoria alocado para o processo
            int nextSegmentAddress = currentSegment->address + blocksCount;
            currentSegment->filled = true;
            
            // se espaco disponivel no segmento eh maior que o requisitado, cria-se um novo segmento, livre
            if (currentSegment->size > blocksCount) {
                int newSegmentSize = currentSegment->size - blocksCount;
                currentSegment->size = blocksCount;

                Segment* newSegment = new Segment(
                    false, nextSegmentAddress, newSegmentSize, currentSegment->nextSegment
                );
                currentSegment->nextSegment = newSegment;
            }
            // retorna o pid do processo para alocacao bem sucedida
            return lastPid;
        }
        // se segmento esta ocupado ou se nao ha espaco suficiente, continua a busca
        currentSegment = currentSegment->nextSegment;
    }
    // se busca acabou e o segmento aponta para nulo, a alocacao nao pode ser realizada
    return -1;
}

int MemoryManager::Allocate (int priority, int blocksCount) {
    // se for processo em tempo real, procura segmento livre nos primeiros 64 blocos
    if (priority == 0)
        GetFreeSegment(segmentsBegin, blocksCount);
    // se for processo de usuario, procura a partir do bloco 64
    else GetFreeSegment(segmentsUserBegin, blocksCount);
}

bool MemoryManager::Deallocate (int pid) {
    // apaga o processo da tabela de processos
    if (not processesTable.count(pid)) return false;
    std::pair<int, int> addressSpace = processesTable[pid];
    processesTable.erase(pid);

    Segment* currentSegment = segmentsBegin;
    while (currentSegment != nullptr) {

        // procura o segmento ocupado pelo processo terminado e o marca como livre
        if (currentSegment->address == addressSpace.first) {
            currentSegment->filled = false;

            // se o proximo segmento tambem eh livre, mescla os dois segmentos
            Segment* nextSegment = currentSegment->nextSegment;
            if (not nextSegment->filled) {
                currentSegment->nextSegment = nextSegment->nextSegment;
                delete(nextSegment);
            }
            return true;
        }
        currentSegment = currentSegment->nextSegment;
    }
    // se o processo ja nao existia a principio, retorna falha
    return false;
}

int MemoryManager::GetSize () {
    return blocksCount;
}

int MemoryManager::GetRealTimeSize () {
    return realTimeBlocksCount;
}

int MemoryManager::GetUserSize () {
    return (blocksCount - realTimeBlocksCount);
}

std::map<int, std::pair<int, int>> MemoryManager::GetProcessesTable () {
    return processesTable;
}