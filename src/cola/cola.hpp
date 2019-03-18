/**
 *  cola.hpp: a single header only COmmand Line Argument parser.
 */
#ifndef COLA_HPP
#define COLA_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <cassert>

namespace cola {

// option argument type
typedef int                  integer;
typedef double               real;
typedef std::string          string;
typedef std::vector<integer> ivector;
typedef std::vector<real>    rvector;
typedef std::vector<string>  svector;

// programming error
class logic_error;

// runtime error
class runtime_error;

/**
 *  Command-line option parser.
 *  $ command [option [argument]]
 */
class parser
{
public:
    /**
     *  Represents an option.
     */
    class option;

    parser();
    ~parser();

    /**
     *  Define a long option.
     *
     *  @param name option name. '--' is automatically added.
     */
    option& define(const string& name, const string& description = "");

    /**
     *  Define a short option.
     *
     *  @param name short option name. '-' is automatically added.
     */
    option& define(char name, const string& description = "");

    /**
     *  Parse command line options.
     */
    void parse(int argc, const char* const argv[]);

    /**
     *  Returns true if the name is defined or aliased.
     */
    bool is_passed(const string& name) const;
    bool is_passed(char name) const;

    /**
     *  Returns passed argument. if T is different from the type of defined one, throw exception.
     *  @tparam T type of argument. integer, real, string is allowed.
     */
    template<typename T> T get(const string& name) const;
    template<typename T> T get(char name) const;

    /**
     *  Returns passed argument as vector. if T is different from the type of defined one, throw exception.
     *  @tparam T type of argument. integer, real, string is allowed.
     */
    template<typename T> std::vector<T> get_vector(const string& name) const;
    template<typename T> std::vector<T> get_vector(char name) const;

    /**
     *  Returns passed arguments.
     */
    svector args() const;

    /**
     *  Returns arguments which is not defined or not an option.
     */
    svector rest_args() const;

    /**
     *  Returns command name.
     */
    string command() const;

    /**
     *  Returns command name and its description.
     *  @param offset offset space from beggining of line
     */
    string descriptions(const string& offset = "") const;

    /**
     *  Output template usage to ostream.
     *  @param overview overview of this program
     *  @param os output ostream
     */
    void easy_usage(const string& overview, std::ostream& os = std::cerr) const;

    /**
     *  Returns the executed command.
     */
    string exec_str() const;

    /**
     *  Reset parsed data.
     *  use this function after parse error.
     */
    void reset();

    /**
     *  True when there are undefined long or short options
     *  in parsed argv.
     */
    bool exists_undefined_options() const;

private:
    enum argument_type
    {
        FLAG,
        INTEGER, REAL, STRING,
        IVECTOR, RVECTOR, SVECTOR
    };

    enum literal_type {
        SHORT,        // -a
        SHORTS,       // -abc
        LONG,         // --option
        LONG_ARG,     // --option=<arg>
        IGNORE,       // -
        IGNORE_AFTER, // --
        ELSE          // else
    };

    typedef std::vector<option*> option_container;
    typedef std::map<string, option*> option_map;
    typedef option_container::iterator       option_iterator;
    typedef option_container::const_iterator option_citerator;
    typedef option_map::iterator       lookup_iterator;
    typedef option_map::const_iterator lookup_citerator;

private:
          option& get_option(const string& name);
    const option& get_option(const string& name) const;
          option& get_option(char name);
    const option& get_option(char name) const;

    bool is_defined(const string& name) const;
    bool is_defined(char name) const;

    void parse_impl(svector::iterator begin, svector::iterator end);

    literal_type judge(const string& arg) const;

    void eval_short_option(svector::iterator& it, svector::iterator end);
    void eval_shorts_option(svector::iterator& it, svector::iterator end);
    void eval_long_option(svector::iterator& it, svector::iterator end);
    void eval_long_param_option(svector::iterator& it, svector::iterator end);

    static string make_arg(const string& name);
    static string make_arg(char name);

    void must_be_parsed(const string& func_name) const;
    void must_not_be_parsed(const string& func_name) const;

    template<typename T> static string to_string(const T& value);
    template<typename T> static string to_string(const std::vector<T>& value);
                         static string to_string(argument_type type);
    template<typename T> static string to_string();

    template<typename T> static T from_string(const string& value);
    template<typename T> static std::vector<T> from_string(const string& value, char delim);

