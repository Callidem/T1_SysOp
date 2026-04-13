tamMem: 1024 palavras
tamPg: 8 palavras
nro_Frames = tamMem/tamPG   (numero de frame)
deslocamento dentro da página = endereço lógico & tamPg

---

- Memória fisica é o vetor real mem->pos[], de tamanho tamMem
- Página é o pedaço lógico do programa, de tamanho fixo tamPg.
- Frame é o pedaço físico da memoria, também de tamanho tamPg
- Tabela de páginas é o mapa que diz basicamente "pagina logica i está no frame fisica x"

- BitMap é uma estrutura de controle compcata para marcar estados binários.
> cada posição do allocated_frames representa 1 frame na memoria
  valor false significa que o frame está livre

Como página e frame têm mesmo tamanho, a tradução fica simples.

Fórmulas:

pagina = enderecoLogico / tamPg
offset = enderecoLogico % tamPg
frame = tabelaPaginas[pagina]
enderecoFisico = frame * tamPg + offset

##### Exemplos

tamMem = 32
tamPg = 8
então nroFrames = 4 (frames 0,1,2,3)
suponha tabela de páginas do processo:
página 0 -> frame 2
página 1 -> frame 0
página 2 -> frame 3
página 3 -> frame 1

##### Traduções:
Endereço lógico 0
página = 0/8 = 0
offset = 0%8 = 0
frame = tabela[0] = 2
físico = 2*8 + 0 = 16

Endereço lógico 9
página = 9/8 = 1
offset = 9%8 = 1
frame = tabela[1] = 0
físico = 0*8 + 1 = 1

Endereço lógico 29
página = 29/8 = 3
offset = 29%8 = 5
frame = tabela[3] = 1
físico = 1*8 + 5 = 13