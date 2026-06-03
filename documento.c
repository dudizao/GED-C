#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "documento.h"

Documento documentos[MAX_DOCUMENTOS];
int totalDocumentos = 0;

/* -------------------------------------------------------
   Leitura e validacao
------------------------------------------------------- */

int lerInteiro(int *destino) {
    char linha[32];

    if (fgets(linha, sizeof(linha), stdin) == NULL) return 0;
    limparQuebraLinha(linha);

    if (sscanf(linha, "%d", destino) != 1) return 0;
    return 1;
}

void limparQuebraLinha(char *texto) {
    texto[strcspn(texto, "\n")] = '\0';
}

void removerPontoEVirgula(char *texto) {
    int i;
    for (i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';') {
            texto[i] = ',';
        }
    }
}

int dataEhVazia(const char *data) {
    return data == NULL || data[0] == '\0';
}

int validarData(const char *data) {
    int dia, mes, ano;
    int diasPorMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (dataEhVazia(data)) return 0;
    if (strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;

    if (!isdigit((unsigned char)data[0]) || !isdigit((unsigned char)data[1])) return 0;
    if (!isdigit((unsigned char)data[3]) || !isdigit((unsigned char)data[4])) return 0;
    if (!isdigit((unsigned char)data[6]) || !isdigit((unsigned char)data[7])) return 0;
    if (!isdigit((unsigned char)data[8]) || !isdigit((unsigned char)data[9])) return 0;

    dia = (data[0] - '0') * 10 + (data[1] - '0');
    mes = (data[3] - '0') * 10 + (data[4] - '0');
    ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100
        + (data[8] - '0') * 10 + (data[9] - '0');

    if (ano < 2000 || ano > 2099) return 0;
    if (mes < 1 || mes > 12) return 0;

    if (mes == 2 && ((ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0)) {
        diasPorMes[2] = 29;
    }

    if (dia < 1 || dia > diasPorMes[mes]) return 0;

    return 1;
}

static int dataParaInteiro(const char *data) {
    int dia, mes, ano;

    if (!validarData(data)) return 0;

    dia = (data[0] - '0') * 10 + (data[1] - '0');
    mes = (data[3] - '0') * 10 + (data[4] - '0');
    ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100
        + (data[8] - '0') * 10 + (data[9] - '0');

    return ano * 10000 + mes * 100 + dia;
}

static int diasAteData(const char *data) {
    struct tm alvo;
    struct tm hojeTm;
    time_t agora;
    time_t tempoAlvo;
    double diferenca;
    int dia, mes, ano;

    if (!validarData(data)) return 999999;

    dia = (data[0] - '0') * 10 + (data[1] - '0');
    mes = (data[3] - '0') * 10 + (data[4] - '0');
    ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100
        + (data[8] - '0') * 10 + (data[9] - '0');

    agora = time(NULL);
    hojeTm = *localtime(&agora);
    hojeTm.tm_hour = 0;
    hojeTm.tm_min = 0;
    hojeTm.tm_sec = 0;
    hojeTm.tm_isdst = -1;

    memset(&alvo, 0, sizeof(alvo));
    alvo.tm_mday = dia;
    alvo.tm_mon = mes - 1;
    alvo.tm_year = ano - 1900;
    alvo.tm_hour = 0;
    alvo.tm_min = 0;
    alvo.tm_sec = 0;
    alvo.tm_isdst = -1;

    tempoAlvo = mktime(&alvo);
    diferenca = difftime(tempoAlvo, mktime(&hojeTm));

    return (int)(diferenca / (60 * 60 * 24));
}

void lerCampoObrigatorio(const char *rotulo, char *destino, int tamanho) {
    do {
        printf("%s", rotulo);
        fgets(destino, tamanho, stdin);
        limparQuebraLinha(destino);
        removerPontoEVirgula(destino);

        if (strlen(destino) == 0) {
            printf("Campo obrigatorio. Digite novamente.\n");
        }
    } while (strlen(destino) == 0);
}

void lerCampoOpcional(const char *rotulo, char *destino, int tamanho) {
    printf("%s", rotulo);
    fgets(destino, tamanho, stdin);
    limparQuebraLinha(destino);
    removerPontoEVirgula(destino);
}

void lerDataObrigatoria(const char *rotulo, char *destino, int tamanho) {
    do {
        printf("%s", rotulo);
        fgets(destino, tamanho, stdin);
        limparQuebraLinha(destino);

        if (!validarData(destino)) {
            printf("Data invalida. Use DD/MM/AAAA.\n");
        }
    } while (!validarData(destino));
}

void lerDataOpcional(const char *rotulo, char *destino, int tamanho) {
    do {
        printf("%s", rotulo);
        fgets(destino, tamanho, stdin);
        limparQuebraLinha(destino);

        if (strlen(destino) == 0) {
            return;
        }

        if (!validarData(destino)) {
            printf("Data invalida. Use DD/MM/AAAA ou pressione Enter para deixar sem validade.\n");
        }
    } while (!validarData(destino));
}

/* -------------------------------------------------------
   Texto e busca
------------------------------------------------------- */

void strParaMinusculo(const char *entrada, char *saida, int tam) {
    int i;

    for (i = 0; i < tam - 1 && entrada[i] != '\0'; i++) {
        saida[i] = (char)tolower((unsigned char)entrada[i]);
    }
    saida[i] = '\0';
}

int contemTexto(const char *texto, const char *busca) {
    char textoMin[256];
    char buscaMin[256];

    strParaMinusculo(texto, textoMin, sizeof(textoMin));
    strParaMinusculo(busca, buscaMin, sizeof(buscaMin));

    return strstr(textoMin, buscaMin) != NULL;
}

int buscarIndicePorCodigo(int codigo) {
    int i;

    for (i = 0; i < totalDocumentos; i++) {
        if (documentos[i].codigo == codigo) {
            return i;
        }
    }

    return -1;
}

static int gerarProximoCodigo(void) {
    int i;
    int maior = 0;

    for (i = 0; i < totalDocumentos; i++) {
        if (documentos[i].codigo > maior) {
            maior = documentos[i].codigo;
        }
    }

    return maior + 1;
}

/* -------------------------------------------------------
   Regra de negocio: situacao de validade
------------------------------------------------------- */

const char *situacaoValidade(const Documento *doc) {
    int dias;

    if (dataEhVazia(doc->dataValidade)) {
        return "Sem validade";
    }

    dias = diasAteData(doc->dataValidade);

    if (dias < 0) {
        return "Vencido";
    }

    if (dias <= 30) {
        return "A vencer";
    }

    return "Valido";
}

/* -------------------------------------------------------
   Exibicao e historico
------------------------------------------------------- */

void exibirDocumento(int i) {
    printf("\nCodigo:                 %d\n", documentos[i].codigo);
    printf("Funcionario:            %s\n", documentos[i].nomeFuncionario);
    printf("Matricula:              %s\n", documentos[i].matriculaFuncionario);
    printf("Setor:                  %s\n", documentos[i].setor);
    printf("Cargo:                  %s\n", documentos[i].cargo);
    printf("Titulo:                 %s\n", documentos[i].titulo);
    printf("Tipo:                   %s\n", documentos[i].tipo);
    printf("Responsavel cadastro:   %s\n", documentos[i].responsavelCadastro);
    printf("Data de emissao:        %s\n", documentos[i].dataEmissao);
    printf("Data de validade:       %s\n", dataEhVazia(documentos[i].dataValidade) ? "Sem validade" : documentos[i].dataValidade);
    printf("Status do processo:     %s\n", documentos[i].status);
    printf("Situacao da validade:   %s\n", situacaoValidade(&documentos[i]));
    printf("--------------------------------------------------\n");
}

static void dataHoraAtual(char *buf, int tam) {
    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);

    snprintf(buf, (size_t)tam, "%02d/%02d/%04d %02d:%02d",
             t->tm_mday,
             t->tm_mon + 1,
             t->tm_year + 1900,
             t->tm_hour,
             t->tm_min);
}

void registrarHistorico(int i, const char *anterior, const char *novo) {
    EntradaHistorico *h;
    int k;

    if (strcmp(anterior, novo) == 0) {
        return;
    }

    if (documentos[i].totalHistorico >= MAX_HISTORICO) {
        for (k = 0; k < MAX_HISTORICO - 1; k++) {
            documentos[i].historico[k] = documentos[i].historico[k + 1];
        }
        documentos[i].totalHistorico = MAX_HISTORICO - 1;
    }

    h = &documentos[i].historico[documentos[i].totalHistorico];

    strncpy(h->statusAnterior, anterior, TAM_STATUS - 1);
    h->statusAnterior[TAM_STATUS - 1] = '\0';

    strncpy(h->statusNovo, novo, TAM_STATUS - 1);
    h->statusNovo[TAM_STATUS - 1] = '\0';

    dataHoraAtual(h->dataHora, TAM_DATAHORA);

    documentos[i].totalHistorico++;
}

void exibirHistorico(int i) {
    int k;

    printf("\n=== HISTORICO DE STATUS ===\n");
    printf("Documento: %s\n", documentos[i].titulo);
    printf("Funcionario: %s\n", documentos[i].nomeFuncionario);

    if (documentos[i].totalHistorico == 0) {
        printf("Nenhuma alteracao registrada.\n");
        return;
    }

    for (k = 0; k < documentos[i].totalHistorico; k++) {
        printf("%s | %s -> %s\n",
               documentos[i].historico[k].dataHora,
               documentos[i].historico[k].statusAnterior,
               documentos[i].historico[k].statusNovo);
    }
}

/* -------------------------------------------------------
   Cadastro
------------------------------------------------------- */

void cadastrarDocumento(void) {
    Documento doc;

    if (totalDocumentos >= MAX_DOCUMENTOS) {
        printf("\nLimite de documentos atingido.\n");
        return;
    }

    memset(&doc, 0, sizeof(doc));

    printf("\n=== CADASTRAR DOCUMENTO/CERTIFICADO DO FUNCIONARIO ===\n");

    doc.codigo = gerarProximoCodigo();
    printf("Codigo gerado automaticamente: %d\n", doc.codigo);

    lerCampoObrigatorio("Nome do funcionario: ", doc.nomeFuncionario, TAM_NOME);
    lerCampoObrigatorio("Matricula do funcionario: ", doc.matriculaFuncionario, TAM_MATRICULA);
    lerCampoObrigatorio("Setor: ", doc.setor, TAM_SETOR);
    lerCampoObrigatorio("Cargo: ", doc.cargo, TAM_CARGO);

    lerCampoObrigatorio("Titulo do documento/certificado: ", doc.titulo, TAM_TITULO);
    lerCampoObrigatorio("Tipo (Ex: Certificado NR10, Contrato, Exame, Documento pessoal): ", doc.tipo, TAM_TIPO);
    lerCampoObrigatorio("Responsavel pelo cadastro: ", doc.responsavelCadastro, TAM_RESP);

    lerDataObrigatoria("Data de emissao/cadastro (DD/MM/AAAA): ", doc.dataEmissao, TAM_DATA);

    do {
        lerDataOpcional("Data de validade (DD/MM/AAAA ou Enter se nao tiver): ", doc.dataValidade, TAM_DATA);

        if (!dataEhVazia(doc.dataValidade) && dataParaInteiro(doc.dataValidade) < dataParaInteiro(doc.dataEmissao)) {
            printf("A data de validade nao pode ser anterior a data de emissao.\n");
            doc.dataValidade[0] = '\0';
        } else {
            break;
        }
    } while (1);

    strcpy(doc.status, "Pendente");
    doc.totalHistorico = 0;

    documentos[totalDocumentos] = doc;
    totalDocumentos++;

    printf("\nDocumento cadastrado com sucesso.\n");
    salvarDocumentos();
}

/* -------------------------------------------------------
   Listagem e busca
------------------------------------------------------- */

void listarDocumentos(void) {
    int i;

    printf("\n=== LISTA DE DOCUMENTOS/CERTIFICADOS ===\n");

    if (totalDocumentos == 0) {
        printf("Nenhum documento cadastrado.\n");
        return;
    }

    for (i = 0; i < totalDocumentos; i++) {
        exibirDocumento(i);
    }
}

static void buscarPorCodigo(void) {
    int codigo;
    int indice;

    printf("Codigo: ");
    while (!lerInteiro(&codigo)) {
        printf("Entrada invalida: ");
    }

    indice = buscarIndicePorCodigo(codigo);

    if (indice == -1) {
        printf("\nDocumento nao encontrado.\n");
    } else {
        exibirDocumento(indice);
    }
}

static void buscarPorCampoTexto(const char *rotulo, const char *nomeCampo) {
    char termo[100];
    int i;
    int encontrados = 0;

    printf("%s", rotulo);
    fgets(termo, sizeof(termo), stdin);
    limparQuebraLinha(termo);

    printf("\n--- Resultados ---\n");

    for (i = 0; i < totalDocumentos; i++) {
        int achou = 0;

        if (strcmp(nomeCampo, "funcionario") == 0) {
            achou = contemTexto(documentos[i].nomeFuncionario, termo);
        } else if (strcmp(nomeCampo, "matricula") == 0) {
            achou = contemTexto(documentos[i].matriculaFuncionario, termo);
        } else if (strcmp(nomeCampo, "setor") == 0) {
            achou = contemTexto(documentos[i].setor, termo);
        } else if (strcmp(nomeCampo, "tipo") == 0) {
            achou = contemTexto(documentos[i].tipo, termo);
        } else if (strcmp(nomeCampo, "titulo") == 0) {
            achou = contemTexto(documentos[i].titulo, termo);
        }

        if (achou) {
            exibirDocumento(i);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum documento encontrado.\n");
    } else {
        printf("Total encontrado: %d\n", encontrados);
    }
}

static void buscarPorStatus(void) {
    int opcao;
    int i;
    int encontrados = 0;
    char statusBusca[TAM_STATUS];

    printf("\nStatus do processo:\n");
    printf("1 - Pendente\n");
    printf("2 - Em Analise\n");
    printf("3 - Aprovado\n");
    printf("4 - Reprovado\n");
    printf("5 - Pendente de Renovacao\n");
    printf("Opcao: ");

    while (!lerInteiro(&opcao)) {
        printf("Entrada invalida: ");
    }

    switch (opcao) {
        case 1: strcpy(statusBusca, "Pendente"); break;
        case 2: strcpy(statusBusca, "Em Analise"); break;
        case 3: strcpy(statusBusca, "Aprovado"); break;
        case 4: strcpy(statusBusca, "Reprovado"); break;
        case 5: strcpy(statusBusca, "Pendente de Renovacao"); break;
        default:
            printf("Opcao invalida.\n");
            return;
    }

    for (i = 0; i < totalDocumentos; i++) {
        if (strcmp(documentos[i].status, statusBusca) == 0) {
            exibirDocumento(i);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum documento encontrado.\n");
    } else {
        printf("Total encontrado: %d\n", encontrados);
    }
}

static void buscarPorValidade(void) {
    int opcao;
    int i;
    int encontrados = 0;
    const char *situacaoBusca;

    printf("\nSituacao de validade:\n");
    printf("1 - Valido\n");
    printf("2 - A vencer\n");
    printf("3 - Vencido\n");
    printf("4 - Sem validade\n");
    printf("Opcao: ");

    while (!lerInteiro(&opcao)) {
        printf("Entrada invalida: ");
    }

    switch (opcao) {
        case 1: situacaoBusca = "Valido"; break;
        case 2: situacaoBusca = "A vencer"; break;
        case 3: situacaoBusca = "Vencido"; break;
        case 4: situacaoBusca = "Sem validade"; break;
        default:
            printf("Opcao invalida.\n");
            return;
    }

    for (i = 0; i < totalDocumentos; i++) {
        if (strcmp(situacaoValidade(&documentos[i]), situacaoBusca) == 0) {
            exibirDocumento(i);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum documento encontrado.\n");
    } else {
        printf("Total encontrado: %d\n", encontrados);
    }
}

void buscarDocumento(void) {
    int opcao;

    printf("\n=== BUSCAR DOCUMENTO ===\n");
    printf("1 - Por codigo\n");
    printf("2 - Por funcionario\n");
    printf("3 - Por matricula\n");
    printf("4 - Por setor\n");
    printf("5 - Por tipo\n");
    printf("6 - Por titulo\n");
    printf("7 - Por status do processo\n");
    printf("8 - Por situacao de validade\n");
    printf("Opcao: ");

    while (!lerInteiro(&opcao)) {
        printf("Entrada invalida: ");
    }

    switch (opcao) {
        case 1:
            buscarPorCodigo();
            break;
        case 2:
            buscarPorCampoTexto("Nome do funcionario: ", "funcionario");
            break;
        case 3:
            buscarPorCampoTexto("Matricula: ", "matricula");
            break;
        case 4:
            buscarPorCampoTexto("Setor: ", "setor");
            break;
        case 5:
            buscarPorCampoTexto("Tipo: ", "tipo");
            break;
        case 6:
            buscarPorCampoTexto("Titulo: ", "titulo");
            break;
        case 7:
            buscarPorStatus();
            break;
        case 8:
            buscarPorValidade();
            break;
        default:
            printf("Opcao invalida.\n");
    }
}

/* -------------------------------------------------------
   Edicao
------------------------------------------------------- */

static void editarTexto(const char *rotulo, char *campo, int tamanho) {
    char novo[120];

    printf("%s atual: %s\n", rotulo, campo);
    printf("Novo valor ou Enter para manter: ");
    fgets(novo, sizeof(novo), stdin);
    limparQuebraLinha(novo);
    removerPontoEVirgula(novo);

    if (strlen(novo) > 0) {
        strncpy(campo, novo, (size_t)tamanho - 1);
        campo[tamanho - 1] = '\0';
    }
}

static void editarDataObrigatoria(const char *rotulo, char *campo, int tamanho) {
    char nova[TAM_DATA];

    do {
        printf("%s atual: %s\n", rotulo, campo);
        printf("Nova data ou Enter para manter: ");
        fgets(nova, sizeof(nova), stdin);
        limparQuebraLinha(nova);

        if (strlen(nova) == 0) {
            return;
        }

        if (validarData(nova)) {
            strncpy(campo, nova, (size_t)tamanho - 1);
            campo[tamanho - 1] = '\0';
            return;
        }

        printf("Data invalida. Use DD/MM/AAAA.\n");
    } while (1);
}

static void editarDataOpcional(const char *rotulo, char *campo, int tamanho) {
    char nova[TAM_DATA];

    do {
        printf("%s atual: %s\n", rotulo, dataEhVazia(campo) ? "Sem validade" : campo);
        printf("Nova data, Enter para manter ou 0 para remover validade: ");
        fgets(nova, sizeof(nova), stdin);
        limparQuebraLinha(nova);

        if (strlen(nova) == 0) {
            return;
        }

        if (strcmp(nova, "0") == 0) {
            campo[0] = '\0';
            return;
        }

        if (validarData(nova)) {
            strncpy(campo, nova, (size_t)tamanho - 1);
            campo[tamanho - 1] = '\0';
            return;
        }

        printf("Data invalida. Use DD/MM/AAAA.\n");
    } while (1);
}

void editarDocumento(void) {
    int codigo;
    int i;

    printf("\n=== EDITAR DOCUMENTO ===\n");
    printf("Codigo do documento: ");

    while (!lerInteiro(&codigo)) {
        printf("Entrada invalida: ");
    }

    i = buscarIndicePorCodigo(codigo);

    if (i == -1) {
        printf("\nDocumento nao encontrado.\n");
        return;
    }

    printf("\nEditando documento de %s\n", documentos[i].nomeFuncionario);
    printf("Pressione Enter para manter o valor atual.\n\n");

    editarTexto("Nome do funcionario", documentos[i].nomeFuncionario, TAM_NOME);
    editarTexto("Matricula", documentos[i].matriculaFuncionario, TAM_MATRICULA);
    editarTexto("Setor", documentos[i].setor, TAM_SETOR);
    editarTexto("Cargo", documentos[i].cargo, TAM_CARGO);
    editarTexto("Titulo", documentos[i].titulo, TAM_TITULO);
    editarTexto("Tipo", documentos[i].tipo, TAM_TIPO);
    editarTexto("Responsavel pelo cadastro", documentos[i].responsavelCadastro, TAM_RESP);
    editarDataObrigatoria("Data de emissao/cadastro", documentos[i].dataEmissao, TAM_DATA);
    editarDataOpcional("Data de validade", documentos[i].dataValidade, TAM_DATA);

    if (!dataEhVazia(documentos[i].dataValidade) &&
        dataParaInteiro(documentos[i].dataValidade) < dataParaInteiro(documentos[i].dataEmissao)) {
        printf("\nAtencao: validade anterior a emissao. A validade foi removida.\n");
        documentos[i].dataValidade[0] = '\0';
    }

    printf("\nDocumento atualizado com sucesso.\n");
    salvarDocumentos();
}

/* -------------------------------------------------------
   Status do processo
------------------------------------------------------- */

void alterarStatus(void) {
    int codigo;
    int i;
    int opcao;
    char anterior[TAM_STATUS];
    char novoStatus[TAM_STATUS];

    printf("\n=== ALTERAR STATUS DO PROCESSO ===\n");
    printf("Codigo do documento: ");

    while (!lerInteiro(&codigo)) {
        printf("Entrada invalida: ");
    }

    i = buscarIndicePorCodigo(codigo);

    if (i == -1) {
        printf("\nDocumento nao encontrado.\n");
        return;
    }

    printf("\nDocumento: %s\n", documentos[i].titulo);
    printf("Funcionario: %s\n", documentos[i].nomeFuncionario);
    printf("Status atual: %s\n\n", documentos[i].status);
    printf("1 - Pendente\n");
    printf("2 - Em Analise\n");
    printf("3 - Aprovado\n");
    printf("4 - Reprovado\n");
    printf("5 - Pendente de Renovacao\n");
    printf("Opcao: ");

    while (!lerInteiro(&opcao)) {
        printf("Entrada invalida: ");
    }

    switch (opcao) {
        case 1: strcpy(novoStatus, "Pendente"); break;
        case 2: strcpy(novoStatus, "Em Analise"); break;
        case 3: strcpy(novoStatus, "Aprovado"); break;
        case 4: strcpy(novoStatus, "Reprovado"); break;
        case 5: strcpy(novoStatus, "Pendente de Renovacao"); break;
        default:
            printf("Opcao invalida.\n");
            return;
    }

    strcpy(anterior, documentos[i].status);

    if (strcmp(anterior, novoStatus) == 0) {
        printf("\nO documento ja esta com esse status. Historico nao foi alterado.\n");
        return;
    }

    strcpy(documentos[i].status, novoStatus);
    registrarHistorico(i, anterior, novoStatus);

    printf("\nStatus alterado para: %s\n", documentos[i].status);
    salvarDocumentos();
}

/* -------------------------------------------------------
   Exclusao
------------------------------------------------------- */

void excluirDocumento(void) {
    int codigo;
    int pos;
    int i;
    char confirmacao[8];

    printf("\n=== EXCLUIR DOCUMENTO ===\n");
    printf("Codigo do documento: ");

    while (!lerInteiro(&codigo)) {
        printf("Entrada invalida: ");
    }

    pos = buscarIndicePorCodigo(codigo);

    if (pos == -1) {
        printf("\nDocumento nao encontrado.\n");
        return;
    }

    exibirDocumento(pos);
    printf("Confirmar exclusao? (s/n): ");
    fgets(confirmacao, sizeof(confirmacao), stdin);
    limparQuebraLinha(confirmacao);

    if (confirmacao[0] != 's' && confirmacao[0] != 'S') {
        printf("\nExclusao cancelada.\n");
        return;
    }

    for (i = pos; i < totalDocumentos - 1; i++) {
        documentos[i] = documentos[i + 1];
    }

    totalDocumentos--;

    printf("\nDocumento excluido com sucesso.\n");
    salvarDocumentos();
}

/* -------------------------------------------------------
   Relatorios
------------------------------------------------------- */

void relatorioResumo(void) {
    int i;
    int pendentes = 0;
    int analise = 0;
    int aprovados = 0;
    int reprovados = 0;
    int renovacao = 0;
    int validos = 0;
    int aVencer = 0;
    int vencidos = 0;
    int semValidade = 0;
    int certificados = 0;

    for (i = 0; i < totalDocumentos; i++) {
        const char *sit = situacaoValidade(&documentos[i]);

        if (strcmp(documentos[i].status, "Pendente") == 0) pendentes++;
        else if (strcmp(documentos[i].status, "Em Analise") == 0) analise++;
        else if (strcmp(documentos[i].status, "Aprovado") == 0) aprovados++;
        else if (strcmp(documentos[i].status, "Reprovado") == 0) reprovados++;
        else if (strcmp(documentos[i].status, "Pendente de Renovacao") == 0) renovacao++;

        if (strcmp(sit, "Valido") == 0) validos++;
        else if (strcmp(sit, "A vencer") == 0) aVencer++;
        else if (strcmp(sit, "Vencido") == 0) vencidos++;
        else if (strcmp(sit, "Sem validade") == 0) semValidade++;

        if (contemTexto(documentos[i].tipo, "certificado")) {
            certificados++;
        }
    }

    printf("\n=== RELATORIO RESUMO DO GED i9TMG ===\n");
    printf("Total de registros:              %d\n", totalDocumentos);
    printf("Certificados identificados:      %d\n", certificados);
    printf("Documentos sem termo certificado:%d\n", totalDocumentos - certificados);

    printf("\nStatus do processo:\n");
    printf("Pendentes:                       %d\n", pendentes);
    printf("Em analise:                      %d\n", analise);
    printf("Aprovados:                       %d\n", aprovados);
    printf("Reprovados:                      %d\n", reprovados);
    printf("Pendentes de renovacao:          %d\n", renovacao);

    printf("\nSituacao de validade:\n");
    printf("Validos:                         %d\n", validos);
    printf("A vencer em ate 30 dias:         %d\n", aVencer);
    printf("Vencidos:                        %d\n", vencidos);
    printf("Sem validade:                    %d\n", semValidade);
}

void relatorioVencimentos(void) {
    int i;
    int encontrados = 0;

    printf("\n=== RELATORIO DE CERTIFICADOS/DOCUMENTOS VENCIDOS OU A VENCER ===\n");

    for (i = 0; i < totalDocumentos; i++) {
        const char *sit = situacaoValidade(&documentos[i]);

        if (strcmp(sit, "Vencido") == 0 || strcmp(sit, "A vencer") == 0) {
            exibirDocumento(i);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhum documento vencido ou a vencer nos proximos 30 dias.\n");
    } else {
        printf("Total encontrado: %d\n", encontrados);
    }
}

/* -------------------------------------------------------
   Ordenacao
------------------------------------------------------- */

static int compararPorCodigo(const void *a, const void *b) {
    const Documento *da = (const Documento *)a;
    const Documento *db = (const Documento *)b;
    return da->codigo - db->codigo;
}

static int compararPorFuncionario(const void *a, const void *b) {
    const Documento *da = (const Documento *)a;
    const Documento *db = (const Documento *)b;
    return strcmp(da->nomeFuncionario, db->nomeFuncionario);
}

static int compararPorSetor(const void *a, const void *b) {
    const Documento *da = (const Documento *)a;
    const Documento *db = (const Documento *)b;
    return strcmp(da->setor, db->setor);
}

static int compararPorValidade(const void *a, const void *b) {
    const Documento *da = (const Documento *)a;
    const Documento *db = (const Documento *)b;
    int valorA;
    int valorB;

    valorA = dataEhVazia(da->dataValidade) ? 99999999 : dataParaInteiro(da->dataValidade);
    valorB = dataEhVazia(db->dataValidade) ? 99999999 : dataParaInteiro(db->dataValidade);

    return valorA - valorB;
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
    printf("1 - Por codigo\n");
    printf("2 - Por funcionario\n");
    printf("3 - Por setor\n");
    printf("4 - Por data de validade\n");
    printf("5 - Por status do processo\n");
    printf("Opcao: ");

    while (!lerInteiro(&opcao)) {
        printf("Entrada invalida: ");
    }

    switch (opcao) {
        case 1:
            qsort(documentos, (size_t)totalDocumentos, sizeof(Documento), compararPorCodigo);
            printf("\nOrdenado por codigo.\n");
            break;
        case 2:
            qsort(documentos, (size_t)totalDocumentos, sizeof(Documento), compararPorFuncionario);
            printf("\nOrdenado por funcionario.\n");
            break;
        case 3:
            qsort(documentos, (size_t)totalDocumentos, sizeof(Documento), compararPorSetor);
            printf("\nOrdenado por setor.\n");
            break;
        case 4:
            qsort(documentos, (size_t)totalDocumentos, sizeof(Documento), compararPorValidade);
            printf("\nOrdenado por validade.\n");
            break;
        case 5:
            qsort(documentos, (size_t)totalDocumentos, sizeof(Documento), compararPorStatus);
            printf("\nOrdenado por status.\n");
            break;
        default:
            printf("Opcao invalida.\n");
            return;
    }

    listarDocumentos();
    salvarDocumentos();
}
