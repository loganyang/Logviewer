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
        LogFilters.insert("program",QStringList() <<"User start the program" << "Start Step"<<"Start the pretest");
        LogFilters.insert("scenario", QStringList() << "Start heating level sensor"
                                   << "Start filling" << "Moveing the rotary valve to the sealing position"
                                   << "Start processing" << "Moving the rotary valve to the tube position"
                                   << ";Draining.");
        LogFilters.insert("info", QStringList()<< ";Info;");
        LogFilters.insert("warning", QStringList()<< ";Warning;");
        LogFilters.insert("error", QStringList()<< ";Error;");
        LogFilters.insert("debug", QStringList()<< ";Debug;");
    }
    QMap<QString,QStringList> LogFilters;
};



#endif // GLOBALDEFINES

