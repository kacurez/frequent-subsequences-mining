#include "algorithm.h"
#include "interface.h"

#include <iostream>
#include <sstream>

using namespace std;


static bool CheckSupport(equivalence_class_t &result,
                  const atom_t &atom, item_t parent_item);

static void RecountTwoSequences(equivalence_class_t &freq_two_sequences,
                         equivalence_class_t &candidates,
                         id_t sid, id_t eid,
                         item_t parent_item,
                         atom_t atom);

void ComputeTwoSequences(equivalence_class_t& freq_two_seqs);

static void PrintSequence(string &prefix,const atom_t &atom,const atom_t &parent_atom);

static string GenerateSequencePrefix(string &prefix,const atom_t &atom,const atom_t &parent_atom);


//*****************************EnumerateFrequentSequences************************************
void EnumerateFrequentSequences(const atom_t &parent_atom, const atom_set_t &atoms,std::string *prefix)
{
  
  atom_set_t::const_iterator aitr = atoms.begin(), aend = atoms.end(), bitr;
  //tmp_eq_class sluzi na ulozenie atomov vytvorenych pre sequence-sequence temporal join
  //  tj ak mam parent atom P->B a urobim join s P->D tak mi moze vzniknut atom P->D->B ktory 
  //  ma ale parenta P->D a neni mozne ho ulozit do atom_setu pre prave vykonavaneho parent atomu(P->B)
  equivalence_class_t tmp_eq_class;
  equivalence_class_t::iterator found_subclass,tmp_eq_itr;
 

  for( ; aitr != aend; ++aitr)
  {
    const atom_t& atom = *aitr;

    if(parent_atom.atom_type == c_bottom_atom && atom.atom_type == c_sequence_atom)
      continue;

    if(prefix)
      PrintSequence(*prefix,atom,parent_atom);
    
    //mnozina frekventovanych atomov vytvorenych z temporal join atom z ostatnymi atomami
    atom_set_t tmp_atom_set;
    atom_set_t &subclass_atom_set = tmp_atom_set;
    //ak uz dana subclassa existuje tak si ju vyberie
    found_subclass = tmp_eq_class.find(atom.item);
    if(found_subclass  != tmp_eq_class.end())
      subclass_atom_set = (*found_subclass).second;

    for(bitr = aitr ; bitr != aend; ++bitr)
    {
      const atom_t& btom = *bitr;
      if(atom.atom_type == c_event_atom && btom.atom_type == c_event_atom && !(atom == btom) )
      {
        //PB join PD ==> PBD
        atom_t result_atom = event_atom_t(btom.item);
        //vlozi do mnoziny atomov result_atom a zoberie si referenciu nanho
        atom_t &result_atom_ref =(atom_t &) *(subclass_atom_set.insert(result_atom).first);        
        //urobi temporal join
        atom.EventEventJoin(btom,result_atom_ref);   
        //ak vysledny atom nema pozadovanu support tak ho z vyslednej mnoziny vymaze
        //  vysledny atom sa neprekopiruvava ak ma pozadovanu support ale odstrani ak ju nema
        if(result_atom_ref.Support() < support_threshold)
          subclass_atom_set.erase(result_atom_ref);

      }
      else
      if(atom.atom_type == c_event_atom && btom.atom_type == c_sequence_atom)
      {
        //PB join P->D ==> PB->D
        atom_t result_atom = sequence_atom_t(btom.item);
        //vlozi do mnoziny atomov result_atom a zoberie si referenciu nanho
        atom_t &result_atom_ref =(atom_t &) *(subclass_atom_set.insert(result_atom).first);
        //ak vysledny atom nema pozadovanu support tak ho z vyslednej mnoziny vymaze
        //  vysledny atom sa neprekopiruvava ak ma pozadovanu support ale odstrani ak ju nema
        atom.EventSequenceJoin(btom,result_atom_ref);

        if(result_atom_ref.Support() < support_threshold)
          subclass_atom_set.erase(result_atom_ref);
      }
      else
       if(atom.atom_type == c_sequence_atom && btom.atom_type == c_sequence_atom && (atom.item != btom.item))
      {      
        //P->B join P->D ==> P->B->D,P->D->B,P->BD

        //*****P->B->D
        atom_t result_seq_a = sequence_atom_t(btom.item);
        atom_t &result_a_ref =(atom_t &) *(subclass_atom_set.insert(result_seq_a).first);
        //*****P->D->B
        //vytvori set(alt_subclass) pre alternativny atom pre ineho parenta(btom.item) a vlozi do mapy tmp_eq_class
        tmp_eq_itr = tmp_eq_class.insert(make_pair(btom.item,atom_set_t())).first;
        atom_set_t &alt_subclass = (*tmp_eq_itr).second;
        atom_t result_seq_b = sequence_atom_t(atom.item);
        //vlozi novy atom do alternativnej atom_set(alt_subclass)
        atom_t &result_b_ref =(atom_t &) *(alt_subclass.insert(result_seq_b).first);

        //*****P->BD
        atom_t result_event = event_atom_t(btom.item);
        atom_t &result_event_ref =(atom_t &) *(subclass_atom_set.insert(result_event).first);

        atom.SequenceSequenceJoin(btom, result_a_ref, result_b_ref, result_event_ref);
        
        if(result_a_ref.Support() < support_threshold)
          subclass_atom_set.erase(result_a_ref);

        if(result_b_ref.Support() < support_threshold)
          alt_subclass.erase(result_b_ref);

        if(result_event_ref.Support() < support_threshold)
          subclass_atom_set.erase(result_event_ref);        
      }
      else
        if(/*atom.atom_type == c_sequence_atom && btom.atom_type == c_sequence_atom && */(atom.item == btom.item))
        {
          //P->A join P->A
          atom_t result_atom = sequence_atom_t(btom.item);
          //vlozi do mnoziny atomov result_atom a zoberie si referenciu nanho
          atom_t &result_atom_ref =(atom_t &) *(subclass_atom_set.insert(result_atom).first);
          //ak vysledny atom nema pozadovanu support tak ho z vyslednej mnoziny vymaze
          //  vysledny atom sa neprekopiruvava ak ma pozadovanu support ale odstrani ak ju nema
          atom.EventSequenceJoin(btom,result_atom_ref);
  
          if(result_atom_ref.Support() < support_threshold)
            subclass_atom_set.erase(result_atom_ref);
        }
      
    }//end for bitr

    if(subclass_atom_set.empty() == false)
    {
       if(prefix)
       {
         string tmp_prefix = GenerateSequencePrefix(*prefix,atom,parent_atom);
         EnumerateFrequentSequences(atom,subclass_atom_set,&tmp_prefix);
       }
       else
         EnumerateFrequentSequences(atom,subclass_atom_set);
    }
    //ak bola subclassa vytiahnuta z tmp_eq_class tak sa vymaze 
    // inac sa subclassa vymaze autmaticky pri dalsej iteracii(dufam aspon:)..)
    if(found_subclass  != tmp_eq_class.end())
      tmp_eq_class.erase(found_subclass);      
    
  }//end for aitr

}

