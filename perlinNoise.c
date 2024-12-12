#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415

/*  CÓDIGO DE ALEATORIEDADE DE SEED

    #include <time.h>
    #include <stdlib.h>

    srand(time(NULL));
*/


void gerarBitmap(const char* nomeArquivo, unsigned char** dados, int largura, int altura) {
    // Tamanho do cabeçalho BMP: 54 bytes
    unsigned char cabecalho[54] = {
        0x42, 0x4D,             // Assinatura "BM"
        0, 0, 0, 0,             // Tamanho total do arquivo (será preenchido depois)
        0, 0, 0, 0,             // Reservado
        54, 0, 0, 0,            // Offset para os dados da imagem
        40, 0, 0, 0,            // Tamanho do cabeçalho da imagem (40 bytes)
        0, 0, 0, 0,             // Largura da imagem (será preenchido depois)
        0, 0, 0, 0,             // Altura da imagem (será preenchido depois)
        1, 0,                   // Planos de cor (sempre 1)
        24, 0,                  // Bits por pixel (24 para RGB)
        0, 0, 0, 0,             // Compressão (0 = sem compressão)
        0, 0, 0, 0,             // Tamanho da imagem (pode ser 0 para sem compressão)
        0x13, 0x0B, 0, 0,       // Resolução horizontal (pixels por metro, padrão 72 DPI)
        0x13, 0x0B, 0, 0,       // Resolução vertical (pixels por metro, padrão 72 DPI)
        0, 0, 0, 0,             // Cores na paleta (0 = todas)
        0, 0, 0, 0              // Cores importantes (0 = todas)
    };

    // Preenche largura e altura no cabeçalho
    int tamanhoArquivo = 54 + largura * altura * 3; // Cada pixel tem 3 bytes (RGB)
    cabecalho[2] = (unsigned char)(tamanhoArquivo);
    cabecalho[3] = (unsigned char)(tamanhoArquivo >> 8);
    cabecalho[4] = (unsigned char)(tamanhoArquivo >> 16);
    cabecalho[5] = (unsigned char)(tamanhoArquivo >> 24);

    cabecalho[18] = (unsigned char)(largura);
    cabecalho[19] = (unsigned char)(largura >> 8);
    cabecalho[20] = (unsigned char)(largura >> 16);
    cabecalho[21] = (unsigned char)(largura >> 24);

    cabecalho[22] = (unsigned char)(altura);
    cabecalho[23] = (unsigned char)(altura >> 8);
    cabecalho[24] = (unsigned char)(altura >> 16);
    cabecalho[25] = (unsigned char)(altura >> 24);

    // Abre o arquivo para escrita binária
    FILE* arquivo = fopen(nomeArquivo, "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    // Escreve o cabeçalho
    fwrite(cabecalho, sizeof(unsigned char), 54, arquivo);

    // Escreve os dados dos pixels (de baixo para cima, como exige o formato BMP)
    for (int i = altura - 1; i >= 0; i--) { // BMP armazena de baixo para cima
        for (int j = 0; j < largura; j++) {
            unsigned char cor = dados[i][j]; // Tonalidade de cinza
            unsigned char pixel[3] = {cor, cor, cor}; // RGB igual para tons de cinza
            fwrite(pixel, sizeof(unsigned char), 3, arquivo);
        }
    }

    fclose(arquivo);
}



unsigned char* novaLinha(unsigned char* linhaInicial, int tamanho, int variacao) {
    unsigned char* linhaNova = (unsigned char*) malloc(tamanho * sizeof(unsigned char));
    int comeco = 0, i = 0;

    // Cria uma nova linha seguindo o padrão da primeira
    while (comeco < 2) {
        for (i = comeco; i < tamanho; i += 2) {
            int valor;

            if (comeco == 0) {
                valor = linhaInicial[i] + (-variacao) + rand() % (variacao - (-variacao) + 1); // Número aleatório de x a y
            } else {
                valor = ((linhaInicial[i] + linhaNova[i - 1] + linhaNova[i + 1]) / 3) + (-variacao) + rand() % (variacao - (-variacao) + 1); // Média dos arrays ao redor + número aleatório
            }

            // Limita o valor ao intervalo de 0 a 255, pra não dar ruim no unsigned char
            valor = (int)fmax(0, fmin(255, valor));
            linhaNova[i] = (unsigned char)valor;
        }
        comeco++;
    }

    return linhaNova;
}


void salvarLinha(unsigned char* linha, int tamanho_da_linha) {
    FILE* arquivo = fopen("linhas.bin", "ab");
    for(int i = 0; i < tamanho_da_linha; i++){
        fwrite(&linha[i], sizeof(unsigned char), 1, arquivo);
    }

    fclose(arquivo);
}


unsigned char getValue(int x, int y, int tamanho_linha){
    FILE* arquivo = fopen("linhas.bin", "rb");
    unsigned char valor;
    
    // Caso não ache o arquivo
    if(arquivo == NULL){ 
        printf("Falha ao abrir o arquivo de linhas");
        return -1;
    }

    fseek(arquivo, sizeof(unsigned char)*((x) + (y * tamanho_linha)), SEEK_SET);
    fread(&valor, sizeof(unsigned char), 1, arquivo);
    fclose(arquivo);

    return valor;
}


void dobraTamanhoArquivo(int n) {
    FILE* arquivo = fopen("linhas.bin", "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo linhas.bin para leitura");
        exit(EXIT_FAILURE);
    }

    // Alocação dinâmica para a matriz 2x maior
    unsigned char** arrayBidimensional = (unsigned char**) malloc(sizeof(unsigned char*) * (n * 2));
    if (!arrayBidimensional) {
        perror("Erro ao alocar memória para o array bidimensional");
        fclose(arquivo);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n * 2; i++) {
        arrayBidimensional[i] = (unsigned char*) malloc(sizeof(unsigned char) * (n * 2));
        if (!arrayBidimensional[i]) {
            perror("Erro ao alocar memória para uma linha do array");
            for (int j = 0; j < i; j++) free(arrayBidimensional[j]);
            free(arrayBidimensional);
            fclose(arquivo);
            exit(EXIT_FAILURE);
        }
    }

    // Inicialização do array com valores do arquivo
    int x = 0, y = 0;
    for (int j = 0; j < n; j++) {
        for (int k = 0; k < n; k++) {
            fread(&arrayBidimensional[y][x], sizeof(unsigned char), 1, arquivo);
            x += 2; // Dobra o espaço horizontal
        }
        y += 2; // Dobra o espaço vertical
        x = 0;
    }

    fclose(arquivo);

    // Preenchendo valores intermediários horizontalmente
    for (y = 0; y < n * 2; y += 2) {
        for (x = 1; x < n * 2 - 1; x += 2) {
            arrayBidimensional[y][x] = (arrayBidimensional[y][x - 1] + arrayBidimensional[y][x + 1]) / 2;
        }
    }

    // Preenchendo valores intermediários verticalmente
    for (y = 1; y < n * 2 - 1; y += 2) {
        for (x = 0; x < n * 2; x += 1) {
            arrayBidimensional[y][x] = (arrayBidimensional[y - 1][x] + arrayBidimensional[y + 1][x]) / 2;
        }
    }

    // Preenchendo valores nos cruzamentos
    for (y = 1; y < n * 2 - 1; y += 2) {
        for (x = 1; x < n * 2 - 1; x += 2) {
            arrayBidimensional[y][x] = (arrayBidimensional[y - 1][x] + arrayBidimensional[y + 1][x] +
                                        arrayBidimensional[y][x - 1] + arrayBidimensional[y][x + 1]) / 4;
        }
    }

    // Salvando o novo array expandido no arquivo
    FILE* arquivo_01 = fopen("linhas.bin", "wb");
    if (!arquivo_01) {
        perror("Erro ao abrir o arquivo linhas.bin para escrita");
        for (int i = 0; i < n * 2; i++) free(arrayBidimensional[i]);
        free(arrayBidimensional);
        exit(EXIT_FAILURE);
    }

    for (y = 0; y < n * 2; y++) {
        for (x = 0; x < n * 2; x++) {
            fwrite(&arrayBidimensional[y][x], sizeof(unsigned char), 1, arquivo_01);
        }
    }

    fclose(arquivo_01);

    // Liberando memória
    for (int i = 0; i < n * 2; i++) {
        free(arrayBidimensional[i]);
    }
    free(arrayBidimensional);

    
}



