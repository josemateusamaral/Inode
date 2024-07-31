#include "xsyscalls.h"

//printar o file descriptor
void printXFILE(XFILE * arquivo){
    printInode_OBJ(arquivo->inode);
    printf("posicaoRelativa:%d\nposicaoFisica:%d\n",arquivo->posicaoRelativa,arquivo->posicaoFisica);
    printf("tamanhoArquivo:%d\n",arquivo->tamanhoArquivo);
}


// formatar disco rapidamente sem limpar os blocos
void xformatFast()
{
    char buffer[4096];
    bzero(buffer, 4096);
    lseek(xDisc, 0, SEEK_SET);
    int totalHd1gb = (1024 * 1024 * 1024) / 4096;
    for (int i = 0; i < totalHd1gb; i++)
    {
        write(xDisc, buffer, 4096);
    }
}

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 23/04/2023.
 * \brief Formating a disk in Inode system means:
 * 1. Creating a superblock.
 * 1.1 Superblock will contain all the information about the disk.
 * 1.2 Superblock will be the first block of the disk.
 * 2. Allocate the first directory entry (root directory) = /.
 * 2.1 Allocate the inode for the root directory.
 * 3. Allocating the inode bitmap including this new instance of root directory.
 * 4. Allocating the data bitmap including the superblock location, inode bitmap, data bitmap, the root directory, and inode reserved sector.
 * \param xDisc The file containing the hd itself.
 **/
void xformat()
{
    // * Inode size increased to 128 bytes (22/06/2023)
    struct SuperBlock block = create_super_block(4096, 128, 1);
    void *buffer = malloc(block.block_size);
    memcpy(buffer, &block, sizeof(struct SuperBlock));
    lseek(xDisc, 0, SEEK_SET);
    write(xDisc, buffer, block.block_size);
    free(buffer);

    // * Inode bitmap already mounted on hd
    int bit_free_inode = return_free_inode_bit(xDisc, block);

    Directory * root = create_dir("/", bit_free_inode);
    struct Inode *inode = create_inode("",DIRECTORY, NULL);

    // * Write Inode in HD
    long int adress_first_inode = block.inode_start;
    lseek(xDisc, adress_first_inode, SEEK_SET);
    write(xDisc, inode, block.inode_size);

    // * Write Directory in HD
    long int adress_dir = block.inode_directory_start;
    lseek(xDisc, adress_dir, SEEK_SET);
    write(xDisc, root, sizeof(Directory));

    // * Data bitmap represents the data blocks that are free or not.
    long int total_data = block.data_start;                    
    int occupied_blocks = ROUND(total_data / block.block_size);

    for (int i = 0; i < occupied_blocks; i++)
    {
        return_free_data_bit(xDisc, block);
    }

    //printf("\n\nOccupied blocks: %d\n", occupied_blocks);

    xReadBlock = block;
}

// testar o discos
void xtest(int xDisc)
{
    xformatFast();
    sleep(2);
    xformat(xDisc);
    sleep(2);

    struct SuperBlock ReadBlock;
    char *buffer = malloc(4096);
    lseek(xDisc, 0, SEEK_SET);
    read(xDisc, buffer, 4096);
    memcpy(&ReadBlock, buffer, sizeof(struct SuperBlock));

    //create_dump_directory_tree();
}

// montar disco
void xmount( char * pathDisco )
{
    if ((xDisc = open(pathDisco, O_RDWR)) == -1)
    {
        printf("Erro ao montar o disco...");
        exit(1);
    }
    xReadBlock = read_superblock(xDisc);

}

// desmontar disco
void xdismount(){
    close(xDisc);
}

// criar diretorio
void xmkdir(char * nomePasta){
    CreateEntry(nomePasta,DIRECTORY);
}

// criar arquivo
XFILE xopen(char * nomeArquivo,char * tipo){

    Directory * directory_instance = find_dir(xpath,nomeArquivo);
    
    //diretorio nao existe
    if(!directory_instance){
        CreateEntry(nomeArquivo,REGULAR_FILE);
    }

    // retornar descritor do arquivo
    Directory * entrada = find_dir(xpath,nomeArquivo);
    struct Inode * inode = readInode(entrada->inode);
    XFILE arquivo;
    arquivo.inode = inode;
    arquivo.posicaoRelativa = 0;
    arquivo.posicaoFisica = 0;
    arquivo.tamanhoArquivo = 0;

    return arquivo;
}

