#include "graph.h"

std::string Link::fullName() 
{ 
    return _start_port.fullName() + "->" + _end_port.fullName(); 
}