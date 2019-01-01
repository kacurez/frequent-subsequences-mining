// This code was generated by CLI, a command line interface
// compiler for C++.
//

#ifndef CMD_LINE_OPTIONS_HXX
#define CMD_LINE_OPTIONS_HXX

#include <iosfwd>
#include <string>
#include <exception>

namespace cli
{
  class unknown_mode
  {
    public:
    enum value
    {
      skip,
      stop,
      fail
    };

    unknown_mode (value v);

    operator value () const 
    {
      return v_;
    }

    private:
    value v_;
  };

  // Exceptions.
  //

  class exception: public std::exception
  {
    public:
    virtual void
    print (std::ostream&) const = 0;
  };

  std::ostream&
  operator<< (std::ostream&, const exception&);

  class unknown_option: public exception
  {
    public:
    virtual
    ~unknown_option () throw ();

    unknown_option (const std::string& option);

    const std::string&
    option () const;

    virtual void
    print (std::ostream&) const;

    virtual const char*
    what () const throw ();

    private:
    std::string option_;
  };

  class unknown_argument: public exception
  {
    public:
    virtual
    ~unknown_argument () throw ();

    unknown_argument (const std::string& argument);

    const std::string&
    argument () const;

    virtual void
    print (std::ostream&) const;

    virtual const char*
    what () const throw ();

    private:
    std::string argument_;
  };

  class missing_value: public exception
  {
    public:
    virtual
    ~missing_value () throw ();

    missing_value (const std::string& option);

    const std::string&
    option () const;

    virtual void
    print (std::ostream&) const;

    virtual const char*
    what () const throw ();

    private:
    std::string option_;
  };

  class invalid_value: public exception
  {
    public:
    virtual
    ~invalid_value () throw ();

    invalid_value (const std::string& option,
                   const std::string& value);

    const std::string&
    option () const;

    const std::string&
    value () const;

    virtual void
    print (std::ostream&) const;

    virtual const char*
    what () const throw ();

    private:
    std::string option_;
    std::string value_;
  };
}

#include <string>

class options
{
  public:

  options (int argc,
           char** argv,
           ::cli::unknown_mode option = ::cli::unknown_mode::fail,
           ::cli::unknown_mode argument = ::cli::unknown_mode::stop);

  options (int start,
           int argc,
           char** argv,
           ::cli::unknown_mode option = ::cli::unknown_mode::fail,
           ::cli::unknown_mode argument = ::cli::unknown_mode::stop);

  options (int argc,
           char** argv,
           int& end,
           ::cli::unknown_mode option = ::cli::unknown_mode::fail,
           ::cli::unknown_mode argument = ::cli::unknown_mode::stop);

  options (int start,
           int argc,
           char** argv,
           int& end,
           ::cli::unknown_mode option = ::cli::unknown_mode::fail,
           ::cli::unknown_mode argument = ::cli::unknown_mode::stop);

  // Option accessors.
  //
  public:

  const unsigned int&
  support_threshold () const;

  const bool&
  print_freq_seqs () const;

  const std::string&
  file () const;

  const bool&
  help () const;

  const bool&
  testing () const;

  private:
  int
  _parse (int start,
          int argc,
          char** argv,
          ::cli::unknown_mode option,
          ::cli::unknown_mode argument);

  public:
  unsigned int support_threshold_;
  bool print_freq_seqs_;
  std::string file_;
  bool help_;
  bool testing_;
};

#include "cmd_line_options.ixx"

#endif // CMD_LINE_OPTIONS_HXX