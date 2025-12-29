## Evolução do Sistema de Tiles
Esta branch implementa um sistema de renderização de níveis baseado em grid (tiles), evoluído para suportar múltiplos materiais e configurações estruturais a partir de um arquivo de texto simples.

## Mapeamento de Caracteres

|CARACTERE | ELEMENTO | TIPO/MATERIAL |  COMPORTAMENTO |
|----------|----------|---------------|----------------|
|    1     | Parede 01 |    Pedra(texParede)    |    Bloqueio de colisão Sólido|
 |   2     |Parede 02  |  Metal(TexParedeMetal) |  Bloqueio de colisão sólido|
  |  0      | Piso 01   |  Comun(texChao)        |  Caminho livre, sem teto|
   | A      | Piso 02    | Alternativo(texPisoAlt)| Caminho Livre, sem teto|
  |  T     |  Teto     |  Teto de concreto      |  Renderiza teto sobre piso comun|
 |   C      | Combo      | Piso Alt + Teto        | Renderiza piso alternativo e teto|
  |  L       |Especial   | Lava                  |  Piso com shadder animado|
  |  P      | Spawn       |Jogador               |  Spawn player|

## 📦 Implementação Técnica

# Mapeamento em Camadas (drawLevel)
A função drawLevel foi reestruturada para processar cada tile em "camadas", permitindo que um único caractere dispare a renderização de múltiplos elementos:

Camada de Chão: Verificada para quase todos os caracteres para garantir que não haja "buracos" no mundo.

Camada de Volume (Paredes): Renderizada apenas para os IDs 1 e 2.

Camada Superior (Teto): Ativada condicionalmente para os IDs T e C.

### Sincronização de Colisão (input.cpp)

A função podeAndar utiliza uma fórmula de normalização para converter coordenadas float do mundo 3D em índices int da matriz:

gridX = \lfloor \frac{pos.x - offsetX}{TILE} \rfloor

Isso garante que, independentemente da textura ou da presença de teto, o jogador interaja corretamente com a geometria física.

### 🖼️ Exemplo de mapa implementado 
1111111111    <- Parede de Pedra

1000A00001    <- Mix de Pisos (Comum '0' e Alternativo 'A')

10T000C001    <- Áreas com Teto ('T') e Combo Piso/Teto ('C')

10000000P1    <- 'P' Define o ponto de spawn do jogador 

1222222221    <- Parede de Metal
