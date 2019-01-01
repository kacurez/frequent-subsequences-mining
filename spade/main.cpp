#include "definitions.h"
#include "interface.h"
#include "algorithm.h"
#include "cmd_line_options.hxx"

#include <time.h>
#include <iostream>
#include <fstream>

using namespace std;

void OpenFile(const char* fname,fstream& file);
void LoadOptions(string& filename,int argc,char* argv[]);

int main(int argc,char *argv[])
{
    try
    {
      fstream file;
      string filename;
      LoadOptions(filename,argc,argv);
      OpenFile(filename.c_str(),file);
      LoadDatabase(file);
      clock_t start,finish;
      
      start = clock();
      Spade2();
      finish = clock(); 
      float t_const = 1; //seconds
      float result = (float)((finish - start)/(CLOCKS_PER_SEC * t_const));

      if(s_options == c_testing)
        cout << result << endl;
      else
        cout << endl << "Algorithm run in:" << result << " miliseconds" <<endl;

      
    }
    catch(exception &e)
    {
      cout<< e.what()<< endl;
      return 1;
    }
    catch(int i)
    {
      return i;
    }
    catch(...)
    {
      cout<< "UNKNOWN ERROR" << endl;
      return 1;
    }
    

    return 0;
}

//********************usage*******************************
void usage ()
{
  cerr << "usage: spade v0.3  | Tomas Kacur 2009@CTU FEL PRAGUE |" << endl
       << "  [--help|-h]" << endl
       << "  [--filename|-f <string>]" << endl
       << "  [--support-threshold|-s <integer>]" << endl
       << "  [--testing|-t]" << endl
       << "  [--print-freq-seqs|-p] " << endl;
       
}

//*************************************************
void LoadOptions(string& filename,int argc,char* argv[])
{
  try
  {
    int end; // End of options.
    options o (argc, argv, end);

    if (o.help ())
    {
      usage ();
      throw(0);
    }
    if(end == 1)
    {
      usage ();
      throw(0);
    
    }

    support_threshold = o.support_threshold();

    filename = o.file();

    if(o.print_freq_seqs())
      s_options << c_print_freq_seqs;

    if(support_threshold < 1)
      throw MyException("support must be greater than 0!!");

    if(filename.empty())
      throw MyException("missing database filename");

    if(o.testing())
      s_options << c_testing;
  
  }
   catch (const cli::exception& e)
  {
    cerr << e << endl;
    
    usage ();
    throw(1);
  }

}




//**********************OTVORI*SUBOR**********************
void OpenFile(const char* fname,fstream& file)
{
  file.open(fname);
  if(!file.is_open())
          throw MyException(string(string("Bad file ") + fname).c_str());
}
