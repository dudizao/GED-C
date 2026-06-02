#ifndef DOCUMENTO_H
#define DOCUMENTO_H

/* -------------------------------------------------------
   Constantes
------------------------------------------------------- */

#define MAX_DOCUMENTOS   100
#define MAX_HISTORICO    20   /* entradas de historico por documento */

/* -------------------------------------------------------
   Structs
------------------------------------------------------- */

/* Registro de uma alteracao de status */
typedef struct {
    char statusAnterior[30];
    char statusNovo[30];
    char dataHora[20];        /* DD/MM/AAAA HH:MM */
} EntradaHistorico;

typedef struct {
    int              codigo;
    char             titulo[80];
    char             tipo[40];
    char             responsavel[60];
    char             data[20];
    char             status[30];
    EntradaHistorico historico[MAX_HISTORICO];
    int              totalHistorico;
} Documento;

/* -------------------------------------------------------
   Variaveis globais (definidas em documento.c)
------------------------------------------------------- */

extern Documento documentos[MAX_DOCUMENTOS];
extern int       totalDocumentos;

/* -------------------------------------------------------
   Prototipos — utilitarios (documento.c)
------------------------------------------------------- */

int  lerInteiro(int *destino);
int  validarData(const char *data);
void strParaMinusculo(const char *entrada, char *saida, int tam);
int  contemTexto(const char *texto, const char *busca);
void exibirDocumento(int i);
void registrarHistorico(int i, const char *anterior, const char *novo);
void exibirHistorico(int i);

/* -------------------------------------------------------
   Prototipos — operacoes CRUD (documento.c)
------------------------------------------------------- */

void cadastrarDocumento(void);
void listarDocumentos(void);
void buscarDocumento(void);
void editarDocumento(void);
void alterarStatus(void);
void excluirDocumento(void);
void relatorioResumo(void);
void ordenarDocumentos(void);

/* -------------------------------------------------------
   Prototipos — persistencia (arquivo.c)
------------------------------------------------------- */

void salvarDocumentos(void);
void carregarDocumentos(void);

#endif /* DOCUMENTO_H */
