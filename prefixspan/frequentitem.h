#ifndef FREQUENT_ITEM_H
#define FREQUENT_ITEM_H
#include "definitions.h"


class FrequentItem
{
public:
  FrequentItem(){}
  FrequentItem(const item_t& item):m_item(item){}
  FrequentItem(const item_t& item, ByteStatus status):m_item(item),m_status(status){}


  std::string operator*()const ;

  
  bool operator<(const FrequentItem& other) const
  {
    if(m_item == other.m_item)
      return m_status < other.m_status;

    return m_item < other.m_item;
  }

  bool operator==(const event_t &evnt)
  {
     return m_item == evnt.item;
  }

  bool operator!=(const event_t &evnt)
  {
     return m_item != evnt.item;
  }

  bool operator==(const FrequentItem& other)
  {
    const FrequentItem& mine = *this;
    if(mine < other == false && other < mine ==false)  
      return true;

    return false;
  }

  bool operator==(ByteStatus status)
  {
    return m_status == status;
  }
 
  ByteStatus m_status;
private:
 
  item_t m_item;  
};

//mapa kandidatov <FrequentItem,support>
typedef std::map<FrequentItem,int> candidates_map_t;


//vector frekventovanych itemov
typedef std::vector<FrequentItem> freqitems_vector_t;
extern freqitems_vector_t freqItems;

#endif



