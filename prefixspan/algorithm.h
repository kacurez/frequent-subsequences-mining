#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "definitions.h"
#include "frequentitem.h"


void PreFindFrequentItems(freqitems_vector_t &freqSet);
void ProjectFirstDb(FrequentItem &freqItem,pseudodatabase_t &result_db);
void Prefixspan(pseudodatabase_t &db,FrequentItem &currFreqItem,std::string *prefixstr);

#endif
