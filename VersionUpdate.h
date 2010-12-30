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

#ifndef VERSIONUPDATE_H
#define VERSIONUPDATE_H

#include <QNetworkAccessManager>
#include <QString>
#include <QUrl>

class QNetworkReply;

class VersionUpdate : public QObject
{
	Q_OBJECT

public:
	VersionUpdate(QObject* parent = 0);
	void parseServerReply(QString phpReply);
	void download();

signals:
	void downloadFinished(QString execName);

private slots:
	void saveDownload(QNetworkReply* reply);

private:
	bool checkFile(int* downloadedBytes = 0);
	void extractDownload();

private:
	QNetworkAccessManager m_manager;
	int m_size;
	QString m_md5;
	QString m_name;
	QUrl m_url;
};

#endif // VERSIONUPDATE_H
