# SLIDE DINO GAME 🦖

**Slide Dino** foi desenvolvido como **trabalho final da disciplina Algoritmos e Estruturas de Dados I**, com o objetivo de aplicar, na prática, os conteúdos abordados ao longo da matéria.

O projeto foi desenvolvido por mim e por mais duas colegas da turma. Juntas, criamos todo o **enredo**, as **imagens**, o **código-fonte** do jogo e selecionamos os **sons**.

---

## 📚 Conteúdos abordados

O jogo contempla os seguintes conteúdos estudados na disciplina:

- Ponteiros  
- Vetores  
- Matrizes  
- Funções  
- Strings  
- Alocação dinâmica  
- Estruturas  
- Ponteiros de estruturas  
- Vetores de estruturas  
- Alocação dinâmica de estruturas  
- Aninhamento de estruturas  
- Enumeração  
- Arquivos (texto ou binário)  
- Busca linear e binária  
- Ordenação  
- Modularização (.c e .h)  
- Listas encadeadas  

---

## 🎮 Sobre o jogo

O **Slide Dino** é um jogo no qual um pequeno dinossauro acabou se perdendo de sua mãe e precisa da ajuda do jogador para encontrá-la.

Durante o jogo, o personagem deve:
- Desviar de obstáculos
- Coletar itens ao longo do caminho
- Sobreviver aos desafios até conseguir reencontrar sua mamãe

O jogo foi desenvolvido em **linguagem C**, utilizando a biblioteca **Allegro 5** para gráficos, áudio e interação com o usuário.

---

## 📂 Estrutura do projeto

- `imagens/` → imagens e sprites do jogo  
- `sons/` → efeitos sonoros e músicas  
- `main.c` → código-fonte  
- `packages.config` → dependências do projeto  
- `.gitignore` → arquivos ignorados pelo Git  

---

## 📦 Dependências

Este projeto utiliza a biblioteca **Allegro 5**, gerenciada por meio do **NuGet** no Visual Studio.

As dependências utilizadas são:
- **Allegro** (versão 5.2.10)
- **AllegroDeps** (versão 1.15.0)

---

## ⚙️ Observações
 
- Para executar o jogo, é necessário compilar o projeto no **Visual Studio**, com a biblioteca **Allegro 5** corretamente configurada.
- Este projeto utiliza a biblioteca **Allegro 5 (versão 5.2.10)**, juntamente com suas dependências (**AllegroDeps 1.15.0**), gerenciadas por meio do **NuGet** no Visual Studio.
- As dependências do projeto estão descritas no arquivo `packages.config`. Ao abrir o projeto pela primeira vez, pode ser necessário permitir que o Visual Studio realize a restauração automática dos pacotes.
- Durante o desenvolvimento, foi necessário manter alguns arquivos de biblioteca na pasta principal do projeto para garantir que o ambiente estivesse corretamente configurado. Esses arquivos binários não fazem parte do repositório, pois podem ser restaurados automaticamente via NuGet.
