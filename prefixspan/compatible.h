#ifndef COMPATIBLE_H
#define COMPATIBLE_H

//#define NDEBUG//pre disablovani assertu
#include <assert.h>
#include <exception>
#include <string>

class StaticException:public std::exception
{
public:
  StaticException(){m_what = "Unspecified exception";}
  StaticException(const char* what):m_what(what){}
  const char* what()const throw() {return m_what;}
private:
  const char* m_what;
};

class MyException: public std::exception
{
public:
  MyException(){m_what = "Unspecified exception";}
  MyException(const char* what)throw(){
   /* m_what = new char[strlen(what) + 1];
    //strcpy(m_what,what,strlen(what));
    m_what[strlen(what)] = 0;*/
    m_what = what;

  }
  const char* what()const throw() {return m_what.c_str();}
  ~MyException()throw(){ }
private:
  std::string m_what;
};

//PORTABILNY DATOVY TYP BOOLEAN - 1 BYTE
const char bTrue = 1;
const char bFalse = 0;

struct ByteBoolean
{
  char m_val;
    ByteBoolean(){m_val = bFalse;}
    ByteBoolean(char val):m_val(val){}
    bool operator==(const bool& other) const{
      if(other)
      return m_val == bTrue;
      else
      return m_val == bFalse;
    }
};


//Clasa reprezentujuca datovy typ statusu - zabera jeden byte
class ByteStatus
{
  public:
  enum Slot{S0 = 0, S1 = 1, S2 = 2, S3 = 4, S4 = 8, S5 = 16, S6 = 32, S7 = 64, S8 = 128};
  ByteStatus(const Slot& slot):m_status((char)slot){}
  ByteStatus(){m_status = 0;}
  bool IsSet(const Slot& slot ) const
  {
    char tmp = slot;

    return (m_status & tmp) > 0;
  }
  //nastavi na 1 dany slot
  void Set(const Slot& slot)
  {

    m_status |= (char)slot;
  }
  //nastavi na 0 dany slot
  void Reset(const Slot& slot)
  {
    m_status &= (~((char)slot));
  }
  bool operator!=(const Slot& slot)const
  {
    return !IsSet(slot);
  }
  //operator ==
  bool operator==(const Slot& slot) const
  {
    return IsSet(slot);
  }

  //operator ==
  bool operator==(ByteStatus other) const
  {
    return m_status == other.m_status;
  }

  //zjednodusene uzivanie fcie Set
  ByteStatus& operator<<(const Slot& slot)
  {
    Set(slot);
    return *this;
  }

  //zjednodusene uzivanie fcie Set
  ByteStatus& operator<<(const ByteStatus& status)
  {
    //Set(slot);
    m_status |= status.m_status;
    return *this;
  }



  //zjednodusene uzivanie fcie Reset
  ByteStatus& operator>>(const Slot& slot)
  {
    Reset(slot);
    return *this;
  }

  bool operator<(ByteStatus other) const
  {
    return m_status < other.m_status;
  }

  private:
  char m_status;
};

#endif

