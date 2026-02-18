# Gestão Hospitalar (MVP) - C11 + TUI ANSI

Aplicação em C puro (C11) com TUI bonita usando ANSI escape codes.
Inclui login/senha (SHA-256 + salt), sessão em memória, RBAC centralizado,
persistência em arquivos binários e auditoria mínima.

## Build / Run
```bash
make build
make run
````

Na primeira execução, a pasta `data/` é criada e o seed roda.

Usuário inicial:

* username: admin
* senha: admin123
* No primeiro login você é obrigado a trocar a senha.

## Teclas

* ↑/↓: navegar listas e menu
* Enter: abrir/confirmar
* Esc: voltar/fechar
* F1: ajuda
* /: busca (onde aplicável)
* Ctrl+S: salvar em alguns formulários (alternativo ao botão)

## Arquitetura (visão rápida)

* `src/ui/`: mini-framework de TUI (layout, widgets, tema, foco)
* `src/app/`: bootstrap, sessão, roteamento e menu lateral
* `src/security/`: SHA-256, hash de senha com salt, RBAC e auth
* `src/storage/`: DAL por arquivos binários (repos por entidade + seeds)
* `src/modules/*`: cada domínio com controller + views

Regras:

* Controller checa RBAC antes de executar ações
* Módulos não acessam storage direto; sempre via repo
* Auditoria registra ações relevantes

## Como adicionar um novo módulo

1. Crie `src/modules/meumodulo/` com:

   * `meumodulo_controller.c/.h` (fluxo e comandos)
   * `meumodulo_views.c/.h` (telas)
2. Registre a rota/tela no `src/app/router.c`
3. Adicione item no sidebar em `src/app/app.c` com permissão adequada
4. Se precisar de persistência, crie `repo_*.c/.h` em `src/storage/`

## Persistência

Arquivos binários por tabela em `data/`:

* users.dat, roles.dat, audit.dat, patients.dat, visits.dat, triage.dat,
  medical.dat, rx.dat, rx_items.dat, meds.dat, stock_lots.dat, stock_moves.dat

Cada repo lê/grava registros com cabeçalho simples e IDs auto-incrementais.
