#include <iostream>  // For standard input/output operations (cout, cin, cerr)
#include <string>    // For std::string
#include <fstream>   // For file stream operations (ofstream, ifstream)
#include <limits>    // For std::numeric_limits (used for input buffer clearing)
#include <iomanip>   // For output formatting (setw, left, right)
#include <algorithm> // For std::find_if (used for trimming whitespace)
#include <cctype>    // For std::isspace (used for trimming whitespace)
#include <vector>    // To help with splitting strings, if needed, but avoiding advanced ADTs

// App settings and Global Data
const int MAX_CLIENTS = 100;
const int MAX_PRODUCTS_PER_CLIENT = 20; // Assuming a reasonable maximum for products per client
const std::string DB_CONNECTION = "clients_and_products.txt"; // Renamed database file
const short int CLIENT_CODE_OFFSET = 100; // Starting client code as per requirements (index + 100)

// Enum for Product Categories
enum ProductCategory {
    CLEANING = 1,
    PERSONAL_HYGIENE = 2,
    OTHER = 3
};

// --- Product Structures ---
typedef struct
{
    unsigned int productID = 0;
    std::string productName = "Unknown Product";
    double price = 0.0;
    unsigned int quantity = 0;
    ProductCategory category = OTHER;
} Product;

typedef struct
{
    Product items[MAX_PRODUCTS_PER_CLIENT];
    int length = 0;
} ProductArray;

// --- Client Structures ---
typedef struct
{
    unsigned int clientCode = 0; // Client code (index + 100)
    unsigned int phone = 0;
    std::string email = "N/A";
    std::string clientName = "Unknown";
    ProductArray products; // Nested ProductArray for products belonging to this client
} Client;

typedef struct
{
    Client items[MAX_CLIENTS];
    int length = 0;
} ClientArray;

// Global counters for unique IDs (simplified for this exercise)
// In a more complex app, these might be loaded from file too or managed differently.
unsigned int next_product_id_counter = 1; // Used to assign unique IDs to products

// Function Prototypes
// --- Utility Functions ---
void clear_input_buffer();
void trim_leading_whitespace(std::string& s);
std::string get_product_category_string(ProductCategory category);
void clear_screen();
void press_enter_to_continue();

// --- Product Management Functions (within a client) ---
Product read_product_from_console();
void display_product(const Product& product_item);
void add_product_to_client(Client& client_item);
void delete_product_from_client(Client& client_item);
void modify_product_in_client(Client& client_item);
void manage_client_products_menu(Client& client_item); // New menu for product CRUD

// --- Client Management Functions ---
Client read_client_from_console(int client_index); // Pass index to calculate clientCode
void display_client(const Client& client_item);
void display_clients(const ClientArray& clients_to_display);
void display_one_client(const ClientArray& clients); // New: Listar Uno / Mostrar Uno
int find_client_index_by_code(const ClientArray& clients, unsigned int code);
bool collect_clients_from_user(ClientArray& clients_to_add_to);
void delete_client_by_code(ClientArray& clients);
void modify_client_data(ClientArray& clients);

// --- File I/O Functions ---
// Note: File format will be:
// ClientCode Phone Email ClientName ProductCount
//   ProductID ProductName Price Quantity ProductCategory
//   ProductID ProductName Price Quantity ProductCategory
// ... (products for this client)
// ClientCode Phone Email ClientName ProductCount
//   ... (next client and their products)
// X (sentinel)

Client read_client_and_products_from_file(std::ifstream& input_file);
bool write_client_and_products_to_file(std::ofstream& output_file, const Client& client_item);
ClientArray load_clients_and_products();
bool save_clients_and_products(const ClientArray& clients_to_save);


// --- Application Flow Functions ---
void display_main_menu();
bool handle_main_menu_choice(int choice, ClientArray& current_client_list);
void run_app();


/**
 * @brief Main function, entry point of the program.
 * Calls the run_app function to start the client and product management application.
 * @return 0 on successful execution.
 */
int main()
{
    run_app();
    return 0;
}


// --- Utility Function Definitions ---

/**
 * @brief Clears the input buffer.
 * This is essential after reading numbers or single words with '>>' and before using 'getline',
 * to consume leftover newline characters and prevent unintended skips in input.
 */
void clear_input_buffer()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * @brief Trims leading whitespace from a string.
 * @param s The string to trim.
 */
