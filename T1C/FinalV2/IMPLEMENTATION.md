# Documentação da Implementação — FinalV2

Resumo rápido

- Cópia da implementação com refatoração do escalonador para `FinalV2/scheduler.c`.
- Escalonador agora encapsulado em API: `scheduler_init`, `scheduler_add`, `scheduler_run_blocking`, etc.
- Mantém a mesma semântica funcional do projeto original, sem `pthread` (escalonador cooperativo/bloqueante).

(versão simplificada para a pasta FinalV2)
