#include "thread-pool.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <stdexcept>

using namespace std;
static mutex oslock;

void sleep_for(int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

// Test para verificar que los hilos se cierran correctamente
void testThreadClosure() {
    cout << "Test: Verificar que los hilos se cierran correctamente." << endl;

    {
        ThreadPool pool(3); // Crear un ThreadPool con 3 hilos
        pool.schedule([] {
            cout << "Ejecutando tarea 1." << endl;
            sleep_for(500); // Simular trabajo
        });
        pool.schedule([] {
            cout << "Ejecutando tarea 2." << endl;
            sleep_for(500); // Simular trabajo
        });
        pool.schedule([] {
            cout << "Ejecutando tarea 3." << endl;
            sleep_for(500); // Simular trabajo
        });
        pool.wait(); // Esperar a que todas las tareas terminen
    } // El ThreadPool se destruye aquí

    cout << "Todos los hilos se cerraron correctamente." << endl;
}

// Test para verificar que las tareas se asignan correctamente
void testTaskAssignment() {
    cout << "Test: Verificar que las tareas se asignan correctamente." << endl;

    ThreadPool pool(2); // Crear un ThreadPool con 2 hilos
    pool.schedule([] {
        cout << "Tarea 1 ejecutada por un worker." << endl;
        sleep_for(500); // Simular trabajo
    });
    pool.schedule([] {
        cout << "Tarea 2 ejecutada por un worker." << endl;
        sleep_for(500); // Simular trabajo
    });
    pool.schedule([] {
        cout << "Tarea 3 ejecutada por un worker." << endl;
        sleep_for(500); // Simular trabajo
    });
    pool.wait(); // Esperar a que todas las tareas terminen

    cout << "Todas las tareas se asignaron y ejecutaron correctamente." << endl;
}

void test1() {
    cout << "Test 1: Un worker ejecuta una tarea." << endl;
    ThreadPool pool(2); // Crear 2 workers
    pool.schedule([] {
        cout << "Worker 1 ejecutando tarea." << endl;
        sleep_for(500); // Simular trabajo
        cout << "Worker 1 terminó la tarea." << endl;
    });
    pool.wait(); // Esperar a que termine
    cout << "Test 1 completado." << endl;
}

void test2() {
    cout << "Test 2: Tres workers ejecutan tres tareas simultáneamente." << endl;
    ThreadPool pool(3); // Crear 3 workers
    for (int i = 0; i < 3; ++i) {
        pool.schedule([i] {
            cout << "Worker " << i + 1 << " ejecutando tarea." << endl;
            sleep_for(500); // Simular trabajo
            cout << "Worker " << i + 1 << " terminó la tarea." << endl;
        });
    }
    pool.wait(); // Esperar a que terminen
    cout << "Test 2 completado." << endl;
}

void test3() {
    cout << "Test 3: Un worker ejecuta dos tareas secuenciales." << endl;
    ThreadPool pool(1); // Crear 1 worker
    pool.schedule([] {
        cout << "Worker 1 ejecutando tarea 1." << endl;
        sleep_for(500); // Simular trabajo
        cout << "Worker 1 terminó la tarea 1." << endl;
    });
    pool.schedule([] {
        cout << "Worker 1 ejecutando tarea 2." << endl;
        sleep_for(500); // Simular trabajo
        cout << "Worker 1 terminó la tarea 2." << endl;
    });
    pool.wait(); // Esperar a que terminen
    cout << "Test 3 completado." << endl;
}

void test4() {
    cout << "Test 4: Varios workers ejecutan la misma tarea." << endl;

    ThreadPool pool(4); // Crear un ThreadPool con 4 workers
    auto sharedTask = [] {
        cout << "Worker ejecutando la misma tarea." << endl;
        sleep_for(500); // Simular trabajo
        cout << "Worker terminó la misma tarea." << endl;
    };

    // Programar la misma tarea varias veces
    for (int i = 0; i < 4; ++i) {
        pool.schedule(sharedTask);
    }

    pool.wait(); // Esperar a que todos los workers terminen
    cout << "Test 4 completado: Todos los workers ejecutaron la misma tarea." << endl;
}

void testHundredTasks() {
    const size_t numWorkers = 8;
    const size_t numTasks = 100;

    ThreadPool pool(numWorkers);

    for (size_t i = 0; i < numTasks; ++i) {
        pool.schedule([i]() {
            {
                lock_guard<mutex> guard(oslock);
                cout << "Task " << i << " started\n" ;
            }
            sleep_for(10 + (i % 5) * 5); // tiempo variable simulado
            {
                lock_guard<mutex> guard(oslock);
                cout << "Task " << i << " finished\n";
            }
        });
    }

    pool.wait(); // Esperar a que todas las tareas terminen

    cout << "All " << numTasks << " tasks completed." << endl;
}

void testEmptyQueueError() {
    cout << "Test: Intentar asignar una tarea con la cola vacía." << endl;

    try {
        ThreadPool pool(2); // Crear un ThreadPool con 2 workers

        // No programar ninguna tarea, pero dejar que el dispatcher intente trabajar
        cout << "Esperando un momento para que el dispatcher detecte la cola vacía..." << endl;
        this_thread::sleep_for(chrono::milliseconds(500));

        // El destructor del ThreadPool debería manejar el error
        cout << "Terminando el ThreadPool." << endl;
    } catch (const runtime_error& e) {
        cerr << "Excepción capturada: " << e.what() << endl;
    } catch (...) {
        cerr << "Excepción desconocida capturada." << endl;
    }

    cout << "Test completado: Intentar asignar una tarea con la cola vacía." << endl;
}

int main() {
    testThreadClosure();
    cout << "-----------------------------" << endl;
    testTaskAssignment();
    cout << "-----------------------------" << endl;
    test1();
    cout << "-----------------------------" << endl;
    test2();
    cout << "-----------------------------" << endl;
    test3();
    cout << "-----------------------------" << endl;
    test4(); 
    cout << "-----------------------------" << endl;
    testHundredTasks();
    cout << "-----------------------------" << endl;
    testEmptyQueueError(); // Reemplaza el último test
    return 0;
}