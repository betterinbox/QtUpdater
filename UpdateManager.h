/*
 Copyright (C) 2010 by BetterInbox <contact@betterinbox.com>
 Original author: Adrien Chey <adrien@betterinbox.com>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/


#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QNetworkAccessManager>
#include <QUrl>

#include "VersionUpdate.h"

class QNetworkReply;

class UpdateManager: public QObject
{
	Q_OBJECT

public:
	UpdateManager(QObject* parent = 0);
	void checkUpdate(const QUrl url);

private slots:
	void checkVersionFinished(QNetworkReply* reply);
	void switchExecutables(QString newName);

private:
	QNetworkAccessManager m_manager;
	VersionUpdate m_newVersion;
};

#endif // UPDATEMANAGER_H
