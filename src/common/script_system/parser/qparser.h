/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QLALR module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPARSER_H
#define QPARSER_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QVarLengthArray>

template <typename _Parser, typename _Table, typename _Value = int>
class QParser: protected _Table
{
public:
    QParser();
    ~QParser();

    bool parse();

    inline _Value &sym(int index);

protected:
	_Value yyval;

private:
    inline int nextToken()
    {
        return static_cast<_Parser*> (this)->nextToken();
    }

    inline void consumeRule(int rule)
    {
        static_cast<_Parser*> (this)->consumeRule(rule);
    }

    enum { DefaultStackSize = 128 };

    struct Data: public QSharedData
    {
        Data(): stackSize (DefaultStackSize), tos (0) {}

        QVarLengthArray<int, DefaultStackSize> stateStack;
        QVarLengthArray<_Value, DefaultStackSize> parseStack;
        int stackSize;
        int tos;

        void reallocateStack() {
            stackSize <<= 1;
            stateStack.resize(stackSize);
            parseStack.resize(stackSize);
        }
    };

    QSharedDataPointer<Data> d;
};

template <typename _Parser, typename _Table, typename _Value>
inline _Value &QParser<_Parser, _Table, _Value>::sym(int n)
{
    return d->parseStack [d->tos + n - 1];
}

template <typename _Parser, typename _Table, typename _Value>
QParser<_Parser, _Table, _Value>::QParser():
    d(new Data()),yyval()
{
}

template <typename _Parser, typename _Table, typename _Value>
QParser<_Parser, _Table, _Value>::~QParser()
{
}

template <typename _Parser, typename _Table, typename _Value>
bool QParser<_Parser, _Table, _Value>::parse()
{
    const int INITIAL_STATE = 0;

    d->tos = 0;
    d->reallocateStack();

    int act = d->stateStack[++d->tos] = INITIAL_STATE;
    int token = -1;

    forever {
        if (token == -1 && - _Table::TERMINAL_COUNT != _Table::action_index[act])
            token = nextToken();

        act = _Table::t_action(act, token);

        if (d->stateStack[d->tos] == _Table::ACCEPT_STATE)
            return true;

        else if (act > 0) {
            if (++d->tos == d->stackSize)
                d->reallocateStack();

            d->parseStack[d->tos] = yyval;//d->parseStack[d->tos - 1];
            d->stateStack[d->tos] = act;
            token = -1;
        }

        else if (act < 0) {
            int r = - act - 1;
            qDebug() << "reducing using rule: " << spell[rule_info[rule_index[r]]] << "\n";
			d->tos -= _Table::rhs[r];
            act = d->stateStack[d->tos++];
            consumeRule(r);
            act = d->stateStack[d->tos] = _Table::nt_action(act, _Table::lhs[r] - _Table::TERMINAL_COUNT);
        }

        else break;
    }

    return false;
}


#endif // QPARSER_H
