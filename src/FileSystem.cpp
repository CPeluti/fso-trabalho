#include "globaldefinitions.h"

FileSystem::FileSystem(std::vector<std::pair<std::string,int>> disk)
{
    this->disk = disk;// disco armazena em cada posição o nome do arquivo que está alocado e o processo que o criou
    // this->log = log; // log-> int1=pid, int2=operacao, int3=tipo do erro na operacao, int4=primeiro bloco, bool= falha (false) ou sucesso (true)
    
    // quando o sistema de aquivos é contruído, a tabela de arquivos é feita com base na inicialização do disco
    std::map<std::string, std::pair<int, int>> ft; // string=nome do arquivo, int1=primeira posicao, int2=tamanho do bloco
    std::string name;
    int start, size, count=0;
    for(int i=0; i< this->disk.size(); i++){
        if(this->disk[i].first != "0" && count == 0){
            name = disk[i].first;
            start = i;
            count++;
        } else if((this->disk[i].first == "0" || this->disk[i].first != name) && count != 0){
            size = count;
            count = 0;
            ft[name] = {start, size};

            if(this->disk[i].first != "0"){
                name = disk[i].first;
                start = i;
                count++; 
            }
        } else if(this->disk[i].first == name && count != 0){
            count++;
        }
    }
    this->fileTable = ft;
}
FileSystem::FileSystem()
{
}

// método para descobrir qual processo criou o arquivo com o nome dado no parâmetro
int FileSystem::findOwnerOfFile(std::string fileName){
    return this->disk[this->fileTable[fileName].first].second;
}

// método para fazer uma operação do sistem de arquivos
bool FileSystem::doOperation(Operation operation, int priority)
{
    int pid = operation.getPid();
    int opCode = operation.getOpcode();
    std::string fileName = operation.getFileName();
    int frst;
    bool flag = false;

    switch(opCode){
            case 0: // 0 para criar um arquivo
            {
                int numBlocks = operation.getFileSize();
                int counter = -1;
                for(int i=0; i<this->disk.size(); i++){
                    if(this->disk[i].first == "0"){             // conta quantos blocos vazios no disco há
                        if(counter == -1){
                            frst = i;                           // guarda o primeiro bloco vazio em que haverá a alocação
                        }
                        counter++;
                    } else{
                        counter = -1;
                    }
                    if(counter == numBlocks-1){                     // Se o número de blocos vazios for o necessário, aloca-se
                        for(int j=frst; j<frst+numBlocks; j++){
                            this->disk[j] = {fileName, pid};
                        }
                        this->fileTable[fileName] = {frst, numBlocks}; // atualiza-se a tabela de arquivos com a posição e a quantidade de blocos
                        this->log.push_back({pid, 0, -1, frst, true}); // coloca no log que a operação foi bem sucedida
                        return true;
                    }
                }
                this->log.push_back({pid, 0, 3, -1, false});   // se não conseguiu alocar, coloca-se no log a falha
                return false;

            }

            case 1: // 1 para deletar um arquivo
            { 
                if(fileTable.find(fileName) != fileTable.end()){  // checa se o arquivo existe mesmo
                    // o arquivo só pode ser deletado se o processo que pede for de tempo real ou se foi o que criou o arquivo
                    if(findOwnerOfFile(fileName) == pid || priority == 0){  
                        // o arquivo é retirado e no lugar de seu nome é completado com 0
                        for(int i = fileTable[fileName].first; i < fileTable[fileName].first+fileTable[fileName].second; i++){
                            this->disk[i] = {"0", -1};
                        }
                        this->fileTable.erase(fileName);             // retira-se o arquivo da tabela de arquivos
                        this->log.push_back({pid, 1, -1, -1, true}); // coloca-se no log que a operação foi bem sucedida
                        return true;
                    } else{
                        // se o processo não puder fazer a operação coloca no log
                        this->log.push_back({pid, 1, 1, -1, false});
                        return false;
                    }
                } else { // se o arquivo não existe, entao coloca no log
                    this->log.push_back({pid, 1, 2, -1, false});
                    return false;
                }

            }

            default:
                // se o código de operação não existe coloca-se no log
                this->log.push_back({pid, opCode, -1, -1, false});
                return false;
    }

    return false;  // retorna-se o disco
}

// método para imprimir o estado do disco na tela
void FileSystem::showDisk()
{
    std::cout << "Mapa de Ocupação do Disco: " << std::endl;
    for(int i=0; i< this->disk.size(); i++){
        std::cout << this->disk[i].first << ' ';
    }
    std::cout << std::endl;
}

// método para se ter acesso ao disco
std::vector<std::pair<std::string,int>> FileSystem::getDisk(){
   return this->disk;
}

// método para se ter acesso ao log
std::vector<std::tuple<int, int, int, int, bool>> FileSystem::getLog(){
   return this->log;
}

// método para se ter acesso a tabela de arquivos
std::map<std::string, std::pair<int, int>> FileSystem::getFileTable(){
   return this->fileTable;
}