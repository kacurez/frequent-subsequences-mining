#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "compatible.h"

#include <vector>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <ostream>


//konstanty pro statusy itemu v sekvenci
const ByteStatus::Slot c_starts_new_event = ByteStatus::S1; //prvni v eventu
const ByteStatus::Slot c_last_in_event = ByteStatus::S2; //posledni v eventu
const ByteStatus::Slot c_last_in_sequence = ByteStatus::S3; //posledni v sekvenci
const ByteStatus::Slot c_assembled = ByteStatus::S4; //c_assembled vyuzivana u frekventitem


typedef int item_t;

//struktura eventu
struct event_t
{
  item_t item;
  ByteStatus status;
  //vymaze status
  void clear(){status = ByteStatus::S0;}
};

//typ sekvence
typedef std::vector<event_t> sequence_t;
//typ databaze
typedef std::vector<sequence_t> database_t;
//databaze
extern database_t database;
//mnozina frekventovany 1-item sekvenci

//minimalni support
extern int support_threshold;
typedef database_t::size_type idx_t;


//*****************PSEUDODATABASE
struct pseudodatabase_t
{
  struct row_t{
   idx_t db_idx;
   std::vector<idx_t> seq_idxs;
   row_t(idx_t idx):db_idx(idx){}
  };

  std::vector<row_t> rows;

  void InsertSeqIdx(idx_t idx)
  { rows.back().seq_idxs.push_back(idx);}

  void InsertRow(idx_t idx)
  { rows.push_back(row_t(idx));}

};




#endif
