#ifndef QMLBOOSTER_H
#define QMLBOOSTER_H

#include "eventhandler.h"
#include "booster.h"

class QMLBoosterData;

class QMLBooster : public Booster
{
public:
    QMLBooster();
    ~QMLBooster();

    virtual const std::string &boosterType() const;
    virtual bool preload();

protected:
    virtual bool receiveDataFromInvoker(int socketFd);
    virtual void preinit();

private:
    QMLBoosterData *data;
    static const std::string m_boosterType;
};

#endif
