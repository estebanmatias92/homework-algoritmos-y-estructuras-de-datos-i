#include <iostream>  // For standard input/output operations (cout, cin, cerr)
#include <string>    // For std::string
#include <fstream>   // For file stream operations (ofstream, ifstream)
#include <limits>    // For std::numeric_limits (used with ignore)
#include <iomanip>   // For output formatting (setw, left, right, fixed, setprecision)
#include <algorithm> // For std::find_if (used for trimming whitespace)
#include <cctype>    // For std::isspace (used for trimming whitespace)

// App settings
const int MAX_GUESTS = 100; // Maximum limit for guests in the array
const std::string DB_CONNECTION = "guests.txt"; // File name for persistent guest data

// Define Guest Statuses
enum GuestStatus {
    PENDING_CONFIRMATION = 1, // Mapped to 1 for intuitive user input
    CONFIRMED = 2,
    WAITLISTED = 3,
    CANCELLED = 4
};

// Define the Guest data structure
struct Guest
{
    unsigned int ID = 0; // Auto-incremented, unique identifier
    short int ticket = 0; // Ticket number
    std::string name = "Unknown"; // Guest's name (can contain spaces)
    unsigned short int phone = 0; // Guest's phone number
    GuestStatus status = PENDING_CONFIRMATION; // Default status
};

// Define the GuestArray structure to hold multiple guests
struct GuestArray
{
    Guest items[MAX_GUESTS];
    int length = 0; // Current number of guests in the array
};

// Global variable for auto-incrementing ID
unsigned int next_guest_id = 1;


// --- Utility Functions ---

/**
 * @brief Clears the input buffer.
 * Essential after reading numbers or single words with '>>' and before using 'getline',
 * to consume leftover newline characters.
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

// --- Domain/Core Logic Functions ---

/**
 * @brief Reads a complete Guest from the console input.
 * Prompts the user for each field. Name is entered first and acts as the 'X' sentinel.
 * Automatically assigns an ID.
 *
 * @return A Guest struct populated with data. On termination ('X' for name) or failure,
 * returns a default-constructed Guest and sets std::cin.fail().
 */
Guest read_guest_from_console()
{
    Guest guest_item;
    int status_choice; // Used for reading numeric choice for enum status

    // Name is entered first and acts as the 'X' sentinel
    std::cout << "  Name (can include spaces, X to finish): ";
    std::getline(std::cin, guest_item.name); // Read name first

    // Check for sentinel 'X' or general input failure
    if (guest_item.name == "X") // User explicitly entered 'X'
    {
        std::cin.setstate(std::ios::failbit); // Set failbit to signal termination to caller
        return Guest(); // Return a default-constructed Guest
    }
    else if (std::cin.fail()) // Other input stream error
    {
        // Do NOT clear cin's failbit here. Let the caller handle it.
        return Guest(); // Return a default-constructed Guest to signal failure
    }

    // Assign auto-incrementing ID after a valid name is entered
    guest_item.ID = next_guest_id++;

    std::cout << "  Ticket Number: ";
    std::cin >> guest_item.ticket;
    if (std::cin.fail()) {
        std::cerr << "[ERROR_READ_GUEST]: Failed to read ticket number from console for Name '" << guest_item.name << "'." << std::endl;
        // Do NOT clear cin's failbit here. Let the caller handle it.
        clear_input_buffer(); // Consume newline after ticket read by operator>>
        return Guest();
    }
    clear_input_buffer(); // Consume newline after ticket read by operator>>

    std::cout << "  Phone (numeric): ";
    std::cin >> guest_item.phone;
    if (std::cin.fail()) {
        std::cerr << "[ERROR_READ_GUEST]: Failed to read phone from console for Name '" << guest_item.name << "'." << std::endl;
        // Do NOT clear cin's failbit here. Let the caller handle it.
        clear_input_buffer(); // Consume newline after phone read by operator>>
        return Guest();
    }
    clear_input_buffer(); // Consume newline after phone read by operator>>

    std::cout << "  Status (1: PENDING, 2: CONFIRMED, 3: WAITLISTED, 4: CANCELLED): ";
    std::cin >> status_choice;
    if (std::cin.fail()) {
        std::cerr << "[ERROR_READ_GUEST]: Failed to read status choice from console for Name '" << guest_item.name << "'." << std::endl;
        // Do NOT clear cin's failbit here. Let the caller handle it.
        clear_input_buffer(); // Consume newline after status_choice read by operator>>
        return Guest();
    }
    clear_input_buffer(); // Consume newline after status_choice read by operator>>

    // Assign status based on user's numeric choice
    switch (status_choice) {
        case 1: guest_item.status = PENDING_CONFIRMATION; break;
        case 2: guest_item.status = CONFIRMED;            break;
        case 3: guest_item.status = WAITLISTED;           break;
        case 4: guest_item.status = CANCELLED;            break;
        default:
            std::cout << "  Invalid status choice. Setting to PENDING_CONFIRMATION." << std::endl;
            guest_item.status = PENDING_CONFIRMATION; // Default for invalid input
            break;
    }

    return guest_item;
}

