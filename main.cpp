/*
Grupo:
Danilo Augusto Nunes - 11611BCC021
Matheus Henrique Ferreira Protásio - 11521BCC020
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>

using namespace std;

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
    // Nó que representa um índice secundário
    struct no {
        char *palavra;
        int posicao; // posicao da palavra na lista invertida

        struct no *prox;
    } typedef No;

    // Ponteiro para o primeiro nó da lista de índices secundários
    struct lista {
        struct no *head;
        int indice; // controla a posição que a palavra deve ser inserida na lista invertida
    } typedef Lista;

    // Registros da lista invertida que ficarão salvos no disco
    struct primary_key {
        int primeira_pos; // primeira posição da palavra
        int offset; // offset da palavra
        int pos_prox_offset; // posição do próximo offset da palavra
    } primary_key;

    // construtor
    listaInvertida() {
        fd = fopen("lista_invertida.dat", "wb+");
        indices_secundarios = cria_indices_secundarios();
    }
    
    // destrutor
    ~listaInvertida() {
        fclose(fd);
    }

    // TUDO AQUI É DO TAD DE INDICES SECUNDARIOS
    // SALVO NA MEMORIA, DEPOIS TENTAR COLOCAR ISSO EM UM ARQUIVO .H SEPARADO
    // PRA MELHOR ENTENDIMENTO DAS FUNÇÕES

    void imprime_lista_indices_secundarios(){
        No *aux;

        if(indices_secundarios->head == NULL)
            printf("Lista vazia\n");

        printf("\n");
        
        for(aux=indices_secundarios->head; aux!=NULL; aux=aux->prox){
            printf("Palavra: %s\n", aux->palavra);
            printf("\n");
        }
    } 

    // Cria lista de índices secundários (armazena na memória)
    Lista* cria_indices_secundarios(){
        Lista* lista = (Lista*) malloc(sizeof(Lista));

        if(lista == NULL){
            printf("Erro ao criar a lista de indices secundarios.\n");
            return NULL;
        }

        lista->head = NULL;
        lista->indice = 0;

        return lista;
    }

    // Insere um indice secundário na lista de indices secundarios
    No* insere_indice_secundario(char *palavra){
        int resultado;

        // Verifica se a lista de indices secundários existe
        if(indices_secundarios == NULL){
            printf("Erro! A lista de indices secundarios nao existe.\n");
            return NULL;
        }
        else {
            // Cria novo nó da lista
            No *no = (No*) malloc(sizeof(No));

            // Não há elementos na lista
            if(indices_secundarios->head == NULL){
                indices_secundarios->head = no;
                no->prox = NULL;
            } 
            else { // Já existem elementos na lista
                No *aux, *ant;
                aux = indices_secundarios->head;

                // Encontra a posicao correta para inserir a palavra
                while(aux != NULL){
                    resultado = strcmp(palavra,aux->palavra);
                    
                    //palavra que vai ser inserida é menor (ordem alfabetica)
                    if(resultado < 0){
                        break;
                    }

                    // palavra já existe na lista de índices secundários
                    if(resultado == 0){
                        free(no); // Esse nó não será inserido na lista
                        return aux;
                    }

                    ant = aux; 
                    aux = aux->prox; 
                }

                // insere na primeira posicao
                if(aux == indices_secundarios->head){
                    no->prox = aux;
                    indices_secundarios->head = no;
                }
                else {
                    no->prox = aux;
                    ant->prox = no;
                }
            }

            // Palavra ainda não existe na lista de indices secundarios
            // Armazena informações (palavra e offset) no novo nó
            no->palavra = (char *) malloc(strlen(palavra) * sizeof(char) + 1);
            strcpy(no->palavra, palavra);
            no->posicao = indices_secundarios->indice;
            indices_secundarios->indice++;
            
            return no;
        }
    }

    void insere_lista_invertida(struct primary_key registro){
        // Posicao da palavra no ARQUIVO da lista invertida
        int pos_lista;
        pos_lista = registro.primeira_pos * sizeof(primary_key);

        fseek(fd,pos_lista,SEEK_SET); // Move o ponteiro do arquivo para a posição
        fwrite(&registro,sizeof(primary_key),1,fd); // Escreve o registro no arquivo
    }

    // adiciona palavra na estrutura
    void adiciona(char *palavra, int offset) {
        int indice_anterior;
        indice_anterior = indices_secundarios->indice;

        // Adiciona palavra na lista de índices secundários
        No* resultado;
        resultado = insere_indice_secundario(palavra);

        // A palavra JÁ existia na lista de índices secundários
        if(indice_anterior == indices_secundarios->indice){
            primary_key.primeira_pos = indices_secundarios->indice;
            primary_key.offset = offset;
            primary_key.pos_prox_offset = resultado->posicao; 

            resultado->posicao = indices_secundarios->indice;
            indices_secundarios->indice++; // atualiza o indice da lista
        }
        else { // Palavra ainda NÃO estava na lista de índices secundários
            primary_key.primeira_pos = resultado->posicao;
            primary_key.offset = offset;
            primary_key.pos_prox_offset = -1; // a última posição sempre aponta para -1
        }

        // Adiciona offset da palavra na lista invertida
        insere_lista_invertida(primary_key);
    }

    // realiza busca, retornando vetor de offsets que referenciam a palavra
    int * busca(char *palavra, int *quantidade) {
        // substituir pelo resultado da busca na lista invertida
        quantidade[0] = 10;
        int *offsets = new int[10];
        int i = 0;
        // exemplo: retornar os primeiros 10 offsets da palavra "terra"
        offsets[i++] = 58;
        offsets[i++] = 69;
        offsets[i++] = 846;
        offsets[i++] = 943;
        offsets[i++] = 1083;
        offsets[i++] = 1109;
        offsets[i++] = 1569;
        offsets[i++] = 1792;
        offsets[i++] = 2041;
        offsets[i++] = 2431;
        return offsets;
    }
private:
    // descritor do arquivo é privado, apenas métodos da classe podem acessa-lo
    FILE *fd;

    // ponteiro para Lista de índices secundários
    Lista* indices_secundarios;
};

// programa principal
int main(int argc, char** argv) {
    // abrir arquivo
    ifstream in("biblia_teste.txt", ios::binary);
    if (!in.is_open()){
        printf("\n\n Nao consegui abrir arquivo biblia.txt. Sinto muito.\n\n\n\n");
    }
    else{
        // vamos ler o arquivo e criar a lista invertida com as palavras do arquivo
        char *palavra = new char[100];
        int offset, contadorDePalavras = 0;

        int teste;

        listaInvertida *lista = new listaInvertida();
        // ler palavras
        while(true){
            // ler palavra
            in >> palavra;
            
            // Modificamos para não ler a última palavra duas vezes
            if(in.eof()){
                break;
            }
            
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
        lista->imprime_lista_indices_secundarios();
        
        // agora que ja construimos o indice, podemos realizar buscas
        do {
            printf("\nDigite a palavra desejada ou \"SAIR\" para sair: ");
            scanf("%s",palavra);
            //toLower(palavra);   PENSAR SE PRECISA COLOCAR ESSA FUNCAO AQUI MESMO
            if (strcmp(palavra,"SAIR") != 0) {
                int quantidade;
                // busca na lista invertida
                int *offsets = lista->busca(palavra,&quantidade);
                // com vetor de offsets, recuperar as linhas que contem a palavra desejada
                if (quantidade > 0) {
                    FILE *f = fopen("biblia.txt","rt");
                    for (int i = 0; i < quantidade; i++)
                        imprimeLinha(offsets[i],f);
                    fclose(f);
                }
                else
                    printf("nao encontrou %s\n",palavra);
            }
        } while (strcmp(palavra,"SAIR") != 0);

        printf("\n\nAte mais!\n\n");
    }

    return (EXIT_SUCCESS);
}

