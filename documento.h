#ifndef DOCUMENTO_H
#define DOCUMENTO_H

#define MAX_DOCUMENTOS 100
#define MAX_HISTORICO  20

#define TAM_TITULO      80
#define TAM_TIPO        40
#define TAM_NOME        80
#define TAM_MATRICULA   30
#define TAM_SETOR       50
#define TAM_CARGO       50
#define TAM_RESP        60
#define TAM_DATA        12
#define TAM_STATUS      30
#define TAM_DATAHORA    20

/*
   Historico de alteracao do status do processo documental.
   Exemplo: Pendente -> Aprovado.
*/
typedef struct {
    char statusAnterior[TAM_STATUS];
    char statusNovo[TAM_STATUS];
    char dataHora[TAM_DATAHORA];
} EntradaHistorico;

/*
   Documento/certificado vinculado a um funcionario.
   Regra de negocio principal:
   cada documento pertence a um funcionario e pode possuir validade.
*/
typedef struct {
    int codigo;

    char nomeFuncionario[TAM_NOME];
    char matriculaFuncionario[TAM_MATRICULA];
    char setor[TAM_SETOR];
    char cargo[TAM_CARGO];

    char titulo[TAM_TITULO];
    char tipo[TAM_TIPO];
    char responsavelCadastro[TAM_RESP];

    char dataEmissao[TAM_DATA];
    char dataValidade[TAM_DATA]; /* vazio = documento sem validade */

    char status[TAM_STATUS];     /* Pendente, Em Analise, Aprovado, Reprovado, Pendente de Renovacao */

    EntradaHistorico historico[MAX_HISTORICO];
    int totalHistorico;
} Documento;

extern Documento documentos[MAX_DOCUMENTOS];
extern int totalDocumentos;

/* Utilitarios */
int  lerInteiro(int *destino);
int  validarData(const char *data);
int  dataEhVazia(const char *data);
int  buscarIndicePorCodigo(int codigo);
void strParaMinusculo(const char *entrada, char *saida, int tam);
int  contemTexto(const char *texto, const char *busca);
void limparQuebraLinha(char *texto);
void removerPontoEVirgula(char *texto);
void lerCampoObrigatorio(const char *rotulo, char *destino, int tamanho);
void lerCampoOpcional(const char *rotulo, char *destino, int tamanho);
void lerDataObrigatoria(const char *rotulo, char *destino, int tamanho);
void lerDataOpcional(const char *rotulo, char *destino, int tamanho);
void exibirDocumento(int i);
void registrarHistorico(int i, const char *anterior, const char *novo);
void exibirHistorico(int i);
const char *situacaoValidade(const Documento *doc);

/* Operacoes principais */
void cadastrarDocumento(void);
void listarDocumentos(void);
void buscarDocumento(void);
void editarDocumento(void);
void alterarStatus(void);
void excluirDocumento(void);
void relatorioResumo(void);
void relatorioVencimentos(void);
void ordenarDocumentos(void);

/* Persistencia */
void salvarDocumentos(void);
void carregarDocumentos(void);

#endif
