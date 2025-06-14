#include <iostream>  // For standard input/output operations (cout, cin, cerr)
#include <string>    // For std::string
#include <fstream>   // For file stream operations (ofstream, ifstream)
#include <limits>    // For std::numeric_limits (used for input buffer clearing)
#include <iomanip>   // For output formatting (setw, left, right)
#include <algorithm> // For std::find_if (used for trimming whitespace)
#include <cctype>    // For std::isspace (used for trimming whitespace)

// App settings and Global Data
const int MAX_GUESTS = 100;
const std::string DB_CONNECTION = "guests.txt";
const short int FIRST_TICKET_NUMBER = 100; // Starting ticket number

enum GuestStatus {
    PENDING_CONFIRMATION = 1,
    CONFIRMED = 2,
    WAITLISTED = 3,
    CANCELLED = 4
};

typedef struct
{
    unsigned int ID = 0;
    short int ticket = 0;
    unsigned int phone = 0;
    std::string email = "N/A";
    GuestStatus status = PENDING_CONFIRMATION;
    std::string name = "Unknown";
} Guest;

typedef struct 
{
    Guest items[MAX_GUESTS];
    int length = 0;
} GuestArray;

unsigned int next_guest_id = 1;
short int next_ticket_number = FIRST_TICKET_NUMBER; // Global for auto-incrementing ticket

// Function Prototypes
// --- Utility Functions ---
void clear_input_buffer();
void trim_leading_whitespace(std::string& s);
std::string get_status_string(GuestStatus status);
void clear_screen();
void press_enter_to_continue();

// --- Domain/Core Logic Functions ---
Guest read_guest_from_console();
Guest read_guest_from_file(std::ifstream& input_file);
GuestArray load_guests();
bool write_guest_to_file(std::ofstream& output_file, const Guest& guest_item);
bool save_guests(const GuestArray& guests_to_save);
bool collect_guests_from_user(GuestArray& guests_to_add_to);
void display_guests(const GuestArray& guests_to_display);
int find_guest_index_by_id(const GuestArray& guests, unsigned int id);
int find_guest_index_by_ticket(const GuestArray& guests, short int ticket);
void delete_guest_by_id(GuestArray& guests);
void modify_guest_data(GuestArray& guests);
void update_attendance_status(GuestArray& guests);
void display_menu();
bool handle_menu_choice(int choice, GuestArray& current_guest_list);
void run_app();


/**
 * @brief Main function, entry point of the program.
 * Calls the run_app function to start the guest management application.
 * @return 0 on successful execution.
 */
int main()
{
    run_app();

    return 0;
}


// --- Function Definitions ---

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
 * @brief Converts a GuestStatus enum value to its string representation for display.
 * @param status The GuestStatus enum value.
 * @return String representation of the status.
 */
