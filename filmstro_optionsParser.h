/*
  ==============================================================================

  Copyright (c) 2017, Filmstro Ltd.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its contributors
     may be used to endorse or promote products derived from this software without
     specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
  OF THE POSSIBILITY OF SUCH DAMAGE.

  ==============================================================================

  BEGIN_JUCE_MODULE_DECLARATION
 
  ID:            filmstro_optionsParser
  vendor:        Filmstro Ltd.
  version:       0.9.0
  name:          Commandline parser for arguments
  description:   Parses a commandline. Generates help text and error messages automatically
  dependencies:  juce_core
  website:       http://www.filmstro.com/
  license:       BSD V2 3-clause

  END_JUCE_MODULE_DECLARATION
 
  ==============================================================================
 */


#ifndef FILMSTRO_OPTIONS_PARSER_H_INCLUDED
#define FILMSTRO_OPTIONS_PARSER_H_INCLUDED

#include <juce_core/juce_core.h>

/**
 This class provides a parser for command line arguments in unix style.
 
 You can add options with long arguments prefixed by "--" or short arguments using "-".
 It will create a help text and error messages automatically.
 An example would look like that:
 
 \code{.cpp}
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
 \endcode

 */
class OptionsParser {
public:

    enum  OptionType {
        OptString = 0,
        OptFile,
        OptInteger,
        OptDouble,
        OptBoolean
    };

    class Option {
    public:
        Option (juce::String optId)
          : optionId  (optId),
            required  (false),
            mustExist (false),
            isSet     (false)
        {}

        juce::String optionId;  //< id to look up an option
        juce::String arg;       //< short argument prefixed by "-"
        juce::String longArg;   //< argument prefixed by "--"
        juce::String helpText;  //< a text to explain the option
        bool         required;  //< parseArgument will fail, if a required option is not set
        bool         mustExist; //< for filenames

        OptionType   type;      //< type of the option

        /** Returns the readable string how the option shall be set (arg or longArg) */
        juce::String getOptionName () const;

        /** Returns the helpText prefixed by the optionName in one line */
        juce::String getHelpText () const;

        /** Returns a description of the expected option type */
        juce::String getVariableName () const;

        /** After parseArguments check if the option was set by the user */
        bool         isOptionSet () const;

        /** For the parser to set a value and set the isSet flag */
        void         setValue (juce::var v);

        /** Use this before parseArguments to set a default value */
        juce::var    value;

    private:
        bool         isSet;

    };

    /** Create an option to be used in the parser */
    OptionsParser::Option* addOption (juce::String optId, juce::String optArg, const OptionType, const bool req=false);

    /** Returns a help text for all options */
    juce::String getHelpText () const;

    /** Read arguments and set them into the options. Returns true, if all requirements are met. */
    bool         parseArguments (const juce::StringArray& arguments, const bool failOnUnknownOption = true);

    /** if parseArguments failed, this will contain a helpful text about bad arguments */
    juce::String getErrorMessage () const;

    /** After parseArguments check if a certain option was set by the user */
    bool         isOptionSet  (juce::StringRef optId) const;

    /** Return a text value set by argument */
    juce::String getOptString (juce::StringRef optId) const;

    /** Return a file set via option. It will also create paths relative to the executable */
    juce::File   getOptFile   (juce::StringRef optId) const;

    /** Return an integer value set by argument */
    int          getOptInt    (juce::StringRef optId) const;

    /** Return a float value set by argument */
    double       getOptDouble (juce::StringRef optId) const;

    /** Return a boolean value set by a flag */
    bool         getOptBoolean(juce::StringRef optId) const;

    /** Returns a pointer to a option instance */
    Option*      getOption    (juce::StringRef optId);
    const Option* getOption   (juce::StringRef optId) const;

    /** This will be printed before the help text */
    juce::String header;
    /** This will be printed after the help text */
    juce::String footer;

private:
    OptionsParser::Option* findOption (const juce::String& argument, const bool endOfArguments);

    void appendErrorMessage (const juce::StringRef message);

    juce::OwnedArray<Option> options;

    juce::String        errorMessage;
};



#endif  // FILMSTRO_OPTIONS_PARSER_H_INCLUDED
