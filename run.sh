mkdir temps

echo "apagando arquivos antigos..."
rm -rf temps/*

gcc main.c -o teste -lm -w
./teste
rm teste