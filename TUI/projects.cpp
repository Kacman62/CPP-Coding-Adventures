#include <iostream>
#include <cmath>
#include <string>

//Collection of basic cpp challenges to improve my coding 

std::string getInput();

int main(){
    std::string output;
    output = getInput();
    std::cout << "Hello " << output << "\n\n";
    return 0;
}

std::string getInput(){
    std::string out;
    std::cout << "Enter your name ";
    std::cin >> out;
    return out;
}

