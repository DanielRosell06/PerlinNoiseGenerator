#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PI 3.1415

/*  CÓDIGO DE ALEATORIEDADE DE SEED

    #include <stdlib.h>

    
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


            /* Codigo que deixa azul e verde, e cria um efeito de sombreamento  

            if (cor > 127){
                unsigned char pixel[3] = {0, 255-cor, 0};
                fwrite(pixel, sizeof(unsigned char), 3, arquivo);
            }else{
                unsigned char pixel[3] = {cor, 0, 0};
                fwrite(pixel, sizeof(unsigned char), 3, arquivo);
            }
            */

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


void aplicaDesfoque(const char* nomeArquivo, int n, int tamanhoJanela) {
    // Verifica se o tamanho da janela é ímpar
    if (tamanhoJanela % 2 == 0) {
        printf("Erro: O tamanho da janela deve ser ímpar.\n");
        exit(1);
    }

    int offset = tamanhoJanela / 2; // Offset com base no tamanho da janela

    // Abre o arquivo binário para leitura
    FILE* arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    // Aloca matriz para os dados da imagem
    unsigned char** imagem = (unsigned char**)malloc(n * sizeof(unsigned char*));
    for (int i = 0; i < n; i++) {
        imagem[i] = (unsigned char*)malloc(n * sizeof(unsigned char));
    }

    // Lê os dados do arquivo para a matriz
    for (int i = 0; i < n; i++) {
        fread(imagem[i], sizeof(unsigned char), n, arquivo);
    }
    fclose(arquivo);

    // Cria uma matriz para armazenar o resultado do desfoque
    unsigned char** imagemBlur = (unsigned char**)malloc(n * sizeof(unsigned char*));
    for (int i = 0; i < n; i++) {
        imagemBlur[i] = (unsigned char*)malloc(n * sizeof(unsigned char));
    }

    // Aplica o filtro de desfoque (desconsidera as bordas)
    for (int y = offset; y < n - offset; y++) {
        for (int x = offset; x < n - offset; x++) {
            int soma = 0, contador = 0;

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    soma += imagem[y + ky][x + kx];
                    contador++;
                }
            }

            imagemBlur[y][x] = soma / contador;
        }
    }

    // Copia as bordas sem modificação
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < offset; y++) {
            imagemBlur[y][x] = imagem[y][x];
            imagemBlur[n - 1 - y][x] = imagem[n - 1 - y][x];
        }
    }
    for (int y = offset; y < n - offset; y++) {
        for (int x = 0; x < offset; x++) {
            imagemBlur[y][x] = imagem[y][x];
            imagemBlur[y][n - 1 - x] = imagem[y][n - 1 - x];
        }
    }

    // Reescreve o arquivo com a imagem desfocada
    arquivo = fopen(nomeArquivo, "wb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo para escrita");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        fwrite(imagemBlur[i], sizeof(unsigned char), n, arquivo);
    }
    fclose(arquivo);

    // Libera a memória alocada
    for (int i = 0; i < n; i++) {
        free(imagem[i]);
        free(imagemBlur[i]);
    }
    free(imagem);
    free(imagemBlur);
}




int main() {

    printf("\nCriando arquivo inicial...");

    // Variavies de começo
    int n = 125, delta = 140, comeco = 0, i, num_linhas = 2;
    // Variaveis de Redimensionar
    int redimensoes = 3; //tamanho do arquivo = n * 2^redimensoes
    // Variavies de Desfoque
    int repeticaoDesfoque = 3, janelaDesfoque = 7/*ímpar*/;

    // Variaveis gerais
    int tamanhoImagem = n * pow(2, redimensoes);

    // Randomizando a Seed
    srand(time(NULL));

    // Abre o arquivo texto para salvar as linhas
    FILE *arquivo_00 = fopen("linhas.bin", "w");
    fclose(arquivo_00);
    

    // Array utilizado como primeira linha
    unsigned char* firstArrayLine = (unsigned char*) malloc(n * sizeof(unsigned char));

    // Código que armazena bytes na arrayLine em proporção com a função seno
    for (i = 0; i < n; i++) {
        firstArrayLine[i] = 127 * (sin(((6 * PI) / n) * i)) + 127;
    }

    printf("\nCriando novas linhas...\n");

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



    
    // APLICANDO DESFOQUE
    printf("Aplicando Desfoque...\n");

    for(i = 0; i < repeticaoDesfoque; i++){
        aplicaDesfoque("linhas.bin", n, janelaDesfoque);
    }


    // REDIMENSIONANDO OS ARQUIVOS
    printf("Redimensionando os arquivos...\n");

    for(i = 0; i < redimensoes; i++){
        dobraTamanhoArquivo(n * pow(2, i));
        aplicaDesfoque("linhas.bin", n * pow(2, i+1), 5);
    }

    //Fazendo um pequeno desfoque pra tirar o problema de granulamento



    //GERANDO A IMAGEM
    printf("Gerando a imagem...\n");   
    
    int largura = tamanhoImagem, altura = tamanhoImagem;

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
