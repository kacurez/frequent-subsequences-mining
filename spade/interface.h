#ifndef INTERFACE_H
#define INTERFACE_H
#include "definitions.h"

//nacita databazu zo suboru a ulozi do database
void LoadDatabase(std::fstream &file);
void PrintFreqItems(std::ostream &out);
void PrintHorizontalDb(horizontal_db_t &hdb,std::ostream &out);
void PrintEquivalenceClass(equivalence_class_t &eq_class,std::ostream &out);
void PrintAtomSet(const atom_set_t& atoms,std::ostream &out);

//********************************pre vystup fo formate sekvenci
void PrintFreqOneSeqs(std::ostream &out);
void PrintFreqTwoSeqs(equivalence_class_t &freq_seqs,std::ostream &out);



#endif
