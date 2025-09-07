#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enum para representar os tipos de operações de edição
typedef enum {
    OP_NONE,
    OP_ADJUST_COLORS,
    OP_ADJUST_COLORS_UNDO,
    OP_APPLY_FILTER,
    OP_CROP,
    // Adicione outros tipos de operações conforme necessário
} OperationType;

// Struct par armanazenas os detalhes de uma edição
typedef struct {
    OperationType type;
        // Aqui você pode adicionar parâmetros específicos para cada operação.
        float brightness_value;
} Operation;

typedef struct Node {
    Operation operation;
    struct Node* previous;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* current;
    int maxSize;
    int currentSize;
} History;

void addState(History* history, Operation newOp) {
    // 1 . Alocar memória para o novo nó
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Erro: falha na alocacao de memoria o novo no.\n");
        return;
    }

    // 2. Popular o novo nó com a operação
    newNode->operation = newOp;
    newNode->next = NULL;

    // 3. Conectar o novo nó à lista
    if (history->current ==  NULL) {
        // Se a lista estiver vazia (primeira edição)
        newNode->previous = NULL;
        history->head = newNode;
        history->current = newNode;
    } else {
        // Remover os nós "futuros" (se existirem) antes de adicionar um novo
        Node* temp = history->current->next;
        free(temp);
        temp = nextTemp;
        history->currentSize--; // Reduz o tamanho para cada nó removido
    }

    newNode->previous = history->current;
    history->current->next = newNode;
    history->current = newNode;

    // 4. Aumentar o tamanho do histórico
    history->currentSize++;

    // 5. Verificar o limite do historico
    if (history->currentSize > history->maxSize) {
        // Remover o nó mais antigo (o head)
        Node* oldestNode = history->head;
        history->head = history->head->next; // O segundo nó se torna o novo head
        if (history->head != NULL) {
            history->head->previous = NULL; // O novo head não tem nó anterior
        }

        printf("Limite de historico atingido. Removendo o no mais antigo.\n");
        free(oldestNode);
        history->currentSize--;        {
        }
        
    }

void applyOperation(Operation op) {
    switch (op.type) {
    case OP_ADJUST_COLORS:
        printf("Aplicando: 'Ajustar Cores' (brilho: %.2f)\n", op.brightness_value);
        break;
    case OP_ADJUST_COLORS_UNDO:
        printf("Desfazendo: 'Ajustar Cores' (restaurando brilho)\n");
        break;
    case OP_APPLY_FILTER:
        printf("Aplicando: 'Aplicar filtro'\n");
        break;
    case OP_CROP:
        printf("Aplicando: 'Recorte'\n");
        break;
    default:
    printf("Operacao desconhecia.\n");
        break;
    }
}

// Função de Desfazer
void undo(History* history) {
    if (history->current != NULL && history->current->previous != NULL) {
        // Move o ponteiro 'current' para o nó anterior
        history->current = history->current->previous;
        printf("\nDesfazendo a ultima operacao...\n");

        // Simula a aplicação da operação inversa.
        if (history->current->operation.type == OP_ADJUST_COLORS) {
            Operation undo_op = {OP_ADJUST_COLORS_UNDO, history->current->operation.brightness_value};
            applyOperation(undo_op);
        } else {
            // Se não for uma operação com undo específico,
            // apenas "volte" no histórico
            applyOperation(history->current->operation);
        }

        } else {
            printf("\nNão ha mais operacoes para desfazer.\n");
        }
    }

    //Funcao de Refazer
    void redo(History* history) {
        if (history->current != NULL && history->current->next != NULL) {
            // Move o ponteiro 'curretn' para o próximo nó
            history->current = history->current->next;
            printf("\nRefazendo a operacao...\n");

            // Simula a reaplicacao da operacao.
            // A gente usa a funcao que já criamos.
            applyOperation(history->current->operation);
        } else {
            printf("\nNao ha mais operacoes para refazer.\n");
        }
    }

int main() {
    // 1. Inicilizar o histórico
    History history;
    history.head = NULL;
    history.current = NULL;
    history.maxSize = 3;
    history.currentSize = 0;

    // 2. Simular o inicio de uma edição e adicionar o historico
    printf("--- Simulando edicoes com limite de %d ---\n", history.maxSize);

    // Edicao 1
    Operation op1 = { OP_ADJUST_COLORS, 0.5 };
    addState(&history, op1);
    printf("Adicionado: Ajuste de Cores. Tamanho atual: %d\n", history.currentSize);

    // Edicao 2
    Operation op2 = { OP_APPLY_FILTER, 0.0 };
    addState(&history, op2);
    printf("Adicionado: Aplicar Filtro. Tamanho atual: %d\n", history.currentSize);

    // Edicao 3
    Operation op3 = { OP_CROP, 0.0 };
    addState(&history, op3);
    printf("Adicionado: Recorte. Tamanho atual: %d\n", history.currentSize);

    // Edicao 4 (vai exceder o limite e remover o primeiro nó)
    Operation op4 = { OP_ADJUST_COLORS, 0.8 };
    addState(&history, op4);
    printf("Adicionado: Ajuste de Cores. Tamanho atual: %d\n", history.currentSize);

    // Agora, a primeira operação (op1) foi removida.
    // Vamos testar o 'undo'. O 'head' agora aponta para op2.
    printf("\n--- Testando UNDO apos atingir o limite ---\n");
    undo(&history); // Deve desfazer a op4
    undo(&history); // Deve desfazer a op3

    // Tentar desfazer novamente (deve desfazer op2, que é a primeira agora)
    undo(&history);
    
    // Tentar desfazer mais uma vez (deve falhar, pois chegamos ao inicio)
    undo(&history);

    // Teste do cénrairo "undo e depois nova edicao"
    printf("Simulando nova edicao apos um UNDO:\n");
    undo(&history); // Volta para a edicao do filtro

    // Nova edicao: Ajsute de Contraste (por exemplo)
    Operation op4 = {OP_ADJUST_COLORS, 0.8};
    addState(&history, op4);
    applyOperation(op4);

    // Tentar reafzer (deve falhar, pois o futuro foi "podado")
    redo(&history);

    // 3. Verificar o estado atual
    if (history.current != NULL) {
        printf("\n--- Verificando o estado atual ---\n");
        printf("Operacao atual no historico: %d\n", history.current->operation.type);
    }

    return 0; 
}

