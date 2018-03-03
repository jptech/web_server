#include <iostream>
#include <cmath>

using namespace std;

int main(int argc, char **argv)
{
  cout << "Hello I am a CGI test script \n";
  if(argc >= 2)
  {
    cout << "The Square-Root of " << atoi(argv[1]) << " is " << sqrt(atoi(argv[1])) << endl;
  }
}