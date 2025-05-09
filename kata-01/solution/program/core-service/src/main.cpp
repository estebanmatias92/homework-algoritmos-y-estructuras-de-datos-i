#include <iostream>
using namespace std;

// Prototipados
int despegar(int velocidad);
int aterrizar(int velocidad);
int acelerar(int velocidad);
int frenar(int velocidad);
void mostrarMenu();

// Posibles estados del drone
enum EstadoDrone {
    EN_TIERRA,
    EN_VUELO
};

// Variables de estado
EstadoDrone estadoActual = EN_TIERRA;

// Funciones básicas (solo muestran estado)
int despegar(int velocidad) {
    if (estadoActual == EN_TIERRA)
    {
        cout << "[DESPEGAR] Estado actual: "
             << "En tierra" << endl
             << "Velocidad: " << velocidad << "m/s" << endl;
        
        estadoActual = EN_VUELO;
        velocidad += 1;

        cout << "En vuelo" << endl;
    }
    else
    {
        cout << "[DESPEGAR] Error: El dron se encuentra en el aire, no puede despega mas!!" << endl;
    }
    
    cout << "Velocidad: " << velocidad << "m/s" << endl;
    
    return velocidad;
}

int aterrizar(int velocidad) {
    if (estadoActual == EN_VUELO) 
    {
        cout << "[ATERRIZAR] Estado actual: "
             << "En vuelo, aterrizando" << endl
             << "Velocidad: " << velocidad << "m/s" << endl;

        if (velocidad > 0) 
            velocidad = 0;

        estadoActual = EN_TIERRA;
    }        
    else
    {
        cout << "[ATERRIZAR] Error: El dron ya se encuentra en tierra!!" << endl; 
    }
    
    cout << "En tierra" << endl
         << "Velocidad: " << velocidad << "m/s" << endl;

    return velocidad;
}

int acelerar(int velocidad) {
    if (estadoActual == EN_VUELO)
    {
        velocidad += 1;

        cout << "[ACELERAR] Velocidad actual: " 
             << velocidad << "m/s " << endl;
    }
    else
    {
        cout << "[ACELERAR] Error: El dron no esta en aire, no puede acelerar!!" << endl
             << "Velocidad: " << velocidad << "m/s" << endl;
    }

    return velocidad;
}

int frenar(int velocidad) {
    if (estadoActual == EN_VUELO)
    {
        if (velocidad > 0)
        {
            cout << "[FRENAR] Velocidad actual: "
                 << "Velocidad: " << velocidad << "m/s " << endl;
       
            velocidad = 0;
            
            cout << "Frenando" << endl
                 << "Velocidad: " << velocidad << "m/s " << endl;
        }
        else
        {
            cout << "[FRENAR] Error: El dron ya estaba frenado!!" << endl;
        }
    }
    else
    {
        cout << "[FRENAR] Error: El dron esta en el piso, no puede frenar mas!!" << endl;
    }

    return velocidad;
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
    int velocidad = 0;  // Velocidad inicial

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
                velocidad = despegar(velocidad);
                break;
            case 2:
                velocidad = aterrizar(velocidad);
                break;
            case 3:
                velocidad = acelerar(velocidad);
                break;
            case 4:
                velocidad = frenar(velocidad);
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
