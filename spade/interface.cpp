#include "interface.h"
#include <fstream>
using namespace std;

//**********************PrintFreqOneSeqs*********************************
void PrintFreqOneSeqs(std::ostream &out)
{
  atom_set_t::const_iterator atom = freq_items.begin(), end = freq_items.end();
  bool isFirst = true;
  for (; atom != end ; ++atom)
  {

    if(isFirst) 
      isFirst = false;
    else
    out<<",";
    
    out << "<" << (*atom).item << ">" ;
  }
  out << endl;
}
//********************PrintFreqTwoSeqs***********************************
void PrintFreqTwoSeqs(equivalence_class_t &freq_seqs,std::ostream &out)
{
  equivalence_class_t::const_iterator seq = freq_seqs.begin(), end = freq_seqs.end();
  for ( ;seq!=end; ++seq)
  {
    atom_set_t::const_iterator atom = (*seq).second.begin(), aend = (*seq).second.end();
    for( ;atom!=aend; ++atom)
    {
      if((*atom).atom_type == c_event_atom)
        out << "<(" << (*seq).first << "," << (*atom).item << ")>";
      else
        out << "<(" << (*seq).first << ")(" << (*atom).item << ")>";
    }
  }

  out << endl;

}

//************************LoadDatabase******************
// Preide databazu a vyhlada frekventovane itemy - naplni strukturu freq_items
void LoadDatabase(fstream &file)
{
  id_t eid,sid;
  item_t item;
  atom_set_t candidates;
  atom_set_t::iterator candidate;
  atom_t tmp_atom(0,c_event_atom);

  while(!file.eof())
  {
    file >> sid >> eid >> item;
    // neviem preco ale posledny riadok suboru nacita furt dvakrat preto tu davam nasledujuci vyraz
    if(file.eof())
     break;
    tmp_atom.item = item;
    //najprv hlada item v freq_items
    candidate = freq_items.find(tmp_atom);
    if(candidate != freq_items.end())
    {
      //ak dany item je uz freq item tak len prda dalsi casovy zaznam
      //tu trocha porusujem pravidla:). std::set obsahuje vsetky hodnoty ako const takze sa nedaju menit a to kvoli tomu ze hodnoty su zaroven klucmi
      //   v tomto pripade sa zmenou atom(t.j. kluca set) nezmeni integrita std::set pretoze klucmi su v skutonosti itemy(atom.item) a tie nikde dalej nemenim
      atom_t &atom = (atom_t&)(*candidate);
      atom.insert(eid,sid);
      continue;
    }

    //ak dany item neni este freq. item tak hlada v docasnej mnozine kanidatov
    candidate = candidates.find(tmp_atom);
    if(candidate != candidates.end())
    {
      //ak najde tak prida zaznam
		    atom_t &atom = (atom_t&)(*candidate);
		    atom.insert(eid,sid);
		    //checkuje ci ma potrebnu support
			  if(atom.Support() == support_threshold)
		  	{
			    //ak ma potrebnu support tak ho premiestni z candidatov do freq items
			    //todo:zisti ci sa to neda swapnut..ale asi neda..
       freq_items.insert(atom);
				   candidates.erase(candidate);
			  }
		  }
		else
		{
		  //ak neexistuje item ani u kandidatov tak sa vytvori novy
		  atom_t atom = event_atom_t(item);
		  atom.insert(eid,sid);
			if(support_threshold == 1)
			  freq_items.insert(atom);
			else
			  candidates.insert(atom);
		}

  }//end  while !EOF

}


//****************vytiskne frek items strukturu****************************
void PrintFreqItems(ostream &out)
{
  PrintAtomSet(freq_items,out);  

}

void PrintAtomSet(const atom_set_t& atoms,ostream &out)
{
  atom_set_t::const_iterator itr = atoms.begin(), end = atoms.end();
  out << "atom set size = " << atoms.size() << " at support threshold =" <<support_threshold << endl;

  for( ; itr!=end; ++itr)
  {
    (*itr).Print(out);
    out << endl;
	}
}
//*********************************************************
void PrintHorizontalDb(horizontal_db_t &hdb,ostream &out)
{
  horizontal_db_t::const_iterator hitr = hdb.begin(), hend = hdb.end();
  id_t sid;

  for( ; hitr != hend; ++hitr)
  {

    sid = (*hitr).first;
    const sequence_row_t& seq_row = (*hitr).second;
    out << sid <<".|" ;
    for( sequence_row_t::const_iterator sitr = seq_row.begin(); sitr != seq_row.end(); ++sitr )
    {
      //(item,eid)
      out << "(" << (*sitr).first << "," << (*sitr).second << ")" ;

    }
    out<<endl;

  }

}

void PrintEquivalenceClass(equivalence_class_t &eq_class,std::ostream &out)
{
  equivalence_class_t::const_iterator eq_itr = eq_class.begin(),eq_end = eq_class.end();
  out << "EQUVALENCE CLASSES" <<endl;
  for( ; eq_itr != eq_end ; ++eq_itr)
  {
    const atom_set_t &atom_set = (*eq_itr).second;
    atom_set_t::const_iterator at_itr = atom_set.begin(), at_end = atom_set.end();
    out<< "**** "<< (*eq_itr).first << endl;
    for( ;at_itr!=at_end; ++at_itr)
    {
      const atom_t &atom = (*at_itr);
      out<< atom.item << ",";
    }
    out<<endl;
  }
  out<< endl;
}
