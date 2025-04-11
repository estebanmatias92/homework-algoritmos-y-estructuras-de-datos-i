#include <iostream>  
#include <string>  // Explicitly include string header

/* Utilizo namespace std para no escribir std::
delante de cada construcción del lenguaje */  
using namespace std;  
  
int main()  
{  
    cout << "Hola ISFT 151!" << endl;

    cout << "Press Enter to exit...";
    cin.get();  // Waits for just one Enter

    return 0;  
}