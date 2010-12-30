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

#include "UpdateManager.h"

#include <QDebug>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QNetworkRequest>
#include <QNetworkReply>

UpdateManager::UpdateManager(QObject *parent) :
	QObject(parent),
	m_manager()
{
	connect(&m_newVersion, SIGNAL(downloadFinished(QString)), this,
			SLOT(switchExecutables(QString)));
}

/*!
  Main public function, a request to the Url should return a formated downloadInformationData
  see ReadMe for more detail
*/
void UpdateManager::checkUpdate(const QUrl url)
{
	connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
			SLOT(checkVersionFinished(QNetworkReply*)));

	QNetworkRequest request(url);
	qDebug() << "UpdateManager: request.url " << url ;
	m_manager.get(request);
}

void UpdateManager::checkVersionFinished(QNetworkReply *reply)
{
	QUrl url = reply->url();
	if (reply->error()) {
		qDebug() << "UpdateManager: Error: Cannot Check " <<  url.toEncoded().constData() << " failed: " << reply->errorString();
	} else {

		QString datas = QString(reply->readAll());
		qDebug() << datas;
		if (!datas.isEmpty()) {
			qDebug() << "UpdateManager: new version exist";
			m_newVersion.parseServerReply(datas);
			m_newVersion.download();
		} else {
			qDebug() << "UpdateManager: no new version exist";
		}
	}
	reply->deleteLater();
}

void UpdateManager::switchExecutables(QString newName)
{
	QString execName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
	QString previousExecName	= execName + ".bak";
	QFile oldExec(execName);
	QFile newExec(newName);

	QFile oldOldExec(previousExecName);
	if (oldOldExec.exists()) {
		if (!oldOldExec.remove()) {
			qDebug() << "UpdateManager: cannot delet the file " << previousExecName;
		}
	}

	if (!oldExec.rename(previousExecName)) {
		qDebug() << "UpdateManager: cannot rename " << execName << " in " << previousExecName;
		return;
	}

	if (!newExec.rename(execName)) {
		qDebug() << "UpdateManager: cannot rename " << newName << " in " << execName;
		return;
	}

	if (!newExec.setPermissions(QFile::ExeUser | QFile::WriteUser)) {
		qDebug() << "UpdateManager: cannot set Permissions " << (QFile::ExeUser | QFile::WriteUser);
		return;
	}

	qDebug() << "UpdateManager: good Switch!";
	//QApplication::exit();
}
