#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "documento.h"

/* -------------------------------------------------------
   Variaveis globais
------------------------------------------------------- */

Documento documentos[MAX_DOCUMENTOS];
int       totalDocumentos = 0;

/* -------------------------------------------------------
   FASE 1 — Leitura segura de inteiro
   fgets + sscanf: nao deixa '\n' residual no buffer.
   Retorna 1 se leu com sucesso, 0 se entrada invalida.
------------------------------------------------------- */

int lerInteiro(int *destino) {
    char linha[32];
    if (fgets(linha, sizeof(linha), stdin) == NULL) return 0;
    linha[strcspn(linha, "\n")] = '\0';
    if (sscanf(linha, "%d", destino) != 1) return 0;
    return 1;
}

/* -------------------------------------------------------
   FASE 1 — Validacao de data DD/MM/AAAA
   Retorna 1 se valida, 0 caso contrario.
------------------------------------------------------- */

int validarData(const char *data) {
    int dia, mes, ano;
    int diasPorMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;

    if (!isdigit((unsigned char)data[0]) || !isdigit((unsigned char)data[1])) return 0;
    if (!isdigit((unsigned char)data[3]) || !isdigit((unsigned char)data[4])) return 0;
    if (!isdigit((unsigned char)data[6]) || !isdigit((unsigned char)data[7])) return 0;
    if (!isdigit((unsigned char)data[8]) || !isdigit((unsigned char)data[9])) return 0;

    dia = (data[0] - '0') * 10 + (data[1] - '0');
    mes = (data[3] - '0') * 10 + (data[4] - '0');
    ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100
        + (data[8] - '0') * 10  + (data[9] - '0');

    if (mes < 1 || mes > 12) return 0;
    if (ano < 2000 || ano > 2099) return 0;

    if (mes == 2 && ((ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0))
        diasPorMes[2] = 29;

    if (dia < 1 || dia > diasPorMes[mes]) return 0;

    return 1;
}

/* -------------------------------------------------------
   Utilitarios gerais
------------------------------------------------------- */

void strParaMinusculo(const char *entrada, char *saida, int tam) {
    int i;
    for (i = 0; i < tam - 1 && entrada[i] != '\0'; i++)
        saida[i] = (char)tolower((unsigned char)entrada[i]);
    saida[i] = '\0';
}

int contemTexto(const char *texto, const char *busca) {
    char textoMin[256], buscaMin[256];
    strParaMinusculo(texto, textoMin, sizeof(textoMin));
    strParaMinusculo(busca, buscaMin, sizeof(buscaMin));
    return strstr(textoMin, buscaMin) != NULL;
}

void exibirDocumento(int i) {
    printf("\nCodigo:      %d\n",   documentos[i].codigo);
    printf("Titulo:      %s\n",     documentos[i].titulo);
    printf("Tipo:        %s\n",     documentos[i].tipo);
    printf("Responsavel: %s\n",     documentos[i].responsavel);
    printf("Data:        %s\n",     documentos[i].data);
    printf("Status:      %s\n",     documentos[i].status);
    printf("-----------------------------\n");
}

/* -------------------------------------------------------
   FASE 3 — Historico de alteracoes de status
------------------------------------------------------- */

/* Grava data e hora atuais no formato DD/MM/AAAA HH:MM */
static void dataHoraAtual(char *buf, int tam) {
    time_t agora = time(NULL);
    struct tm *t  = localtime(&agora);
    snprintf(buf, (size_t)tam, "%02d/%02d/%04d %02d:%02d",
             t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
             t->tm_hour, t->tm_min);
}

void registrarHistorico(int i, const char *anterior, const char *novo) {
    EntradaHistorico *h;

    if (documentos[i].totalHistorico >= MAX_HISTORICO) {
        /* Deslocar para liberar espaco (descarta o mais antigo) */
        int k;
        for (k = 0; k < MAX_HISTORICO - 1; k++)
            documentos[i].historico[k] = documentos[i].historico[k + 1];
        documentos[i].totalHistorico = MAX_HISTORICO - 1;
    }

    h = &documentos[i].historico[documentos[i].totalHistorico];
    strncpy(h->statusAnterior, anterior, sizeof(h->statusAnterior) - 1);
    h->statusAnterior[sizeof(h->statusAnterior) - 1] = '\0';
    strncpy(h->statusNovo, novo, sizeof(h->statusNovo) - 1);
    h->statusNovo[sizeof(h->statusNovo) - 1] = '\0';
    dataHoraAtual(h->dataHora, sizeof(h->dataHora));

    documentos[i].totalHistorico++;
}

void exibirHistorico(int i) {
    int k;

    printf("\n=== HISTORICO DE STATUS: %s ===\n", documentos[i].titulo);

    if (documentos[i].totalHistorico == 0) {
        printf("Nenhuma alteracao registrada.\n");
        return;
    }

    for (k = 0; k < documentos[i].totalHistorico; k++) {
        printf("%s  %-12s -> %s\n",
               documentos[i].historico[k].dataHora,
               documentos[i].historico[k].statusAnterior,
               documentos[i].historico[k].statusNovo);
    }
}

/* -------------------------------------------------------
   Cadastrar documento
------------------------------------------------------- */

void cadastrarDocumento(void) {
    int i;

    if (totalDocumentos >= MAX_DOCUMENTOS) {
        printf("\nLimite de documentos atingido!\n");
        return;
    }

    Documento doc;
    memset(&doc, 0, sizeof(doc));

    printf("\n=== CADASTRAR DOCUMENTO ===\n");

    printf("Codigo do documento: ");
    while (!lerInteiro(&doc.codigo))
        printf("Entrada invalida. Digite um numero inteiro: ");

    for (i = 0; i < totalDocumentos; i++) {
        if (documentos[i].codigo == doc.codigo) {
            printf("\nErro: ja existe um documento com o codigo %d.\n", doc.codigo);
            return;
        }
    }

    printf("Titulo do documento: ");
    fgets(doc.titulo, sizeof(doc.titulo), stdin);
    doc.titulo[strcspn(doc.titulo, "\n")] = '\0';

    printf("Tipo (Ex: Contrato, Nota Fiscal, Relatorio): ");
    fgets(doc.tipo, sizeof(doc.tipo), stdin);
    doc.tipo[strcspn(doc.tipo, "\n")] = '\0';

    printf("Responsavel pelo documento: ");
    fgets(doc.responsavel, sizeof(doc.responsavel), stdin);
    doc.responsavel[strcspn(doc.responsavel, "\n")] = '\0';

    do {
        printf("Data de cadastro (DD/MM/AAAA): ");
        fgets(doc.data, sizeof(doc.data), stdin);
        doc.data[strcspn(doc.data, "\n")] = '\0';
        if (!validarData(doc.data))
            printf("Data invalida. Use DD/MM/AAAA com valores reais.\n");
    } while (!validarData(doc.data));

    strcpy(doc.status, "Pendente");
    doc.totalHistorico = 0;

    documentos[totalDocumentos] = doc;
    totalDocumentos++;

    printf("\nDocumento cadastrado com sucesso!\n");
}

/* -------------------------------------------------------
   Listar documentos
------------------------------------------------------- */

void listarDocumentos(void) {
    int i;

    printf("\n=== LISTA DE DOCUMENTOS ===\n");

    if (totalDocumentos == 0) {
        printf("Nenhum documento cadastrado.\n");
        return;
    }

    for (i = 0; i < totalDocumentos; i++)
        exibirDocumento(i);
}

/* -------------------------------------------------------
   Busca por titulo, tipo e status
------------------------------------------------------- */

static void buscarPorTitulo(void) {
    char termo[80];
    int i, encontrados = 0;

    printf("Parte do titulo: ");
    fgets(termo, sizeof(termo), stdin);
    termo[strcspn(termo, "\n")] = '\0';

    printf("\n--- Resultados para \"%s\" ---\n", termo);
    for (i = 0; i < totalDocumentos; i++) {
        if (contemTexto(documentos[i].titulo, termo)) {
            exibirDocumento(i);
            encontrados++;
        }
    }

    printf(encontrados == 0
           ? "Nenhum documento encontrado.\n"
           : "Total encontrado: %d\n", encontrados);
}

static void buscarPorTipo(void) {
    char termo[40];
    int i, encontrados = 0;

    printf("Tipo a buscar (Ex: Contrato): ");
    fgets(termo, sizeof(termo), stdin);
    termo[strcspn(termo, "\n")] = '\0';

    printf("\n--- Resultados para tipo \"%s\" ---\n", termo);
    for (i = 0; i < totalDocumentos; i++) {
        if (contemTexto(documentos[i].tipo, termo)) {
            exibirDocumento(i);
            encontrados++;
        }
    }

    printf(encontrados == 0
           ? "Nenhum documento encontrado.\n"
           : "Total encontrado: %d\n", encontrados);
}

static void buscarPorStatus(void) {
    int opcao, i, encontrados = 0;
    char statusBusca[30];

    printf("\nFiltrar por status:\n");
    printf("1 - Pendente\n2 - Em Analise\n3 - Aprovado\n4 - Reprovado\n");
    printf("Opcao: ");
    while (!lerInteiro(&opcao))
        printf("Entrada invalida: ");

    switch (opcao) {
        case 1: strcpy(statusBusca, "Pendente");   break;
        case 2: strcpy(statusBusca, "Em Analise"); break;
        case 3: strcpy(statusBusca, "Aprovado");   break;
        case 4: strcpy(statusBusca, "Reprovado");  break;
        default: printf("Opcao invalida.\n"); return;
    }

    printf("\n--- Status: %s ---\n", statusBusca);
    for (i = 0; i < totalDocumentos; i++) {
        if (strcmp(documentos[i].status, statusBusca) == 0) {
            exibirDocumento(i);
            encontrados++;
        }
    }

    printf(encontrados == 0
           ? "Nenhum documento encontrado.\n"
           : "Total encontrado: %d\n", encontrados);
}

void buscarDocumento(void) {
    int opcao;

    printf("\n=== BUSCAR DOCUMENTO ===\n");
    printf("1 - Por codigo\n2 - Por titulo\n3 - Por tipo\n4 - Por status\n");
    printf("Opcao: ");
    while (!lerInteiro(&opcao))
        printf("Entrada invalida: ");

    switch (opcao) {
        case 1: {
            int codigo, i, encontrado = 0;
            printf("Codigo: ");
            while (!lerInteiro(&codigo))
                printf("Entrada invalida: ");
            for (i = 0; i < totalDocumentos; i++) {
                if (documentos[i].codigo == codigo) {
                    printf("\nDocumento encontrado!\n");
                    exibirDocumento(i);
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) printf("\nDocumento nao encontrado.\n");
            break;
        }
        case 2: buscarPorTitulo(); break;
        case 3: buscarPorTipo();   break;
        case 4: buscarPorStatus(); break;
        default: printf("Opcao invalida.\n");
    }
}

/* -------------------------------------------------------
   Editar campos
------------------------------------------------------- */

void editarDocumento(void) {
    int codigo, i, encontrado = 0;
    char novo[80];

    printf("\n=== EDITAR DOCUMENTO ===\n");
    printf("Codigo do documento: ");
    while (!lerInteiro(&codigo))
        printf("Entrada invalida: ");

    for (i = 0; i < totalDocumentos; i++) {
        if (documentos[i].codigo == codigo) {
            encontrado = 1;

            printf("\nEditando: %s\n", documentos[i].titulo);
            printf("(Pressione Enter para manter o valor atual)\n\n");

            printf("Titulo atual: %s\nNovo titulo:  ", documentos[i].titulo);
            fgets(novo, 80, stdin);
            novo[strcspn(novo, "\n")] = '\0';
            if (strlen(novo) > 0) strcpy(documentos[i].titulo, novo);

            printf("\nTipo atual: %s\nNovo tipo:  ", documentos[i].tipo);
            fgets(novo, 40, stdin);
            novo[strcspn(novo, "\n")] = '\0';
            if (strlen(novo) > 0) strcpy(documentos[i].tipo, novo);

            printf("\nResponsavel atual: %s\nNovo responsavel:  ", documentos[i].responsavel);
            fgets(novo, 60, stdin);
            novo[strcspn(novo, "\n")] = '\0';
            if (strlen(novo) > 0) strcpy(documentos[i].responsavel, novo);

            printf("\nData atual: %s\nNova data (DD/MM/AAAA ou Enter para manter): ",
                   documentos[i].data);
            fgets(novo, 20, stdin);
            novo[strcspn(novo, "\n")] = '\0';
            if (strlen(novo) > 0) {
                while (!validarData(novo)) {
                    printf("Data invalida. Use DD/MM/AAAA: ");
                    fgets(novo, 20, stdin);
                    novo[strcspn(novo, "\n")] = '\0';
                    if (strlen(novo) == 0) break;
                }
                if (strlen(novo) > 0) strcpy(documentos[i].data, novo);
            }

            printf("\nDocumento atualizado com sucesso!\n");
            break;
        }
    }

    if (!encontrado) printf("\nDocumento nao encontrado.\n");
}

/* -------------------------------------------------------
   Alterar status (com registro de historico)
------------------------------------------------------- */

void alterarStatus(void) {
    int codigo, i, opcao, encontrado = 0;

    printf("\n=== ALTERAR STATUS ===\n");
    printf("Codigo do documento: ");
    while (!lerInteiro(&codigo))
        printf("Entrada invalida: ");

    for (i = 0; i < totalDocumentos; i++) {
        if (documentos[i].codigo == codigo) {
            char anterior[30];
            strcpy(anterior, documentos[i].status);

            printf("\nDocumento: %s\n", documentos[i].titulo);
            printf("Status atual: %s\n\n", documentos[i].status);
            printf("1 - Pendente\n2 - Em Analise\n3 - Aprovado\n4 - Reprovado\n");
            printf("Opcao: ");
            while (!lerInteiro(&opcao))
                printf("Entrada invalida: ");

            switch (opcao) {
                case 1: strcpy(documentos[i].status, "Pendente");   break;
                case 2: strcpy(documentos[i].status, "Em Analise"); break;
                case 3: strcpy(documentos[i].status, "Aprovado");   break;
                case 4: strcpy(documentos[i].status, "Reprovado");  break;
                default:
                    printf("\nOpcao invalida.\n");
                    return;
            }

            /* FASE 3 — gravar no historico */
            registrarHistorico(i, anterior, documentos[i].status);

            printf("\nStatus alterado para: %s\n", documentos[i].status);
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) printf("\nDocumento nao encontrado.\n");
}

/* -------------------------------------------------------
   Excluir documento
------------------------------------------------------- */

void excluirDocumento(void) {
    int codigo, i, pos = -1;
    char confirmacao[4];

    printf("\n=== EXCLUIR DOCUMENTO ===\n");
    printf("Codigo do documento: ");
    while (!lerInteiro(&codigo))
        printf("Entrada invalida: ");

    for (i = 0; i < totalDocumentos; i++) {
        if (documentos[i].codigo == codigo) { pos = i; break; }
    }

    if (pos == -1) {
        printf("\nDocumento nao encontrado.\n");
        return;
    }

    exibirDocumento(pos);
    printf("Confirmar exclusao? (s/n): ");
    fgets(confirmacao, sizeof(confirmacao), stdin);
    confirmacao[strcspn(confirmacao, "\n")] = '\0';

    if (confirmacao[0] != 's' && confirmacao[0] != 'S') {
        printf("\nExclusao cancelada.\n");
        return;
    }

    for (i = pos; i < totalDocumentos - 1; i++)
        documentos[i] = documentos[i + 1];
    totalDocumentos--;

    printf("\nDocumento excluido com sucesso!\n");
}

/* -------------------------------------------------------
   Relatorio resumo
------------------------------------------------------- */

void relatorioResumo(void) {
    int i, pendentes = 0, analise = 0, aprovados = 0, reprovados = 0;

    for (i = 0; i < totalDocumentos; i++) {
        if      (strcmp(documentos[i].status, "Pendente")   == 0) pendentes++;
        else if (strcmp(documentos[i].status, "Em Analise") == 0) analise++;
        else if (strcmp(documentos[i].status, "Aprovado")   == 0) aprovados++;
        else if (strcmp(documentos[i].status, "Reprovado")  == 0) reprovados++;
    }

    printf("\n=== RELATORIO DO GED i9TMG ===\n");
    printf("Total de documentos: %d\n", totalDocumentos);
    printf("Pendentes:           %d\n", pendentes);
    printf("Em analise:          %d\n", analise);
    printf("Aprovados:           %d\n", aprovados);
    printf("Reprovados:          %d\n", reprovados);
}

/* -------------------------------------------------------
   FASE 3 — Ordenacao com qsort
------------------------------------------------------- */

static int compararPorCodigo(const void *a, const void *b) {
    const Documento *da = (const Documento *)a;
    const Documento *db = (const Documento *)b;
    return da->codigo - db->codigo;
}

/* Data no formato DD/MM/AAAA — inverte para AAAA/MM/DD antes de comparar */
static int compararPorData(const void *a, const void *b) {
    const Documento *da = (const Documento *)a;
    const Documento *db = (const Documento *)b;
    char fa[11], fb[11];

    /* Reorganiza para AAAA/MM/DD para comparacao lexicografica correta */
    snprintf(fa, sizeof(fa), "%.4s/%.2s/%.2s",
             da->data + 6, da->data + 3, da->data);
    snprintf(fb, sizeof(fb), "%.4s/%.2s/%.2s",
             db->data + 6, db->data + 3, db->data);

    return strcmp(fa, fb);
}

static int compararPorStatus(const void *a, const void *b) {
    const Documento *da = (const Documento *)a;
    const Documento *db = (const Documento *)b;
    return strcmp(da->status, db->status);
}

void ordenarDocumentos(void) {
    int opcao;

    if (totalDocumentos == 0) {
        printf("\nNenhum documento cadastrado.\n");
        return;
    }

    printf("\n=== ORDENAR DOCUMENTOS ===\n");
    printf("1 - Por codigo (crescente)\n");
    printf("2 - Por data   (crescente)\n");
    printf("3 - Por status (alfabetico)\n");
    printf("Opcao: ");
    while (!lerInteiro(&opcao))
        printf("Entrada invalida: ");

    switch (opcao) {
        case 1:
            qsort(documentos, (size_t)totalDocumentos,
                  sizeof(Documento), compararPorCodigo);
            printf("\nOrdenado por codigo.\n");
            break;
        case 2:
            qsort(documentos, (size_t)totalDocumentos,
                  sizeof(Documento), compararPorData);
            printf("\nOrdenado por data.\n");
            break;
        case 3:
            qsort(documentos, (size_t)totalDocumentos,
                  sizeof(Documento), compararPorStatus);
            printf("\nOrdenado por status.\n");
            break;
        default:
            printf("Opcao invalida.\n");
            return;
    }

    listarDocumentos();
}
