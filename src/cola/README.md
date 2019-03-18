cola
====

`cola` is a single header only COmmand Line Argument parser.

## Requirements
cola is written in pure C++98. Any other library is not needed, and C++11 support is not required.

## Example
```C++
#include <iostream>
#include "cola.hpp"

int main(int argc, char* argv[])
{
    // the class only you need is 'cola::parser'.
    cola::parser parser;

    // Define an option '--foo'.
    parser.define("foo");

    // Define an option '--bar' and its description.
    // cola::parser::define() returns a class that represents an option.
    // You can customize the option by so-called "Named Parameter" idiom.
    // In this case, by alias('b'), an option '-b' will be exactly the same as '--bar'.
    parser.define("bar", "description for bar")
          .alias('b');

    // Define an option '--baz' and its description.
    // You can define an option that takes an argument with 'with_arg<T>()'. 
    // Template parameter T is restricted int, double, or std::string.
    // The argument of with_arg<T>() is default value.
    parser.define("baz", "option with argument")
          .with_arg<int>(42);

    // Define an option '--qux' and its description.
    // You can define an option that takes arguments with 'with_arg_vector<T>()'. 
    // To pass multiple argument value, use ','.
    // ex) ./run --qux "1, 2, 3"
    // You can also use "1, 2, 3" for default setting.
    parser.define("qux", "option with vector argument")
          .with_arg_vector<int>("1, 2, 3");


    // Parse command line arguments.
    // You can't define new options after parsing command line arguments.
    parser.parse(argc, argv);


    // cola::parser::is_passed() returns true when the option is passed.
    if(parser.is_passed("foo")) {
        std::cout << "--foo is passed!" << std::endl;
    }

    // "bar" has been defined and "b" is the alias of "bar"
    // so is_passed("bar") returns true when '--bar' or '-b' is passed. 
    if(parser.is_passed("bar")) {
        std::cout << "--bar or -b is passed!" << std::endl;
    }

    // You can get the passed argument by cola::parser::get<T>().
    // Template parameter of get<T> must be the same as with_arg<T>().
    const int baz = parser.get<int>("baz");
    std::cout << "baz = " << baz << std::endl;

    // cola::parser::get_vector<T>() is usef for get vector argument.
    std::vector<int> qux = parser.get_vector<int>("qux");
    std::cout << "qux = [" << qux[0];
    for(std::size_t i=1; i<qux.size(); i++) {
        std::cout << ", " << qux[i];
    }
    std::cout << "]" << std::endl;

    return 0;
}
```

some execution examples:

```bash
$ ./run --foo --baz=123
--foo is passed!
baz = 123
qux = [1, 2, 3]
```

```bash
$ ./run -b --qux "4, 5, 6, 7"
--bar or -b is passed!
baz = 42
qux = [4, 5, 6, 7]
```

For more details, see public member functions of class `cola::parser` and class `cola::parser::option` in `cola.hpp`, and see `test` directory.

## License

This is free and unencumbered public domain software. For more information, see http://unlicense.org/ or the accompanying LICENSE file.
