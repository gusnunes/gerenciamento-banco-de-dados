/*
Grupo:
Danilo Augusto Nunes - 11611BCC021
Matheus Henrique Ferreira Protásio - 11521BCC020
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>

#include <map> // Implementa uma árvore AVL
using namespace std;

void inverte_vetor(int *vetor, int tamanho){
    int aux, indice_limite;

    indice_limite = tamanho/2;
    for(int i=0; i<indice_limite; i++){
        aux = vetor[i];
        vetor[i] = vetor[tamanho-1];
        vetor[tamanho-1] = aux;
        
        tamanho--;
    }
}

// palavra fica com todos caracteres minusculos
void toLower(char str[]){
    for(int i=0; str[i]!='\0'; i++){
        //Verifica se é maiúscula
        if(str[i] >= 65 && str[i] <= 90){
            str[i] = str[i] + 32;
        }
    }
}

// remove pontuacao de uma palavra
void removePontuacao (char *palavra) {
    int length = strlen(palavra);
    if (
        (palavra[length-1] == '.') || (palavra[length-1] == ',') || (palavra[length-1] == ';') ||
        (palavra[length-1] == ':') || (palavra[length-1] == '?') || (palavra[length-1] == '!')
       )
        palavra[length-1] = '\0';
}

// imprime linha do arquivo com base no offset da palavra
void imprimeLinha(int offset,FILE *f) {
    // a ultima palavra do arquivo fica com um offset a mais: -1
    if(offset == -1){
        return;
    }

    int pos = ftell(f);
    char linha[2048];
    while (pos < offset) {
        fgets(linha,2047,f);
        pos = ftell(f);
    }
    printf("%s",linha);
}

// classe que implementa a lista invertida
class listaInvertida {
public:
    // Registro da lista invertida que ficará salvo no disco
    struct primary_key {
        int primeira_pos; // primeira posição da palavra
        int offset; // offset da palavra
        int pos_prox_offset; // posição do próximo offset da palavra
    } primary_key;

    // construtor
    listaInvertida() {
        fd = fopen("lista_invertida.dat", "wb+");
    }
    
    // destrutor
    ~listaInvertida() {
        fclose(fd);
    }

    // adiciona palavra na estrutura
    void adiciona(char *palavra, int offset) {
        int indice;
        
        // Posição que palavra terá na lista invertida
        indice = ftell(fd)/sizeof(primary_key);

        // Tenta inserir a palavra nos índices secundários
        std::pair<std::map<string,int>::iterator,bool> resultado;
        resultado = indices_secundarios.insert(std::make_pair(palavra,indice));
        
        // Palavra JÁ estava presente nos índices secundários
        if(resultado.second == false) {
            primary_key.primeira_pos = indice;
            primary_key.offset = offset;

            // Organiza as posições da palavra na lista invertida
            primary_key.pos_prox_offset = resultado.first->second; // A primeira posição agora será a segunda
            indices_secundarios[palavra] = indice; // Atualiza a nova primeira posição
        }
        else { // Palavra ainda NÃO estava presente nos índices secundários
            primary_key.primeira_pos = indice;
            primary_key.offset = offset;
            primary_key.pos_prox_offset = -1; // a última posição sempre aponta para -1
        }

        // Adiciona o registro no ARQUIVO da lista invertida
        // Posição é sempre no final do arquivo
        fwrite(&primary_key,sizeof(primary_key),1,fd);
    }

    // realiza busca, retornando vetor de offsets que referenciam a palavra
    int * busca(char *palavra, int *quantidade) {
        int posicao, tamanho;
        int pos_lista, contador=0;

        // Procura a palavra nos índices secundários
        std::map<string,int>::iterator resultado;
        resultado = indices_secundarios.find(palavra);

        // NÃO achou a palavra
        if(resultado == indices_secundarios.end()){
            *quantidade = 0;
            return NULL;
        }
        
        // ACHOU a palavra    
        posicao = resultado->second;
            
        // Tamanho inicial do buffer para armazenar os offsets
        // Caso precise de mais, faz realloc
        tamanho = 1000;
        int *offsets = (int *) malloc(tamanho * sizeof(int));

        // Move para a primeira posição da palavra na lista invertida
        pos_lista = posicao * sizeof(primary_key);
        fseek(fd,pos_lista,SEEK_SET);
    
        while(true){
            fread(&primary_key,sizeof(primary_key),1,fd);
            offsets[contador] = primary_key.offset;
            contador++;

            // Chegou no fim da lista invertida para aquele registro
            if(primary_key.pos_prox_offset == -1){
                break;
            }

            // Dobra o tamanho do buffer
            if(contador == tamanho-1){
                tamanho += 1000;
                offsets = (int *) realloc(offsets,tamanho*sizeof(int));
            }

            // Move para a próxima posição da palavra na lista invertida
            pos_lista = primary_key.pos_prox_offset * sizeof(primary_key);
            fseek(fd,pos_lista,SEEK_SET);
        }

        *quantidade = contador;

        inverte_vetor(offsets,contador);
        return offsets;
    }
private:
    // descritor do arquivo é privado, apenas métodos da classe podem acessa-lo
    FILE *fd;

    // Conjunto dos índices secundários
    std::map<string,int> indices_secundarios;
};

// programa principal
int main(int argc, char** argv) {
    // abrir arquivo
    ifstream in("biblia.txt", ios::binary);
    if (!in.is_open()){
        printf("\n\n Nao consegui abrir arquivo biblia.txt. Sinto muito.\n\n\n\n");
    }
    else{
        // vamos ler o arquivo e criar a lista invertida com as palavras do arquivo
        char *palavra = new char[100];
        int offset, contadorDePalavras = 0;

        listaInvertida *lista = new listaInvertida();
        // ler palavras 
        while(!in.eof()){
            // ler palavra
            in >> palavra;
            // pegar offset
            offset = in.tellg();
            // remover pontuacao
            removePontuacao(palavra);
            // desconsiderar palavras que sao marcadores do arquivo
            if (!((palavra[0] == '#') || (palavra[0] == '[') || ((palavra[0] >= '0') && (palavra[0] <= '9')))) {
                //printf("%d %s\n", offset,palavra); fflush(stdout); // debug :-)
                toLower(palavra);
                lista->adiciona(palavra, offset);
                contadorDePalavras++;
                if (contadorDePalavras % 1000 == 0) { printf(".");  fflush(stdout); }
            }
        }

        in.close();
        
        // agora que ja construimos o indice, podemos realizar buscas
        do {
            printf("\nDigite a palavra desejada ou \"SAIR\" para sair: ");
            scanf("%s",palavra);
            if (strcmp(palavra,"SAIR") != 0) {
                int quantidade;
                // busca na lista invertida
                toLower(palavra);
                int *offsets = lista->busca(palavra,&quantidade);
                // com vetor de offsets, recuperar as linhas que contem a palavra desejada
                if (quantidade > 0) {
                    printf("Quantidade: %d\n",quantidade);
                    FILE *f = fopen("biblia.txt","rt");
                    for (int i = 0; i < quantidade; i++)
                        imprimeLinha(offsets[i],f);
                    fclose(f);
                }
                else {
                    printf("nao encontrou %s\n",palavra);
                }
                
                free(offsets); // desaloca a memória que guarda os offsets
            }
        } while (strcmp(palavra,"SAIR") != 0);

        printf("\n\nAte mais!\n\n");
    }

    return (EXIT_SUCCESS);
}