    parser(const option& other);
    void operator=(const option& other);

private:
    svector          args_;
    svector          rest_args_;
    option_container options_;
    option_map       lookup_;
    bool             parsed_;
    bool             exists_undefined_;
};

/**
 *  represents an option.
 */
class parser::option
{
public:
    ~option();

    /**
     *  Let this option have an argument.
     *  @tparam T type of argument. integer, real, string are allowed.
     */
    template<typename T> option& with_arg(T default_value = T());

    /**
     *  Let this option have a vector argument.
     *  @tparam T type of argument. integer, real, string are allowed.
     */
    template<typename T> option& with_arg_vector(const std::vector<T>& default_value = std::vector<T>());

    /**
     *  Let this option have a vector argument.
     *  @param default_value string expression of default vector. ex) "1,2,3"
     *  @tparam T type of argument. integer, real, string are allowed.
     */
    template<typename T> option& with_arg_vector(const string& default_value);

    /**
     *  Set an alias name of this option.
     *  @param name long option name. '--' is automatically added.
     */
    option& alias(const string& long_name);

    /**
     *  Set an alias name of this option.
     *  @param name short option name. '-' is automatically added. 
     */
    option& alias(char short_name);

    /**
     *  Set delimiter to parse vector value.
     */
    option& delimiter(char d);

    /**
     *  Hide this option from usage.
     */
    option& hidden();

private:
    typedef svector name_container;

private:
    friend class parser; // parser is only class that can generate option.
    option(const string& name, const string& description, parser& mother);
    option(char name, const string& description, parser& mother);

    template<typename T> T as() const;
    template<typename T> std::vector<T> as_vector() const;

    void check_multi(const string& name) const;
    void passed();
    bool has_arg() const;
    void parse_arg(const string& arg);
    void reset();

