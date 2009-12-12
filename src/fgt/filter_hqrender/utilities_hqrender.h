#include <QDir.h>
#include <QString.h>
#include <QStringList.h>
#include <math.h>

namespace UtilitiesHQR {
  QString getDirFromPath(QString* path);
  QString getFileNameFromPath(QString* path, bool type = true);
  QString quotesPath(QString* path);
  bool checkDir(QString destDirString, QString path);
  bool copyFiles(QDir templateDir,QDir destDir,QStringList dirs);
  bool delDir(QDir dir, QString toDel);
  int numberOfCiphers(int number);
};