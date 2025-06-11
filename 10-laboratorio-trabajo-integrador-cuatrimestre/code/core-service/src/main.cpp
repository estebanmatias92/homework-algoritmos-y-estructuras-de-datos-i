#include <iostream>  // For std::cout, std::cin, std::cerr
#include <string>    // For std::string, std::getline
#include <fstream>   // For std::ofstream, std::ifstream
#include <limits>    // For std::numeric_limits
#include <iomanip>   // For std::setw, std::left, std::right, std::fixed, std::setprecision (for display)

// App settings
const int MAX_PRODUCTS = 100;
const std::string DB_INVENTORY = "inventory.txt"; // Persistency settings

// Define the types
typedef struct {
    std::string ID = "0000000A";        // Default ID
    std::string name = "Unknown Product"; // Default name
    int quantity = 0;                   // Default quantity (changed from short int for consistency with main)
    double price = 0.0;                 // Default price
} Product;

typedef struct {
    Product items[MAX_PRODUCTS];    // Array of Product structs
    int length = 0;                 // Number of actual products in the list
} ProductList;


// --- Utility Functions ---

// Function to clear the input buffer.
// Essential after reading numbers/words with '>>' and before using 'getline'.
void clearInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// --- Domain/Core Logic Functions ---

/**
 * @brief Reads details of a single product from an input file stream.
 * Assumes the product's ID has already been read by the caller.
 *
 * @param inFile The input file stream, passed by reference (modified).
 * @param currentID The ID of the product currently being read (already extracted).
 * @param item A reference to the Product struct to populate (modified).
 * @return true if the product details were read successfully, false otherwise.
 */
bool readProductDetails(std::ifstream& inFile, const std::string& currentID, Product& item) {
    item.ID = currentID; // Assign the ID that was already read by the caller

    // Temporary variables for the rest of the data
    int quantity;
    double price;
    std::string productName;

    // Read quantity and price
    inFile >> quantity >> price;

    // Check if reading numbers failed (e.g., end of file, bad data)
    if (inFile.fail()) {
        std::cerr << "[ERROR_READ_DETAILS]: Failed to read quantity or price for ID: '" << currentID << "'." << std::endl;
        return false;
    }

    // Consume the remaining newline character from the buffer before getline
    inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Read the product name (can contain spaces)
    std::getline(inFile, productName);

    // Check if reading the product name failed
    if (inFile.fail()) {
        std::cerr << "[ERROR_READ_DETAILS]: Failed to read product name for ID: '" << currentID << "'." << std::endl;
        return false;
    }

    // Populate the Product struct
    item.quantity = quantity;
    item.price = price;
    item.name = productName;

    return true; // Reading successful
}

/**
 * @brief Loads all products from the inventory file into a ProductList struct.
 *
 * @return A ProductList containing all loaded products. Returns an empty list on error or if file is empty.
 */
ProductList loadInventory() {
    std::ifstream inFile(DB_INVENTORY); // Open the file
    ProductList products;               // Instantiate the list of elements to populate

    // Check if the file exists or could be opened
    if (!inFile.is_open()) {
        std::cerr << "[ERROR_LOAD_INVENTORY]: File not found or could not be opened: '" << DB_INVENTORY << "'." << std::endl;
        return products; // Return an empty list to signal failure
    }

    // Variable to hold the ID read from the file in each iteration
    std::string currentIDFromFile;

    // Attempt to read the very first ID/flag from the file
    inFile >> currentIDFromFile;

    // If the initial read failed (e.g., file is completely empty) or if it's the 'X' flag immediately
    if (inFile.fail() || currentIDFromFile == "X") {
        std::cout << "[INFO_LOAD_INVENTORY]: Inventory file is empty or starts with 'X'. No products to load." << std::endl;
        inFile.close(); // Close the stream
        return products; // Return an empty ProductList
    }

    // Loop through the file and fill the data structure
    // Continue as long as currentIDFromFile is not the end flag 'X' AND the stream is in a good state
    while (currentIDFromFile != "X" && inFile.good()) {
        // Check if we have space in our fixed-size array
        if (products.length >= MAX_PRODUCTS) {
            std::cout << "[WARNING_LOAD_INVENTORY]: Maximum product limit reached (" << MAX_PRODUCTS << "). Cannot load more products." << std::endl;
            break; // Exit the loop if the array is full
        }

        // Call helper function to read details for the current product
        if (!readProductDetails(inFile, currentIDFromFile, products.items[products.length])) {
            std::cerr << "[ERROR_LOAD_INVENTORY]: Error reading product details for ID '" << currentIDFromFile << "'. Inventory load incomplete." << std::endl;
            break; // Exit loop due to format error
        }

        products.length++; // Advance the list length (only if product details were read successfully)

        // Read the ID for the next record, or the 'X' flag, for the next loop iteration
        inFile >> currentIDFromFile;
    }

    inFile.close(); // Close reading stream
    return products; // Return the populated ProductList
}

/**
 * @brief Writes details of a single product to an output file stream.
 *
 * @param outFile The output file stream, passed by reference (modified).
 * @param item The Product struct to write, passed by constant reference.
 * @return true if the product details were written successfully, false otherwise.
 */
