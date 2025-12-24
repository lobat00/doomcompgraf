# 🐛 Correção de Colisão: Bug das Diagonais (Clipping)

Esta atualização corrige o problema onde a câmera "entrava" nas paredes ao andar na diagonal ou ao encostar em quinas de blocos.

## 📄 Resumo do Problema
O sistema de colisão anterior tratava o jogador como um ponto ou uma cruz (**+**). Isso deixava "pontos cegos" nas diagonais. Se o jogador andasse contra uma quina, o sensor central passava pelo vazio, mas o "ombro" do jogador atravessava a parede.

## 🛠️ A Solução: Bounding Box (Caixa de Colisão)
Alteramos a lógica para tratar o jogador como um **Quadrado (⬛)** com largura física. Agora, a cada movimento, testamos os **4 cantos** do jogador. Se *qualquer* um dos cantos tocar na parede, o movimento é bloqueado.

---

## 🔄 Comparativo Técnico

### 🔴 Antes: Lógica "Cruz" (Sensor Único)
Verificávamos apenas um ponto à frente, baseado na direção do movimento.

```ascii
      ^ (Sensor Frente)
      |
      P (Jogador)
```
      
### Código Removido (input.cpp):
```ascii
// Calculava apenas a ponta do vetor de direção
float cX = (tentX > 0) ? tentX + margem : tentX - margem;
float cZ = (tentZ > 0) ? tentZ + margem : tentZ - margem;

// Testava apenas se o centro deslocado batia
if (podeAndar(camX + cX, camZ)) { ... }
```
Falha: Ao passar raspando em uma quina, o sensor (seta) não batia em nada, mas a lateral da câmera entrava na parede.

### 🟢 Depois: Lógica "Bounding Box" (4 Cantos)
```ascii
  (Canto) X-------X (Canto)
            |   P   |
    (Canto) X-------X (Canto)
```
Código Adicionado (input.cpp):
```ascii
  // Define a largura física do jogador (do centro até a borda)
float LARGURA = 1.0f; 

// Testamos os 4 CANTOS simultaneamente para cada eixo
// Se qualquer um retornar false (parede), colidiuX vira true.
if (!podeAndar(proximoX + LARGURA, camZ + LARGURA)) colidiuX = true; // Frente-Direita
if (!podeAndar(proximoX + LARGURA, camZ - LARGURA)) colidiuX = true; // Trás-Direita
if (!podeAndar(proximoX - LARGURA, camZ + LARGURA)) colidiuX = true; // Frente-Esquerda
if (!podeAndar(proximoX - LARGURA, camZ - LARGURA)) colidiuX = true; // Trás-Esquerda
```
Correção: Agora é impossível uma quina de parede "furar" o jogador, pois um dos 4 cantos sempre detectará a colisão antes da câmera atravessar.