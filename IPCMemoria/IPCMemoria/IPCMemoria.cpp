#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <stdio.h>


#define BUFFER_SIZE 500

using namespace std;

double time1, timedif;

struct SharedMemory {
    int counter1; // contador da esteira 1
    int counter2; // contador da esteira 2
    int total_weight[BUFFER_SIZE]; // peso de cada item
    int total_items_processed; // quantidade de itens processados
};

int random_weight() {
    return rand() % 10 + 1; // retorna um número aleatório entre 1 e 10
}

int main() {
    // Inicialização da memória compartilhada
    HANDLE file_handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedMemory), TEXT("SharedMemory"));
    if (file_handle == NULL) {
        cerr << "Erro na criacao da memoria compartilhada. Codigo de erro: " << GetLastError() << endl;
        return 1;
    }
    SharedMemory* shared_memory = (SharedMemory*)MapViewOfFile(file_handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
    if (shared_memory == NULL) {
        cerr << "Erro ao mapear a memoria compartilhada. Codigo de erro: " << GetLastError() << endl;
        CloseHandle(file_handle);
        return 1;
    }

    // Inicialização de variáveis
    int item_weight;
    int total_items_processed = 0;

    // Loop principal
    while (true) {
        item_weight = random_weight();
        if (item_weight >= 5) { // Esteira 1
            cout << "Contador esteira 1: " << shared_memory->counter1 + 1 << " Peso do item: " << item_weight << endl;
            Sleep(1000); // Pausa de 2 segundos
            shared_memory->counter1++;
        }
        else { // Esteira 2
            cout << "Contador esteira 2: " << shared_memory->counter2 + 1 << " Peso do item: " << item_weight << endl;
            Sleep(500); // Pausa de 1 segundo
            shared_memory->counter2++;
        }

        // Escrita na memória compartilhada
        shared_memory->total_weight[total_items_processed] = item_weight;
        total_items_processed++;
        shared_memory->total_items_processed = total_items_processed;

        // Atualização do display a cada 2 segundos
        if (total_items_processed % 10 == 0) { // A cada 500 itens
            int pesoTotal = 0;
            for (int i = 0; i < total_items_processed; i++) {
                pesoTotal += shared_memory->total_weight[i];
            }
            cout << "Peso total processado: " << pesoTotal << " Quantidade itens: " << shared_memory->total_items_processed << endl;
        }

        if (shared_memory->counter1 + shared_memory->counter2 >= 500) { //Criterio de parada para a obtencao de resultado de testes
            timedif = (((double)clock()) / CLOCKS_PER_SEC) - time1;
            cout << "Tempo Total " << timedif;
            abort();
        }
    }
    // Liberação da memória compartilhada
    UnmapViewOfFile(shared_memory);
    CloseHandle(file_handle);
    return 0;
}