//*************************Spade****************************
//neprepocitava frekventovane 2-sekvenceale rovno vola Enumerate na freq_items(1-sekvence)
//hlavna verzia - prechadza cely lattice DO HLBKY
void Spade2()
{
  atom_t atom(0,c_bottom_atom);
  
  atom_set_t complete_freq_items;
  atom_set_t::const_iterator f_itr= freq_items.begin(), f_end = freq_items.end();
  for( ;f_itr != f_end; ++f_itr )
  {
    atom_t tmp = (*f_itr);
    tmp.atom_type = c_sequence_atom;
    complete_freq_items.insert(tmp);
    complete_freq_items.insert(*f_itr);  
  }

  freq_items.clear();

  
  if(s_options == c_print_freq_seqs && !(s_options == c_testing))
  {
    string prefix;
    EnumerateFrequentSequences(atom,complete_freq_items,&prefix);
  }
  else
    EnumerateFrequentSequences(atom,complete_freq_items,NULL);
    
  freq_items.clear();
}

//*********************************Spade**************************************
//DEPRECATED!!!!
//verzia s kompletnym vytvorenim prveho patra lattice struktury t.j. do sirky(BFS approach) - zabera strasne vela pamate + casu
void Spade()
{
  //1. found frequent items or 1-sequences 
  //  to sa uz uurobi pri nacitani databazy

  //2. generate frequent 2-sequences

  equivalence_class_t freq_two_seqs;
  //PrintFreqOneSeqs(cout);
  ComputeTwoSequences(freq_two_seqs);

  //3. for all equeivalence classe do enumaretefrequensequences
  equivalence_class_t::const_iterator eq_itr = freq_two_seqs.begin();
  equivalence_class_t::const_iterator eq_end = freq_two_seqs.end();
  
  for( ; eq_itr != eq_end; ++eq_itr)
  {
    if(s_options == c_print_freq_seqs)
    {
    
    string prefix;
    //dodelat prefix
    EnumerateFrequentSequences(event_atom_t((*eq_itr).first), (*eq_itr).second,&prefix);
    }
    else
      EnumerateFrequentSequences(event_atom_t((*eq_itr).first), (*eq_itr).second,NULL);
  }

  //DONE!!

}

