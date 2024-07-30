
![image](https://github.com/SandyHoffmann/Inode/assets/60969091/9aba0c19-6c11-4d19-8dae-644b210d538a)

# Inode

Sistema que simula a parte de gerenciamento de arquivos de um sistema operacional. Possui uma divisão semelhante a figura abaixo:
![Untitled](https://github.com/SandyHoffmann/Inode/assets/60969091/4a4f762c-7cd6-4a0d-942a-22aa7df8739a)
- Magic Number - permite que o sistema operacional identifique o sistema de arquivos.
    - 0xf0f03410
- Blocks - numero total de blocos
- InodeBlocks - numero de blocos pré definidos para setar inodes (geralmente 10%)
- Inodes - Numero total de inodes nos InodeBlocks
  
