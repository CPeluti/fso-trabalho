#include "FileSystem.h"
#include "globaldefinitions.h"

int main (int argc, char *argv[]) {
    std::string processesFilename, filesFilename;
    processesFilename = argv[argc-2];
    filesFilename = argv[argc-1];

    Parser::ParseInputFiles(processesFilename, filesFilename);
    Hardware::disk = Parser::DiskInstantiator(Parser::diskBlocksCount, Parser::filesDescriptor);

    MemoryManager memoryManager;
    ProcessesManager processesManager(15);
    FileSystem fs(Hardware::disk);

    Dispatcher::Start(&memoryManager, &processesManager, &fs);
    Dispatcher::PrintLog();

    return 0;
}