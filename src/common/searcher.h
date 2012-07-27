#ifndef SEARCHER_H
#define SEARCHER_H

#include<QString>
#include<QMap>
#include<QList>
#include<QAction>
#include<QRegExp>
#include<QVector>
#include<QSet>

class WordActionsMap
{
public:
	WordActionsMap();
	void addWordsPerAction(QAction& act,const QStringList& words);
	void removeActionReferences(QAction& act);
	bool getActionsPerWord( const QString& word,QList<QAction*>& res ) const;
private:
	QMap<QString,QList<QAction*>> wordacts;
	const QList<QAction*> invalidemptylist;
};

class RankedMatches;

class WordActionsMapAccessor
{
public:
	WordActionsMapAccessor();
	void addWordsPerAction(QAction& act,const QString& st);
	inline void removeActionReferences(QAction& act) {map.removeActionReferences(act);}
	inline void setSeparator(const QRegExp& sep) {sepexp = sep;}
	inline void setIgnoredWords(const QRegExp& ign) {ignexp = ign;}
	int rankedMatchesPerInputString(const QString& input,RankedMatches& rm) const;
	inline QRegExp separtor() const {return sepexp;}
	inline QRegExp ignored() const {return ignexp;}
	void purifiedSplit(const QString& input,QStringList& res) const;

private:
	WordActionsMap map;
	QRegExp sepexp;
	QRegExp ignexp;
};

class RankedMatches
{
public:
	RankedMatches();
	void getActionsWithNMatches(const int n,QList<QAction*>& res); 
private:
	friend int WordActionsMapAccessor::rankedMatchesPerInputString(const QString& input,RankedMatches& rm) const;
	int computeRankedMatches(const QStringList& inputst,const WordActionsMap& map);
	QMap<QAction*,int> wordmatchesperaction;
	QVector<QList<QAction*>> ranking;
};

#endif