int main() {

    printf("\nCriando arquivo inicial\n");
    int n = 100, delta = 140, comeco = 0, i, num_linhas = 2;

    // Abre o arquivo texto para salvar as linhas
    FILE *arquivo_00 = fopen("linhas.bin", "w");
    fclose(arquivo_00);
    FILE *arquivo_01 = fopen("blocos.bin", "w");
    fclose(arquivo_01);

    // Array utilizado como primeira linha
    unsigned char* firstArrayLine = (unsigned char*) malloc(n * sizeof(unsigned char));

    // Código que armazena bytes na arrayLine em proporção com a função seno
    for (i = 0; i < n; i++) {
        firstArrayLine[i] = 127 * (sin(((6 * PI) / n) * i)) + 127;
    }


    // Cria e salva novas linhas seguindo o padrão da primeira
    // esta salvando a partir da linha n, pra sair do padrão do seno
    unsigned char* secondArrayLine = novaLinha(firstArrayLine, n, delta);

    while (num_linhas < n*2) {
        firstArrayLine = novaLinha(secondArrayLine, n, delta);
        if (num_linhas>n){
            salvarLinha(firstArrayLine, n);
        }

        secondArrayLine = firstArrayLine;

        num_linhas++;
    }

    // Libera memória alocada
    free(firstArrayLine);
    free(secondArrayLine);


    printf("Redimensionando os arquivos\n");

    dobraTamanhoArquivo(n);
    dobraTamanhoArquivo(n*2);
    dobraTamanhoArquivo(n*4);
    dobraTamanhoArquivo(n*8);
    dobraTamanhoArquivo(n*16);

    printf("Arquivos redimensionados com sucesso!");


    //GERANDO A IMAGEM
    
    int largura = 3200, altura = 3200;

    // Aloca matriz para os dados da imagem
    unsigned char** dados = (unsigned char**)malloc(altura * sizeof(unsigned char*));
    for (int i = 0; i < altura; i++) {
        dados[i] = (unsigned char*)malloc(largura * sizeof(unsigned char));
    }

    FILE* arquivoDados = fopen("linhas.bin", "rb");

    // Preenche a matriz com valores de tons de cinza
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            fread(&dados[i][j], sizeof(unsigned char), 1, arquivoDados);
        }
    }

    fclose(arquivoDados);

    // Gera o arquivo BMP
    gerarBitmap("imagem.bmp", dados, largura, altura);

    // Libera memória
    for (int i = 0; i < altura; i++) {
        free(dados[i]);
    }
    free(dados);

    printf("Imagem BMP gerada com sucesso!\n");


    return 0;
}
