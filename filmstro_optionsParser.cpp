
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

    filmstro_optionsParser.cpp
    Created: 16 Feb 2017 4:15:52pm
    Author:  Daniel Walz / Filmstro Ltd.

 ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

OptionsParser::Option* OptionsParser::addOption (juce::String optId, juce::String optArg,
                                                   const OptionType type, const bool req)
{
    OptionsParser::Option* o = new OptionsParser::Option (optId);
    o->arg      = optArg;
    o->type     = type;
    return options.add (o);
}

OptionsParser::Option* OptionsParser::findOption (const juce::String& argument, const bool endOfArguments)
{
    if (!endOfArguments && argument.startsWith("--")) {
        for (Option* o : options)
            if (o->longArg == argument.substring(2))
                return o;
    }
    else if (!endOfArguments && argument.startsWith("-")) {
        for (Option* o : options)
            if (o->arg == argument.substring(1))
                return o;
    }
    else {
        for (Option* o : options)
            if (o->arg.isEmpty() && o->longArg.isEmpty() && !o->isOptionSet()) {
                // options without arg and optArg we set directly
                o->setValue (argument);
                return o;
            }
    }

    return nullptr;
}

juce::String OptionsParser::getHelpText () const
{
    juce::String text (header);
    for (Option* o : options) {
        if (text.isNotEmpty()) text += NewLine();
        text += o->getHelpText();
    }
    if (footer.isNotEmpty()) {
        if (text.isNotEmpty()) text += NewLine();
        text += footer;
    }
    return text;
}

bool OptionsParser::parseArguments (const juce::StringArray& arguments, const bool failOnUnknownOption)
{
    errorMessage.clear();
    bool ok = true;
    bool endOfArguments = false;

    for (int pos = 0; pos < arguments.size(); ++pos) {
        if (arguments [pos] == "--") {
            endOfArguments = true;
            continue;
        }
        if (OptionsParser::Option* option = findOption (arguments [pos], endOfArguments)) {
            if (! option->isOptionSet()) {
                if (option->type == OptionsParser::OptBoolean) {
                    option->setValue (true);
                }
                else if (option->type == OptionsParser::OptFile) {
                    if (pos + 1 < arguments.size()) {
                        juce::String arg = arguments [++pos];
                        if (File::isAbsolutePath (arg)) {
                            option->setValue (arg);
                        }
                        else {
                            option->setValue (File::getCurrentWorkingDirectory().getChildFile (arg).getFullPathName());
                        }
                    }
                    else {
                        appendErrorMessage ("Missing path for argument " + arguments [pos]);
                        ok = false;
                    }
                }
                else {
                    if (pos + 1 < arguments.size()) {
                        option->setValue (arguments [++pos]);
                    }
                    else {
                        appendErrorMessage ("Missing value for argument " + arguments [pos]);
                        ok = false;
                    }
                }
            }
        }
        else {
            if (failOnUnknownOption) {
                appendErrorMessage ("Unknown option: " + arguments [pos]);
                ok = false;
            }
            else {
                appendErrorMessage ("Ignoring unknown option: " + arguments [pos]);
            }
        }
    }

    // check if all requireds are met
    for (Option* o : options) {
        if (o->required && !o->isOptionSet()) {
            appendErrorMessage ("Argument is required: " + o->getOptionName ());
            ok = false;
        }
    }

    return ok;
}

void OptionsParser::appendErrorMessage (const juce::StringRef message)
{
    if (errorMessage.isNotEmpty()) errorMessage += NewLine();
    errorMessage += message;
}

juce::String OptionsParser::getErrorMessage () const
{
    return errorMessage;
}

bool OptionsParser::isOptionSet  (juce::StringRef optId) const
{
    if (const Option* o = getOption (optId)) {
        return o->isOptionSet ();
    }
    return false;
}

juce::String OptionsParser::getOptString (juce::StringRef optId) const
{
    if (const Option* o = getOption (optId))
        return o->value;
    return String();
}

juce::File OptionsParser::getOptFile (juce::StringRef optId) const
{
    if (const Option* o = getOption (optId))
        return File (o->value);
    return File();
}

int OptionsParser::getOptInt (juce::StringRef optId) const
{
    if (const Option* o = getOption (optId))
        return o->value;
    return 0;
}

double OptionsParser::getOptDouble (juce::StringRef optId) const
{
    if (const Option* o = getOption (optId))
        return o->value;
    return 0.0;
}

bool OptionsParser::getOptBoolean (juce::StringRef optId) const
{
    if (const Option* o = getOption (optId))
        return o->value;
    return false;
}

OptionsParser::Option* OptionsParser::getOption (juce::StringRef optId)
{
    for (OptionsParser::Option* o : options) {
        if (o->optionId == optId) {
            return o;
        }
    }
    return nullptr;
}

const OptionsParser::Option* OptionsParser::getOption   (juce::StringRef optId) const
{
    for (const OptionsParser::Option* o : options) {
        if (o->optionId == optId) {
            return o;
        }
    }
    return nullptr;

}

juce::String OptionsParser::Option::getOptionName () const
{
    if (arg.isEmpty()) {
        if (longArg.isNotEmpty()) return longArg;
        return optionId;
    }
    if (longArg.isEmpty()) return arg;
    return arg + " | " + longArg;
}

juce::String OptionsParser::Option::getHelpText () const
{
    juce::String text;
    arg.isNotEmpty () ? text += "  -" + arg + "  " : text += "      ";
    if (longArg.isNotEmpty ()) text += "--" + longArg;
    text += " " + getVariableName();

    if (helpText.isNotEmpty()) {
        text = text.paddedRight (' ', 30) + helpText;
    }
    if (! value.isVoid() && ! isSet) {
        text += " (default: " + value.toString() + ")";
    }
    return text;
}

juce::String OptionsParser::Option::getVariableName () const
{
    switch (type) {
        case OptString:  return "<name>";
        case OptFile:    return "<filename>";
        case OptInteger: return "<number>";
        case OptDouble:  return "<number>";
        default:         return String();
    }
}

bool OptionsParser::Option::isOptionSet () const
{
    return isSet;
}

void OptionsParser::Option::setValue (juce::var v)
{
    value = v;
    isSet = true;
}
