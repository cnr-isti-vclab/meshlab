#include "utilities_hqrender.h"

//path must have the filename
QString UtilitiesHQR::getDirFromPath(const QString* path) {
	//return path->left(path->lastIndexOf(QDir::separator())); //don't work :/
	if(path->lastIndexOf('\\') == -1 && path->lastIndexOf('/') == -1)
		return ".";
	return path->left(std::max<int>(path->lastIndexOf('\\'),path->lastIndexOf('/')));
}

QString UtilitiesHQR::getFileNameFromPath(const QString* path, bool type) {
	//return path->right(path->size() - 1 - path->lastIndexOf(QDir::separator())); //don't work :/
	QString temp = path->right(path->size() - 1 - std::max<int>(path->lastIndexOf('\\'),path->lastIndexOf('/')));
	if(type)
		return temp;
	else
		return temp.left(temp.lastIndexOf('.'));
}

//if path contains a space, is wrapped in quotes (e.g. ..\"Program files"\..)	
QString UtilitiesHQR::quotesPath(const QString* path) {
	QStringList dirs = path->split(QDir::separator());
	QString temp("");
	for(int i = 0; i < dirs.size(); i++) {
		if(!dirs[i].contains(" "))
			temp += dirs[i];
		else
			temp = temp + "\"" + dirs[i] + "\"";
		temp += QDir::separator();
	}
	//the final of path is separator!!!
	return temp;
}

//if dir not exist, create it
bool UtilitiesHQR::checkDir(const QString* destDirString, const QString* path) {
	QDir destDir(*destDirString);
	QStringList pathDirs = path->split('/');
	foreach(QString dir, pathDirs) {
		if(!destDir.cd(dir)) {
			destDir.mkdir(dir);
			destDir.cd(dir);
			qDebug("created: %s", qPrintable(destDir.absolutePath()));	
		}
	}
	return true;
}

//take all files in fromDir/[dirs] directories and copy them in dest/[dirs]
bool UtilitiesHQR::copyFiles(const QDir* fromDir, const QDir* destDir, const QStringList* dirs) {
	QDir src = *fromDir, dest = *destDir;
	foreach(QString dir, *dirs) {
		if(dir != "." && src.cd(dir)) {
			if(!dest.mkdir(dir)) {
				if(!dest.cd(dir))
					return false;
			}
			else
				dest.cd(dir);

			QStringList filesList = src.entryList(QDir::Files);
			foreach(QString file, filesList) {
				qDebug("copy file from %s to %s", 
					qPrintable(src.absolutePath() + QDir::separator() + file),
					qPrintable(dest.absolutePath() + QDir::separator() + file));	
				QFile::copy(src.absolutePath() + QDir::separator() + file, dest.absolutePath() + QDir::separator() + file);
			}
		}
	}
	return true;
}

//delete a directory and all file and subdirectory (recursive calls)
bool UtilitiesHQR::delDir(QDir* dir, const QString* toDel) {
	qDebug("Deleting: %s in %s", qPrintable(*toDel), qPrintable(dir->absolutePath()));
	if(!dir->rmdir(*toDel)) {
		dir->cd(*toDel);
		qDebug("I'm in %s", qPrintable(dir->absolutePath()));
		QStringList dirs = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
		foreach(QString entry, dirs) {
			qDebug("Cycle1 deleting file: %s in %s", qPrintable(entry), qPrintable(dir->absolutePath()));	
			dir->remove(entry);
		}
		dirs = dir->entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		foreach(QString entry, dirs) {
			qDebug("Cycle2 deleting dir: %s in %s", qPrintable(entry), qPrintable(dir->absolutePath()));	
			if(!dir->rmdir(entry)) {
				QDir temp = *dir;
				delDir(&temp, &entry);
			}
		}
		dir->cdUp();
		if(!dir->rmdir(*toDel))
			return false;
	}
	return true;
}

//return the number of ciphers of a integer
int UtilitiesHQR::numberOfCiphers(int number) {
	float n = 0;
	while( (number / pow(float(10),++n)) > 1);
	return int(n - 1);
}
