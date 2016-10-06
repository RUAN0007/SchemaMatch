#include <iostream>
#ifdef DEBUG
	#define DEBUG_STDERR(x) (std::cerr << (x))
	#define DEBUG_STDOUT(x) (std::cout << "[DEBUG] "<< __FILE__ << "(" << __LINE__ << "): "<< (x) << endl)
 #else 
	#define DEBUG_STDERR(x)
	#define DEBUG_STDOUT(x)
#endif