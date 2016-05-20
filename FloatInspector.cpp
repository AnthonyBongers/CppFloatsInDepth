#include <iostream>
#include <bitset>

int main() 
{
  // Change this float to see the underlying bits!
  float f = 1.1f;
  int i = *(int*)&f;
  
  std::bitset<1>  sign     { (i >> 31) & 0x00000001 };
  std::bitset<8>  exponent { (i >> 23) & 0x000000FF };
  std::bitset<23> mantissa { (i      ) & 0x007FFFFF };
  
  std::cout << "Input: " << f << '\n'
            << "| + | exponent |        mantissa         |\n"
            << "| " << sign << " | " << exponent << " | " << mantissa << " |";
}

// Example Output:
//
// Input: 1.1
// | + | exponent |        mantissa         |
// | 0 | 01111111 | 00011001100110011001101 |
