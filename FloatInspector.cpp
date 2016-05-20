#include <iostream>
#include <string>
using namespace std;

string createHeader(string title, int bits)
{
  int total_width = (bits * 3)      // Each column contains the bit + 2 chars of spacing on each side
                  + (bits - 1)      // Take into account the pipes separating each column
                  - title.length(); // The padding needs to factor in the title length
	                
  int left_pad  = total_width / 2;
  int right_pad = total_width - left_pad;
	
  return string(left_pad, ' ') + title + string(right_pad, ' ');
}

int main() 
{
  float f = 1.1f;    // Change this float to see it's underlying bits!
  int i = *(int*)&f; // Reinterpret the bits of the float into an int
	
  cout << '|' << createHeader("+", 1)
       << '|' << createHeader("exponent", 8)
       << '|' << createHeader("mantissa", 23)
       << "|\n| ";
	
  for (int bit = 31; bit >= 0; --bit)
  {
    cout << ((i >> bit) & 1) << " | ";
  }
}

// Example Output:
// Input: f = 1.1f;
//
// | + |           exponent            |                                         mantissa                                          |
// | 0 | 0 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 | 0 | 1 | 1 | 0 | 0 | 1 | 1 | 0 | 0 | 1 | 1 | 0 | 0 | 1 | 1 | 0 | 1 |
