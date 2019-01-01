#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include "compatible.h"
#include <map>
#include <set>
#include <ostream>
#include <string>

extern std::ostream& output;

typedef unsigned int item_t;
typedef unsigned int id_t;
typedef unsigned int support_t;


//zoznam vyskytu itemu v databazy - vertikalni databaze

//mnozina eventu v sekvenci
typedef std::set<id_t> event_set_t;
//mnozina vyskytu itemu v sekvenciach a ich eventoch
typedef std::map<id_t,event_set_t> id_list_t;

//typ atomu
const ByteStatus::Slot c_event_atom = ByteStatus::S1;
const ByteStatus::Slot c_sequence_atom = ByteStatus::S2;
const ByteStatus::Slot c_bottom_atom = ByteStatus::S3;

//options
const ByteStatus::Slot c_print_freq_seqs = ByteStatus::S1;
const ByteStatus::Slot c_testing = ByteStatus::S2;

extern ByteStatus s_options;


struct atom_t
{
  item_t item;
  ByteStatus atom_type;
  id_list_t id_list;
  //item_t parent_atom; DEPRECATED

	//ctor
  atom_t(item_t item, ByteStatus atype):item(item),atom_type(atype){}

  //operator< pro strict-weak ordering - usporadani
  bool operator<(const atom_t &b) const;

  bool operator==(const atom_t &b) const;

	//vklada do id_list id sekvence a id eventu
  void insert(id_t eid,id_t sid)
  {
    id_list_t::iterator found = id_list.find(sid);
    if( found == id_list.end())
    {
      event_set_t e_set;
      e_set.insert(eid);
      id_list.insert(std::make_pair(sid,e_set));
    }
    else
    {
      ((*found).second).insert(eid);      
    }

  }
  //vraci support
  support_t Support() const;
  void Print(std::ostream& out) const;
  
  void EventEventJoin(const atom_t& btom,atom_t &result_atom)const; 
  void EventSequenceJoin(const atom_t& btom,atom_t &result_atom)const; 
  void SequenceSequenceJoin(const atom_t&btom, atom_t &result_seq_a, atom_t &result_seq_b, atom_t &result_event)const;
};

//***************podedene triedy pre rozne typy atomu
//event atom
struct event_atom_t: public atom_t
{
  //ctor
  event_atom_t(item_t item):atom_t(item,c_event_atom){}
};
//sequence atom
struct sequence_atom_t: public atom_t
{
  //ctor
  sequence_atom_t(item_t item):atom_t(item,c_sequence_atom){}
};



//********vertikalni databaze - usporiadana lexikograficky podla atomov
typedef std::set<atom_t> atom_set_t;

extern atom_set_t freq_items;


//support
extern support_t support_threshold;

//reprezentuje jednu sekvenci//riadok ako item a jeho eid v horizontalnej db
typedef std::multimap<item_t,id_t> sequence_row_t;

// reprezentuje horizontalnu databazu ako sid,sekvence
typedef std::map<id_t,sequence_row_t> horizontal_db_t;

//EQUEVALENCE CLASS
//jedno patro sublattice - ako parentclass(item_t) a jej atomy(atom_set_t)
typedef std::map<item_t,atom_set_t> equivalence_class_t;

#endif