// escrever no arquivo
void xwrite( XFILE * arquivo, char * buffer, int tamanhoEscrita){
    struct Inode * inode = arquivo->inode;

    // configurar novo tamanha do arquivo
    if( arquivo->posicaoRelativa + tamanhoEscrita > arquivo->tamanhoArquivo ){
        arquivo->tamanhoArquivo = arquivo->posicaoRelativa + tamanhoEscrita;
    }

    // configurar acesso aos indirects
    long int total_data_indirects_1 = xReadBlock.block_size / sizeof(long int);
    long int total_data_indirects_2 = pow(xReadBlock.block_size / sizeof(long int),2);
    long int active_indirect = 0;
    long int * all_indirects = malloc(sizeof(long int)*3);
    memset(all_indirects, 0, sizeof(long int)*3);

    // configurar variaveis para a escrita
    long int total_size_block = 512;
    long int total_blocks_read = 0;
    long int total_escrita = 0;
    long int inicioBlocoEscrita = arquivo->tamanhoArquivo / xReadBlock.block_size;
    long int inicioEscrita = (arquivo->posicaoRelativa % xReadBlock.block_size);

    //escrever nos blocos do indirect 1 caso tenha algo para escrever
    if(inicioBlocoEscrita <= total_data_indirects_1){
        for (int i = 0; i < (arquivo->tamanhoArquivo / xReadBlock.block_size) + 1; i++){
            if( total_blocks_read >= inicioBlocoEscrita ){
                unsigned char * leitura = (unsigned char)malloc(xReadBlock.block_size);
                leitura = read_block(inode->indirect1, i);
                //printf("\n\nBLOCO XWRITE:\n");
                for(int cada = inicioEscrita ; cada < xReadBlock.block_size; cada++ ){
                    //printf("%02X ",buffer[cada]);
                    if( cada <= inicioEscrita + tamanhoEscrita){
                        leitura[cada] = buffer[total_escrita];
                        total_escrita++;
                    }
                }
                // printf("\n");
                long int physical_offset_num = physicalAddress(xReadBlock.block_size,inode->indirect1)+(i*sizeof(long int));
                long int bloco;
                lseek(xDisc, physical_offset_num , SEEK_SET);
                read(xDisc, &bloco, sizeof(long int));
                write_block(leitura,bloco);
                inicioEscrita = 0;
            }
            total_blocks_read++;
            if (total_blocks_read == total_size_block || total_escrita + arquivo->posicaoRelativa > arquivo->tamanhoArquivo){
                break;
            }
        }
    }else{
        printf("\nEscrita muito grande");
    }
}

// ler dados de um inode
void xread(XFILE * arquivo, char * dadosLidos, int tamanhoLeitura){
        
    struct Inode * inode = arquivo->inode;

    // * Information about the pointers
    // * each indirect pointer to a block of data (block_size or 4096 bytes)
    long int total_data_indirects_1 = xReadBlock.block_size / sizeof(long int);
    long int total_data_indirects_2 = pow(xReadBlock.block_size / sizeof(long int),2);
    long int total_size_block = 512;
    long int total_blocks_read = 0;
    long int total_leitura = 0;
    long int active_indirect = 0;
    long int posicaoLeitura = 0;

    long int inicioBlocoLeitura = arquivo->tamanhoArquivo / xReadBlock.block_size;
    long int inicioLeitura = arquivo->posicaoRelativa % xReadBlock.block_size;

    long int * all_indirects = malloc(sizeof(long int)*3);
    memset(all_indirects, 0, sizeof(long int)*3);

    for (int i = 0; i < (arquivo->tamanhoArquivo / total_data_indirects_1); i++){
        if( total_blocks_read >= inicioBlocoLeitura ){
            unsigned char * leitura = (unsigned char *)malloc(xReadBlock.block_size);
            leitura = read_block(inode->indirect1, i);
            for(int cada = 0 ; cada < xReadBlock.block_size; cada++ ){
                posicaoLeitura++;
                if(posicaoLeitura>inicioLeitura && posicaoLeitura<inicioLeitura+tamanhoLeitura+1){
                    dadosLidos[total_leitura] = leitura[cada];
                    total_leitura++;
                }
            }
        }
        total_blocks_read++;
        if (total_blocks_read == total_size_block){
            break;
        }
    }

}


//configurar posicao para leitura e escrita em um arquivo
void xseek(XFILE * file, long int offset, int posicionador){
    
    //salvar a posicao atual para o caso dos parametros estourarem
    //OBS: neste contexto, estourar é quando o file.posicaoNoArquivo for menor que 0 ou maior que o tamanho do arquivo
    int posicaoAtualBackup = file->posicaoRelativa;
    
    //definir o posicionador usando o SEEK_SET e o SEEK_END igual as chamadas originais
    switch(posicionador){
        case SEEK_SET:
            file->posicaoRelativa = 0;
            break;
        case SEEK_END:
            file->posicaoRelativa = file->tamanhoArquivo;
            break;
    }

    //aplicar o offset
    file->posicaoRelativa += offset;

    //verificar se estourou
    if( file->posicaoRelativa < 0 || file->posicaoRelativa > file->tamanhoArquivo){
        file->posicaoRelativa = posicaoAtualBackup;
        printf("\n!! ATENÇÂO: fseek inválido. Você saiu de onde o arquivo esta !!\n");
    }

}



//mudar diretorio
void xchdir(char * dir_name){

    //diretorio
    long int dir = find_dir(xpath,dir_name);
    if( !dir ){
        printf("diretorio nao existe...");
        return 0;
    }
    Directory * diretorio = returnDirPhysicalLocation(dir);
    struct Inode * no = readInode(diretorio->inode);

    printf("Inicio dir %d\n",no->indirect1);


}

