typedef struct lista Lista;

Lista* cria_indices_secundarios();
void imprime_lista_indices_secundarios(Lista *indices_secundarios);
int insere_indice_secundario(Lista *indices_secundarios, char *palavra, int offset);