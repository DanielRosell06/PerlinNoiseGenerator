#include <stdio.h>
#include <stdlib.h>

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

int main() {
    int largura = 1600, altura = 1600;

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