/**
 * @brief Reads a complete Guest from a file input stream.
 * Matches the file's storage format (ID TICKET PHONE STATUS_INT NAME - space-separated).
 * The name is read last using getline to handle spaces and acts as the 'X' sentinel.
 *
 * @param input_file The input file stream, passed by reference (modified).
 * @return A Guest struct populated with data. On termination ('X' for name) or failure,
 * returns a default-constructed Guest and sets input_file.fail().
 */
Guest read_guest_from_file(std::ifstream& input_file)
{
    Guest guest_item;
    int status_int; // To read status as an integer

    // Attempt to read the ID first. If it fails, it might be the 'X' sentinel.
    input_file >> guest_item.ID;

    if (input_file.fail()) // If reading ID failed (e.g., hit EOF, or 'X' was encountered)
    {
        // Try to read the entire line as a string to check for the "X" sentinel
        std::string line_content;
        input_file.clear(); // Clear the fail bit to allow reading string
        std::getline(input_file, line_content); // Read the rest of the current line (which might be "X")
        trim_leading_whitespace(line_content);  // Remove any leading spaces

        // Set fail bit to signal termination by 'X'
        if (line_content == "X") { input_file.setstate(std::ios::failbit); }

        // If it's not 'X' and input failed, it's a genuine read error or unexpected EOF
        // The failbit is already set by the initial `input_file >> guest_item.ID;`
        return Guest(); // Return default-constructed Guest
    }

    input_file >> guest_item.ticket;
    if (input_file.fail()) { std::cerr << "[ERROR_READ_GUEST]: Failed to read ticket from file for ID '" << guest_item.ID << "'." << std::endl; return Guest(); }

    input_file >> guest_item.phone;
    if (input_file.fail()) { std::cerr << "[ERROR_READ_GUEST]: Failed to read phone from file for ID '" << guest_item.ID << "'." << std::endl; return Guest(); }

    input_file >> status_int;
    if (input_file.fail()) { std::cerr << "[ERROR_READ_GUEST]: Failed to read status from file for ID '" << guest_item.ID << "'." << std::endl; return Guest(); }
    
    // Convert integer back to enum, handle invalid values gracefully
    if (status_int >= PENDING_CONFIRMATION && status_int <= CANCELLED)
    {
        guest_item.status = static_cast<GuestStatus>(status_int);
    } 
    else 
    {
        guest_item.status = PENDING_CONFIRMATION; // Default to pending if invalid status
    }

    // After reading the last numeric field (status_int) with operator>>,
    // the stream cursor is at the whitespace before the name.
    // std::getline will read from here to the end of the line, including leading spaces.
    // We then trim these leading spaces.
    std::getline(input_file, guest_item.name); // Reads the name, which should be the rest of the line
    
    // Check for 'X' sentinel after reading the name
    // This case should ideally be caught by the initial check for `X` as a standalone line.
    // However, if the file has data like "1 2 3 4 X", this would still catch it.
    if (guest_item.name == "X") 
    {
        input_file.setstate(std::ios::failbit); // Set fail bit to signal termination
        return Guest(); // Return default-constructed Guest
    }

    // Trim any leading whitespace that might have been picked up by getline
    // if there was a space between status_int and name in the file.
    trim_leading_whitespace(guest_item.name);

    if (input_file.fail()) // Check if getline failed for other reasons (e.g., actual EOF instead of 'X')
    {
        std::cerr << "[ERROR_READ_GUEST]: Failed to read name from file for ID '" << guest_item.ID << "'." << std::endl;
        return Guest();
    }

    return guest_item;
}