bool writeProductDetails(std::ofstream& outFile, const Product& item) {
    // Write the data to the file, separated by spaces and ending with a newline
    outFile << item.ID << " "
            << item.quantity << " "
            << item.price << " "
            << item.name << std::endl;

    // Check if anything went wrong while writing the data to the file
    if (outFile.fail()) {
        std::cerr << "[ERROR_WRITE_DETAILS]: Failed to write data for product ID '" << item.ID << "'." << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Saves all products from the ProductList to the inventory file.
 *
 * @param products The ProductList to save, passed by constant reference.
 * @return true if saving was successful, false otherwise.
 */
bool saveInventory(const ProductList& products) {
    // Open the file stream in write mode (this truncates the file if it exists)
    std::ofstream outFile(DB_INVENTORY);

    // Check if the file could be opened for writing
    if (!outFile.is_open()) {
        std::cerr << "[ERROR_SAVE_INVENTORY]: Could not open file '" << DB_INVENTORY << "' for saving." << std::endl;
        return false;
    }

    // Iterate through the products in memory and save them to the file
    for (int i = 0; i < products.length; ++i) {
        // Call helper function to write a single product
        if (!writeProductDetails(outFile, products.items[i])) {
            // If writing fails for any product, log general error, close file, and return false
            std::cerr << "[ERROR_SAVE_INVENTORY]: Failed to save product ID '" << products.items[i].ID << "'. Save operation incomplete." << std::endl;
            outFile.close(); // Close the file immediately on error
            return false;
        }
    }

    // Write the end-of-list flag "X" to the file, ensuring it's on its own line
    outFile << "X" << std::endl;

    // Close the stream
    outFile.close();

    // Verify if any error occurred during flushing or closing the file
    if (outFile.fail()) {
        std::cerr << "[ERROR_SAVE_INVENTORY]: An error occurred while writing or closing the file '" << DB_INVENTORY << "'." << std::endl;
        return false;
    }

    return true; // Saving successful
}


// --- New Functions for Modularity in Main ---

/**
 * @brief Prompts the user for product data and saves it directly to the inventory file.
 * This effectively replaces the initial data entry loop in main.
 *
 * @return true if the process was completed without file open/write errors, false otherwise.
 */
bool promptAndSaveInitialProducts() {
    std::string ID, productName;
    int quantity;
    double price;

    std::ofstream outFile; // Using a local ofstream for this function
    outFile.open(DB_INVENTORY);

    if (!outFile.is_open()) {
        std::cerr << "[ERROR_PROMPT_SAVE]: Could not open file '" << DB_INVENTORY << "' for initial data entry." << std::endl;
        return false;
    }

    std::cout << "--- Initial Product Entry (Enter 'X' for ID to finish) ---" << std::endl;
    std::cout << "ID del producto: ";
    std::cin >> ID;

    while (ID != "X") {
        clearInputBuffer(); // Clear buffer after reading ID with '>>'

        std::cout << "Product Name: ";
        std::getline(std::cin, productName);

        std::cout << "Quantity: ";
        std::cin >> quantity;

        std::cout << "Price: ";
        std::cin >> price;

        // Writing to file (Product details are space separated, product name at the end)
        outFile << ID << " " << quantity << " " << price << " " << productName << std::endl;

        if (outFile.fail()) {
            std::cerr << "[ERROR_PROMPT_SAVE]: Failed to write product ID '" << ID << "' to file. Data entry stopped." << std::endl;
            outFile.close();
            return false;
        }

        std::cout << "ID del producto (X para terminar): ";
        std::cin >> ID;
    }

    outFile << "X" << std::endl; // Write the end-of-list flag
    outFile.close();

    if (outFile.fail()) {
        std::cerr << "[ERROR_PROMPT_SAVE]: Final error during write or close of '" << DB_INVENTORY << "'." << std::endl;
        return false;
    }

    std::cout << "--- Initial product entry complete. ---" << std::endl;
    return true;
}

/**
 * @brief Displays the products contained in a ProductList to the console.
 *
 * @param products The ProductList to display, passed by constant reference.
 */
void displayProducts(const ProductList& products) {
    if (products.length == 0) {
        std::cout << "\n--- Inventory Display ---" << std::endl;
        std::cout << "No products to show in the inventory." << std::endl;
        return;
    }

    std::cout << "\n--- Current Inventory ---" << std::endl;
    // Set up formatting for table-like output
    std::cout << std::left << std::setw(10) << "ID"
              << std::left << std::setw(25) << "Name"
              << std::right << std::setw(10) << "Quantity"
              << std::right << std::setw(15) << "Price" << std::endl;
    std::cout << std::string(60, '-') << std::endl; // Separator line

    for (int i = 0; i < products.length; ++i) {
        std::cout << std::left << std::setw(10) << products.items[i].ID
                  << std::left << std::setw(25) << products.items[i].name
                  << std::right << std::setw(10) << products.items[i].quantity
                  << std::right << std::setw(15) << std::fixed << std::setprecision(2) << products.items[i].price
                  << std::endl;
    }
    std::cout << std::string(60, '-') << std::endl;
    std::cout << "Total products: " << products.length << std::endl;
}

// --- Main Function ---
int main()
{
    // 1. Prompt user for initial products and save them to the file
    // Check the return value to see if the initial save was successful
    if (!promptAndSaveInitialProducts()) {
        std::cerr << "Application terminated due to error during initial product entry." << std::endl;
        return 1; // Indicate error exit
    }

    // 2. Load products from the file into memory
    ProductList productsInMemory = loadInventory();

    // 3. Display the loaded products
    displayProducts(productsInMemory);

    // Optional: Example of saving the (potentially modified) inventory back
    // (In a real app, this would happen after user makes changes or exits)
    // if (saveInventory(productsInMemory)) {
    //     std::cout << "\nInventory successfully saved back to file." << std::endl;
    // } else {
    //     std::cerr << "\nFailed to save inventory back to file." << std::endl;
    // }


    return 0;
}
