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

// palavra fica com caracteres minusculos
void toLower(char str[]){
    int i;

    for(i=0; str[i]!='\0'; i++){
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
    struct no{
        char *palavra;
        int offset;
        int quantidade; // quantidade de vezes que achou a mesma palavra no texto

        struct no *prox;
    }typedef No;

    // Ponteiro para o primeiro nó da lista de índices secundários
    struct lista{
        struct no *head;
    } typedef Lista;

    // Registros que ficarão salvos no disco
    struct primary_key {
        int offset;
        int prox_offset;
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
            printf("Offset: %d\n", aux->offset);
            printf("Quantidade: %d\n",aux->quantidade);
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
        return lista;
    }

    // Insere um indice secundário na lista de indices secundarios
    No* insere_indice_secundario(char *palavra, int offset, int *contador){
        int resultado;
        
        *contador = 0;  // marcar a posição da palavra

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

                    // palavra já existe na lista de indices secundários
                    if(resultado == 0){
                        aux->quantidade++;
                        free(no); // Esse nó não é inserido na lista
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

                (*contador)++;
            }

            // Palavra ainda não existe na lista de indices secundarios
            // Armazena informações (palavra e offset) no novo nó
            no->palavra = (char *) malloc(strlen(palavra) * sizeof(char) + 1);
            strcpy(no->palavra, palavra);
            no->offset = offset;
            no->quantidade = 1;
            
            return no;
        }
    }

    void insere_lista_invertida(struct primary_key registro, int posicao){
        //rewind(fd);

        fseek(fd,0,SEEK_END);

        printf("Proximos offsets: %d\n",registro.prox_offset);
        
        // insere no final do arquivo
        if(registro.prox_offset == -1){
            fwrite(&registro,sizeof(primary_key),1,fd);
        }
    }

    // adiciona palavra na estrutura
    void adiciona(char *palavra, int offset) {
        // Adiciona palavra na lista de índices secundários
        No* resultado;
        int posicao; // posição da palavra na lista de índices secundários
        resultado = insere_indice_secundario(palavra, offset, &posicao);

        // primeira vez que a palavra aparece no texto
        if(resultado->quantidade == 1){
            primary_key.offset = resultado->offset;
            primary_key.prox_offset = -1;
        }
        else {
            primary_key.offset = offset;
            primary_key.prox_offset = resultado->offset;
        }

        // Adiciona offset da palavra na lista invertida
        insere_lista_invertida(primary_key,posicao);
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

