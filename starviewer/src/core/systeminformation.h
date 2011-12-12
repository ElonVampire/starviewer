#ifndef UDGSYSTEMINFORMATION_H
#define UDGSYSTEMINFORMATION_H

// Qt
#include <QString>
#include <QList>
#include <QSize>

namespace udg {

class SystemInformation {
public:
    enum OperatingSystem { OSWindows, OSMac, OSLinux };
    
    /// Destructor
    virtual ~SystemInformation();

    /// Crea una nova inst�ncia d'alguna de les classes que implementa la interf�cie
    static SystemInformation* newInstance();

    virtual OperatingSystem getOperatingSystem();
    // Arquitectura de 32-bits o 64-bits
    virtual bool isOperatingSystem64BitArchitecture();
    virtual QString getOperatingSystemVersion();
    /// Retorna la versi� de service pack instal�lat, nom�s en windows
    virtual QString getOperatingSystemServicePackVersion();

    /// Retorna la quantitat total de mem�ria RAM en MegaBytes
    virtual unsigned int getRAMTotalAmount();
    //En MBytes
    virtual QList<unsigned int> getRAMModulesCapacity();
    //En MHz
    virtual QList<unsigned int> getRAMModulesFrequency();

    virtual unsigned int getCPUNumberOfCores();
    virtual QList<unsigned int> getCPUFrequencies();
    virtual unsigned int getCPUL2CacheSize(); // en KBytes

    virtual QList<QString> getGPUBrand();
    virtual QList<QString> getGPUModel();
    virtual QList<unsigned int> getGPURAM();
    virtual QList<QString> getGPUOpenGLCompatibilities();
    virtual QString getGPUOpenGLVersion();
    virtual QList<QString> getGPUDriverVersion();

    //Screen, Display, Monitor, Desktop, ...
    QList<QSize> getScreenResolutions();
    virtual QList<QString> getScreenVendors();

    virtual QList<QString> getHardDiskDevices();
    virtual unsigned int getHardDiskCapacity(const QString &device);
    virtual unsigned int getHardDiskFreeSpace(const QString &device);
    virtual bool doesOpticalDriveHaveWriteCapabilities();

    virtual unsigned int getNetworkAdapterSpeed();

protected:
    SystemInformation();
};

}

#endif