    option(const option& other);
    void operator=(const option& other);

private:
    parser& mother_;
    argument_type type_;
    union {
        integer  value_i_;
        real     value_r_;
        string*  value_s_;
        ivector* value_iv_;
        rvector* value_rv_;
        svector* value_sv_;
    };
    bool is_passed_;
    string description_;
    name_container names_;  // contains '--', '-'
    string default_value_;
    char delim_; // delimiter to parse vector
    bool hidden_;
};

class logic_error: public std::logic_error
{
public:
    logic_error(const string& msg): std::logic_error(msg) {}
};

class runtime_error: public std::runtime_error
{
public:
    runtime_error(const string& msg): std::runtime_error(msg) {}
};

// parser ------------------------------------------------------------------------------------
// ctor/dtor ---------------------------------------------------------------------------------
inline parser::parser():
    parsed_(false), exists_undefined_(false)
{
}

inline parser::~parser()
{
    for(option_iterator it = options_.begin(); it != options_.end(); ++it) {
        delete *it;
    }
}

// define ------------------------------------------------------------------------------------
inline parser::option& parser::define(const string& name, const string& description)
{
    must_not_be_parsed("parser::define");

    if(is_defined(name)) {
        throw logic_error("the option '" + make_arg(name) + "' is already defined");
    }

    if(name.empty()) {
        throw logic_error("don't pass empty string to parser::define");
    }

    const string desc = description.empty()? "option #" + to_string(options_.size() + 1)
                                           : description;

    if(name.size() == 1) {
        options_.push_back(new option(name[0], desc, *this));
    } else {
        options_.push_back(new option(name, desc, *this));
    }
    return *options_.back();
}

inline parser::option& parser::define(char name, const string& description)
{
    must_not_be_parsed("parser::define");

    if(is_defined(name)) {
        throw logic_error("the option '" + make_arg(name) + "' is already defined");
    }

    if(name == ' ') {
        throw logic_error("don't pass white space to parser::define");
    }

    const string desc = description.empty()? "option #" + to_string(options_.size() + 1)
                                           : description;

    options_.push_back(new option(name, desc, *this));
    return *options_.back();
}

// parse ------------------------------------------------------------------------------------
inline void parser::parse(int argc, const char* const argv[])
{
    if(parsed_) {
        throw logic_error("call parser::parse() only once");
    }

    args_.assign(argv, argv + argc);
    parse_impl(args_.begin(), args_.end());
}

// passed ------------------------------------------------------------------------------------
inline bool parser::is_passed(const string& name) const
{
    must_be_parsed("parser::is_passed");
    return get_option(name).is_passed_;
}

inline bool parser::is_passed(char name) const
{
    must_be_parsed("parser::is_passed");
    return get_option(name).is_passed_;
}

// get ------------------------------------------------------------------------------------
template<typename T> 
inline T parser::get(const string& name) const
{
    must_be_parsed("parser::get");
    return get_option(name).as<T>();
}
template<typename T> 
inline T parser::get(char name) const
{
    must_be_parsed("parser::get");
    return get_option(name).as<T>();
}

template<typename T> 
inline std::vector<T> parser::get_vector(const string& name) const
{
    must_be_parsed("parser::get_vector");
    return get_option(name).as_vector<T>();
}

template<typename T> 
inline std::vector<T> parser::get_vector(char name) const
{
    must_be_parsed("parser::get_vector");
    return get_option(name).as_vector<T>();
}

// other ------------------------------------------------------------------------------------
inline svector parser::args() const
{
    must_be_parsed("parser::args");
    return args_;
}

inline svector parser::rest_args() const
{
    must_be_parsed("parser::rest_args");
    return rest_args_;
}

inline string parser::command() const
{
    must_be_parsed("parser::command");

    return args_[0].substr(args_[0].find_last_of('/') + 1);
}

inline string parser::descriptions(const string& offset) const
{
    must_be_parsed("parser::descriptions");
    svector optheads;
    std::vector<std::size_t> sizes;

    option_citerator it = options_.begin();
    for(; it!=options_.end(); ++it) {
        std::ostringstream oss;
        const option& opt = **it;
        const svector& names = opt.names_;

        if(opt.hidden_) continue;

        oss << offset << names[0];
        if(names.size() >= 2) oss << " (";
        for(svector::const_iterator it = names.begin()+1; it != names.end(); ++it) {
            oss << *it << (it == names.end() - 1? "" : ", ");
        } 
        if(names.size() >= 2) oss << ')';
        if(opt.has_arg()) {
            oss << " <" << to_string(opt.type_) << ">";
        }
        optheads.push_back(oss.str());
    }

    std::size_t max_width = 0;
    svector::const_iterator it2 = optheads.begin();
    for(; it2!=optheads.end(); ++it2) {
        max_width = std::max(max_width, it2->size());
        sizes.push_back(it2->size());
    }

    std::ostringstream oss;
    int i = 0;
    it = options_.begin();
    for(; it!=options_.end(); ++it) {
        const option& opt = **it;

        if(opt.hidden_) continue;

        oss << std::left << std::setw(max_width) << optheads[i]
            << " | " << opt.description_;
        if(opt.has_arg()) {
            oss << " (default = " << opt.default_value_ << ")";
        }
        oss << std::endl;
        i++;
    }
    return oss.str();
}

inline void parser::easy_usage(const string& overview, std::ostream& os) const
{
    must_be_parsed("parser::easy_usage");
    os << "overview: " << overview << '\n'
       << "usage: $ " << command() << " [options...]\n"
       << "options:\n"
       << descriptions("    ")
       << std::endl;
}

inline string parser::exec_str() const
{
    must_be_parsed("parser::exec_str");

    svector optheads;
    std::ostringstream os;
    for(svector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
        os << *it << ' ';
    }
    return os.str();
}

inline void parser::reset()
{
    args_.clear();
    rest_args_.clear();
    for(option_iterator it = options_.begin(); it != options_.end(); ++it) {
        (**it).reset();
    }
    parsed_ = false;
}

inline bool parser::exists_undefined_options() const
{
    return exists_undefined_;
}


// private --------------------------------------------------------------------------------------------
// get option -----------------------------------------------------------------------------------------
inline parser::option& parser::get_option(const string& name)
{
    const string arg = make_arg(name);
    lookup_iterator it = lookup_.find(arg);
    if(it == lookup_.end()) throw logic_error("an option '" + arg + "' is undefined");
    return *it->second;
}

inline const parser::option& parser::get_option(const string& name) const
{
    const string arg = make_arg(name);
    lookup_citerator it = lookup_.find(arg);
    if(it == lookup_.end()) throw logic_error("an option '" + arg + "' is undefined");
    return *it->second;
}

inline parser::option& parser::get_option(char name)
{
    const string arg = make_arg(name);
    lookup_iterator it = lookup_.find(arg);
    if(it == lookup_.end()) throw logic_error("an option '" + arg + "' is undefined");
    return *it->second;
}

inline const parser::option& parser::get_option(char name) const
{
    const string arg = make_arg(name);
    lookup_citerator it = lookup_.find(arg);
    if(it == lookup_.end()) throw logic_error("an option '" + arg + "' is undefined");
    return *it->second;
}

// defined ------------------------------------------------------------------------------------
inline bool parser::is_defined(const string& name) const
{
    return lookup_.count(make_arg(name)) != 0; 
}
inline bool parser::is_defined(char name) const
{
    return lookup_.count(make_arg(name)) != 0; 
}


// parse ---------------------------------------------------------------------------------------
inline void parser::parse_impl(svector::iterator begin, svector::iterator end)
{
    svector::iterator it = begin + 1;

    if(it == end) goto parse_end;

    while(it != end) {

        const string& arg = *it;

        switch(judge(arg)) {
        case SHORT: // -a
            eval_short_option(it, end);
            break;
        
        case SHORTS: // -abc
            eval_shorts_option(it, end);
            break;
        
        case LONG: // --option
            eval_long_option(it, end);
            break;
        
        case LONG_ARG: // --option=<arg>
            eval_long_param_option(it, end);
            break;
        
        case IGNORE: // -
            ++it;
            break;

        case IGNORE_AFTER: // --
            ++it;
            for(; it != end; ++it) {
                rest_args_.push_back(*it);
            }
            goto parse_end;
        
        case ELSE:
            rest_args_.push_back(arg);
            ++it;
            break;
        
        default:
            assert(0);
        }
    }
parse_end:
    parsed_ = true;
}

inline parser::literal_type parser::judge(const string& arg) const
{
    const std::size_t len = arg.size();

    if(arg[0] == '-') {
        
        if(len == 1) return IGNORE; // arg == "-"

        if(arg[1] == '-') { // arg == "--something" 
            
            if(len == 2) return IGNORE_AFTER; // arg == "--"

            string::size_type is_optarg = arg.find_first_of('=');

            if(is_optarg == string::npos) {
                return LONG;     // arg == "--something"
            } else {
                return LONG_ARG; // arg == "--something=opt_arg"
            }

        } else {
            if(len != 2) return SHORTS; // arg == "-something"
            else         return SHORT;  // arg == "-s"
        }

    } else {
        return ELSE;
    }
}

// eval each arguments ------------------------------------------------------------------------
inline void parser::eval_short_option(svector::iterator& it, svector::iterator end)
{
    const string& arg = *it++; // it points next of arg "-a"
    const char c = arg[1];     // after '-'

    if(!is_defined(c)) {
        exists_undefined_ = true;
        rest_args_.push_back(arg);
        return;
    }

    option& opt = get_option(c);

    if(opt.has_arg()) {
        if(it == end) {
            throw runtime_error("the option '"+arg+"' needs argument");
        } else {
            opt.parse_arg(*it++);
        }
    }

    opt.passed();
}

inline void parser::eval_shorts_option(svector::iterator& it, svector::iterator)
{
    const string& arg = *it++; // it points next of arg "-abc"

    // -a123
    option& opt1 = get_option(arg[1]);
    if(opt1.has_arg()) {
        opt1.parse_arg(string(arg.begin()+2, arg.end()));
        opt1.passed();
        return;
    }

    // -abc
    for(svector::size_type j=1; j<arg.size(); j++) {
        const char c = arg[j]; // a b c
        option& opt = get_option(c);

        if(opt.has_arg()) {
            throw runtime_error("the option '"+make_arg(c)+"' needs argument");
        }
        opt.passed();
    }
}

inline void parser::eval_long_option(svector::iterator& it, svector::iterator end)
{
    const string& arg = *it++; // it points next of arg "--abc"
    const string name(arg.begin()+2, arg.end());

    if(!is_defined(name)) {
        exists_undefined_ = true;
        rest_args_.push_back(arg);
        return;
    }

    option& opt = get_option(name);

    if(opt.has_arg()) {
        if(it == end) {
            throw runtime_error("the option '"+arg+"' needs argument");
        }
        opt.parse_arg(*it++);
    }

    opt.passed();
}

inline void parser::eval_long_param_option(svector::iterator& it, svector::iterator)
{
    const string& arg = *it++; // it points next of arg "--abc=arg"

    const string::size_type pos = arg.find_first_of('=');
    const string name(arg.begin()+2, arg.begin()+pos);
    
    if(!is_defined(name)) {
        exists_undefined_ = true;
        rest_args_.push_back(arg);
        return;
    }

    option& opt = get_option(name);

    const string arg2 = string(arg.begin()+pos+1, arg.end());
    if(arg2.empty()) {
        throw runtime_error("the option '"+name+"' needs argument");
    }

    opt.parse_arg(arg2);
        
    opt.passed();
}

// utility ---------------------------------------------------------------------------------
inline string parser::make_arg(const string& name)
{
    return (name.size() == 1)? make_arg(name[0]) : "--" + name;
}

inline string parser::make_arg(char name)
{
    return string(1, '-') + name;
}

inline void parser::must_be_parsed(const string& func_name) const
{
    if(!parsed_) {
        throw logic_error("call '"+func_name+"' after parse arguments");
    }
}

inline void parser::must_not_be_parsed(const string& func_name) const
{
    if(parsed_) {
        throw logic_error("call '"+func_name+"' before parse arguments");
    }
}

template<typename T> 
inline string parser::to_string(const T& value)
{
    std::stringstream ss;
    ss << value; 
    return ss.str();
}
template<> 
inline string parser::to_string<string>(const string& value)
{
    std::stringstream ss;
    ss << '"' + value + '"'; 
    return ss.str();
}

template<typename T> 
inline string parser::to_string(const std::vector<T>& value)
{
    std::stringstream ss;
    ss << "[";
    for(typename std::vector<T>::const_iterator it = value.begin(); it != value.end(); ++it) {
        ss << *it << ((it == value.end()-1)? "" : ", ");
    }
    ss << "]";
    return ss.str();
}

template<> 
inline string parser::to_string<string>(const std::vector<string>& value)
{
    std::stringstream ss;
    ss << "[";
    for(std::vector<string>::const_iterator it = value.begin(); it != value.end(); ++it) {
        ss << '"' + *it + '"' << ((it == value.end()-1)? "" : ", ");
    }
    ss << "]";
    return ss.str();
}

template<> inline string parser::to_string<integer>()
{
    return "integer";
}

template<> inline string parser::to_string<real>()
{
    return "real";
}

template<> inline string parser::to_string<string>()
{
    return "string";
}

template<> inline string parser::to_string<ivector>()
{
    return "vector<integer>";
}

template<> inline string parser::to_string<rvector>()
{
    return "vector<real>";
}

template<> inline string parser::to_string<svector>()
{
    return "vector<string>";
}

inline string parser::to_string(argument_type type)
{
    switch(type) {
        case FLAG:    return "flag";
        case INTEGER: return "integer";
        case REAL:    return "real";
        case STRING:  return "string";
        case IVECTOR: return "vector<integer>";
        case RVECTOR: return "vector<real>";
        case SVECTOR: return "vector<string>";
        default: assert(0);
    }
}

template<typename T> 
inline T parser::from_string(const string& value)
{
    std::stringstream ss;
    T ret;
    if(!(ss << value && ss >> ret && ss.eof())) {
        throw runtime_error("cast failed: '"+value+"' to "+to_string<T>()); 
    }
    return ret;
}

template<> 
inline string parser::from_string<string>(const string& value)
{
    return value;
}


template<typename T> 
inline std::vector<T> parser::from_string(const string& value, char delim)
{
    std::vector<T> ret;
    std::stringstream ss(value);
    string item;
    while(std::getline(ss, item, delim)) {
        if(!item.empty()) {
            ret.push_back(from_string<T>(item));
        }
    }
    return ret;
}

// parser::option ------------------------------------------------------------------------------
// dtor -----------------------------------------------------------------------------------
inline parser::option::~option()
{
    switch(type_) {
        case FLAG:    break;
        case INTEGER: break;
        case REAL:    break;
        case STRING:  delete value_s_;  value_s_  = NULL; break;
        case IVECTOR: delete value_iv_; value_iv_ = NULL; break;
        case RVECTOR: delete value_rv_; value_rv_ = NULL; break;
        case SVECTOR: delete value_sv_; value_sv_ = NULL; break;
        default: assert(0);
    }
}

// with_arg ---------------------------------------------------------------------------------
template<> 
inline parser::option& parser::option::with_arg<integer>(integer default_value)
{
    mother_.must_not_be_parsed("parser::option::with_arg");

    if(type_ != FLAG) throw logic_error("parser::option::with_arg() cal be called only once");
    value_i_ = default_value;
    default_value_ = to_string(default_value);
    type_ = INTEGER; 
    return *this;
}

template<> 
inline parser::option& parser::option::with_arg<real>(real default_value)
{
    mother_.must_not_be_parsed("parser::option::with_arg");

    if(type_ != FLAG) throw logic_error("parser::option::with_arg() cal be called only once");
    value_r_ = default_value;
    default_value_ = to_string(default_value);
    type_ = REAL; 
    return *this;
}

template<> 
inline parser::option& parser::option::with_arg<string>(string default_value)
{
    mother_.must_not_be_parsed("parser::option::with_arg");

    if(type_ != FLAG) throw logic_error("parser::option::with_arg() cal be called only once");
    value_s_ = new string(default_value);
    default_value_ = to_string(default_value);
    type_ = STRING; 
    return *this;
}

// with_arg_vector(vector) -----------------------------------------------------------------------------------
template<> 
inline parser::option& parser::option::with_arg_vector<integer>(const std::vector<integer>& default_value)
{
    mother_.must_not_be_parsed("parser::option::with_arg_vector");

    if(type_ != FLAG) throw logic_error("parser::option::with_arg() cal be called only once");
    value_iv_ = new ivector(default_value); 
    default_value_ = to_string(default_value);
    type_ = IVECTOR;
    return *this;
}

template<> 
inline parser::option& parser::option::with_arg_vector<real>(const std::vector<real>& default_value)
{
    mother_.must_not_be_parsed("parser::option::with_arg_vector");

    if(type_ != FLAG) throw logic_error("parser::option::with_arg() cal be called only once");
    value_rv_ = new rvector(default_value); 
    default_value_ = to_string(default_value);
    type_ = RVECTOR;
    return *this;
}

template<> 
inline parser::option& parser::option::with_arg_vector<string>(const std::vector<string>& default_value)
{
    mother_.must_not_be_parsed("parser::option::with_arg_vector");

    if(type_ != FLAG) throw logic_error("parser::option::with_arg() cal be called only once");
    value_sv_ = new svector(default_value); 
    default_value_ = to_string(default_value);
    type_ = SVECTOR;
    return *this;
}

// with_arg_string(string) -----------------------------------------------------------------------------------
template<> 
inline parser::option& parser::option::with_arg_vector<integer>(const string& default_value)
{
    mother_.must_not_be_parsed("parser::option::with_arg_vector");

    if(type_ != FLAG) throw logic_error("parser::option::with_arg() cal be called only once");
    value_iv_ = new ivector(); 
    *value_iv_ = from_string<integer>(default_value, delim_);
    default_value_ = '[' + default_value + ']';
    type_ = IVECTOR;
    return *this;
}

template<> 
inline parser::option& parser::option::with_arg_vector<real>(const string& default_value)
{
    mother_.must_not_be_parsed("parser::option::with_arg_vector");

    if(type_ != FLAG) throw logic_error("parser::option::with_arg() cal be called only once");
    value_rv_ = new rvector(); 
    *value_rv_ = from_string<real>(default_value, delim_);
    default_value_ = '[' + default_value + ']';
    type_ = RVECTOR;
    return *this;
}

template<> 
inline parser::option& parser::option::with_arg_vector<string>(const string& default_value)
{
    mother_.must_not_be_parsed("parser::option::with_arg_vector");

    if(type_ != FLAG) throw logic_error("parser::option::with_arg() cal be called only once");
    value_sv_ = new svector(); 
    *value_sv_ = from_string<string>(default_value, delim_);
    default_value_ = '[' + default_value + ']';
    type_ = SVECTOR;
    return *this;
}





// alias -----------------------------------------------------------------------------------
inline parser::option& parser::option::alias(const string& long_name)
{
    mother_.must_not_be_parsed("parser::option::alias");

    if(long_name.size() == 1) {
        alias(long_name[0]);
        return *this;
    }
    const string name = make_arg(long_name);
    check_multi(name);
    names_.push_back(name);
    mother_.lookup_.insert(std::make_pair(name, this));
    return *this;
}

inline parser::option& parser::option::alias(char short_name)
{
    mother_.must_not_be_parsed("parser::option::alias");

    const string name = make_arg(short_name);
    check_multi(name);
    names_.push_back(name);
    mother_.lookup_.insert(std::make_pair(name, this));
    return *this;
}

// others -----------------------------------------------------------------------------------
inline parser::option& parser::option::delimiter(char d)
{
    mother_.must_not_be_parsed("parser::option::delimiter");

    delim_ = d;
    return *this;
}

inline parser::option& parser::option::hidden()
{
    hidden_ = true;
    return *this;
}

// private -----------------------------------------------------------------------------------
// ctor -----------------------------------------------------------------------------------
inline parser::option::option(const string& name, const string& description, parser& mother):
    mother_(mother),
    type_(FLAG),
    value_s_(NULL),
    is_passed_(false),
    description_(description),
    delim_(','),
    hidden_(false)
{
    alias(name);
}

inline parser::option::option(char name, const string& description, parser& mother):
    mother_(mother),
    type_(FLAG),
    value_s_(NULL),
    is_passed_(false),
    description_(description),
    delim_(','),
    hidden_(false)
{
    alias(name);
}

// as ----------------------------------------------------------------------------------------
template<> 
inline integer parser::option::as<integer>() const
{
    mother_.must_be_parsed("parser::option::as");

    if(type_ != INTEGER) throw logic_error("argument type of '"+names_[0]+"' is not integer");
    return value_i_;
}

template<> 
inline real parser::option::as<real>() const
{
    mother_.must_be_parsed("parser::option::as");

    if(type_ != REAL) throw logic_error("argument type of '"+names_[0]+"' is not real");
    return value_r_;
}

template<> 
inline string parser::option::as<string>() const
{
    mother_.must_be_parsed("parser::option::as");

    if(type_ != STRING) throw logic_error("argument type of '"+names_[0]+"' is not string");
    return *value_s_;
}

// as_vector -----------------------------------------------------------------------------------
template<> 
inline ivector parser::option::as_vector<integer>() const
{
    mother_.must_be_parsed("parser::option::as_vector");

    if(type_ != IVECTOR) throw logic_error("argument type of '"+names_[0]+"' is not vector<integer>");
    return *value_iv_;
}

template<> 
inline rvector parser::option::as_vector<real>() const
{
    mother_.must_be_parsed("parser::option::as_vector");

    if(type_ != RVECTOR) throw logic_error("argument type of '"+names_[0]+"' is not vector<real>");
    return *value_rv_;
}

template<> 
inline svector parser::option::as_vector<string>() const
{
    mother_.must_be_parsed("parser::option::as_vector");

    if(type_ != SVECTOR) throw logic_error("argument type of '"+names_[0]+"' is not vector<string>");
    return *value_sv_;
}

template<> 
inline ivector parser::option::as<ivector>() const
{
    mother_.must_be_parsed("parser::option::as");

    return as_vector<integer>();
}

template<> 
inline rvector parser::option::as<rvector>() const
{
    mother_.must_be_parsed("parser::option::as");

    return as_vector<real>();
}

template<> 
inline svector parser::option::as<svector>() const
{
    mother_.must_be_parsed("parser::option::as");

    return as_vector<string>();
}

// utility --------------------------------------------------------------------------------------------
inline bool parser::option::has_arg() const
{
    return type_ != FLAG;
}


inline void parser::option::passed() 
{
    is_passed_ = true;
}

inline void parser::option::check_multi(const string& name) const
{
    if(std::find(names_.begin(), names_.end(), name) != names_.end()) {
        throw logic_error("'"+name+"' is already defined");
    }
}

inline void parser::option::parse_arg(const string& arg)
{
    switch(type_) {
        case INTEGER:
            value_i_ = from_string<integer>(arg);
            break;
        case REAL:
            value_r_ = from_string<real>(arg);
            break;
        case STRING:
            *value_s_ = arg;
            break;
        case IVECTOR:
            *value_iv_ = from_string<integer>(arg, delim_);
            break;
        case RVECTOR:
            *value_rv_ = from_string<real>(arg, delim_);
            break;
        case SVECTOR:
            *value_sv_ = from_string<string>(arg, delim_);
            break;
        default: 
            throw logic_error("the option '"+names_[0]+"' don't take an argument.");
    }
}

inline void parser::option::reset()
{
    is_passed_ = false;
}


} // namespace cola

#endif // COLA_HPP
