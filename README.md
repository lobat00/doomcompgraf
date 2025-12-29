## Sistema de Colisão e Mapeamento de Câmera

Este documento descreve a implementação da lógica de colisão e o sistema de conversão de coordenadas para o projeto "Um dia vai ser DOOM".

## Lógica de Conversão (Mundo para Grid)

O maior desafio técnico foi garantir que a posição da câmera(camX, camZ) correspondesse exatamente ao índice correto na matriz de dados do mapa. Como o mapa é desenhado de forma centralizada, utilizamos a seguinte lógica matemática:

A Fórmula: Para encontrar o índice do tile, aplicamos:

gridX = \lfloor \frac{pos.x - offsetX}{TILE} \rfloor
gridZ = \lfloor \frac{pos.z - offsetZ}{TILE} \rfloor


+ offsetX / offsetZ: Calculados como -(largura_mapa * TILE) / 2. 

Isso alinha o sistema de colisão com o sistema de coordenadas do OpenGL onde o centro do mundo é (0,0,0).

+ TILE: Definido como 4.0f para garantir que a escala do movimento seja proporcional ao tamanho dos blocos desenhados.

```cpp

bool podeAndar(float x, float z) {
    // 1. Obtemos as métricas do mapa
    float TILE = 4.0f;
    LevelMetrics m = LevelMetrics::fromMap(gMap, TILE);
    // 2. Convertemos a posição do mundo (x, z) para coordenadas de grid (col, row)
    int gridX, gridZ;
    
    // Implementação da fórmula: mapX = floor(playerPos.x / blockSize) + (mapWidth / 2)
    gridX = (int)floor((x / TILE) + (gMap.getWidth() / 2.0f));
    gridZ = (int)floor((z / TILE) + (gMap.getHeight() / 2.0f));

    //bounds check
    // Verifica se as coordenadas estão dentro dos limites do mapa
    if (gridZ < 0 || gridZ >= (int)gMap.getHeight() || gridX < 0) return false;
    // Verifica se gridX está dentro da largura da linha correspondente
    const auto &data = gMap.data();
    if (gridX >= (int)data[gridZ].size()) return false;

    // 3. Verificamos o caractere na célula correspondente do mapa
    char celula = data[gridZ][gridX];

    // Se a célula for '1' (Pedra) OU '2' (Metal), o jogador NÃO pode andar
    if (celula == '1' || celula == '2') {
        return false;
    }

    // Para qualquer outro caractere (0, T, A, C, L, B), o caminho está livre
    return true; 
}

```
# Margem de Segurança (Hitbox do Player)

Para evitar que a câmera entre nas paredes ou veja através delas (clipping), implementamos uma Margem de Segurança (simulando o corpo do jogador).

No arquivo input.cpp, a função atualizaMovimento() não verifica apenas a posição central da câmera, mas sim a posição futura somada a uma margem:

```cpp
flòat margem = 1.5f;
float cX = (tentX > 0) ? tentX + margem : tentX - margem;
float cZ = (tentZ > 0) ? tentZ + margem : tentZ - margem;

if (podeAndar(camX + cX, camZ)) { camX += tentX; }
if (podeAndar(camX, camZ + cZ)) { camZ += tentZ; }

```
 garante que, se o jogador estiver a menos de 1.5 unidades de uma parede, o movimento naquele eixo seja bloqueado.

### Verificação de Tipos de Bloco (podeAndar)

A função podeAndar é o "porteiro" do movimento. Ela acessa a estrutura MapLoader e verifica o caractere na célula destino:

Bloqueio Total: Caracteres '1' (Pedra) e '2' (Metal).

Caminho Livre: Caracteres '0', 'A', 'T', 'C', 'L' e '9' (Spawn).

ForaFora do Mapa: Qualquer coordenada fora dos limites da matriz retorna false por segurança.

como demonstrado abaixo:


```cpp
//bounds check
    // Verifica se as coordenadas estão dentro dos limites do mapa
    if (gridZ < 0 || gridZ >= (int)gMap.getHeight() || gridX < 0) return false;
    // Verifica se gridX está dentro da largura da linha correspondente
    const auto &data = gMap.data();
    if (gridX >= (int)data[gridZ].size()) return false;

    // 3. Verificamos o caractere na célula correspondente do mapa
    char celula = data[gridZ][gridX];

    // Se a célula for '1' (Pedra) OU '2' (Metal), o jogador NÃO pode andar
    if (celula == '1' || celula == '2') {
        return false;
    }

    // Para qualquer outro caractere (0, T, A, C, L, B), o caminho está livre
    return true; 
}

```
### Sliding Collisionsion


garantir uma navegação fluida pelos corredores do mapa, o sistema de colisão processa os eixos X e Z de forma independente. Isso permite que o jogador deslize pelas superfícies das paredes ao colidir em ângulo, evitando a interrupção do movimento


Em vez de verificar se a posição final (X + dx, Z + dz) é válida, o código faz duas verificações separadas:Teste o eixo X: "Se eu mover apenas no eixo X, eu colido?" Se não, atualize camX.Teste o eixo Z: "Se eu mover apenas no eixo Z, eu colido?" Se não, atualize camZ.

//

```cpp


// Tenta mover no X (Independente)
if (podeAndar(camX + cX, camZ)) {
    camX += tentX;
}

// Tenta mover no Z (Independente)
if (podeAndar(camX, camZ + cZ)) {
    camZ += tentZ;
}

```

O componente de movimento para a direita é cancelado porque podeAndar(camX + cX, camZ) retorna false.

O componente de movimento para frente continua válido porque podeAndar(camX, camZ + cZ) retorna true.