/**
 * @brief Loads all guests from the guest data file into a GuestArray struct.
 * This function uses the `read_guest_from_file` helper function.
 * Initializes `next_guest_id` based on the highest ID found.
 *
 * @return A GuestArray containing all loaded guests. Returns an empty list on error or if file is empty.
 */
GuestArray load_guests()
{
    std::ifstream input_file(DB_CONNECTION);
    GuestArray guests_in_file;
    unsigned int max_id_found = 0; // To track the highest ID for auto-increment

    if (!input_file.is_open())
    {
        std::cerr << "[ERROR_LOAD_GUESTS]: File not found or could not be opened: '" << DB_CONNECTION << "'." << std::endl;
        // next_guest_id remains 1 if file not found
        return guests_in_file;
    }

    while (true)
    {
        Guest temp_guest = read_guest_from_file(input_file);

        if (input_file.fail())
        {
            if (input_file.eof())
            {
                std::cout << "[INFO_LOAD_GUESTS]: Reached end of file." << std::endl;
            }
            else // Format error or 'X' sentinel encountered
            {
                std::cout << "[INFO_LOAD_GUESTS]: Guest list load terminated by sentinel 'X' or format error." << std::endl;
            }

            break; // Exit loop on 'X' or any read error
        }

        if (guests_in_file.length >= MAX_GUESTS)
        {
            std::cout << "[WARNING_LOAD_GUESTS]: Maximum guest limit reached (" << MAX_GUESTS << "). Cannot load more guests." << std::endl;
            break;
        }

        guests_in_file.items[guests_in_file.length] = temp_guest;
        guests_in_file.length++;

        // Update max_id_found
        if (temp_guest.ID > max_id_found) max_id_found = temp_guest.ID;
    }

    input_file.close();
    // Set next_guest_id for auto-increment
    next_guest_id = max_id_found + 1;
    // Ensure ID starts at 1 if no guests were loaded or max_id_found was 0 initially.
    if (next_guest_id == 0 || max_id_found == 0) next_guest_id = 1;

    return guests_in_file;
}

/**
 * @brief Writes details of a single guest to an output file stream.
 * All fields are written on a single line, separated by spaces. The name is last.
 *
 * @param output_file The output file stream, passed by reference (modified).
 * @param guest_item The Guest struct to write, passed by constant reference.
 * @return true if the guest details were written successfully, false otherwise.
 */
