#include "interface.h"
#include  "frequentitem.h"
#include "definitions.h"
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <algorithm>


using namespace std;

typedef std::set<item_t> event_set_t;
float input_support = 1;
ByteStatus options;

static void FillEvent(sequence_t &sequence, event_set_t &eventSet);
static void RecountSupport(const item_t& item, event_set_t& items_in_seq_set, candidates_map_t& candidates);

/*************************LoadData************************************/
bool LoadData(fstream &file, database_t &database)
{
  candidates_map_t candidates;


  event_set_t eventSet,items_in_seq_set;
  int oldSeqId = -1,oldEventId = -1;
  int seqId,eventId;
  sequence_t sequence;

  item_t item;

  database.clear();
  eventSet.clear();
  while(!file.eof())
  {

    file >> seqId >> eventId >> item ;

    //event
    if(eventId != oldEventId)
    {
      //new event
      if(!eventSet.empty())
      {
        FillEvent(sequence,eventSet);
        eventSet.clear();

				//ak je nova sekvence tak vymaze itemy zo stare sekvence
				if(seqId != oldSeqId)
				  items_in_seq_set.clear();

				RecountSupport(item, items_in_seq_set, candidates);

        eventSet.insert(item);
      }
      else
      {//first time
        RecountSupport(item, items_in_seq_set, candidates);
        eventSet.insert(item);
      }
    }
    else
    {
      RecountSupport(item, items_in_seq_set, candidates);
      eventSet.insert(item);
    }

    //sequence
    if(seqId != oldSeqId)
    {

      //new sequence
      if(!sequence.empty())
      {
        sequence[sequence.size() - 1].status << c_last_in_sequence;
        database.push_back(sequence);
        sequence.clear();
      }
	   }
  oldSeqId = seqId;
  oldEventId = eventId;
  }//end while

  FillEvent(sequence,eventSet);
  sequence[sequence.size() - 1].status << c_last_in_sequence;
  database.push_back(sequence);

  if(options == c_show_database && !(options == c_testing) )
      PrintDatabase(database,cerr);
  //VYMAZE NEFREKVENTOVANE ITEMY Z DATABAZE
  FrequentItem freqItem;
  for( size_t j = 0; j < database.size(); j++)
  {
    //size_t start = 0;
    sequence_t &seqnc = database[j];
    for(size_t i = 0 ; i < seqnc.size() ; i++)
    {
      freqItem = FrequentItem(seqnc[i].item);
      //ak najde item ktory ma zmazat
      //if(find(freqItems.find(freqItem) == freqItems.end() )
      if(find(freqItems.begin(),freqItems.end(),freqItem) == freqItems.end() )
      {
        ByteStatus status = seqnc[i].status;
        //ak je prvy item a zaroven nie posledny tak nastavy ako prvy item na druhy item(i+1)
        if( status == c_starts_new_event &&
            (status != c_last_in_event && status != c_last_in_sequence))
        {
          seqnc[i+1].status << c_starts_new_event;
        }
        else
        {
          //ak je posledny item a nie prvy zaroven tak nastavi ten prednim ako posledny
          if(
             status != c_starts_new_event &&
            (status == c_last_in_event || status == c_last_in_sequence)
            )
             seqnc[i-1].status << seqnc[i].status;
        }
        //TODO: dodelat tak aby mazal rozsah po sebe iducich itemo a nie po jednom iteme

        seqnc.erase(seqnc.begin() + i);
        i--;
      }//end if


    }//end sequence for

    //vymaze sekvenci z databaze ak je prazdna
    if(seqnc.empty())
    {
      database.erase(database.begin() + j);
      j--;
    }
  }/*end db for*/

  file.close();
  return true;
}
//*****************************RecountSupport***********************************
//prepoicta support pre dany item
void RecountSupport(const item_t& item, event_set_t& items_in_seq_set, candidates_map_t& candidates)
{
  if(items_in_seq_set.insert(item).second == false)
    return;
  candidates_map_t::iterator candidate;
  FrequentItem tmpFreqItem(item);

  candidate = candidates.find(tmpFreqItem);

	//ak nenajde tak vlozi inac zvysi support
	if(candidate == candidates.end())
	{
	  candidates.insert(make_pair(FrequentItem(item),1));
 }
	else
	{
	  const FrequentItem& freqItem = (*candidate).first;
	  int &support = (*candidate).second;
		++support;

		if(support == support_threshold)
		{
		  freqItems.push_back(freqItem);
		}
	}
}

