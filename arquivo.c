#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "documento.h"

/* -------------------------------------------------------
   Salvar documentos
   Formato: codigo;titulo;tipo;responsavel;data;status;totalHistorico
            seguido de totalHistorico linhas:
            statusAnterior;statusNovo;dataHora
------------------------------------------------------- */

void salvarDocumentos(void) {
    FILE *arquivo;
    int i, k;

    arquivo = fopen("documentos_i9mtg.txt", "w");

    if (arquivo == NULL) {
        printf("\nErro ao abrir arquivo para escrita.\n");
        return;
    }

    for (i = 0; i < totalDocumentos; i++) {
        fprintf(arquivo, "%d;%s;%s;%s;%s;%s;%d\n",
                documentos[i].codigo,
                documentos[i].titulo,
                documentos[i].tipo,
                documentos[i].responsavel,
                documentos[i].data,
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

/* -------------------------------------------------------
   Carregar documentos
   Le o cabecalho de cada documento e, em seguida,
   le as linhas de historico conforme totalHistorico.
------------------------------------------------------- */

void carregarDocumentos(void) {
    FILE *arquivo;
    int k, total;

    arquivo = fopen("documentos_i9mtg.txt", "r");
    if (arquivo == NULL) return;   /* arquivo ainda nao existe */

    while (totalDocumentos < MAX_DOCUMENTOS) {
        Documento *d = &documentos[totalDocumentos];

        /* Linha principal do documento */
        if (fscanf(arquivo,
                   "%d;%79[^;];%39[^;];%59[^;];%19[^;];%29[^;];%d\n",
                   &d->codigo,
                   d->titulo,
                   d->tipo,
                   d->responsavel,
                   d->data,
                   d->status,
                   &total) != 7)
            break;

        /* Limitar para evitar estouro */
        if (total > MAX_HISTORICO) total = MAX_HISTORICO;
        d->totalHistorico = 0;

        /* Linhas de historico */
        for (k = 0; k < total; k++) {
            EntradaHistorico *h = &d->historico[k];
            if (fscanf(arquivo, "%29[^;];%29[^;];%19[^\n]\n",
                       h->statusAnterior,
                       h->statusNovo,
                       h->dataHora) != 3)
                break;
            d->totalHistorico++;
        }

        totalDocumentos++;
    }

    fclose(arquivo);
}
