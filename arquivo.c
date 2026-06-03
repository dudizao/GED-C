#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "documento.h"

#define ARQUIVO_DADOS "documentos_i9mtg.txt"
#define MAX_LINHA 512
#define MAX_CAMPOS 20

/*
   Formato atual salvo no arquivo:
   codigo;nomeFuncionario;matriculaFuncionario;setor;cargo;titulo;tipo;responsavelCadastro;dataEmissao;dataValidade;status;totalHistorico
   Depois de cada documento, o arquivo guarda totalHistorico linhas:
   statusAnterior;statusNovo;dataHora

   O carregamento tambem aceita o formato antigo:
   codigo;titulo;tipo;responsavel;data;status
   codigo;titulo;tipo;responsavel;data;status;totalHistorico
*/

static int dividirCampos(char *linha, char *campos[], int maxCampos) {
    int total = 0;
    char *token;

    limparQuebraLinha(linha);

    token = strtok(linha, ";");
    while (token != NULL && total < maxCampos) {
        campos[total++] = token;
        token = strtok(NULL, ";");
    }

    return total;
}

static void copiarCampo(char *destino, int tamanho, const char *origem) {
    if (origem == NULL) origem = "";
    strncpy(destino, origem, (size_t)tamanho - 1);
    destino[tamanho - 1] = '\0';
}

void salvarDocumentos(void) {
    FILE *arquivo;
    int i, k;

    arquivo = fopen(ARQUIVO_DADOS, "w");
    if (arquivo == NULL) {
        printf("\nErro ao abrir arquivo para escrita.\n");
        return;
    }

    for (i = 0; i < totalDocumentos; i++) {
        fprintf(arquivo, "%d;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%d\n",
                documentos[i].codigo,
                documentos[i].nomeFuncionario,
                documentos[i].matriculaFuncionario,
                documentos[i].setor,
                documentos[i].cargo,
                documentos[i].titulo,
                documentos[i].tipo,
                documentos[i].responsavelCadastro,
                documentos[i].dataEmissao,
                documentos[i].dataValidade,
                documentos[i].status,
                documentos[i].totalHistorico);

        for (k = 0; k < documentos[i].totalHistorico; k++) {
            fprintf(arquivo, "%s;%s;%s\n",
                    documentos[i].historico[k].statusAnterior,
                    documentos[i].historico[k].statusNovo,
                    documentos[i].historico[k].dataHora);
        }
    }

    fclose(arquivo);
    printf("\nDocumentos salvos com sucesso!\n");
}

void carregarDocumentos(void) {
    FILE *arquivo;
    char linha[MAX_LINHA];
    char *campos[MAX_CAMPOS];
    int totalCampos;
    int historicosDeclarados;
    int k;

    arquivo = fopen(ARQUIVO_DADOS, "r");
    if (arquivo == NULL) {
        return;
    }

    totalDocumentos = 0;

    while (totalDocumentos < MAX_DOCUMENTOS && fgets(linha, sizeof(linha), arquivo) != NULL) {
        Documento *d;

        if (linha[0] == '\n' || linha[0] == '\0') {
            continue;
        }

        totalCampos = dividirCampos(linha, campos, MAX_CAMPOS);
        if (totalCampos < 6) {
            continue;
        }

        d = &documentos[totalDocumentos];
        memset(d, 0, sizeof(Documento));

        historicosDeclarados = 0;

        if (totalCampos >= 12) {
            /* Formato novo */
            d->codigo = atoi(campos[0]);
            copiarCampo(d->nomeFuncionario, TAM_NOME, campos[1]);
            copiarCampo(d->matriculaFuncionario, TAM_MATRICULA, campos[2]);
            copiarCampo(d->setor, TAM_SETOR, campos[3]);
            copiarCampo(d->cargo, TAM_CARGO, campos[4]);
            copiarCampo(d->titulo, TAM_TITULO, campos[5]);
            copiarCampo(d->tipo, TAM_TIPO, campos[6]);
            copiarCampo(d->responsavelCadastro, TAM_RESP, campos[7]);
            copiarCampo(d->dataEmissao, TAM_DATA, campos[8]);
            copiarCampo(d->dataValidade, TAM_DATA, campos[9]);
            copiarCampo(d->status, TAM_STATUS, campos[10]);
            historicosDeclarados = atoi(campos[11]);
        } else {
            /* Formato antigo: codigo;titulo;tipo;responsavel;data;status(;totalHistorico) */
            d->codigo = atoi(campos[0]);
            copiarCampo(d->nomeFuncionario, TAM_NOME, "Nao informado");
            copiarCampo(d->matriculaFuncionario, TAM_MATRICULA, "Nao informado");
            copiarCampo(d->setor, TAM_SETOR, "Nao informado");
            copiarCampo(d->cargo, TAM_CARGO, "Nao informado");
            copiarCampo(d->titulo, TAM_TITULO, campos[1]);
            copiarCampo(d->tipo, TAM_TIPO, campos[2]);
            copiarCampo(d->responsavelCadastro, TAM_RESP, campos[3]);
            copiarCampo(d->dataEmissao, TAM_DATA, campos[4]);
            copiarCampo(d->dataValidade, TAM_DATA, "");
            copiarCampo(d->status, TAM_STATUS, campos[5]);
            if (totalCampos >= 7) {
                historicosDeclarados = atoi(campos[6]);
            }
        }

        if (historicosDeclarados < 0) historicosDeclarados = 0;
        if (historicosDeclarados > MAX_HISTORICO) historicosDeclarados = MAX_HISTORICO;

        d->totalHistorico = 0;

        for (k = 0; k < historicosDeclarados; k++) {
            char linhaHistorico[MAX_LINHA];
            char *camposHistorico[3];
            int totalHistoricoCampos;

            if (fgets(linhaHistorico, sizeof(linhaHistorico), arquivo) == NULL) {
                break;
            }

            totalHistoricoCampos = dividirCampos(linhaHistorico, camposHistorico, 3);
            if (totalHistoricoCampos == 3) {
                copiarCampo(d->historico[d->totalHistorico].statusAnterior, TAM_STATUS, camposHistorico[0]);
                copiarCampo(d->historico[d->totalHistorico].statusNovo, TAM_STATUS, camposHistorico[1]);
                copiarCampo(d->historico[d->totalHistorico].dataHora, TAM_DATAHORA, camposHistorico[2]);
                d->totalHistorico++;
            }
        }

        totalDocumentos++;
    }

    fclose(arquivo);
}
