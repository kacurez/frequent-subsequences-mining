#include "algorithm.h"
#include "frequentitem.h"
#include "interface.h"

#include <iostream>
#include <set>


using namespace std;

void FindFreqItems(pseudodatabase_t &parent_db,FrequentItem &parentFreqItem,freqitems_vector_t &result_freqitems);
void ProjectDbForItem(pseudodatabase_t &parent_db,pseudodatabase_t &result_db,FrequentItem &freqItem);
void ProjectDbFor_Item(pseudodatabase_t &parent_db,pseudodatabase_t &result_db,FrequentItem &freqItem,FrequentItem &parentFreqItem);
string PrintFreqSequence(string& str,const FrequentItem &fitem,const FrequentItem &parent);

//********************************Prefixspan*********************************************************
void Prefixspan(pseudodatabase_t &db,FrequentItem &currFreqItem,std::string *prefixstr)
{
  freqitems_vector_t tFreqItems;
  pseudodatabase_t newdb;
  string *newprefixstr = NULL;
 // bool vymazma = false;


  FindFreqItems(db,currFreqItem,tFreqItems);

  for(idx_t fidx = 0; fidx < tFreqItems.size(); fidx++)
  {
    FrequentItem& cFreqITem = tFreqItems[fidx];

    if(prefixstr)
    {
      //vypis
      newprefixstr = new string();
      *newprefixstr = PrintFreqSequence(*prefixstr,cFreqITem,currFreqItem);
    }

    newdb.rows.clear();

    if(cFreqITem.m_status == c_assembled)
    {
      ProjectDbFor_Item(db, newdb, cFreqITem, currFreqItem);

    }
    else
    {
      ProjectDbForItem(db, newdb, cFreqITem);

    }

    /*if(vymazma)
      PrintPseudoDb(newdb);*/



    if( !newdb.rows.empty() )
      Prefixspan(newdb,cFreqITem,newprefixstr);
  }
}

//************************PrintFreqSequence************************************
string PrintFreqSequence(string& str,const FrequentItem &fitem,const FrequentItem &parent)
{
  string retstr = str;
  if(fitem.m_status == c_assembled)
    retstr += string(",") += *fitem;
  else
    retstr += string(")(") += *fitem;

  cout << retstr << ")>" << endl;

  return retstr;
}


//***********************************************************************************
//*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-
//---------------------------FIND FREQUENT ITEMS a spol. FUNKCE----------------------
//*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-
//***********************************************************************************
typedef std::set<item_t> items_set_t;
candidates_map_t candidates; //vyuzivane v recoutnsupport

void RecountSupport(freqitems_vector_t &result_freqitems, const FrequentItem& freqitem);


//************************************FindFreqItems*****************************************************
//vyhlada vsetky frekventovane itemy v danej databaze parent_db
void FindFreqItems(pseudodatabase_t &parent_db,FrequentItem &parentFreqItem,freqitems_vector_t &result_freqitems)
{
  candidates.clear();
  int vymazma = 0;
  items_set_t freqSetElem; //pocita vsetky itemy ktore mozu tvorit samostatny element(not appended)
 	items_set_t freqSetEvent;//poceita vsetky itemy ktore moze tvorit item s parentfreqitem(appended)

  for(idx_t rowidx = 0; rowidx < parent_db.rows.size(); rowidx++)
  {
    idx_t database_idx = parent_db.rows[rowidx].db_idx;
    //vybere sekvenci ze stare pseudodatabaze
    //assert(database_idx < database.size())
    const sequence_t &sequence = database[database_idx];
    freqSetElem.clear();
    freqSetEvent.clear();
    bool isFirst = true;
    for(idx_t tmpidx = 0; tmpidx < parent_db.rows[rowidx].seq_idxs.size(); tmpidx++)
    {

    idx_t seqidx = parent_db.rows[rowidx].seq_idxs[tmpidx];

    //ak sa nachadza primo v eventu elementu tak vsetky itemy toho eventu zobere ako _item t assembled
    if(sequence[seqidx].status != c_last_in_event )
    {
     seqidx++;

     while(seqidx < sequence.size() && sequence[seqidx].status != c_starts_new_event)
     {
       if(freqSetEvent.insert(sequence[seqidx].item).second == true)
       RecountSupport(result_freqitems, FrequentItem(sequence[seqidx].item, c_assembled) );

       seqidx++;
     }
    }
    else
      seqidx++;

    if(isFirst == false)
      continue;

    for( seqidx = seqidx ;seqidx < sequence.size(); seqidx++)
    {
      item_t item = sequence[seqidx].item;

      if(freqSetElem.insert(item).second == true)
        RecountSupport(result_freqitems, FrequentItem(item) );
    }//end for sequence

     isFirst = false;
    }// end tmpidx

  }//end for parend_db
}

//******************************RecountSupport********************************************

void RecountSupport(freqitems_vector_t &result_freqitems, const FrequentItem &freqitem)
{
  candidates_map_t::iterator candidate = candidates.find(freqitem);

  if(candidate == candidates.end())
  {
    //novy kandidat
    //vlozi do mapy kandidatov
    candidates.insert(make_pair(FrequentItem(freqitem),1));
  }
  else
  {
    //stary kandidat - zvysi mu support o 1
    const FrequentItem& tmpFreqItem = (*candidate).first;
    int &support = (*candidate).second;
    ++support;
    //ak support dosiahla bodu frekventovanosti :)
    if(support == support_threshold)
    {
      //vlozi do vysledneho vectoru freqitem
      result_freqitems.push_back(tmpFreqItem);
    }
  }
}




