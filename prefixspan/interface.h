#ifndef INTERFACE_H
#define INTERFACE_H

#include "definitions.h"

//nacita data zo suboru a ulozi ich do struktury datbase_t:
const ByteStatus::Slot c_show_database = ByteStatus::S1;
const ByteStatus::Slot c_show_patterns = ByteStatus::S2;
const ByteStatus::Slot c_support_in_percents = ByteStatus::S3;
const ByteStatus::Slot c_testing = ByteStatus::S4;

extern float input_support; //cislo supportu nacitane zo vstupu
extern const char* c_version ;

extern ByteStatus options;
void LoadOptions(int argc,char *argv[],std::string &filename);
bool LoadData(std::fstream &file, database_t &database);
void OpenFile(std::fstream &file,const char* fname);
void PrintDatabase(database_t &database,std::ostream &output);
void PrintPseudoDb(pseudodatabase_t &db);

#endif

