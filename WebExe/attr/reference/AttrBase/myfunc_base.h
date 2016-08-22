#ifndef MYFUNC_BASE_H
#define MYFUNC_BASE_H
#include <QString>

void myFunc_pathSlashAdd(QString& fpath);
void myFunc_confirmDirExist(const QString& fpath);
void getStdPercent(const char* fpath, double &stdPercent);

#endif // MYFUNC_BASE_H