std::string get_status_string(GuestStatus status) {
    switch (status) {
        case PENDING_CONFIRMATION: return "PENDING";
        case CONFIRMED:            return "CONFIRMED";
        case WAITLISTED:           return "WAITLISTED";
        case CANCELLED:            return "CANCELLED";
        default:                   return "UNKNOWN";
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
    std::cout << "\nPress Enter to continue...";
    clear_input_buffer();
}

/**
 * @brief Reads a complete Guest from the console input.
 * Prompts the user for each field except ticket, which is auto-generated.
 * Automatically assigns an ID using the global next_guest_id.
 * @return A Guest struct populated with data. On input failure,
 * returns a default-constructed Guest and sets std::cin.fail().
 */
Guest read_guest_from_console()
{
    Guest guest_item;
    int status_choice;

    std::cout << "\n  Full Name : ";
    std::getline(std::cin, guest_item.name);

    if (std::cin.fail())
    {
        std::cerr << "[ERROR_READ_GUEST]: Input stream error while reading name." << std::endl;
        return Guest();
    }

    guest_item.ID = next_guest_id++;
    guest_item.ticket = next_ticket_number++; // Auto-assign ticket number

    std::cout << "  Phone: ";
    std::cin >> guest_item.phone;
    if (std::cin.fail()) {
        std::cerr << "[ERROR_READ_GUEST]: Failed to read phone for Name '" << guest_item.name << "'." << std::endl;
        clear_input_buffer();
        return Guest();
    }
    clear_input_buffer();

    std::cout << "  Email: ";
    std::cin >> guest_item.email;
    if (std::cin.fail()) {
        std::cerr << "[ERROR_READ_GUEST]: Failed to read email for Name '" << guest_item.name << "'." << std::endl;
        clear_input_buffer();
        return Guest();
    }
    clear_input_buffer();

    std::cout << "  Status (1: PENDING, 2: CONFIRMED, 3: WAITLISTED, 4: CANCELLED): ";
    std::cin >> status_choice;
    if (std::cin.fail()) {
        std::cerr << "[ERROR_READ_GUEST]: Failed to read status choice for Name '" << guest_item.name << "'." << std::endl;
        clear_input_buffer();
        return Guest();
    }
    clear_input_buffer();

    switch (status_choice) {
        case 1: guest_item.status = PENDING_CONFIRMATION; break;
        case 2: guest_item.status = CONFIRMED;            break;
        case 3: guest_item.status = WAITLISTED;           break;
        case 4: guest_item.status = CANCELLED;            break;
        default:
            std::cout << "  Invalid status choice. Setting to PENDING_CONFIRMATION." << std::endl;
            guest_item.status = PENDING_CONFIRMATION;
            break;
    }

    return guest_item;
}

/**
 * @brief Reads a complete Guest from a file input stream.
 * Matches the file's storage format: ID TICKET PHONE EMAIL STATUS_INT NAME (space-separated).
 * The name is read last using getline to handle spaces.
 * @param input_file The input file stream, passed by reference.
 * @return A Guest struct populated with data. On encountering the 'X' sentinel or a read failure,
 * returns a default-constructed Guest and sets input_file.fail().
 */
Guest read_guest_from_file(std::ifstream& input_file)
{
    Guest guest_item;
    int status_int;

    input_file >> guest_item.ID;

    if (input_file.fail())
    {
        // If reading ID failed, check for 'X' sentinel or actual EOF.
        std::string line_content;
        input_file.clear(); // Clear fail bit to allow reading string
        std::getline(input_file, line_content);
        trim_leading_whitespace(line_content);

        if (line_content == "X") { input_file.setstate(std::ios::failbit); } // Set fail bit to signal termination

        return Guest();
    }

    input_file >> guest_item.ticket;
    if (input_file.fail()) { std::cerr << "[ERROR_READ_GUEST]: Failed to read ticket from file for ID '" << guest_item.ID << "'." << std::endl; return Guest(); }

    input_file >> guest_item.phone;
    if (input_file.fail()) { std::cerr << "[ERROR_READ_GUEST]: Failed to read phone from file for ID '" << guest_item.ID << "'." << std::endl; return Guest(); }

    input_file >> guest_item.email;
    if (input_file.fail()) { std::cerr << "[ERROR_READ_GUEST]: Failed to read email from file for ID '" << guest_item.ID << "'." << std::endl; return Guest(); }

    input_file >> status_int;
    if (input_file.fail()) { std::cerr << "[ERROR_READ_GUEST]: Failed to read status from file for ID '" << guest_item.ID << "'." << std::endl; return Guest(); }
    
    if (status_int >= PENDING_CONFIRMATION && status_int <= CANCELLED)
    {
        guest_item.status = static_cast<GuestStatus>(status_int);
    } 
    else 
    {
        guest_item.status = PENDING_CONFIRMATION;
    }

    // Read the rest of the line for the name.
    std::getline(input_file, guest_item.name);
    
    if (guest_item.name == "X") 
    {
        input_file.setstate(std::ios::failbit);
        return Guest();
    }

    trim_leading_whitespace(guest_item.name); // Remove any leading whitespace

    if (input_file.fail())
    {
        std::cerr << "[ERROR_READ_GUEST]: Failed to read name from file for ID '" << guest_item.ID << "'." << std::endl;
        return Guest();
    }

    return guest_item;
}

/**
 * @brief Loads all guests from the guest data file into a GuestArray struct.
 * Initializes `next_guest_id` and `next_ticket_number` based on the highest values found in the file.
 * If the file does not exist, it prints an informative message and returns an empty GuestArray.
 * @return A GuestArray containing all loaded guests. Returns an empty list on error or if file is empty.
 */
GuestArray load_guests()
{
    std::ifstream input_file(DB_CONNECTION);
    GuestArray guests_in_file;
    unsigned int max_id_found = 0;
    short int max_ticket_found = FIRST_TICKET_NUMBER - 1;

    if (!input_file.is_open())
    {
        std::cout << "[INFO]: No existing guest data found. A new list will be started." << std::endl;
        return guests_in_file;
    }

    while (true)
    {
        Guest temp_guest = read_guest_from_file(input_file);

        if (input_file.fail())
        {
            if (!input_file.eof())
            {
                std::cout << "[INFO_LOAD_GUESTS]: Guest list load terminated by sentinel 'X' or format error." << std::endl;
            }
            break;
        }

        if (guests_in_file.length >= MAX_GUESTS)
        {
            std::cout << "[WARNING_LOAD_GUESTS]: Maximum guest limit reached (" << MAX_GUESTS << "). Cannot load more guests." << std::endl;
            break;
        }

        guests_in_file.items[guests_in_file.length] = temp_guest;
        guests_in_file.length++;

        if (temp_guest.ID > max_id_found) max_id_found = temp_guest.ID;
        if (temp_guest.ticket > max_ticket_found) max_ticket_found = temp_guest.ticket;
    }

    input_file.close();
    next_guest_id = max_id_found + 1;
    if (next_guest_id == 0) next_guest_id = 1;

    next_ticket_number = max_ticket_found + 1;
    if (next_ticket_number < FIRST_TICKET_NUMBER) next_ticket_number = FIRST_TICKET_NUMBER;

    return guests_in_file;
}

/**
 * @brief Writes details of a single guest to an output file stream.
 * All fields are written on a single line, separated by spaces, matching the defined file format.
 * @param output_file The output file stream, passed by reference.
 * @param guest_item The Guest struct to write, passed by constant reference.
 * @return true if the guest details were written successfully, false otherwise.
 */
bool write_guest_to_file(std::ofstream& output_file, const Guest& guest_item)
{
    output_file << guest_item.ID << " "
                << guest_item.ticket << " "
                << guest_item.phone << " "
                << guest_item.email << " "
                << static_cast<int>(guest_item.status) << " "
                << guest_item.name << std::endl;

    if (output_file.fail())
    {
        std::cerr << "[ERROR_WRITE_DETAILS]: Failed to write data for guest ID '" << guest_item.ID << "'." << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief Saves all guests from the GuestArray to the guest data file.
 * This function overwrites the existing file and automatically writes an 'X' sentinel at the end.
 * @param guests_to_save The GuestArray to save, passed by constant reference.
 * @return true if saving was successful, false otherwise.
 */
bool save_guests(const GuestArray& guests_to_save)
{
    std::ofstream output_file(DB_CONNECTION);

    if (!output_file.is_open())
    {
        std::cerr << "[ERROR_SAVE_GUESTS]: Could not open file '" << DB_CONNECTION << "' for saving." << std::endl;
        return false;
    }

    for (int i = 0; i < guests_to_save.length; ++i)
    {
        if (!write_guest_to_file(output_file, guests_to_save.items[i]))
        {
            std::cerr << "[ERROR_SAVE_GUESTS]: Failed to save guest ID '" << guests_to_save.items[i].ID << "'. Save operation incomplete." << std::endl;
            output_file.close();
            return false;
        }
    }

    output_file << "X" << std::endl; // Automatically write the 'X' sentinel

    output_file.close();

    if (output_file.fail())
    {
        std::cerr << "[ERROR_SAVE_GUESTS]: An error occurred while writing or closing the file '" << DB_CONNECTION << "'." << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief Collects new guest data from user input (console) and adds it to an existing GuestArray.
 * Allows continuous input until the user chooses to stop or MAX_GUESTS is reached.
 * @param guests_to_add_to A reference to the GuestArray to which new guests will be added (modified).
 * @return true if the collection process completed successfully, false if a critical error occurred during input.
 */
bool collect_guests_from_user(GuestArray& guests_to_add_to)
{
    std::cout << "\n--- Enter New Guests ---\n" << std::endl;
    char continue_adding_choice = 'y';

    while (guests_to_add_to.length < MAX_GUESTS && (continue_adding_choice == 'y' || continue_adding_choice == 'Y'))
    {
        std::cout << "\n--- Adding Guest " << (guests_to_add_to.length + 1) << " (Auto-assigned Ticket: " << next_ticket_number << ") ---\n" << std::endl;
        Guest new_guest = read_guest_from_console();

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cout << "Invalid input detected for guest. Please try again or choose to stop." << std::endl;
        } else {
            guests_to_add_to.items[guests_to_add_to.length] = new_guest;
            guests_to_add_to.length++;
        }
        
        if (guests_to_add_to.length < MAX_GUESTS)
        {
            std::cout << "\nWould you like to continue adding guests? (y/N): ";
            std::cin >> continue_adding_choice;
            clear_input_buffer();
        } else {
            std::cout << "\n[WARNING]: Maximum guest limit reached (" << MAX_GUESTS << "). Cannot add more guests." << std::endl;
            continue_adding_choice = 'n';
        }
    }
    std::cout << "\n--- Guest entry complete. ---" << std::endl;
    return true;
}

/**
 * @brief Displays the guests contained in a GuestArray to the console in a formatted table.
 * @param guests_to_display The GuestArray to display, passed by constant reference.
 */
void display_guests(const GuestArray& guests_to_display)
{
    std::cout << "\n--- Current Guest List ---\n" << std::endl;

    if (guests_to_display.length == 0)
    {
        std::cout << "The guest list is empty." << std::endl;
        return;
    }

    std::cout << std::left << std::setw(6) << "ID"
              << std::left << std::setw(30) << "Name"
              << std::left << std::setw(8) << "Ticket"
              << std::left << std::setw(15) << "Phone"
              << std::left << std::setw(10) << "Status"
              << std::endl;
    std::cout << std::string(79, '-') << std::endl;

    for (int i = 0; i < guests_to_display.length; ++i)
    {
        std::cout << std::left << std::setw(6) << guests_to_display.items[i].ID
                  << std::left << std::setw(30) << guests_to_display.items[i].name
                  << std::left << std::setw(8) << guests_to_display.items[i].ticket
                  << std::left << std::setw(15) << guests_to_display.items[i].phone
                  << std::left << std::setw(10) << get_status_string(guests_to_display.items[i].status)
                  << std::endl;
    }

    std::cout << std::string(79, '-') << std::endl;
    std::cout << "Total guests: " << guests_to_display.length << std::endl;
}

/**
 * @brief Finds a guest in the GuestArray by their ID and returns its index.
 * @param guests The GuestArray to search, passed by constant reference.
 * @param id The ID to search for.
 * @return The index of the Guest if found, otherwise -1.
 */
int find_guest_index_by_id(const GuestArray& guests, unsigned int id) {
    for (int i = 0; i < guests.length; ++i) {
        if (guests.items[i].ID == id) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Finds a guest in the GuestArray by their ticket number and returns its index.
 * @param guests The GuestArray to search, passed by constant reference.
 * @param ticket The ticket number to search for.
 * @return The index of the Guest if found, otherwise -1.
 */
int find_guest_index_by_ticket(const GuestArray& guests, short int ticket) {
    for (int i = 0; i < guests.length; ++i) {
        if (guests.items[i].ticket == ticket) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Handles the process of deleting a guest by their ID.
 * Displays the current guest list, prompts for an ID, asks for confirmation,
 * then removes the guest and saves the updated list.
 * @param guests A reference to the GuestArray to modify.
 */
void delete_guest_by_id(GuestArray& guests) {
    std::cout << "\n--- Delete Guest ---\n" << std::endl;
    display_guests(guests);

    unsigned int id_to_delete;
    std::cout << "\nEnter the ID of the guest to delete: ";
    std::cin >> id_to_delete;
    if (std::cin.fail()) {
        std::cerr << "Invalid input. Please enter a valid ID." << std::endl;
        std::cin.clear();
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int found_index = find_guest_index_by_id(guests, id_to_delete);

    if (found_index != -1) {
        std::cout << "\nAre you sure you want to delete guest: " 
                  << guests.items[found_index].name << " (ID: " 
                  << guests.items[found_index].ID << ")? (y/N): ";
        char confirm_choice;
        std::cin >> confirm_choice;
        clear_input_buffer();

        if (confirm_choice == 'y' || confirm_choice == 'Y') {
            // Algorithm to remove an element from a fixed-size array by shifting subsequent elements
            for (int i = found_index; i < guests.length - 1; ++i) {
                guests.items[i] = guests.items[i + 1];
            }
            guests.length--; // Reduce the effective length of the array
            std::cout << "\nGuest with ID " << id_to_delete << " deleted successfully." << std::endl;
            if (save_guests(guests)) {
                std::cout << "Guest list saved." << std::endl;
            } else {
                std::cerr << "Failed to save updated guest list." << std::endl;
            }
        } else {
            std::cout << "\nDeletion cancelled." << std::endl;
        }
    } else {
        std::cout << "\nGuest with ID " << id_to_delete << " not found." << std::endl;
    }
}

/**
 * @brief Handles the process of modifying data for an existing guest by their ID.
 * Displays the current guest list, prompts for an ID, shows current data,
 * then allows modification of selected fields and saves the updated list.
 * @param guests A reference to the GuestArray to modify.
 */
void modify_guest_data(GuestArray& guests) {
    std::cout << "\n--- Modify Guest Data ---\n" << std::endl;
    display_guests(guests);

    unsigned int id_to_modify;
    std::cout << "\nEnter the ID of the guest to modify: ";
    std::cin >> id_to_modify;
    if (std::cin.fail()) {
        std::cerr << "Invalid input. Please enter a valid ID." << std::endl;
        std::cin.clear();
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int found_index = find_guest_index_by_id(guests, id_to_modify);

    if (found_index != -1) {
        Guest& guest_ref = guests.items[found_index];
        
        clear_screen();
        std::cout << "\n--- Modifying Guest: " << guest_ref.name << " (ID: " << guest_ref.ID << ") ---\n" << std::endl;
        std::cout << "  Current Data:" << std::endl;
        std::cout << "    Name:   " << guest_ref.name << std::endl;
        std::cout << "    Ticket: " << guest_ref.ticket << std::endl;
        std::cout << "    Phone:  " << guest_ref.phone << std::endl;
        std::cout << "    Email:  " << guest_ref.email << std::endl;
        std::cout << "    Status: " << get_status_string(guest_ref.status) << std::endl;

        int choice;
        std::cout << "\nWhat do you want to modify?" << std::endl;
        std::cout << "1. Name" << std::endl;
        std::cout << "2. Phone Number" << std::endl;
        std::cout << "3. Email" << std::endl;
        std::cout << "4. Status" << std::endl;
        std::cout << "\nEnter your choice: ";
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cerr << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear();
            clear_input_buffer();
            return;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                std::cout << "  Current Name: " << guest_ref.name << std::endl;
                std::cout << "  Enter new name: ";
                std::getline(std::cin, guest_ref.name);
                break;
            case 2:
                std::cout << "  Current Phone Number: " << guest_ref.phone << std::endl;
                std::cout << "  Enter new phone number: ";
                std::cin >> guest_ref.phone;
                if (std::cin.fail()) { std::cerr << "Invalid phone number." << std::endl; }
                clear_input_buffer();
                break;
            case 3:
                std::cout << "  Current Email: " << guest_ref.email << std::endl;
                std::cout << "  Enter new email: ";
                std::cin >> guest_ref.email;
                if (std::cin.fail()) { std::cerr << "Invalid email." << std::endl; }
                clear_input_buffer();
                break;
            case 4:
                int status_choice;
                std::cout << "  Current Status: " << get_status_string(guest_ref.status) << std::endl;
                std::cout << "  Enter new status (1: PENDING, 2: CONFIRMED, 3: WAITLISTED, 4: CANCELLED): ";
                std::cin >> status_choice;
                if (std::cin.fail() || status_choice < 1 || status_choice > 4) {
                    std::cerr << "Invalid status choice. Status not changed." << std::endl;
                } else {
                    guest_ref.status = static_cast<GuestStatus>(status_choice);
                }
                clear_input_buffer();
                break;
            default:
                std::cout << "Invalid modification choice." << std::endl;
                break;
        }

        std::cout << "\nGuest data modified successfully." << std::endl;
        if (save_guests(guests)) {
            std::cout << "Guest list saved." << std::endl;
        } else {
            std::cerr << "Failed to save updated guest list." << std::endl;
        }
    } else {
        std::cout << "\nGuest with ID " << id_to_modify << " not found." << std::endl;
    }
}

/**
 * @brief Confirms or cancels attendance for a guest by their ticket number.
 * Displays the current guest list, prompts for a ticket number, shows current status,
 * then allows updating the status to CONFIRMED or CANCELLED and saves the list.
 * @param guests A reference to the GuestArray to modify.
 */
void update_attendance_status(GuestArray& guests) {
    std::cout << "\n--- Confirm/Cancel Attendance ---\n" << std::endl;
    display_guests(guests);

    short int ticket_number;
    std::cout << "\nEnter the ticket number of the guest: ";
    std::cin >> ticket_number;
    if (std::cin.fail()) {
        std::cerr << "Invalid input. Please enter a valid ticket number." << std::endl;
        std::cin.clear();
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    int found_index = find_guest_index_by_ticket(guests, ticket_number);

    if (found_index != -1) {
        Guest& guest_ref = guests.items[found_index];

        std::cout << "\nGuest found: ID " << guest_ref.ID << ", Name: " << guest_ref.name
                  << ", Current Status: " << get_status_string(guest_ref.status) << std::endl;

        int status_choice;
        std::cout << "  Set new status (2: CONFIRMED, 4: CANCELLED): ";
        std::cin >> status_choice;
        if (std::cin.fail() || (status_choice != CONFIRMED && status_choice != CANCELLED)) {
            std::cerr << "Invalid status choice. Status not changed." << std::endl;
        } else {
            guest_ref.status = static_cast<GuestStatus>(status_choice);
            std::cout << "Attendance status updated to " << get_status_string(guest_ref.status) << "." << std::endl;
            if (save_guests(guests)) {
                std::cout << "Guest list saved." << std::endl;
            } else {
                std::cerr << "Failed to save updated guest list." << std::endl;
            }
        }
        clear_input_buffer();
    } else {
        std::cout << "\nGuest with ticket number " << ticket_number << " not found." << std::endl;
    }
}

/**
 * @brief Displays the main menu options to the console.
 */
void display_menu() {
    std::cout << "\n--- Studio 54 Guest Management System ---\n" << std::endl;
    std::cout << "1. Display All Guests" << std::endl;
    std::cout << "2. Add New Guests" << std::endl;
    std::cout << "3. Delete a Guest (by ID)" << std::endl;
    std::cout << "4. Modify Guest Data (by ID)" << std::endl;
    std::cout << "5. Confirm/Cancel Attendance (by Ticket Number)" << std::endl;
    std::cout << "0. Exit Application" << std::endl;
    std::cout << "\nEnter your choice: ";
}

/**
 * @brief Handles the execution of a selected menu option.
 * This function encapsulates the logic for each menu choice,
 * including calls to specific functionality and handling screen clears and pauses.
 * @param choice The integer representing the user's menu choice.
 * @param current_guest_list A reference to the GuestArray being managed.
 * @return true if the application should continue running, false if the user chose to exit.
 */
bool handle_menu_choice(int choice, GuestArray& current_guest_list) {
    switch (choice)
    {
        case 1:
            display_guests(current_guest_list);
            press_enter_to_continue();
            return true;
        case 2:
            collect_guests_from_user(current_guest_list);
            clear_screen();
            display_guests(current_guest_list);
            press_enter_to_continue();
            return true;
        case 3:
            delete_guest_by_id(current_guest_list);
            press_enter_to_continue();
            return true;
        case 4:
            modify_guest_data(current_guest_list);
            press_enter_to_continue();
            return true;
        case 5:
            update_attendance_status(current_guest_list);
            press_enter_to_continue();
            return true;
        case 0:
            std::cout << "\nExiting application. Saving current guest list..." << std::endl;
            return false;
        default:
            std::cout << "\nInvalid choice. Please try again." << std::endl;
            press_enter_to_continue();
            return true;
    }
}


/**
 * @brief The main entry point for the Studio 54 Guest Management Application.
 * Handles loading/saving guests and presenting the main menu.
 * Manages screen clearing and pausing for user interaction.
 */
void run_app()
{
    clear_screen();
    GuestArray current_guest_list = load_guests();
    
    display_guests(current_guest_list); 
    press_enter_to_continue(); 

    int choice;
    bool running = true;

    while (running)
    {
        clear_screen();
        display_menu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cerr << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear();
            clear_input_buffer();
            press_enter_to_continue();
            continue;
        }
        clear_input_buffer();

        clear_screen();
        running = handle_menu_choice(choice, current_guest_list);
    }

    if (save_guests(current_guest_list))
    {
        std::cout << "Guest list successfully updated and saved to file." << std::endl;
    }
    else
    {
        std::cerr << "Failed to save updated guest list to file. Data might be lost." << std::endl;
    }
    
    press_enter_to_continue();
}