void trim_leading_whitespace(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

/**
 * @brief Converts a ProductCategory enum value to its string representation for display.
 * @param category The ProductCategory enum value.
 * @return String representation of the category.
 */
std::string get_product_category_string(ProductCategory category) {
    switch (category) {
        case CLEANING:         return "LIMPIEZA";
        case PERSONAL_HYGIENE: return "HIGIENE_PERSONAL";
        case OTHER:            return "OTRO";
        default:               return "DESCONOCIDA";
    }
}

/**
 * @brief Clears the console screen, compatible with multiple operating systems.
 */
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * @brief Pauses execution and waits for the user to press Enter.
 */
void press_enter_to_continue() {
    std::cout << "\nPresione Enter para continuar...";
    clear_input_buffer();
}


// --- Product Management Functions (within a client) ---

/**
 * @brief Reads a complete Product from the console input.
 * Prompts the user for each field. Assigns a unique productID.
 * @return A Product struct populated with data. On input failure,
 * returns a default-constructed Product and sets std::cin.fail().
 */
Product read_product_from_console() {
    Product product_item;
    int category_choice;

    product_item.productID = next_product_id_counter++; // Assign unique ID

    std::cout << "      Nombre del Producto: ";
    std::getline(std::cin, product_item.productName);
    if (std::cin.fail()) {
        std::cerr << "[ERROR]: Error al leer el nombre del producto." << std::endl;
        return Product();
    }

    std::cout << "      Precio: ";
    std::cin >> product_item.price;
    if (std::cin.fail()) {
        std::cerr << "[ERROR]: Fallo al leer el precio del producto '" << product_item.productName << "'." << std::endl;
        clear_input_buffer();
        return Product();
    }

    std::cout << "      Cantidad: ";
    std::cin >> product_item.quantity;
    if (std::cin.fail()) {
        std::cerr << "[ERROR]: Fallo al leer la cantidad del producto '" << product_item.productName << "'." << std::endl;
        clear_input_buffer();
        return Product();
    }
    clear_input_buffer(); // Clear buffer after reading numbers

    std::cout << "      Categoría (1: LIMPIEZA, 2: HIGIENE_PERSONAL, 3: OTRO): ";
    std::cin >> category_choice;
    if (std::cin.fail()) {
        std::cerr << "[ERROR]: Fallo al leer la opción de categoría para el producto '" << product_item.productName << "'." << std::endl;
        clear_input_buffer();
        return Product();
    }
    clear_input_buffer();

    switch (category_choice) {
        case 1: product_item.category = CLEANING;         break;
        case 2: product_item.category = PERSONAL_HYGIENE; break;
        case 3: product_item.category = OTHER;            break;
        default:
            std::cout << "      Opción de categoría inválida. Estableciendo como 'OTRO'." << std::endl;
            product_item.category = OTHER;
            break;
    }
    return product_item;
}

/**
 * @brief Displays the details of a single product in a formatted way.
 * @param product_item The Product struct to display.
 */
void display_product(const Product& product_item) {
    std::cout << std::left << std::setw(12) << product_item.productID
              << std::left << std::setw(30) << product_item.productName
              << std::left << std::setw(10) << std::fixed << std::setprecision(2) << product_item.price
              << std::left << std::setw(10) << product_item.quantity
              << std::left << get_product_category_string(product_item.category)
              << std::endl;
}

/**
 * @brief Adds a new product to a client's product list.
 * @param client_item A reference to the Client struct to add the product to.
 */
void add_product_to_client(Client& client_item) {
    if (client_item.products.length >= MAX_PRODUCTS_PER_CLIENT) {
        std::cout << "\n    [ADVERTENCIA]: El cliente ha alcanzado el límite máximo de productos ("
                  << MAX_PRODUCTS_PER_CLIENT << "). No se pueden añadir más productos." << std::endl;
        return;
    }

    std::cout << "\n    --- Añadir Nuevo Producto para " << client_item.clientName << " ---" << std::endl;
    Product new_product = read_product_from_console();

    if (!std::cin.fail()) {
        client_item.products.items[client_item.products.length] = new_product;
        client_item.products.length++;
        std::cout << "\n    Producto '" << new_product.productName << "' añadido con éxito al cliente "
                  << client_item.clientName << "." << std::endl;
    } else {
        std::cout << "\n    Error al añadir el producto. Revise los datos introducidos." << std::endl;
        std::cin.clear(); // Clear the error flag
        clear_input_buffer(); // Clear the invalid input
    }
}

/**
 * @brief Deletes a product from a client's product list by product ID.
 * @param client_item A reference to the Client struct from which to delete the product.
 */
void delete_product_from_client(Client& client_item) {
    if (client_item.products.length == 0) {
        std::cout << "\n    El cliente '" << client_item.clientName << "' no tiene productos registrados." << std::endl;
        return;
    }

    std::cout << "\n    --- Eliminar Producto de " << client_item.clientName << " ---" << std::endl;
    std::cout << "    Productos actuales:" << std::endl;
    std::cout << std::left << std::setw(12) << "ID Producto"
              << std::left << std::setw(30) << "Nombre"
              << std::left << std::setw(10) << "Precio"
              << std::left << std::setw(10) << "Cantidad"
              << std::left << "Categoría"
              << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    for (int i = 0; i < client_item.products.length; ++i) {
        display_product(client_item.products.items[i]);
    }
    std::cout << std::string(80, '-') << std::endl;

    unsigned int product_id_to_delete;
    std::cout << "    Ingrese el ID del producto a eliminar: ";
    std::cin >> product_id_to_delete;
    if (std::cin.fail()) {
        std::cerr << "    Entrada inválida. Por favor, ingrese un ID de producto válido." << std::endl;
        std::cin.clear();
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int found_index = -1;
    for (int i = 0; i < client_item.products.length; ++i) {
        if (client_item.products.items[i].productID == product_id_to_delete) {
            found_index = i;
            break;
        }
    }

    if (found_index != -1) {
        std::cout << "    ¿Está seguro que desea eliminar el producto '"
                  << client_item.products.items[found_index].productName << "' (ID: "
                  << client_item.products.items[found_index].productID << ")? (s/N): ";
        char confirm_choice;
        std::cin >> confirm_choice;
        clear_input_buffer();

        if (confirm_choice == 's' || confirm_choice == 'S') {
            for (int i = found_index; i < client_item.products.length - 1; ++i) {
                client_item.products.items[i] = client_item.products.items[i + 1];
            }
            client_item.products.length--;
            std::cout << "\n    Producto eliminado con éxito." << std::endl;
        } else {
            std::cout << "\n    Eliminación cancelada." << std::endl;
        }
    } else {
        std::cout << "\n    Producto con ID " << product_id_to_delete << " no encontrado para este cliente." << std::endl;
    }
}

/**
 * @brief Modifies a product in a client's product list by product ID.
 * @param client_item A reference to the Client struct whose product list will be modified.
 */
void modify_product_in_client(Client& client_item) {
    if (client_item.products.length == 0) {
        std::cout << "\n    El cliente '" << client_item.clientName << "' no tiene productos registrados para modificar." << std::endl;
        return;
    }

    std::cout << "\n    --- Modificar Producto de " << client_item.clientName << " ---" << std::endl;
    std::cout << "    Productos actuales:" << std::endl;
    std::cout << std::left << std::setw(12) << "ID Producto"
              << std::left << std::setw(30) << "Nombre"
              << std::left << std::setw(10) << "Precio"
              << std::left << std::setw(10) << "Cantidad"
              << std::left << "Categoría"
              << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    for (int i = 0; i < client_item.products.length; ++i) {
        display_product(client_item.products.items[i]);
    }
    std::cout << std::string(80, '-') << std::endl;

    unsigned int product_id_to_modify;
    std::cout << "    Ingrese el ID del producto a modificar: ";
    std::cin >> product_id_to_modify;
    if (std::cin.fail()) {
        std::cerr << "    Entrada inválida. Por favor, ingrese un ID de producto válido." << std::endl;
        std::cin.clear();
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int found_index = -1;
    for (int i = 0; i < client_item.products.length; ++i) {
        if (client_item.products.items[i].productID == product_id_to_modify) {
            found_index = i;
            break;
        }
    }

    if (found_index != -1) {
        Product& product_ref = client_item.products.items[found_index];
        int choice;

        clear_screen();
        std::cout << "\n    --- Modificando Producto: " << product_ref.productName
                  << " (ID: " << product_ref.productID << ") ---" << std::endl;
        std::cout << "    Datos actuales:" << std::endl;
        std::cout << "      Nombre:     " << product_ref.productName << std::endl;
        std::cout << "      Precio:     " << std::fixed << std::setprecision(2) << product_ref.price << std::endl;
        std::cout << "      Cantidad:   " << product_ref.quantity << std::endl;
        std::cout << "      Categoría:  " << get_product_category_string(product_ref.category) << std::endl;

        std::cout << "\n    ¿Qué desea modificar?" << std::endl;
        std::cout << "    1. Nombre" << std::endl;
        std::cout << "    2. Precio" << std::endl;
        std::cout << "    3. Cantidad" << std::endl;
        std::cout << "    4. Categoría" << std::endl;
        std::cout << "    Ingrese su opción: ";
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cerr << "    Entrada inválida. Por favor, ingrese un número." << std::endl;
            std::cin.clear();
            clear_input_buffer();
            return;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                std::cout << "    Nombre actual: " << product_ref.productName << std::endl;
                std::cout << "    Ingrese nuevo nombre: ";
                std::getline(std::cin, product_ref.productName);
                break;
            case 2:
                std::cout << "    Precio actual: " << std::fixed << std::setprecision(2) << product_ref.price << std::endl;
                std::cout << "    Ingrese nuevo precio: ";
                std::cin >> product_ref.price;
                if (std::cin.fail()) { std::cerr << "    Precio inválido." << std::endl; }
                clear_input_buffer();
                break;
            case 3:
                std::cout << "    Cantidad actual: " << product_ref.quantity << std::endl;
                std::cout << "    Ingrese nueva cantidad: ";
                std::cin >> product_ref.quantity;
                if (std::cin.fail()) { std::cerr << "    Cantidad inválida." << std::endl; }
                clear_input_buffer();
                break;
            case 4:
                int category_choice;
                std::cout << "    Categoría actual: " << get_product_category_string(product_ref.category) << std::endl;
                std::cout << "    Ingrese nueva categoría (1: LIMPIEZA, 2: HIGIENE_PERSONAL, 3: OTRO): ";
                std::cin >> category_choice;
                if (std::cin.fail() || category_choice < 1 || category_choice > 3) {
                    std::cerr << "    Opción de categoría inválida. Categoría no cambiada." << std::endl;
                } else {
                    product_ref.category = static_cast<ProductCategory>(category_choice);
                }
                clear_input_buffer();
                break;
            default:
                std::cout << "    Opción de modificación inválida." << std::endl;
                break;
        }
        std::cout << "\n    Producto modificado con éxito." << std::endl;
    } else {
        std::cout << "\n    Producto con ID " << product_id_to_modify << " no encontrado para este cliente." << std::endl;
    }
}

/**
 * @brief Displays a menu for managing products for a specific client and handles user choices.
 * @param client_item A reference to the Client struct whose products will be managed.
 */
void manage_client_products_menu(Client& client_item) {
    int choice;
    bool managing_products = true;

    while (managing_products) {
        clear_screen();
        std::cout << "\n--- Gestión de Productos para el Cliente: " << client_item.clientName
                  << " (Código: " << client_item.clientCode << ") ---\n" << std::endl;
        std::cout << "  1. Ver Productos del Cliente" << std::endl;
        std::cout << "  2. Añadir Producto" << std::endl;
        std::cout << "  3. Modificar Producto" << std::endl;
        std::cout << "  4. Eliminar Producto" << std::endl;
        std::cout << "  0. Volver al Menú Principal" << std::endl;
        std::cout << "\n  Ingrese su opción: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cerr << "  Entrada inválida. Por favor, ingrese un número." << std::endl;
            std::cin.clear();
            clear_input_buffer();
            press_enter_to_continue();
            continue;
        }
        clear_input_buffer();

        clear_screen();
        switch (choice) {
            case 1:
                std::cout << "\n--- Productos del Cliente: " << client_item.clientName << " ---" << std::endl;
                if (client_item.products.length == 0) {
                    std::cout << "  No hay productos registrados para este cliente." << std::endl;
                } else {
                    std::cout << std::left << std::setw(12) << "ID Producto"
                              << std::left << std::setw(30) << "Nombre"
                              << std::left << std::setw(10) << "Precio"
                              << std::left << std::setw(10) << "Cantidad"
                              << std::left << "Categoría"
                              << std::endl;
                    std::cout << std::string(80, '-') << std::endl;
                    for (int i = 0; i < client_item.products.length; ++i) {
                        display_product(client_item.products.items[i]);
                    }
                    std::cout << std::string(80, '-') << std::endl;
                }
                press_enter_to_continue();
                break;
            case 2:
                add_product_to_client(client_item);
                press_enter_to_continue();
                break;
            case 3:
                modify_product_in_client(client_item);
                press_enter_to_continue();
                break;
            case 4:
                delete_product_from_client(client_item);
                press_enter_to_continue();
                break;
            case 0:
                managing_products = false;
                std::cout << "\nVolviendo al menú principal." << std::endl;
                break;
            default:
                std::cout << "\nOpción inválida. Por favor, inténtelo de nuevo." << std::endl;
                press_enter_to_continue();
                break;
        }
    }
}

// --- Client Management Functions ---

/**
 * @brief Reads a complete Client from the console input.
 * Prompts the user for client details. The clientCode is automatically generated.
 * @param client_index The current index in the client array, used to calculate clientCode.
 * @return A Client struct populated with data. On input failure, returns a default-constructed Client.
 */
Client read_client_from_console(int client_index) {
    Client client_item;

    client_item.clientCode = CLIENT_CODE_OFFSET + client_index; // Client code = 100 + array index

    std::cout << "\n  Nombre del Cliente: ";
    std::getline(std::cin, client_item.clientName);
    if (std::cin.fail()) {
        std::cerr << "[ERROR]: Error al leer el nombre del cliente." << std::endl;
        return Client();
    }

    std::cout << "  Teléfono: ";
    std::cin >> client_item.phone;
    if (std::cin.fail()) {
        std::cerr << "[ERROR]: Fallo al leer el teléfono del cliente '" << client_item.clientName << "'." << std::endl;
        clear_input_buffer();
        return Client();
    }
    clear_input_buffer();

    std::cout << "  Email: ";
    std::cin >> client_item.email;
    if (std::cin.fail()) {
        std::cerr << "[ERROR]: Fallo al leer el email del cliente '" << client_item.clientName << "'." << std::endl;
        clear_input_buffer();
        return Client();
    }
    clear_input_buffer();

    // Initialize products array for the new client
    client_item.products.length = 0;
    std::cout << "\n  Cliente con código " << client_item.clientCode << " creado. Ahora puede añadir productos." << std::endl;

    // Offer to add products immediately after creating a client
    char add_products_now;
    std::cout << "  ¿Desea añadir productos a este cliente ahora? (s/N): ";
    std::cin >> add_products_now;
    clear_input_buffer();
    if (add_products_now == 's' || add_products_now == 'S') {
        manage_client_products_menu(client_item); // Call product management menu
    }

    return client_item;
}

/**
 * @brief Displays the details of a single client, including their products.
 * @param client_item The Client struct to display.
 */
void display_client(const Client& client_item) {
    std::cout << "\n--- Detalles del Cliente ---" << std::endl;
    std::cout << "  Código de Cliente: " << client_item.clientCode << std::endl;
    std::cout << "  Nombre:            " << client_item.clientName << std::endl;
    std::cout << "  Teléfono:          " << client_item.phone << std::endl;
    std::cout << "  Email:             " << client_item.email << std::endl;

    std::cout << "\n  Productos Adquiridos (" << client_item.products.length << "):" << std::endl;
    if (client_item.products.length == 0) {
        std::cout << "    Este cliente no tiene productos registrados." << std::endl;
    } else {
        std::cout << std::left << std::setw(12) << "ID Producto"
                  << std::left << std::setw(30) << "Nombre"
                  << std::left << std::setw(10) << "Precio"
                  << std::left << std::setw(10) << "Cantidad"
                  << std::left << "Categoría"
                  << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        for (int i = 0; i < client_item.products.length; ++i) {
            display_product(client_item.products.items[i]);
        }
        std::cout << std::string(80, '-') << std::endl;
    }
}

/**
 * @brief Displays all clients contained in a ClientArray to the console in a formatted table.
 * Includes a summary of products for each client.
 * @param clients_to_display The ClientArray to display, passed by constant reference.
 */
void display_clients(const ClientArray& clients_to_display)
{
    std::cout << "\n--- Lista de Clientes Actual ---\n" << std::endl;

    if (clients_to_display.length == 0)
    {
        std::cout << "La lista de clientes está vacía." << std::endl;
        return;
    }

    std::cout << std::left << std::setw(10) << "Código"
              << std::left << std::setw(30) << "Nombre Cliente"
              << std::left << std::setw(15) << "Teléfono"
              << std::left << std::setw(25) << "Email"
              << std::left << std::setw(15) << "Productos"
              << std::endl;
    std::cout << std::string(95, '-') << std::endl;

    for (int i = 0; i < clients_to_display.length; ++i)
    {
        std::cout << std::left << std::setw(10) << clients_to_display.items[i].clientCode
                  << std::left << std::setw(30) << clients_to_display.items[i].clientName
                  << std::left << std::setw(15) << clients_to_display.items[i].phone
                  << std::left << std::setw(25) << clients_to_display.items[i].email
                  << std::left << clients_to_display.items[i].products.length << " items"
                  << std::endl;
    }

    std::cout << std::string(95, '-') << std::endl;
    std::cout << "Total de clientes: " << clients_to_display.length << std::endl;
}

/**
 * @brief Searches for a client by their code and displays their details.
 * This is the new "Listar Uno" / "Mostrar Uno" functionality.
 * @param clients The ClientArray to search within.
 */
void display_one_client(const ClientArray& clients) {
    std::cout << "\n--- Buscar y Mostrar Cliente por Código ---" << std::endl;
    if (clients.length == 0) {
        std::cout << "La lista de clientes está vacía. No hay clientes para buscar." << std::endl;
        return;
    }

    unsigned int code_to_find;
    std::cout << "Ingrese el código del cliente a buscar: ";
    std::cin >> code_to_find;
    if (std::cin.fail()) {
        std::cerr << "Entrada inválida. Por favor, ingrese un código válido." << std::endl;
        std::cin.clear();
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int found_index = find_client_index_by_code(clients, code_to_find);

    if (found_index != -1) {
        display_client(clients.items[found_index]);
    } else {
        std::cout << "\nCliente con código " << code_to_find << " no encontrado." << std::endl;
    }
}

/**
 * @brief Finds a client in the ClientArray by their client code and returns its index.
 * @param clients The ClientArray to search, passed by constant reference.
 * @param code The client code to search for.
 * @return The index of the Client if found, otherwise -1.
 */
int find_client_index_by_code(const ClientArray& clients, unsigned int code) {
    for (int i = 0; i < clients.length; ++i) {
        if (clients.items[i].clientCode == code) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Collects new client data from user input (console) and adds it to an existing ClientArray.
 * Allows continuous input until the user chooses to stop or MAX_CLIENTS is reached.
 * @param clients_to_add_to A reference to the ClientArray to which new clients will be added (modified).
 * @return true if the collection process completed successfully, false if a critical error occurred during input.
 */
bool collect_clients_from_user(ClientArray& clients_to_add_to)
{
    std::cout << "\n--- Ingresar Nuevos Clientes ---\n" << std::endl;
    char continue_adding_choice = 's';

    while (clients_to_add_to.length < MAX_CLIENTS && (continue_adding_choice == 's' || continue_adding_choice == 'S'))
    {
        std::cout << "\n--- Añadiendo Cliente " << (clients_to_add_to.length + 1)
                  << " (Código Auto-asignado: " << (CLIENT_CODE_OFFSET + clients_to_add_to.length) << ") ---\n" << std::endl;
        
        Client new_client = read_client_from_console(clients_to_add_to.length);

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cout << "Entrada inválida detectada para el cliente. Por favor, inténtelo de nuevo o elija detenerse." << std::endl;
        } else {
            clients_to_add_to.items[clients_to_add_to.length] = new_client;
            clients_to_add_to.length++;
        }
        
        if (clients_to_add_to.length < MAX_CLIENTS)
        {
            std::cout << "\n¿Desea continuar añadiendo clientes? (s/N): ";
            std::cin >> continue_adding_choice;
            clear_input_buffer();
        } else {
            std::cout << "\n[ADVERTENCIA]: Se alcanzó el límite máximo de clientes (" << MAX_CLIENTS << "). No se pueden añadir más clientes." << std::endl;
            continue_adding_choice = 'n';
        }
    }
    std::cout << "\n--- Entrada de clientes completa. ---" << std::endl;
    return true;
}

/**
 * @brief Handles the process of deleting a client by their code.
 * Displays the current client list, prompts for a code, asks for confirmation,
 * then removes the client and prepares to save the updated list.
 * @param clients A reference to the ClientArray to modify.
 */
void delete_client_by_code(ClientArray& clients) {
    std::cout << "\n--- Eliminar Cliente ---\n" << std::endl;
    display_clients(clients);

    unsigned int code_to_delete;
    std::cout << "\nIngrese el código del cliente a eliminar: ";
    std::cin >> code_to_delete;
    if (std::cin.fail()) {
        std::cerr << "Entrada inválida. Por favor, ingrese un código válido." << std::endl;
        std::cin.clear();
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int found_index = find_client_index_by_code(clients, code_to_delete);

    if (found_index != -1) {
        std::cout << "\n¿Está seguro que desea eliminar al cliente: "
                  << clients.items[found_index].clientName << " (Código: "
                  << clients.items[found_index].clientCode << ") y todos sus productos? (s/N): ";
        char confirm_choice;
        std::cin >> confirm_choice;
        clear_input_buffer();

        if (confirm_choice == 's' || confirm_choice == 'S') {
            // Algorithm to remove an element from a fixed-size array by shifting subsequent elements
            for (int i = found_index; i < clients.length - 1; ++i) {
                clients.items[i] = clients.items[i + 1];
                // Update client codes to maintain (index + 100) rule
                clients.items[i].clientCode = CLIENT_CODE_OFFSET + i;
            }
            clients.length--; // Reduce the effective length of the array
            std::cout << "\nCliente con código " << code_to_delete << " eliminado con éxito." << std::endl;
        } else {
            std::cout << "\nEliminación cancelada." << std::endl;
        }
    } else {
        std::cout << "\nCliente con código " << code_to_delete << " no encontrado." << std::endl;
    }
}

/**
 * @brief Handles the process of modifying data for an existing client by their code.
 * Displays the current client list, prompts for a code, shows current data,
 * then allows modification of selected fields and saves the updated list.
 * Includes an option to manage the client's products.
 * @param clients A reference to the ClientArray to modify.
 */
void modify_client_data(ClientArray& clients) {
    std::cout << "\n--- Modificar Datos del Cliente ---\n" << std::endl;
    display_clients(clients);

    unsigned int code_to_modify;
    std::cout << "\nIngrese el código del cliente a modificar: ";
    std::cin >> code_to_modify;
    if (std::cin.fail()) {
        std::cerr << "Entrada inválida. Por favor, ingrese un código válido." << std::endl;
        std::cin.clear();
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int found_index = find_client_index_by_code(clients, code_to_modify);

    if (found_index != -1) {
        Client& client_ref = clients.items[found_index];

        clear_screen();
        std::cout << "\n--- Modificando Cliente: " << client_ref.clientName << " (Código: " << client_ref.clientCode << ") ---\n" << std::endl;
        std::cout << "  Datos Actuales:" << std::endl;
        std::cout << "    Nombre:   " << client_ref.clientName << std::endl;
        std::cout << "    Teléfono: " << client_ref.phone << std::endl;
        std::cout << "    Email:    " << client_ref.email << std::endl;
        std::cout << "    Productos: " << client_ref.products.length << " items" << std::endl;

        int choice;
        std::cout << "\n¿Qué desea modificar?" << std::endl;
        std::cout << "1. Nombre" << std::endl;
        std::cout << "2. Teléfono" << std::endl;
        std::cout << "3. Email" << std::endl;
        std::cout << "4. Gestionar Productos del Cliente" << std::endl; // Option to manage products
        std::cout << "\nIngrese su opción: ";
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cerr << "Entrada inválida. Por favor, ingrese un número." << std::endl;
            std::cin.clear();
            clear_input_buffer();
            return;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                std::cout << "  Nombre actual: " << client_ref.clientName << std::endl;
                std::cout << "  Ingrese nuevo nombre: ";
                std::getline(std::cin, client_ref.clientName);
                break;
            case 2:
                std::cout << "  Teléfono actual: " << client_ref.phone << std::endl;
                std::cout << "  Ingrese nuevo teléfono: ";
                std::cin >> client_ref.phone;
                if (std::cin.fail()) { std::cerr << "Teléfono inválido." << std::endl; }
                clear_input_buffer();
                break;
            case 3:
                std::cout << "  Email actual: " << client_ref.email << std::endl;
                std::cout << "  Ingrese nuevo email: ";
                std::cin >> client_ref.email;
                if (std::cin.fail()) { std::cerr << "Email inválido." << std::endl; }
                clear_input_buffer();
                break;
            case 4:
                manage_client_products_menu(client_ref); // Call the product management menu
                break;
            default:
                std::cout << "Opción de modificación inválida." << std::endl;
                break;
        }

        std::cout << "\nDatos del cliente modificados con éxito (o se gestionaron productos)." << std::endl;
    } else {
        std::cout << "\nCliente con código " << code_to_modify << " no encontrado." << std::endl;
    }
}


// --- File I/O Functions ---

/**
 * @brief Reads a complete Client (and its products) from a file input stream.
 * Matches the file's storage format (ClientCode Phone Email ClientName ProductCount,
 * followed by ProductID ProductName Price Quantity ProductCategory lines for each product).
 * Reads until a new client or the 'X' sentinel is encountered.
 * @param input_file The input file stream, passed by reference.
 * @return A Client struct populated with data. On encountering the 'X' sentinel or a read failure,
 * returns a default-constructed Client and sets input_file.fail().
 */
Client read_client_and_products_from_file(std::ifstream& input_file)
{
    Client client_item;
    int product_count;

    // Read client basic data
    input_file >> client_item.clientCode;
    if (input_file.fail()) {
        std::string line_content;
        input_file.clear(); // Clear fail bit to allow reading string
        std::getline(input_file, line_content);
        trim_leading_whitespace(line_content);

        if (line_content == "X") { input_file.setstate(std::ios::failbit); } // Set fail bit to signal termination
        return Client();
    }

    input_file >> client_item.phone;
    if (input_file.fail()) { std::cerr << "[ERROR_READ_CLIENT]: Fallo al leer teléfono para cliente con código '" << client_item.clientCode << "'." << std::endl; return Client(); }

    input_file >> client_item.email;
    if (input_file.fail()) { std::cerr << "[ERROR_READ_CLIENT]: Fallo al leer email para cliente con código '" << client_item.clientCode << "'." << std::endl; return Client(); }

    input_file >> product_count; // Read the number of products for this client
    if (input_file.fail()) { std::cerr << "[ERROR_READ_CLIENT]: Fallo al leer el conteo de productos para cliente con código '" << client_item.clientCode << "'." << std::endl; return Client(); }
    
    std::getline(input_file, client_item.clientName); // Read the rest of the line for client name
    trim_leading_whitespace(client_item.clientName);
    if (input_file.fail()) { std::cerr << "[ERROR_READ_CLIENT]: Fallo al leer nombre para cliente con código '" << client_item.clientCode << "'." << std::endl; return Client(); }

    // Read products for this client
    client_item.products.length = 0; // Reset product array length
    for (int i = 0; i < product_count; ++i) {
        if (client_item.products.length >= MAX_PRODUCTS_PER_CLIENT) {
            std::cerr << "[ADVERTENCIA_READ_CLIENT]: Límite de productos por cliente alcanzado para el cliente "
                      << client_item.clientCode << ". Algunos productos no serán cargados." << std::endl;
            // Skip remaining product lines for this client to avoid reading them as next client's data
            std::string dummy_line;
            std::getline(input_file, dummy_line); // Read and discard the rest of the product lines for this client if limit reached
            continue;
        }

        Product product_item;
        int category_int;

        input_file >> product_item.productID;
        if (input_file.fail()) { std::cerr << "[ERROR_READ_PRODUCT_FILE]: Fallo al leer ID de producto para cliente " << client_item.clientCode << "." << std::endl; break; }

        input_file >> product_item.price;
        if (input_file.fail()) { std::cerr << "[ERROR_READ_PRODUCT_FILE]: Fallo al leer precio de producto " << product_item.productID << "." << std::endl; break; }

        input_file >> product_item.quantity;
        if (input_file.fail()) { std::cerr << "[ERROR_READ_PRODUCT_FILE]: Fallo al leer cantidad de producto " << product_item.productID << "." << std::endl; break; }

        input_file >> category_int;
        if (input_file.fail()) { std::cerr << "[ERROR_READ_PRODUCT_FILE]: Fallo al leer categoría de producto " << product_item.productID << "." << std::endl; break; }
        product_item.category = static_cast<ProductCategory>(category_int);

        std::getline(input_file, product_item.productName); // Read the rest of the line for product name
        trim_leading_whitespace(product_item.productName);
        if (input_file.fail()) { std::cerr << "[ERROR_READ_PRODUCT_FILE]: Fallo al leer nombre de producto " << product_item.productID << "." << std::endl; break; }

        client_item.products.items[client_item.products.length++] = product_item;

        // Update next_product_id_counter if a higher ID is found
        if (product_item.productID >= next_product_id_counter) {
            next_product_id_counter = product_item.productID + 1;
        }
    }

    return client_item;
}

/**
 * @brief Writes details of a single client and all its products to an output file stream.
 * All fields are written on separate lines (or logically grouped), matching the defined file format.
 * @param output_file The output file stream, passed by reference.
 * @param client_item The Client struct to write, passed by constant reference.
 * @return true if the client and product details were written successfully, false otherwise.
 */
bool write_client_and_products_to_file(std::ofstream& output_file, const Client& client_item)
{
    // Write client basic data
    output_file << client_item.clientCode << " "
                << client_item.phone << " "
                << client_item.email << " "
                << client_item.products.length << " " // Write product count
                << client_item.clientName << std::endl; // Client name can have spaces, last on line

    if (output_file.fail())
    {
        std::cerr << "[ERROR_WRITE_CLIENT]: Fallo al escribir datos básicos para el cliente con código '" << client_item.clientCode << "'." << std::endl;
        return false;
    }

    // Write products for this client
    for (int i = 0; i < client_item.products.length; ++i) {
        const Product& product_item = client_item.products.items[i];
        output_file << product_item.productID << " "
                    << std::fixed << std::setprecision(2) << product_item.price << " "
                    << product_item.quantity << " "
                    << static_cast<int>(product_item.category) << " "
                    << product_item.productName << std::endl; // Product name can have spaces, last on line

        if (output_file.fail()) {
            std::cerr << "[ERROR_WRITE_PRODUCT_FILE]: Fallo al escribir datos para el producto ID '"
                      << product_item.productID << "' del cliente '" << client_item.clientCode << "'." << std::endl;
            return false;
        }
    }

    return true;
}

/**
 * @brief Loads all clients (and their associated products) from the data file into a ClientArray struct.
 * Initializes `next_product_id_counter` based on the highest product ID found in the file.
 * If the file does not exist, it prints an informative message and returns an empty ClientArray.
 * @return A ClientArray containing all loaded clients. Returns an empty list on error or if file is empty.
 */
ClientArray load_clients_and_products()
{
    std::ifstream input_file(DB_CONNECTION);
    ClientArray clients_in_file;
    unsigned int max_product_id_found = 0;

    if (!input_file.is_open())
    {
        std::cout << "[INFO]: No se encontraron datos de clientes existentes. Se iniciará una nueva lista." << std::endl;
        return clients_in_file;
    }

    while (true)
    {
        Client temp_client = read_client_and_products_from_file(input_file);

        if (input_file.fail())
        {
            if (!input_file.eof())
            {
                std::cout << "[INFO_LOAD_CLIENTS]: La carga de la lista de clientes finalizó por el centinela 'X' o error de formato." << std::endl;
            }
            break;
        }

        if (clients_in_file.length >= MAX_CLIENTS)
        {
            std::cout << "[ADVERTENCIA_LOAD_CLIENTS]: Se alcanzó el límite máximo de clientes (" << MAX_CLIENTS << "). No se pueden cargar más clientes." << std::endl;
            break;
        }

        clients_in_file.items[clients_in_file.length] = temp_client;
        clients_in_file.length++;

        // Update global next_product_id_counter based on products loaded
        for (int i = 0; i < temp_client.products.length; ++i) {
            if (temp_client.products.items[i].productID >= max_product_id_found) {
                max_product_id_found = temp_client.products.items[i].productID;
            }
        }
    }

    input_file.close();
    next_product_id_counter = max_product_id_found + 1;
    if (next_product_id_counter == 0) next_product_id_counter = 1; // Ensure it starts from at least 1

    return clients_in_file;
}

/**
 * @brief Saves all clients (and their associated products) from the ClientArray to the data file.
 * This function overwrites the existing file and automatically writes an 'X' sentinel at the end.
 * @param clients_to_save The ClientArray to save, passed by constant reference.
 * @return true if saving was successful, false otherwise.
 */
bool save_clients_and_products(const ClientArray& clients_to_save)
{
    std::ofstream output_file(DB_CONNECTION);

    if (!output_file.is_open())
    {
        std::cerr << "[ERROR_SAVE_CLIENTS]: No se pudo abrir el archivo '" << DB_CONNECTION << "' para guardar." << std::endl;
        return false;
    }

    for (int i = 0; i < clients_to_save.length; ++i)
    {
        if (!write_client_and_products_to_file(output_file, clients_to_save.items[i]))
        {
            std::cerr << "[ERROR_SAVE_CLIENTS]: Fallo al guardar el cliente con código '" << clients_to_save.items[i].clientCode << "'. Operación de guardado incompleta." << std::endl;
            output_file.close();
            return false;
        }
    }

    output_file << "X" << std::endl; // Automatically write the 'X' sentinel

    output_file.close();

    if (output_file.fail())
    {
        std::cerr << "[ERROR_SAVE_CLIENTS]: Ocurrió un error al escribir o cerrar el archivo '" << DB_CONNECTION << "'." << std::endl;
        return false;
    }

    return true;
}


// --- Application Flow Functions ---

/**
 * @brief Displays the main menu options to the console.
 */
void display_main_menu() {
    std::cout << "\n--- Sistema de Gestión de Clientes y Productos ---\n" << std::endl;
    std::cout << "1. Mostrar Todos los Clientes" << std::endl;
    std::cout << "2. Añadir Nuevo Cliente" << std::endl;
    std::cout << "3. Eliminar Cliente (por Código)" << std::endl;
    std::cout << "4. Modificar Datos del Cliente y Productos (por Código)" << std::endl;
    std::cout << "5. Buscar y Mostrar Un Cliente (por Código)" << std::endl; // New Option
    std::cout << "0. Salir de la Aplicación" << std::endl;
    std::cout << "\nIngrese su opción: ";
}

/**
 * @brief Handles the execution of a selected main menu option.
 * This function encapsulates the logic for each menu choice,
 * including calls to specific functionality and handling screen clears and pauses.
 * @param choice The integer representing the user's menu choice.
 * @param current_client_list A reference to the ClientArray being managed.
 * @return true if the application should continue running, false if the user chose to exit.
 */
bool handle_main_menu_choice(int choice, ClientArray& current_client_list) {
    switch (choice)
    {
        case 1:
            display_clients(current_client_list);
            press_enter_to_continue();
            return true;
        case 2:
            collect_clients_from_user(current_client_list);
            clear_screen();
            display_clients(current_client_list); // Show updated list
            press_enter_to_continue();
            return true;
        case 3:
            delete_client_by_code(current_client_list);
            press_enter_to_continue();
            return true;
        case 4:
            modify_client_data(current_client_list);
            press_enter_to_continue();
            return true;
        case 5: // New functionality: Display one client
            display_one_client(current_client_list);
            press_enter_to_continue();
            return true;
        case 0:
            std::cout << "\nSaliendo de la aplicación. Guardando la lista de clientes actual..." << std::endl;
            return false;
        default:
            std::cout << "\nOpción inválida. Por favor, inténtelo de nuevo." << std::endl;
            press_enter_to_continue();
            return true;
    }
}


/**
 * @brief The main entry point for the Client and Product Management Application.
 * Handles loading/saving clients and presenting the main menu.
 * Manages screen clearing and pausing for user interaction.
 */
void run_app()
{
    clear_screen();
    ClientArray current_client_list = load_clients_and_products();

    // Initial display after loading
    display_clients(current_client_list);
    press_enter_to_continue();

    int choice;
    bool running = true;

    while (running)
    {
        clear_screen();
        display_main_menu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cerr << "Entrada inválida. Por favor, ingrese un número." << std::endl;
            std::cin.clear();
            clear_input_buffer();
            press_enter_to_continue();
            continue;
        }
        clear_input_buffer();

        clear_screen();
        running = handle_main_menu_choice(choice, current_client_list);
    }

    if (save_clients_and_products(current_client_list))
    {
        std::cout << "Lista de clientes y productos actualizada y guardada con éxito en el archivo." << std::endl;
    }
    else
    {
        std::cerr << "Fallo al guardar la lista de clientes y productos actualizada en el archivo. Los datos podrían perderse." << std::endl;
    }

    press_enter_to_continue();
}
