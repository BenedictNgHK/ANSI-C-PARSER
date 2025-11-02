#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <list>
using namespace std;
char parseHexToChar(const std::string &input)
{
    // Check if the input starts with "\\x"
    // if (input.size() < 4 || input.substr(0, 2) != "\\x") {
    //     throw std::invalid_argument("Invalid format: expected \\xH or \\xHH");
    // }

    // Extract the hex part (can be 1 or 2 characters after \\x)
    std::string hexStr = input.substr(2);

    // Limit the extracted string to 2 characters
    if (hexStr.size() > 2)
    {
        hexStr = hexStr.substr(0, 2);
    }

    // Convert hex string to integer
    int value;
    std::stringstream ss;
    ss << std::hex << hexStr;
    ss >> value;

    // Return the corresponding character
    return static_cast<char>(value);
}

int main()
{
    list<int> test;
    for (int i = 0; i < 10; i++)
        test.push_back(i);

    auto itr = std::prev(test.end());
    cout << "Initialized itr is" << *itr << endl;
    for (int i = 10; i < 20; i++)
        test.push_back(i);
    cout << "Itr's next is" << *++itr << endl;

    return 0;
}