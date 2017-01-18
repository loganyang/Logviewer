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
        LogFilters.insert("program",QStringList() <<"67174418" << "67175216"<<"67174912" //start program; start step; start pre-test
                          <<"67174420"<<"67175205" <<"67174421" //pause program; pause successfully; abort program
                          <<"67175218" <<"67175224" //dry step ; cooling down
                          <<"67174656" // start the self-test
                          );
        LogFilters.insert("scenario", QStringList() << "67174995" //Start heating level sensor
                                   << "67175210"<<"67175212"      //Start filling" ; "Moveing the rotary valve to the sealing position"
                                   << "67175213"<< "67175211"//"Start processing" << "Moving the rotary valve to the tube position"
                                   << "67175207"); //";Draining.");
//        LogFilters.insert("UserOperations",QStringList()<<"67174416" // user shutdow device
//                          <<"67176214" // power failure
//                          <<"67174927" // bottle check
//                          <<"67174420" // puase
//                          <<"67174421" // abort
//                          );
        LogFilters.insert("info", QStringList()<< ";Info;");
        LogFilters.insert("warning", QStringList()<< ";Warning;");
        LogFilters.insert("error", QStringList()<< ";Error;");
        LogFilters.insert("debug", QStringList()<< ";Debug;");

    }
    QMap<QString,QStringList> LogFilters;
};



#endif // GLOBALDEFINES

