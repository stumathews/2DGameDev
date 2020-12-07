#include "game_exception.h"
using namespace std;

game_exception::game_exception(const string message, const string subsystem) : base_exception(message, subsystem)
{
	
}