//***********************************************************************************
//*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-
//---------------------------PROJEKCNI FUNKCE----------------------------------------
//*-*-*-*-*-*-*-*-**-*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*--*-*-*-
//***********************************************************************************



//**********************ProjectFirstDb************************
//vyprojektuje databazu z originalnej databazy - database
// tato fce sa vola ako prva pri projektovani freqitemov
void ProjectFirstDb(FrequentItem &freqItem,pseudodatabase_t &result_db)
{
  for(idx_t dbidx = 0; dbidx < database.size(); dbidx++)
  {
   bool newRow = true;
   sequence_t &sequence = database[dbidx];

    for(idx_t index = 0; index < sequence.size(); index++)
    {

      if(freqItem == sequence[index] )
      {
        //vytvori novy row iba ak dana seqnc obsahuje freqitem
        if(newRow)
        {
          result_db.InsertRow(dbidx);
          newRow = false;
        }
        //vlozi index sekvence ktora obsahuje dany item
        result_db.InsertSeqIdx(index);
      }//end if
    }//end for sequence
  }//end db for
}

//****************************ProjectDbForItem*********************************************
//projekce databaze pre item t.j. neappendovany item - hladaju sa vsetky itemy ktore sa rovnaju freqItem
//moze vratit prazdnu databazu ak sa vsetky freqitemy sa nachadzaju na konci sekvenci
//@ parent_db - databaze ktera obsahuje freqitem jako frekventny item:)
//@ result_db - nova vyprojektovana databaze z parent_db  databaze
//@ freqItem  - item ktery je frekventovany v parent_db a podla ktoreho sa result_db vyprojektuje
void ProjectDbForItem(pseudodatabase_t &parent_db,pseudodatabase_t &result_db,FrequentItem &freqItem)
{

  for(idx_t rowidx = 0; rowidx  < parent_db.rows.size(); rowidx ++)
  {
    idx_t database_idx = parent_db.rows[rowidx].db_idx;
    //vybere sekvenci ze stare pseudodatabaze
    //assert(database_idx < database.size())
    const sequence_t &sequence = database[database_idx];
    bool newRow = true,isAfterFristEvent = false;

    //vybere prvy index do sekevence ze stare db kde se nachazi frekvent item a posune na dalsi
    int startIdx = parent_db.rows[rowidx].seq_idxs[0] + 1;
    //TODO:moze sa stat ze vyprojektuje prazdnu databazu - pridat niekde na rozumne miesto check na prazdnu databazu aby ju neposuval dalej,ale asi to nemusi vadit
    for (idx_t seqidx = startIdx ; seqidx < sequence.size(); seqidx++)
    {
      //vyjde z prveho eventu tak aby neprojektoval pre _item
      if(isAfterFristEvent == false)
      {
       if(sequence[seqidx].status == c_starts_new_event)
         isAfterFristEvent = true;
       else
         continue;
      }

      if(freqItem == sequence[seqidx] )
      {
       if(newRow)
       {
         newRow = false;
         //vlozi novy row podla indexu stareho row-u z parent_db
         result_db.InsertRow(database_idx);
       }
       result_db.InsertSeqIdx(seqidx);
      }
    }
  }
}

//**********************ProjectDbFor_Item************************************88
//projekce databaze pro _item t.j. c_appended - hladaju sa vsetky zasebou iduce itemy/dvojice (parentFreqItem,freqItem) nachadzajuce sa v
//                                               eventu/elementu sekvence parent_db databaze
//@ parent_db - databaze ktera obsahuje freqitem jako frekventny item:)
//@ result_db - nova vyprojektovana databaze z parent_db  databaze
//@ freqItem  - item ktery je frekventovany v parent_db a podla ktoreho sa vyprojektuje nova databaza - result_db
//@ parentFreqItem - item ktory sposobil ze freqitem je appendend - t.j. parentfreqItem sa nachadza prave pred freqItem v jednom elementu
//                 - podle neho je vyprojektovana parent_db
void ProjectDbFor_Item(pseudodatabase_t &parent_db,pseudodatabase_t &result_db,FrequentItem &freqItem,FrequentItem &parentFreqItem)
{
  //prochazi rows v parent_db
  for(idx_t rowidx = 0; rowidx < parent_db.rows.size(); rowidx++ )
  {
    //vybere sekvenci z hlavni databazy podle dat z parent_db
    idx_t database_idx = parent_db.rows[rowidx].db_idx;
    //assert(database_idx < database.size())
    const sequence_t &sequence = database[database_idx];
    bool newRow = true;
    //vyberie prvy index do sekvence
    for(idx_t tmpidx = 0; tmpidx < parent_db.rows[rowidx].seq_idxs.size();tmpidx++)
    {
    idx_t startIdx = parent_db.rows[rowidx].seq_idxs[tmpidx] ;
    for(idx_t seqidx = startIdx; seqidx < sequence.size() ; seqidx++)
    {
      if(freqItem == sequence[seqidx])
      {
        if(newRow)
        {
          newRow = false;
          result_db.InsertRow(database_idx);
        }
        result_db.InsertSeqIdx(seqidx);
      }

      if(sequence[seqidx].status == c_last_in_event)
        break;
    }//end sequence for

   }//end tmpidx for

  }//end db for
}




