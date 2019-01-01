#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "definitions.h"

void GeneretaHorizontalDb(horizontal_db_t& horizontal_db);//DEPRECATED - vid Spade()
void EnumerateFrequentSequences(const atom_t &parent_atom,const atom_set_t &atoms,std::string *prefix = NULL); 
void Spade(); //DEPRECATED - vid Spade()
void Spade2();

#endif
