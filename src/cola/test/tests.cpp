#include "utility.hpp"
#include "cola.hpp"

void test_flag()
{
    cola::parser parser;
    parser.define("a", "A for ant").alias("apple");
    parser.define('b', "B for bat").alias("ball");
    parser.define("c", "C for cat").alias("cat"); 
    
    {
        // pass short option.
        parser.reset();
        const char* argv[] = {
            "./run", "-a"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        // "a", 'a', "apple" is the same.
        assert( parser.is_passed('a'));
        assert( parser.is_passed("a"));
        assert( parser.is_passed("apple"));
        assert(!parser.is_passed('b'));
        assert(!parser.is_passed("cat"));
    }

    {
        // pass options.
        parser.reset();
        const char* argv[] = {
            "./run", "-a", "--cat", "-b"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        assert(parser.is_passed('a'));
        assert(parser.is_passed('b'));
        assert(parser.is_passed('c'));
    }

    {
        // short options can be summarized.
        parser.reset();
        const char* argv[] = {
            "./run", "-abc" // == -a -b -c
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        assert(parser.is_passed('a'));
        assert(parser.is_passed('b'));
        assert(parser.is_passed('c'));
    }

    success("[[[ test_flag() PASSED ]]]");
}



void test_args()
{
    cola::parser parser;
    parser.define("a", "A for ant").alias("apple").with_arg<int>(42);
    parser.define('b', "B for bat").alias("ball") .with_arg<double>(3.14);
    parser.define("c", "C for cat").alias("cat")  .with_arg<std::string>("hello");
    
    {
        // default value of option -a is 42, -b is 3.14, -c is "hello".
        parser.reset();
        const char* argv[] = {
            "./run"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        assert(!parser.is_passed("a"));
        assert(parser.get<int>("a")  == 42);
        assert(std::fabs(parser.get<double>('b') - 3.14) < 1e-5);
        assert(parser.get<std::string>("cat") == "hello");
    }

    {
        // passed value of short option -a is 123.
        parser.reset();
        const char* argv[] = {
            "./run", "-a", "123"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        assert(parser.get<int>("a") == 123);
    }

    {
        // another way to pass argument to short option.
        parser.reset();
        const char* argv[] = {
            "./run", "-a123"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        assert(parser.get<int>("a") == 123);
    }

    {
        // passed value of long option --ball is 2.17.
        parser.reset();
        const char* argv[] = {
            "./run", "--ball", "2.17"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        assert(std::fabs(parser.get<double>('b') - 2.17) < 1e-7);
    }

    {
        // another way to pass argument to long option.
        parser.reset();
        const char* argv[] = {
            "./run", "--apple=123"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        assert(parser.get<int>("a") == 123);
    }

    std::cout << "\033[34m[[[ test_args() PASSED ]]]\033[m" << std::endl;
}



void test_vector_args()
{
    cola::parser parser;
    parser.define("a", "A for ant").alias("apple").with_arg_vector<int>();
    parser.define('b', "B for bat").alias("ball") .with_arg_vector<double>();
    parser.define("c", "C for cat").alias("cat")  .with_arg_vector<std::string>().delimiter(':');
    
    {
        // ',' is used to separate array element in default.
        parser.reset();
        const char* argv[] = {
            "./run", "-a", "1,2,3"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        std::vector<int> ivec = parser.get_vector<int>("apple");
        assert(ivec[0] == 1);
        assert(ivec[1] == 2);
        assert(ivec[2] == 3);
    }

    {
        // another way to pass argument to short option.
        parser.reset();
        const char* argv[] = {
            "./run", "-a1,2,3"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        std::vector<int> ivec = parser.get_vector<int>("apple");
        assert(ivec[0] == 1);
        assert(ivec[1] == 2);
        assert(ivec[2] == 3);
    }

    {
        // pass argument to long option.
        parser.reset();
        const char* argv[] = {
            "./run", "--apple=1,2,3"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        std::vector<int> ivec = parser.get_vector<int>("apple");
        assert(ivec[0] == 1);
        assert(ivec[1] == 2);
        assert(ivec[2] == 3);
    }

    {
        // parser::option::delimiter(char) is used to change delimiter.
        parser.reset();
        const char* argv[] = {
            "./run", "--cat", "/bin:/usr/local/bin:/opt/bin"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        std::vector<std::string> svec = parser.get_vector<std::string>("cat");
        assert(svec[0] == "/bin");
        assert(svec[1] == "/usr/local/bin");
        assert(svec[2] == "/opt/bin");
    }

    std::cout << "\033[34m[[[ test_vector_args() PASSED ]]]\033[m" << std::endl;
}



void test_vector_initialize()
{
    cola::parser parser;

    std::vector<int> default_ivec(3);
    default_ivec[0] = 1;
    default_ivec[1] = 2;
    default_ivec[2] = 3;

    parser.define("a", "A for ant").with_arg_vector<int>(default_ivec);
    parser.define("b", "B for bat").delimiter('x').with_arg_vector<int>("1x2x3");
    parser.define("c", "C for cat").delimiter('/').with_arg_vector<std::string>("/usr/local/bin");

    const char* argv[] = {
        "./run",
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    parser.parse(argc, argv);

    assert(parser.get_vector<int>("a")[0] == parser.get_vector<int>("b")[0]);
    assert(parser.get_vector<int>("a")[1] == parser.get_vector<int>("b")[1]);
    assert(parser.get_vector<int>("a")[2] == parser.get_vector<int>("b")[2]);
    assert(parser.get_vector<std::string>("c")[0] == "usr");
    assert(parser.get_vector<std::string>("c")[1] == "local");
    assert(parser.get_vector<std::string>("c")[2] == "bin");

    std::cout << "\033[34m[[[ test_vector_initialize() PASSED ]]]\033[m" << std::endl;
}



void test_rest_arg()
{
    cola::parser parser;

    parser.define("a", "A for ant");
    parser.define("b", "B for bat");
    parser.define("c", "C for cat");

    const char* argv[] = {
        "./run", "-a", "-b", "--abc", "-d", "-c", "target.dat", "with-dash-arg"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);

    parser.parse(argc, argv);

    assert(parser.rest_args()[0] == "--abc");
    assert(parser.rest_args()[1] == "-d");
    assert(parser.rest_args()[2] == "target.dat");
    assert(parser.rest_args()[3] == "with-dash-arg");

    std::cout << "\033[34m[[[ test_rest_arg() PASSED ]]]\033[m" << std::endl;
}


void test_undefined_options()
{
    cola::parser parser;

    parser.define("a", "A for ant");
    parser.define("b", "B for bat");
    parser.define("c", "C for cat");

    {
        parser.reset();
        const char* argv[] = {
            "./run", "-a", "-b", "-c"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        assert(!parser.exists_undefined_options());
    }

    {
        parser.reset();
        const char* argv[] = {
            "./run", "-a", "-b", "-c", "-d"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        assert(parser.exists_undefined_options());
    }

    {
        parser.reset();
        const char* argv[] = {
            "./run", "-a", "-b", "-c", "--undefined"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        parser.parse(argc, argv);

        assert(parser.exists_undefined_options());
    }

    std::cout << "\033[34m[[[ test_undefined_options() PASSED ]]]\033[m" << std::endl;
}


void usage_example(int argc, char* argv[])
{
    cola::parser parser;

    parser.define("output", "output file")
          .alias('o')
          .with_arg<std::string>("output.dat");

    parser.define("test-cases", "test cases to run")
          .alias('t')
          .with_arg_vector<int>("1, 2, 3");

    parser.define("optimize-level", "optimization level")
          .alias('O')
          .with_arg<int>(0);

    parser.define("debug-level", "debug level")
          .alias('d')
          .with_arg<int>(0);

    parser.define("dry-run", "enable dry run mode");

    parser.define("version", "print version and exit")
          .alias('v')
          .alias('V');

    parser.define("help", "print this text and exit")
          .alias('h');

    parser.define("hidden", "this is hidden option")
          .hidden();

    parser.parse(argc, argv);

    if(parser.is_passed("hidden")) {
        std::cout << "+++++++++++++++++++\n"
                     "--hidden is passed!\n"
                     "+++++++++++++++++++\n" << std::endl;
    }

    std::cerr << "== USAGE EXAMPLE =================================================================\n";
    parser.easy_usage("test program for cola.", std::cerr);
    std::cerr << "==================================================================================\n";
}