//******************ComputeTwoSequences*****************************
//vygeneruje  frekventovane 2-sekvencie z freq_items
//DEPRECATED - vid Spade()
void ComputeTwoSequences(equivalence_class_t& freq_two_seqs)
{

  horizontal_db_t horizontal_db;
  GeneretaHorizontalDb(horizontal_db);
  //PrintHorizontalDb(horizontal_db, cout);

  equivalence_class_t candidates;
  freq_two_seqs.clear();
  //tu sa kludne mozem zbavit freq_items pretoze uz ich mam v inej forme v horizontal_db
  //   aby zbytocne nezaberali pamat..
  freq_items.clear();
  horizontal_db_t::const_iterator hitr = horizontal_db.begin(), hend = horizontal_db.end();
  id_t sid,eid,j_eid,j_item;
  sequence_row_t::const_iterator ipair,jpair,iend,jend,seq_end;
  item_t item;
  bool was_different = true,next_was_different = true;


  for( ;hitr != hend; ++hitr)
  {
    const sequence_row_t& sequence = (*hitr).second;
    sid = (*hitr).first;
    ipair = sequence.begin();
    seq_end = sequence.end();

    eid = (*ipair).second;
    item = (*ipair).first;
    ++ipair;
    was_different = true;
    if( ipair != seq_end )
        next_was_different = !(item == (*ipair).first);

    for( ;ipair != seq_end; ++ipair)
    {
      for(jpair = ipair ; jpair != seq_end; ++jpair)
      {
        j_eid = (*jpair).second;
        j_item = (*jpair).first;

        //ak druhykrat prechadze stejny item ako je j_item tak sa preskoci lebo uz v kandidatoch je
        /*if(j_item == item && was_different == false)
          continue;*/

        if(eid < j_eid)
          RecountTwoSequences(freq_two_seqs, candidates, sid, j_eid, item, sequence_atom_t(j_item));
          //vyberie sa item ako parent a j_item sa vlozi do jeho  atomov
        else
        if( eid > j_eid)
          RecountTwoSequences(freq_two_seqs, candidates, sid, eid, j_item, sequence_atom_t(item));
          //vyberie sa j_item ako parent a item sa vklada do jeho atomov
        else
        {
          //eid == j_eid
          if( j_item < item)
            RecountTwoSequences(freq_two_seqs, candidates, sid, eid, j_item, event_atom_t(item));
            //vyberie sa j_item ako parent a item sa vklada ako jeho atom
          else
          if( item < j_item)
            RecountTwoSequences(freq_two_seqs, candidates, sid, eid, item, event_atom_t(j_item));
            //vyberie sa item ako parent a j_item sa vklada do jeho atomov
          else
            throw MyException("V eventu jedny sekvence su stejne itemy: item,eid == j_item,j_edi");
        }

      }//end for jpair

      eid = (*ipair).second;
      item = (*ipair).first;
      was_different = next_was_different;
    }//end for ipair

  }//end for horizontal_db


}
//*****************************RecountTwoSequences*******************************************
//prepocita support noveho atomu a popripade vlozi do vyslednej freq_two_sequences
//DEPRECATED - vid Spade()
static void RecountTwoSequences(equivalence_class_t &freq_two_sequences,
                         equivalence_class_t &candidates,
                         id_t sid, id_t eid,
                         item_t parent_item,
                         atom_t atom)
{
  equivalence_class_t::iterator itr_item = freq_two_sequences.find(parent_item);
  atom_set_t::iterator itr_atom;

  if( itr_item != freq_two_sequences.end())
  {
    //ak dany item uz je frekventovany tak sa updatuje len jeho support..
    atom_set_t &atom_set = (*itr_item).second;
    itr_atom = atom_set.find(atom);
    if(itr_atom != atom_set.end())
    {
      ((atom_t&)(*itr_atom)).insert(eid,sid);
      return;
    }
  }

  itr_item = candidates.find(parent_item);

  if( itr_item != candidates.end())
  {
    atom_set_t &atom_set = (*itr_item).second;
    //vytiahne si alebo vytvori atom
    itr_atom = atom_set.find(atom);
    if(itr_atom != atom_set.end())
    {
      //ak atom uz je tak updatuje support a popripade prida do freq_two_sequences
      ((atom_t&)(*itr_atom)).insert(eid,sid);
      CheckSupport(freq_two_sequences, (*itr_atom), parent_item);
      return;

    }
    else
    {
      //ak nenasiel dany atom k parentovi tak ho vytvori
      atom.insert(eid,sid);
      atom_set.insert(atom);
      CheckSupport(freq_two_sequences, atom, parent_item);
      return;
    }

  } //end if itr_item exists
  else
  {
    //neexistuje ani parent v kandidatoch
    atom.insert(eid,sid);
    atom_set_t atom_set;
    atom_set.insert(atom);
    candidates.insert(make_pair(parent_item,atom_set));
    CheckSupport(freq_two_sequences, atom, parent_item);
    return;
  }

}

