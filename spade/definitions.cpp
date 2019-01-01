#include "definitions.h"
#include <algorithm>
#include <iostream>

atom_set_t freq_items;
support_t support_threshold = 2;
ByteStatus s_options;

std::ostream& output = std::cout;
using namespace std;
//************************EventEventJoin************************************************
//PB join PD
void atom_t::EventEventJoin(const atom_t& btom,atom_t &result_atom) const
{
  
  event_set_t::iterator new_set;
  id_list_t::const_iterator a_sequence = id_list.begin();
  id_list_t::const_iterator b_sequence;
  id_t sid;

  for( ; a_sequence != id_list.end(); ++a_sequence)
  {
    //pokusi sa najst stejnou sekvenci v btomu
    sid = (*a_sequence).first;
    b_sequence = btom.id_list.find(sid);
    if(b_sequence == btom.id_list.end())
      continue;

    //vyberie si mnoziny eids 
   const  event_set_t& a_set = (event_set_t&)(*a_sequence).second;
   const  event_set_t& b_set = (event_set_t&)(*b_sequence).second;

    event_set_t result_set;
    //hodi do id_listu prazdnu mnozinu eventu a potom ju napli
    //  tak zabrani zbytocnemu prekopirovaniu mnoziny..
    result_atom.id_list.insert(make_pair(sid,result_set));
    
    //find set in map of id_list: 
    //  find vracia iterator do mapy a ten si dereferencuje co mu vrati pair(sid,event_set)
    //  dalej z toho vyberie event_set a podchyti si ho referenciou
    event_set_t &result_set_ref = (*(result_atom.id_list.find(sid))).second;
    
  
    event_set_t::const_iterator a_itr = a_set.begin(), a_end =  a_set.end();
    event_set_t::const_iterator b_itr = b_set.begin(), b_end = b_set.end();        

    //urobi prunik zoradenych mnozin
    while( a_itr!=a_end && b_itr!=b_end )
    {
      if (*a_itr<*b_itr) ++a_itr;
       else if (*b_itr<*a_itr) ++b_itr;
       else { 
         result_set_ref.insert(*a_itr++); 
         b_itr++; 
       }    
    }

    if(result_set_ref.empty())   
      result_atom.id_list.erase(sid);  
  
  }//end for
}

