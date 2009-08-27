#ifndef UDGPRINTERMANAGER_H
#define UDGPRINTERMANAGER_H

#include <QList>

/**
* Interfície pels manipuladors d'impressores (Afegir, Esborrar...).
*/

namespace udg{

    class Printer;

class PrinterManager
{
 public:
	virtual void addPrinter(Printer &_printer)=0;
    virtual void updatePrinter(Printer &_printer)=0;
    virtual void removePrinter(Printer &_printer)=0;
    virtual void removePrinter(QString &_reference)=0;
};
}; 
#endif