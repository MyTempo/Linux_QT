#include "m_thread.h"

m_thread::m_thread(QObject *parent) : QObject(parent)
{
}

void m_thread::working()
{
    int a = 0;
    isMonitor = true;
    unsigned long result;
    while (isMonitor) {
        TagInfo tag;
        result = GetTagUii(hComm, &tag, 10000);
        if (result == 0x00) {
            emit curInfo(tag);


        }
    }
}