//***************************EventSequenceJoin*****************************************
//PB join P->D
void atom_t::EventSequenceJoin(const atom_t& btom,atom_t &result_atom) const
{
  id_list_t::const_iterator a_sequence = id_list.begin();
  id_list_t::const_iterator b_sequence;
  
  id_t sid;

  for( ; a_sequence != id_list.end(); ++a_sequence)
  {
    //pokusi sa najst stejnou sekvenci v btomu
    sid = (*a_sequence).first;
    b_sequence = btom.id_list.find(sid);
    if(b_sequence == btom.id_list.end())
      continue;

    //vyberie si mnoziny eids
    const event_set_t& a_set = (*a_sequence).second;
    const event_set_t& b_set = (*b_sequence).second;

    event_set_t result_set;
    //hodi do id_listu prazdnu mnozinu eventu a potom ju napli
    //  tak zabrani zbytocnemu prekopirovaniu mnoziny..
    id_list_t::iterator new_set_itr = result_atom.id_list.insert(make_pair(sid,result_set)).first;
    //TODO: da sa to urobit aj cez insert vsetko..insert vrati pair z ktoreho si vyberiem iterator do mapy atd...

    //find a set in map of id_list and get the set by reference
    // find vracia iterator do mapy a ten si dereferencuje co mu vrati pair(sid,event_set)
    //  dalej vyberie event_set z toho pairu a prichyti si ho referenciou do result_set_ref    
    event_set_t &result_set_ref = (*new_set_itr).second;

    for(event_set_t::const_iterator a_eid = a_set.begin(); a_eid != a_set.end(); ++a_eid)
    {
      //pre kazde a_eid najde upper bound v b_set a prekopiruje zbytok do result_set
      //najde v b_set hranicu kde od ktorej su vsetky itemy vacsie ako a_eid
      event_set_t::const_iterator b_eid = b_set.upper_bound(*a_eid);
      //kazde b_eid vlozi do vyslednej sety
      for( ;b_eid != b_set.end(); ++b_eid)
        result_set_ref.insert(*b_eid);   
    }//end for a_eid

    if(result_set_ref.empty())   
      result_atom.id_list.erase(sid);
    
  }//end for id_list

}
//********************SequenceSequenceJoin****************************************
//P->B join P->D
void atom_t::SequenceSequenceJoin(const atom_t&btom, atom_t &result_seq_a, atom_t &result_seq_b, atom_t &result_event) const
{
  id_list_t::const_iterator a_sequence = id_list.begin();
  id_list_t::const_iterator b_sequence;
  
  id_t sid;

  for( ; a_sequence != id_list.end(); ++a_sequence)
  {
    //pokusi sa najst stejnou sekvenci v btomu
    sid = (*a_sequence).first;
    b_sequence = btom.id_list.find(sid);
    if(b_sequence == btom.id_list.end())
      continue;

    //vyberie si mnoziny eids
    const event_set_t& a_set = (*a_sequence).second;
    const event_set_t& b_set = (*b_sequence).second;
    event_set_t::const_iterator a_eid = a_set.begin(), a_end = a_set.end();
    event_set_t::const_iterator b_eid = b_set.begin(), b_end = b_set.end();
    event_set_t tmp_atom_set;

    //vytvori mnoziny eid v id_listu pre sid pro kazdy vysledny atom
    id_list_t::iterator a_newset_itr    = result_seq_a.id_list.insert(make_pair(sid,tmp_atom_set)).first;
    id_list_t::iterator b_newset_itr    = result_seq_b.id_list.insert(make_pair(sid,tmp_atom_set)).first;
    id_list_t::iterator evnt_newset_itr = result_event.id_list.insert(make_pair(sid,tmp_atom_set)).first;

    //podchyti si vytvorene mnoziny z mapy id_listu aby ich potom zbytocne nekopiroval
    event_set_t &result_set_a     = (*a_newset_itr).second;
    event_set_t &result_set_b     = (*b_newset_itr).second;
    event_set_t &result_set_event = (*evnt_newset_itr).second;


    for( ; a_eid != a_end; ++a_eid)
    {
      for(b_eid = b_set.begin() ; b_eid != b_end; ++b_eid)
      {
        if( *a_eid < *b_eid) 
        {
          //vlozi do mnoziny eids sekvence sid id_listu vysledneho atomu pre P->B->D eid z atomu P->D
          result_set_a.insert(*b_eid);       
        }
        else
          if( *a_eid > *b_eid) 
          {
            //vlozi do mnoziny eids sekvence sid id_listu vysledneho atomu pre P->D->B eid z atomu P->B
            result_set_b.insert(*a_eid);                 
          }
          else
          {
            //*a_eid == *b_eid
            //vlozi do mnoziny eids sekvence sid id_listu vysledneho atomu P->BD jedno z eid(aeid == beid)
            result_set_event.insert(*a_eid);                                 
          }      
              
      }//end for b_eid    
    }//end for a_eid

    if(result_set_a.empty())
      result_seq_a.id_list.erase(sid);

    if(result_set_b.empty())
      result_seq_b.id_list.erase(sid);

    if(result_set_event.empty())
      result_event.id_list.erase(sid);

  }//end for a_sequence

}

//****************spocita support id listu***************************
//sucet vsetkych kluc nachadzajucich sa v mape..
support_t atom_t::Support() const
  {    
    return id_list.size();
	}

//**************Print*************
//vytiskne id_list pre dany item
void atom_t::Print(ostream& out) const
{
  id_list_t::const_iterator itr = id_list.begin(), end = id_list.end();
  char *typ;
  if(atom_type == c_event_atom)
    typ = (char*)"evn";
  else
    typ = (char*)"seq";

	out << item << typ << "||" ;
	for(; itr != end; ++itr)
	{
   for(event_set_t::const_iterator sitr = (*itr).second.begin(); sitr != (*itr).second.end(); ++sitr  )
	    out << "(" << (*itr).first << "," << (*sitr) << ")";
	}
}

//******************operator<***********************************
//zoraduje najprv podla typu atomu(even_atom,sequence_atom) a potom podla samotnych itemov/atomov
bool atom_t::operator<(const atom_t &b) const
{
  if(atom_type == b.atom_type)
    return item < b.item;

  return atom_type < b.atom_type;
}

//*********************operator==**********************************
bool atom_t::operator==(const atom_t &b) const
{
  const atom_t& this_atom = *this;
  
  if(this_atom < b == false && b < this_atom == false)
    return true;

  return false;
}

