#ifndef GLOBALDEFINES
#define GLOBALDEFINES
#include <QHash>
#include <QStringList>
#include <QRegularExpression>
#include <QHashIterator>

class GlobalDefines{

public:
    static GlobalDefines& Instance()
    {
        static GlobalDefines instance;
        return instance;
    }
    QMap<QString,QStringList>& GetLogFilter()
    {
        return LogFilters;
    }
    bool interest(QString line, QString key = "")
    {
        if(key.isEmpty())
        {
            return true;
        }
        else if(LogFilters.contains(key))
        {
            return line.contains(QRegularExpression(LogFilters.value(key).join("|")));
        }
        else
        {
            return true;
//            QHashIterator<QString, QStringList> i(LogLevelType);
//            while(i.hasNext())
//            {
//                i.next();
//                if(line.contains(i.value().join("|")))
//                {
//                    return true;
//                    break;
//                }
//            }
        }
        return false;
    }

private:
    GlobalDefines()
    {
        LogFilters.insert("program",QStringList() <<"67174418" << "67175216"<<"67174912"); //start program; start step; start pretest.
        LogFilters.insert("scenario", QStringList() << "67174995" //Start heating level sensor
                                   << "67175210"<<"67175212"      //Start filling" ; "Moveing the rotary valve to the sealing position"
                                   << "67175213"<< "67175211"//"Start processing" << "Moving the rotary valve to the tube position"
                                   << "67175207"); //";Draining.");
        LogFilters.insert("info", QStringList()<< ";Info;");
        LogFilters.insert("warning", QStringList()<< ";Warning;");
        LogFilters.insert("error", QStringList()<< ";Error;");
        LogFilters.insert("debug", QStringList()<< ";Debug;");
    }
    QMap<QString,QStringList> LogFilters;
};



#endif // GLOBALDEFINES