bool write_guest_to_file(std::ofstream& output_file, const Guest& guest_item)
{
    // Format: ID TICKET PHONE STATUS_INT NAME_WITH_SPACES
    output_file << guest_item.ID << " "
                << guest_item.ticket << " "
                << guest_item.phone << " "
                << static_cast<int>(guest_item.status) << " "
                << guest_item.name << std::endl; // Name is last, gets the rest of the line

    if (output_file.fail())
    {
        std::cerr << "[ERROR_WRITE_DETAILS]: Failed to write data for guest ID '" << guest_item.ID << "'." << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief Saves all guests from the GuestArray to the guest data file.
 * Writes a special sentinel record ('X' in name) at the end.
 *
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

    // Write the 'X' sentinel record (single 'X' on a new line)
    output_file << "X" << std::endl;

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
 * This function uses the `read_guest_from_console` helper function.
 *
 * @param guests_to_add_to A reference to the GuestArray to which new guests will be added (modified).
 * @return true if the collection process completed successfully (or user terminated), false if an error occurred during input.
 */
bool collect_guests_from_user(GuestArray& guests_to_add_to)
{
    std::cout << "\n--- Enter New Guests (Type 'X' for Name to finish) ---" << std::endl;

    // Loop continues until 'X' is entered for name or MAX_GUESTS is reached
    while (guests_to_add_to.length < MAX_GUESTS)
    {
        Guest new_guest = read_guest_from_console();

        // Check if read_guest_from_console set failbit (due to 'X' for name or format error)
        if (std::cin.fail())
        {
            std::cin.clear(); // Clear cin error state (essential after failbit for continued operation)
            std::cout << "--- Guest entry complete. ---" << std::endl;
            break; // Exit the loop
        }
        
        // This check is redundant because the loop condition already covers MAX_GUESTS.
        // It's left for clarity if guests_to_add_to.length might increase unexpectedly within the loop.
        if (guests_to_add_to.length >= MAX_GUESTS)
        {
                std::cout << "[WARNING_INPUT]: Guest list full, cannot add more guests from user input." << std::endl;
                break;
        }

        guests_to_add_to.items[guests_to_add_to.length] = new_guest;
        guests_to_add_to.length++;
    }
    
    return true;
}

/**
 * @brief Displays the guests contained in a GuestArray to the console.
 *
 * @param guests_to_display The GuestArray to display, passed by constant reference.
 *
 * @return void
 */
void display_guests(const GuestArray& guests_to_display)
{
    std::cout << "\n--- Current Guest List ---" << std::endl;

    if (guests_to_display.length == 0)
    {
        std::cout << "The guest list is empty." << std::endl;
        return;
    }

    // Set up formatting for table-like output
    std::cout << std::left << std::setw(6) << "ID"
              << std::left << std::setw(25) << "Name" // Name moved to second column
              << std::left << std::setw(8) << "Ticket"
              << std::left << std::setw(10) << "Phone" // Width adjusted for short int
              << std::left << std::setw(10) << "Status"
              << std::endl;
    std::cout << std::string(79, '-') << std::endl; // Separator line

    for (int i = 0; i < guests_to_display.length; ++i)
    {
        std::cout << std::left << std::setw(6) << guests_to_display.items[i].ID
                  << std::left << std::setw(25) << guests_to_display.items[i].name // Display Name here
                  << std::left << std::setw(8) << guests_to_display.items[i].ticket
                  << std::left << std::setw(10) << guests_to_display.items[i].phone
                  << std::left << std::setw(10) << get_status_string(guests_to_display.items[i].status)
                  << std::endl;
    }

    std::cout << std::string(79, '-') << std::endl;
    std::cout << "Total guests: " << guests_to_display.length << std::endl;
}

// --- Main Function ---
int main()
{
    // 1. Load existing guests from the file into memory.
    // This also initializes 'next_guest_id' based on the highest ID found in the file.
    GuestArray current_guest_list = load_guests();
    std::cout << "\n--- Initial Guest List Loaded ---" << std::endl;
    display_guests(current_guest_list); // Show what was loaded initially

    // 2. Ask for more guests from user input and add them to the current array in memory.
    // 'next_guest_id' will be used to assign IDs to these new guests.
    if (!collect_guests_from_user(current_guest_list))
    {
        std::cerr << "Application terminated due to error during guest input." << std::endl;
        return 1;
    }

    // 3. After finishing guest entry, save the updated guest list from memory back to the file.
    // This overwrites the old file with the combined list (initial + new).
    std::cout << "\n--- Saving Updated Guest List ---" << std::endl;
    if (save_guests(current_guest_list))
    {
        std::cout << "Guest list successfully updated and saved to file." << std::endl;
    }
    else
    {
        std::cerr << "Failed to save updated guest list to file. Data might be lost." << std::endl;
        return 1;
    }

    // 4. Load the final guest list from the file one last time and display it,
    // to confirm that saving and loading are working as expected.
    std::cout << "\n--- Final Guest List from File ---" << std::endl;
    GuestArray final_guest_list_from_file = load_guests();
    display_guests(final_guest_list_from_file);

    return 0;
}
