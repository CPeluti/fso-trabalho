#include "globaldefinitions.h"

int Parser::diskBlocksCount = 0;
std::vector<std::vector<std::string>> Parser::filesDescriptor;
std::vector<std::vector<std::string>> Parser::operationDescriptor;
std::vector<std::vector<std::string>> Parser::processesDescriptor;

// faz a marotagem toda
void Parser::ParseInputFiles (std::string processesFilename, std::string filesFilename) {
    ParseProcessesFile(processesFilename);
    ParseFilesFile(filesFilename);
}

// retorna o arquivo de files.txt em uma tupla de {diskSize, operations, inicialização}
void Parser::ParseFilesFile (std::string filesFilename) {
    int diskSize, diskSegmentsFilled;
    std::vector<std::vector<std::string>> parsedFileLines;
    std::vector<std::vector<std::string>> parsedOperationLines;

    std::string line;
    std::ifstream file;
    file.open(filesFilename);

    // quantidade de blocos do disco
    std::getline(file, line);
    diskSize = stoi(line);

    // quantidade de blocos ocupados
    std::getline(file, line);
    diskSegmentsFilled = stoi(line);

    // arquivos salvos no disco incialmente
    while (std::getline(file, line)) {
        std::vector<std::string> parsedFileLine = split(line, ", ");
        if (diskSegmentsFilled-- > 0)
            parsedFileLines.push_back(parsedFileLine);
        else break;
    }
    // operacoes programadas do sistema de arquivos
    do {
        std::vector<std::string> parsedOperationLine = split(line, ", ");
        if (parsedOperationLine.size() < 4)
            parsedOperationLine.push_back("-1");
        parsedOperationLines.push_back(parsedOperationLine);
    } while (std::getline(file, line));

    diskBlocksCount = diskSize;
    filesDescriptor = parsedFileLines;
    operationDescriptor = parsedOperationLines;
}

// retorna o parse do arquivo de processos;
void Parser::ParseProcessesFile (std::string processesFilename) {
    std::vector<std::vector<std::string>> parsedProcessLines;

    std::string line;
    std::ifstream file;
    file.open(processesFilename);

    while (std::getline(file, line)) {
        std::vector<std::string> parsedLine = split(line,", ");
        parsedProcessLines.push_back(parsedLine);
    }
    processesDescriptor = parsedProcessLines;
}

// Recebe vetor dos processos a partir do parser e instancia eles no vetor de processos instanciados
void Parser::ProcessInstantiator (std::vector<std::vector<std::string>> parsedProcesses) {
    for(int i = 0; i<(int)parsedProcesses.size(); i++){
        Process p(
            stoi(parsedProcesses[i][0]),
            stoi(parsedProcesses[i][1]),
            stoi(parsedProcesses[i][2]),
            stoi(parsedProcesses[i][3]),
            stoi(parsedProcesses[i][4]),
            stoi(parsedProcesses[i][5]),
            stoi(parsedProcesses[i][6]),
            stoi(parsedProcesses[i][7])
        );
        Dispatcher::instantiatedProcesses.push_back(p);
    }
}

// Cria o vetor de disk a partir do tamanho e dos files iniciais onde quem criou é -1
std::vector<std::pair<std::string, int>> Parser::DiskInstantiator (int diskBlocksCount, std::vector<std::vector<std::string>> parsedFileLines) {
    std::vector<std::pair<std::string,int>> disk(diskBlocksCount, {"0", -1});

    for(int i=0; i < (int)parsedFileLines.size(); i++){
        int start = stoi(parsedFileLines[i][1]);
        int size = stoi(parsedFileLines[i][2]);

        for(int j=start; j < start+size; j++)
            disk[j] = {parsedFileLines[i][0], -1};
    }
    return disk;
}