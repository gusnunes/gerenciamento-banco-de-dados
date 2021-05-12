#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "indices_secundarios.h"

// Nó que representa um índice secundário
    struct no{
        char *palavra;
        int offset;

        struct no *prox;
    }typedef No;

// Ponteiro para o primeiro nó da lista de índices secundários
struct lista{
    struct no *head;
    int quantidade;
} typedef Lista;


// Cria lista de índices secundários (armazena na memória)
    Lista* cria_indices_secundarios(){
        Lista* lista = (Lista*) malloc(sizeof(Lista));

        if(lista == NULL){
            printf("Erro ao criar a lista de indices secundarios.\n");
            return NULL;
        }

        lista->head = NULL;
        lista->quantidade = 0;

        return lista;
    }

     void imprime_lista_indices_secundarios(Lista *indices_secundarios){
        No *aux;

        if(indices_secundarios->head == NULL)
            printf("Lista vazia\n");

        printf("\n");
        
        for(aux=indices_secundarios->head; aux!=NULL; aux=aux->prox){
            printf("Palavra: %s\n", aux->palavra); 
            printf("Offset: %d\n", aux->offset);
        }
    }

    // Insere um indice secundário na lista de indices secundarios
    int insere_indice_secundario(Lista *indices_secundarios, char *palavra, int offset){
        int resultado;

        // Verifica se a lista existe
        if(indices_secundarios == NULL){
            printf("Erro! A lista de indices secundarios nao existe.\n");
            return 0;
        }
        else {
            // Cria novo nó da lista
            No *no = (No*) malloc(sizeof(No));

            // Não há elementos na lista
            if(indices_secundarios->head == NULL){
                indices_secundarios->head = no;
                no->prox = NULL;

                // Armazena informações (palavra e offset) no nó
                no->palavra = (char *) malloc(strlen(palavra) * sizeof(char) + 1);
                strcpy(no->palavra, palavra);
                no->offset = offset;

                return 1; // Sucesso
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

                    // só um teste, para palavras repetidas
                    if(resultado == 0){
                        return 5;
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

                no->palavra = (char *) malloc(strlen(palavra) * sizeof(char) + 1);
                strcpy(no->palavra, palavra);
                no->offset = offset;
                return 1;
            }
        }
    }