//*******************FillEvent**********************************//
//zoradi event vzestupne vysypanim itemov zo set::eventSet priamo do sekvence
void FillEvent(sequence_t &sequence, event_set_t &eventSet)
{

  event_set_t::const_iterator sitr,sbegin = eventSet.begin(), send = eventSet.end();
  event_t evnt;
  for( sitr = sbegin; sitr != send; ++sitr)
  {
    evnt.clear();
    evnt.item = *sitr;

    if( sitr == sbegin)
      evnt.status << c_starts_new_event;

    sequence.push_back(evnt);
  }
  sequence[sequence.size() - 1].status << c_last_in_event;
}

//***************************LoadOptions*********************************************
// nacita volby programu nacjastejsie z command line
//  usage ./prefixspan <filename> <support> [--support-in-percents] [--show-database] [--show-patterns]

const char* c_usage_msg = "usage: ./prefixspan <filename> <support_threshold> [--show-database] [--show-patterns] [-t for testing]";
void PrintUsage()
{

   cerr << "Prefixspan: version=" << c_version << " | Tomas Kacur 2009 |" << endl;
   cerr << endl << c_usage_msg << endl;
   exit(1);
}


void LoadOptions(int argc,char *argv[],string &filename)
{
  if(argc < 3)
  {
    PrintUsage();
  }

  filename = argv[1];
  stringstream ss;
  ss << argv[2];
  ss >> input_support;
  if(ss.fail() || ss.bad())
    throw MyException(string(string("Support typed incorrectly:") + argv[2]).c_str());

  for(int i = 3; i< argc; i++)
  {
    /*if(string("--support-in-percents") == argv[i] )
      options << c_support_in_percents;
      else*/
      if(string("--show-database") == argv[i] )
        options << c_show_database;
        else
        if(string("-t") == argv[i] )
        options << c_testing;
        else
        if(string("--show-patterns") == argv[i]  )
          options << c_show_patterns;
          else
          {
           cerr << "Unknown option:" <<  argv[i] << endl;
           PrintUsage();
          }
  }




}

//*****************PrintDatabase********************************************
//vytiskne databazi v rozumnem formatu do cout streamu
//<(event)...>
void PrintDatabase(database_t &database,ostream &output)
{
  output << endl << "DATABASE:" << endl;
  database_t::const_iterator itr = database.begin(), end = database.end();
  sequence_t::const_iterator sitr,send;

  for( ; itr != end; ++itr)
  {
    const sequence_t &seq = (*itr);
    send = seq.end();
    output << "<";
    for(sitr = seq.begin(); sitr != send; ++sitr)
    {
      const event_t &evnt = (*sitr);
      if(evnt.status == c_starts_new_event)
         output << "(";
      output << evnt.item;
      if(evnt.status == c_last_in_event)
      output << ")";

      if(evnt.status != c_last_in_sequence && evnt.status != c_last_in_event)
      output << ",";
    }
    output << ">" << endl;
  }
  output << flush;

}
//**********************PrintPseudoDb**************************************
void PrintPseudoDb(pseudodatabase_t &db)
{

  for(idx_t rowidx = 0; rowidx  < db.rows.size(); rowidx ++)
  {
    idx_t database_idx = db.rows[rowidx].db_idx;
    //vybere sekvenci ze stare pseudodatabaze
    //assert(database_idx < database.size())
    const sequence_t &sequence = database[database_idx];
    cout << "SEQUENCE:" << database_idx << endl;
    for(idx_t tmpidx = 0; tmpidx < db.rows[rowidx].seq_idxs.size();tmpidx++)
    {
    int startIdx = db.rows[rowidx].seq_idxs[tmpidx] + 1;
    bool isFirst = true;
    for (idx_t seqidx = startIdx ; seqidx < sequence.size(); seqidx++)
    {
      event_t evnt = sequence[seqidx];
      if(evnt.status == c_starts_new_event)
        cout << "(" << evnt.item ;
        else
          if(evnt.status == c_last_in_event )
            cout << (isFirst?",_":",") << evnt.item << ")";
            else
              cout << (isFirst?",_":",") << evnt.item;

      isFirst = false;
    }
    cout << ">" <<endl;
    }

  }

}

//**********************OpenFile**************************************
//otvori subor
void OpenFile(fstream &file,const char* fname)
{
  file.open(fname);
  if(!file.is_open())
          throw MyException(string(string("Bad file ") + fname).c_str());

}
