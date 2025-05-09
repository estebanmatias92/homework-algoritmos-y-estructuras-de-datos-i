#include <iostream>
using namespace std;

// Posibles estados del drone
enum EstadoDrone {
    EN_TIERRA,
    EN_VUELO,
    ESTADO_DESCONOCIDO
};

// Variables de estado
EstadoDrone estadoActual = EN_TIERRA;
int velocidad = 0;  // Velocidad inicial

// Funciones básicas (solo muestran estado)
void despegar() {
    cout << "[DESPEGAR] Estado actual: " 
         << "Despegando!!" << endl;
}

void aterrizar() {
    cout << "[ATERRIZAR] Estado actual: " 
         << "Aterrizando!!" << endl;
}

void acelerar() {
    cout << "[ACELERAR] Velocidad actual: " << "Acelerando!!" << endl;
}

void frenar() {
    cout << "[FRENAR] Velocidad actual: " << "Frenando!!" << endl;
}

// 1. Menú interactivo
void mostrarMenu() {
    cout << "\n=== CONTROL DE DRONE ===" << endl << endl;
    cout << "1. Despegar" << endl;
    cout << "2. Aterrizar" << endl;
    cout << "3. Acelerar" << endl;
    cout << "4. Frenar" << endl;
    cout << "5. Salir" << endl;
    cout << "Elije una opción (1-5): ";
}

int main() {
    int opcion = 0;
    bool salir = false;

    do {
        // Limpiar antes de retornar al menu
        system("cls||clear");
        
        // Mostrar menu principal
        mostrarMenu();
        cin >> opcion;

        cout << endl;
        
        // Limpiar buffer en caso de entrada no válida
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Entrada inválida. Intenta nuevamente." << endl;
            continue;
        }

        switch (opcion) {
            case 1:
                despegar();
                break;
            case 2:
                aterrizar();
                break;
            case 3:
                acelerar();
                break;
            case 4:
                frenar();
                break;
            case 5:
                salir = true;
                cout << "Saliendo del sistema..." << endl;
                break;
            default:
                cout << "Opción no válida. Intenta nuevamente." << endl;
        }

        // Pausar para ver resultados
        cout << endl << "Presiones enter para continuar...";
        cin.ignore().get();;
        

    } while (!salir);
    
    return 0;
}
