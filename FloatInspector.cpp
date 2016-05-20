#include <iostream>
#include <bitset>

using namespace std;

int main() 
{
  float f = 1.1f;    // Change this float to see the underlying bits!
  int i = *(int*)&f; // Reinterpret the bits of the float into an int
  
  bitset<1>  sign     { (i >> 31) & 0x00000001 };
  bitset<8>  exponent { (i >> 23) & 0x000000FF };
  bitset<23> mantissa { (i      ) & 0x007FFFFF };
  
  cout << "Input: " << f << endl;
  cout << "| + | exponent |        mantissa         |" << endl;
  cout << "| " << sign << " | " << exponent << " | " << mantissa << " |";
}

// Example Output:
//
// Input: 1.1
// | + | exponent |        mantissa         |
// | 0 | 01111111 | 00011001100110011001101 |
