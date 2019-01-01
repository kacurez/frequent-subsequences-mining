
#include "interface.h"
#include "algorithm.h"
#include "frequentitem.h"
#include "definitions.h"
#include <iostream>
#include <time.h>

using namespace std;
void PrefixSpan(fstream &file);
const char* c_version = "0.7 LEVEL-BY-LEVEL PROJECTION";
//usage ./prefixspan <filename> <-sa<support_in_absolute> || -sp<support_in_percents> > --show-database --show-patterns
//**********************main******************************************************
int main(int argc, char* argv[])
{
  support_threshold = 2;
  try
  {
    string filename;
    LoadOptions(argc,argv,filename);
    fstream file;
    OpenFile(file,filename.c_str());
    //if(options != c_support_in_percents) DEPRECATED: nemam ako zistit pocet sekvenci z atoho vypocitat support
    support_threshold = (int)input_support;
    LoadData(file,database);
    clock_t start,finish;
    //start time
    start = clock();
    PrefixSpan(file);
    finish = clock();
    //stop time
    float t_const = 1;//seconds
    float result = (float)((finish - start)/(CLOCKS_PER_SEC * t_const));

    if(options == c_testing)
    cout << result << endl;
    else
    cout << endl << "Algorithm run in:" << result << " miliseconds" <<endl;

  }
  catch(exception &e)
  {
	   cout<<e.what()<<endl;
	}
	return 0;
}

//*******************PrefixSpan************************************
//nacita databazu
//vyhlada prve frekventovane itemy
//pre keazdy item vyprojektuje databazu a zavola prefixspan z modulu algorithm.cpp
//vysledok je v globalnej premennej freqItems
void PrefixSpan(fstream &file)
{
  pseudodatabase_t newdb;
  std::string *prefixstr = NULL;
  bool vymazma = false;

  for(idx_t fidx = 0; fidx < freqItems.size(); fidx++)
  {
    FrequentItem& pfreqItem = freqItems[fidx];

    if(options == c_show_patterns && !(options == c_testing) )
    {
      prefixstr = new string();
      *prefixstr = "<(" + *pfreqItem;
      cout << (*prefixstr) << ")>" << endl;
    }

    newdb.rows.clear();
    ProjectFirstDb(pfreqItem, newdb);
    if(vymazma)
      PrintPseudoDb(newdb);
    Prefixspan(newdb, pfreqItem,prefixstr) ;
  }

}


