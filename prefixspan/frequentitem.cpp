#include "frequentitem.h"
#include "definitions.h"
#include <sstream>

using namespace std;



//******************operator******************************
//vrati item v string podobe
string FrequentItem::operator*()const
{
  stringstream ss;
  ss << m_item;
  return ss.str();
}
