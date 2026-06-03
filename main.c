#include <stdio.h>
#include "documento.h"

int main(void) {
    int opcao;

    carregarDocumentos();

    do {
        printf("\n==============================================\n");
        printf(" GED i9TMG - Documentos e Certificados RH\n");
        printf("==============================================\n");
        printf("1  - Cadastrar documento/certificado\n");
        printf("2  - Listar documentos\n");
        printf("3  - Buscar documento\n");
        printf("4  - Editar documento\n");
        printf("5  - Alterar status do processo\n");
        printf("6  - Excluir documento\n");
        printf("7  - Relatorio resumo\n");
        printf("8  - Relatorio de vencimentos\n");
        printf("9  - Ordenar documentos\n");
        printf("10 - Ver historico de status\n");
        printf("11 - Salvar documentos\n");
        printf("0  - Sair\n");
        printf("Escolha uma opcao: ");

        while (!lerInteiro(&opcao)) {
            printf("Entrada invalida. Digite uma opcao: ");
        }

        switch (opcao) {
            case 1:
                cadastrarDocumento();
                break;
            case 2:
                listarDocumentos();
                break;
            case 3:
                buscarDocumento();
                break;
            case 4:
                editarDocumento();
                break;
            case 5:
                alterarStatus();
                break;
            case 6:
                excluirDocumento();
                break;
            case 7:
                relatorioResumo();
                break;
            case 8:
                relatorioVencimentos();
                break;
            case 9:
                ordenarDocumentos();
                break;
            case 10: {
                int codigo, indice;
                printf("\nCodigo do documento: ");
                while (!lerInteiro(&codigo)) {
                    printf("Entrada invalida: ");
                }

                indice = buscarIndicePorCodigo(codigo);
                if (indice == -1) {
                    printf("\nDocumento nao encontrado.\n");
                } else {
                    exibirHistorico(indice);
                }
                break;
            }
            case 11:
                salvarDocumentos();
                break;
            case 0:
                salvarDocumentos();
                printf("\nEncerrando o sistema GED i9TMG...\n");
                break;
            default:
                printf("\nOpcao invalida. Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}
