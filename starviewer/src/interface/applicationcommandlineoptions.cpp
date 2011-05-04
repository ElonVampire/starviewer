#include "applicationcommandlineoptions.h"

#include <QString>
#include <QObject>

namespace udg {

const QString ApplicationCommandLineOptions::optionSelectorCharacter("-");

bool ApplicationCommandLineOptions::addOption(QString optionName, bool optionArgumentIsRequired, QString description)
{
    if (!m_commandLineOptions.contains(optionName))
    {
        Option newOption;

        newOption.name = optionName;
        newOption.description = description;
        newOption.argumentIsRequired = optionArgumentIsRequired;

        m_commandLineOptions.insert(optionName, newOption);
        return true;
    }
    else return false;
}

bool ApplicationCommandLineOptions::parseArgumentList(QStringList argumentList)
{
    m_argumentList = argumentList;
    return parse();
}

QStringList ApplicationCommandLineOptions::getArgumentList()
{
    return m_argumentList;
}

/**Les comandes de línia està compostes per un opció que es precedida d'un '-' i llavors pot contenir a continuació un argument,
  * indicant el valor de la opció*/
bool ApplicationCommandLineOptions::parse()
{
    QStringList argumentList = m_argumentList;
    QString parameter;
    bool lastParameterWasAnOption = false, nextParameterHasToBeAnArgumentOption = false;
    Option lastOption;

    m_parserErrorMessage = "";
    argumentList.removeFirst();//Treiem el primer string que és el nom de l'aplicació;

    //Mentre hi hagi arguments per processar o no s'hagi produït un error parsegem els arguments
    while (!argumentList.isEmpty() && m_parserErrorMessage.isEmpty())
    {
        parameter = argumentList.takeFirst();

        if (isAnOption(parameter))
        {
            if (!nextParameterHasToBeAnArgumentOption)
            {
                parameter = parameter.right(parameter.length() -1); //treiem el "-" del paràmetre
                if (m_commandLineOptions.contains(parameter)) //Comprovem si és una opció vàlida
                {
                    //Si és una opció que ens han especificat com a vàlida l'inserim com a parsejada, de moment com argument de l'opció hi posem ""
                    m_parsedOptions.insert(parameter, "");

                    lastParameterWasAnOption = true;
                    lastOption = m_commandLineOptions.value(parameter);
                    nextParameterHasToBeAnArgumentOption = lastOption.argumentIsRequired;
                }
                else m_parserErrorMessage += QObject::tr("Unknown option ") + optionSelectorCharacter + parameter + "\n";
            }
            else
            {
                /*Si l'últim paràmetre parsejat era una opció que se li havia de passar obligatòriament un argument ex "-accessionnumber 12345"
                 *i no se li ha especificat cap argument ex: "-accessionnumber -studyUID" guardem l'error i parem.*/
                m_parserErrorMessage += lastOption.name + QObject::tr(" option requires an argument") + "\n";
            }
        }
        else
        {
            //és un argument
            if (lastParameterWasAnOption)
            {
                //Si tenim un argument i l'últim paràmetre era un opció, vol dir aquest paràmetre és un argument
                m_parsedOptions[lastOption.name] = parameter;
            }
            else m_parserErrorMessage += QObject::tr("Unexpected value ") + parameter + "\n";

            lastParameterWasAnOption = false;
            nextParameterHasToBeAnArgumentOption = false;
        }
    }

    if (nextParameterHasToBeAnArgumentOption)
    {
        m_parserErrorMessage += lastOption.name + QObject::tr(" option requires an argument") + "\n";
    }

    return m_parserErrorMessage.isEmpty();
}

bool ApplicationCommandLineOptions::isSet(QString optionName)
{
    return m_parsedOptions.contains(optionName);
}

QString ApplicationCommandLineOptions::getOptionArgument(QString optionName)
{
    if (isSet(optionName))
    {
        return m_parsedOptions[optionName];
    }
    else return NULL;
}

int ApplicationCommandLineOptions::getNumberOfParsedOptions()
{
    return m_parsedOptions.count();
}

QString ApplicationCommandLineOptions::getParserErrorMessage()
{
    return m_parserErrorMessage;
}

QString ApplicationCommandLineOptions::getOptionsDescription()
{
    QString optionsDescription;

    foreach (Option option, m_commandLineOptions.values())
    {
        optionsDescription += optionSelectorCharacter + option.name;

        if (option.argumentIsRequired)
        {
            optionsDescription += QObject::tr(" <value>");
        }

        optionsDescription += "\n\t" + option.description + "\n";
    }

    return optionsDescription;
}

bool ApplicationCommandLineOptions::isAnOption(QString optionName)
{
    return optionName.startsWith(optionSelectorCharacter); //Les opcions sempre comencen "-"
}

bool ApplicationCommandLineOptions::isAnArgument(QString argument)
{
    return !argument.startsWith(optionSelectorCharacter);
}

}
