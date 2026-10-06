#include "indices.h"

void indices_inicializar(Indices *indices) {
    btree_inicializar(&indices->tipo);
    btree_inicializar(&indices->regiao);
    btree_inicializar(&indices->data);
}

// Coloca a ocorrencia nas tres arvores.
void indices_adicionar(Indices *indices, Ocorrencia *o) {
    btree_inserir(&indices->tipo, o->tipo, o->id);
    btree_inserir(&indices->regiao, o->regiao, o->id);
    btree_inserir(&indices->data, o->data_hora, o->id);
}

// Tira a ocorrencia das tres arvores (precisa receber os valores que estavam guardados nelas).
void indices_remover(Indices *indices, Ocorrencia *o) {
    btree_remover(&indices->tipo, o->tipo, o->id);
    btree_remover(&indices->regiao, o->regiao, o->id);
    btree_remover(&indices->data, o->data_hora, o->id);
}

void indices_liberar(Indices *indices) {
    btree_liberar(&indices->tipo);
    btree_liberar(&indices->regiao);
    btree_liberar(&indices->data);
}
