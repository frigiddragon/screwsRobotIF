#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include<QThread>

class cameraThread : public QThread
{
    Q_OBJECT
public:
    cameraThread();
    void run() {}
};

#endif // CAMERATHREAD_H
