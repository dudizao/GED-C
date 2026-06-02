#include <stdio.h>
#include "documento.h"

int main(void) {
    int opcao;

    carregarDocumentos();

    do {
        printf("\n=====================================\n");
        printf("         GED i9TMG — Fase 3           \n");
        printf("=====================================\n");
        printf("1 - Cadastrar documento\n");
        printf("2 - Listar documentos\n");
        printf("3 - Buscar documento\n");
        printf("4 - Editar documento\n");
        printf("5 - Alterar status\n");
        printf("6 - Excluir documento\n");
        printf("7 - Relatorio resumo\n");
        printf("8 - Ordenar documentos\n");
        printf("9 - Ver historico de status\n");
        printf("10 - Salvar documentos\n");
        printf("0  - Sair\n");
        printf("Escolha uma opcao: ");
        while (!lerInteiro(&opcao))
            printf("Entrada invalida. Digite uma opcao: ");

        switch (opcao) {
            case 1:  cadastrarDocumento(); break;
            case 2:  listarDocumentos();   break;
            case 3:  buscarDocumento();    break;
            case 4:  editarDocumento();    break;
            case 5:  alterarStatus();      break;
            case 6:  excluirDocumento();   break;
            case 7:  relatorioResumo();    break;
            case 8:  ordenarDocumentos();  break;
            case 9: {
                int codigo, i, encontrado = 0;
                printf("\nCodigo do documento: ");
                while (!lerInteiro(&codigo))
                    printf("Entrada invalida: ");
                for (i = 0; i < totalDocumentos; i++) {
                    if (documentos[i].codigo == codigo) {
                        exibirHistorico(i);
                        encontrado = 1;
                        break;
                    }
                }
                if (!encontrado) printf("\nDocumento nao encontrado.\n");
                break;
            }
            case 10: salvarDocumentos(); break;
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