//**********************CheckSupport**************************************
static bool CheckSupport(equivalence_class_t &result,
                  const atom_t &atom, item_t parent_item)
{
  if(atom.Support() != support_threshold)
    return false;
  equivalence_class_t::iterator found = result.find(parent_item);
  if(found != result.end())
  {
    (*found).second.insert(atom);
  }
  else
  {
    atom_set_t atom_set;
    atom_set.insert(atom);
    result.insert(make_pair(parent_item,atom_set));
  }

  return true;
}

//*************************PrintSequence********************************
void PrintSequence(std::string &prefix,const atom_t &atom,const atom_t& parent_atom)
{
  output << endl << prefix;
  item_t item = atom.item;
  if(parent_atom.atom_type == c_bottom_atom)
    output << "<(" << item;
  else
  if(atom.atom_type == c_event_atom)
    output << "," << item;
  else
    output << ")(" << item;

  output << ")>";

}

//**************************GenerateSequencePrefix************************************
string GenerateSequencePrefix(string &prefix,const atom_t &atom,const atom_t& parent_atom)
{
  string result = prefix ;
  item_t item = atom.item;
  stringstream ss;
  ss << item;
  if(parent_atom.atom_type == c_bottom_atom)
  {
    result += string("<(") +=ss.str() ;
  }
  else
  if(atom.atom_type == c_event_atom)
  {  
    result += string(",") += ss.str() ;  
  }
  else
  {
    //sequence_atom
    result += string(")(") += ss.str();
  
  }
  
  return result;
}

//*********************GeneretaHorizontalDb**********************************
//pretransformuje vertikalnu datbazu z freq_items na horizontalnu
//  @ horizontal_db - vysledna struktura ktora sanplni datami
//DEPRECATED - vid Spade()
void GeneretaHorizontalDb(horizontal_db_t& horizontal_db )
{
  horizontal_db.clear();
  horizontal_db_t::iterator sequence;
  id_list_t::const_iterator a_itr, a_end;
  atom_set_t::const_iterator fitr = freq_items.begin(), fend = freq_items.end();
  id_t eid,sid;

  for( ; fitr != fend; ++fitr )
  {
    const atom_t& atom = (*fitr);
    a_itr = atom.id_list.begin();
    a_end = atom.id_list.end();

    //preide id_list pre dany item(atom)
    for( ; a_itr != a_end; ++a_itr)
    {
      sid = (*a_itr).first;
      event_set_t::const_iterator sitr = (*a_itr).second.begin();
      //proide pres eventy v sekvenci pro item
      for( ; sitr != (*a_itr).second.end(); ++sitr )
      {
        eid = (*sitr);

        //doplni vyskyt itemu do horizontalnej databazy
        sequence = horizontal_db.find(sid);
        if(sequence != horizontal_db.end())
        {
          //ak uz sekvencia s sid existuje
          sequence_row_t& seq_row = (*sequence).second;
          seq_row.insert(make_pair(atom.item,eid));
        }
        else
        {
          //pre novu sekvenci s sid
          sequence_row_t seq_row;
          seq_row.insert(make_pair(atom.item,eid));
          horizontal_db.insert(make_pair(sid,seq_row));
        }

      }//end for event_set_t

    }//end for id_list

  }//end for frq_items

}
