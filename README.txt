GED i9TMG - Documentos e Certificados de Funcionarios

Arquivos do projeto:
- main.c: menu principal do sistema.
- documento.h: structs, constantes e prototipos das funcoes.
- documento.c: regras de negocio, cadastro, busca, edicao, status, relatorios e ordenacao.
- arquivo.c: salvamento e carregamento dos dados em documentos_i9mtg.txt.
- Makefile: automacao da compilacao.

Principais melhorias implementadas:
1. Documento vinculado a funcionario.
2. Campos de nome, matricula, setor e cargo.
3. Separacao entre responsavel pelo cadastro e funcionario dono do documento.
4. Campo de data de emissao/cadastro.
5. Campo de data de validade, opcional.
6. Situacao automatica de validade: Valido, A vencer, Vencido ou Sem validade.
7. Relatorio de vencimentos para documentos vencidos ou a vencer em ate 30 dias.
8. Busca por funcionario, matricula, setor, tipo, titulo, status e validade.
9. Salvamento automatico apos cadastro, edicao, exclusao, alteracao de status e ordenacao.
10. Validacao de campos obrigatorios e troca de ponto e virgula por virgula nos textos.
11. Compatibilidade com o formato antigo do arquivo documentos_i9mtg.txt.

Como compilar no Mac/Linux:
make
./ged

Como compilar no Windows com GCC/MinGW:
gcc -Wall -Wextra -std=c99 main.c documento.c arquivo.c -o ged.exe
ged.exe

Observacao:
O arquivo documentos_i9mtg.txt sera criado automaticamente na primeira vez que o sistema salvar os dados.
