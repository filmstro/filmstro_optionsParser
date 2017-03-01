This class provides a parser for command line arguments in unix style.
 
You can add options with long arguments prefixed by "--" or short arguments using "-".
It will create a help text and error messages automatically.
An example would look like that:
 
    OptionsParser options;

    options.header = "My console application";
    options.footer = "Build date: " + String(__DATE__) + " at " + String(__TIME__);

    OptionsParser::Option* option = options.addOption ("help", "h", OptionsParser::OptBoolean);
    option->longArg     = "help";
    option->helpText    = "Display this help text and exit";

    option = options.addOption ("logfile", "l", OptionsParser::OptFile, false);
    option->longArg     = "logfile";
    option->helpText    = "Set a logfile to enable logging";

    if (! options.parseArguments (StringArray::fromTokens (commandLine, true))) {
        std::cout << options.getErrorMessage () << std::endl;
        exit (-1);
    }
    if (options.getErrorMessage().isNotEmpty()) // there can also be warnings...
        std::cout << options.getErrorMessage () << std::endl;

    if (options.getOptBoolean("help")) {
        std::cout << options.getHelpText() << std::endl;
        exit (0);
    }
 
    if (options.isOptionSet ("logfile")) {
        File logfile = options.getOptFile("logfile");
        Logger::setCurrentLogger (new FileLogger (logfile, "Started My Application"));
    }

It will understand the following types:

1.  OptionsParser::OptString
    a normal string value. Use quotes if you want spaces in one argument

2.  OptionsParser::OptFile
    will also support relative strings. You can add mustExist to the option

3.  OptionsParser::OptInteger
    an numeric value

4.  OptionsParser::OptDouble
    a rational number (floating point)

5.  OptionsParser::OptBoolean
    a yes/no flag. this needs no argument, option present means true, option not present means false.
    Note that a boolean with required makes no sense.

For an option you can set following options

1.  optionId: to retrieve the value later
2.  arg: the letter, will be prefixed with one -
3.  longArg: the name, will be prefixed with --
4.  helpText: this text will be displayed with getHelpText(). The arg and longArg are added and formatted automatically
5.  required: the parser will fail, if this option is not supplied
6.  mustExist: for files, the parser will fail, if the file does not exist
7.  type: the expected type of the argument

Brighton, 2017

