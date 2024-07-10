
![image](https://github.com/SandyHoffmann/Inode/assets/60969091/9aba0c19-6c11-4d19-8dae-644b210d538a)

# Inode

Sistema que simula a parte de gerenciamento de arquivos de um sistema operacional. Possui uma divisão semelhante a figura abaixo:
![Untitled](https://github.com/SandyHoffmann/Inode/assets/60969091/4a4f762c-7cd6-4a0d-942a-22aa7df8739a)
- Magic Number - permite que o sistema operacional identifique o sistema de arquivos.
    - 0xf0f03410
- Blocks - numero total de blocos
- InodeBlocks - numero de blocos pré definidos para setar inodes (geralmente 10%)
- Inodes - Numero total de inodes nos InodeBlocks
  
Para mais informações acessar: [LINK DO ARQUIVO]

## Inicialização

Criar HD virtual utilizando o comando:
```
dd if=/dev/zero of=bla.hd bs=GB count=2
```
Criar um fifo para ver o processo do paralelismo
```
mkfifo output
```
Compilar o código
```
gcc main.c -lm
```

## Rodar o código
Abra dois terminais, em um digite:
```
./a.out
```
No outro abra o fifo
```
cat output
```
Conforme figura abaixo:
![image](https://github.com/SandyHoffmann/Inode/assets/60969091/9706bafa-b1f8-497f-8949-5b85058a9c01)

## Comandos disponíveis
- `ls`: Listar diretórios na árvore atual
- `touch nomeArquivo`: Cria pasta que contém arquivo
- `cat nomeArquivo s`: Mostra metadados do arquivo (`s`: mostra conteudo | `n`: não mostra conteudo)
- `cd nomeArquivo`: Navega para o diretório (ls muda também, e os arquivos vão ser criados apartir daquele diretório)
- `quit`: Sair

---
## Autores
- Sandy Hoffmann @SandyHoffmann
- Leonardo de Souza Fiamoncini @LeonardoFiamoncini
