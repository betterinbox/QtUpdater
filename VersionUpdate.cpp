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

#include "VersionUpdate.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>
#include <QCryptographicHash>
#include <QFile>

VersionUpdate::VersionUpdate(QObject* parent) :
	QObject(parent),
	m_manager(),
	m_size(0),
	m_md5(),
	m_name(),
	m_url()
{
}

/*!
  Instanciate the value of the VersionUpdate atribute such as URL MD5 SIZE
*/
void VersionUpdate::parseServerReply(QString phpReply)
{
	QStringList fields = phpReply.split("\n", QString::SkipEmptyParts);

	foreach (QString field, fields) {
		field = field.remove(' ');
		int pos = field.indexOf(":");
		QString key = field.left(pos).toLower();
		QString val = field.mid(pos + 1);
		if (key.isEmpty() || val.isEmpty()) {
			qDebug() << "UpdateManager: Invalide phpReply on field: " << field;
		}
		if (key == "url") {
			m_url = QUrl(val);
			if (!m_url.isValid()) {
				qDebug() << "UpdateManager: Invalide field Url: " << m_url;
				return;
			}
			m_name = val.mid(val.lastIndexOf("/") + 1);
			if (m_name.isEmpty()) {
				qDebug() << "UpdateManager: Invalide file name in Url: " << m_url;
				return;
			}
		} else if (key == "md5") {
			m_md5 = val.toLower();
			if (m_md5.count() != 32) {
				qDebug() << "UpdateManager: Invalide field md5: " << m_md5;
				return;
			}
		} else if (key == "size") {
			m_size = val.toInt();
			if (m_size <= 0) {
				qDebug() << "UpdateManager: Invalide field size: " << m_md5;
				return;
			}
		}
	}
	if (m_size == 0) {
		qDebug() << "UpdateManager: missing field size for start downloding new vertion";
		return;
	}
	if (m_url.isEmpty()) {
		qDebug() << "UpdateManager: missing field url for start downloding new vertion";
		return;
	}
	if (m_md5.isEmpty()){
		qDebug() << "UpdateManager: missing field md5 for start downloding new vertion";
		return;
	}
	qDebug() << "UpdateManager: information correctly get from update server";
}

/*!
  Check if file already exists and set the optional parameter downloadedBytes
*/
bool VersionUpdate::checkFile(int* downloadedBytes)
{
	QFile file(m_name);
	if (file.exists()) {
		if (downloadedBytes) {
			*downloadedBytes = file.size();
		}
		if (file.size() == m_size) {
			if (!file.open(QIODevice::ReadOnly)) {
				qDebug() << "UpdateManager: Cannot open file: " << m_name;
			}
			QByteArray tmp = file.readAll();
			QByteArray fileMd5 = QCryptographicHash::hash(tmp, QCryptographicHash::Md5);
			if (m_md5 == fileMd5.toHex().toLower()) {
				qDebug() << "UpdateManager: Md5 correct on File "  << m_name;
				return true;
			} else {
				qDebug() << "UpdateManager: Md5 uncorrect "  << m_name;
				if (!file.remove()) {
					qDebug() << "UpdateManager: Cannot remove invalide file: "  << m_name;
				}
			}
		}
		file.close();
	}
	return false;
}

/*!
  Download the version if necessary
*/
void VersionUpdate::download()
{
	int downloadedBytes = 0;
	if (checkFile(&downloadedBytes)) {
		qDebug() << "UpdateManager: No need to download";
		extractDownload();
	} else {
		connect(&m_manager, SIGNAL(finished(QNetworkReply*)), this,
				SLOT(saveDownload(QNetworkReply*)));

		QNetworkRequest request(m_url);

		if (downloadedBytes > 0) {
			QString range = "bytes=" + QString::number(downloadedBytes) + "-" + QString::number(m_size);
			qDebug() << "--- " << range;
			request.setRawHeader("Range", range.toAscii());
		}
		m_manager.get(request);
	}
}

/*!
  Slot call when the download is finish
*/
void VersionUpdate::saveDownload(QNetworkReply* reply)
{
	QUrl url = reply->url();
	if (reply->error()) {
		qDebug() << "UpdateManager: Download of: " << reply->errorString() << " failed: " << reply->errorString();
	} else {
		QFile file(m_name);
		if (!file.open(QIODevice::Append)) {
			qDebug() << "UpdateManager: Could not open " <<  m_name << " for writing: " << file.errorString();
			return;
		}

		file.write(reply->readAll());
		file.close();
		qDebug() << "UpdateManager: File save as: " << m_name;
		extractDownload();
	}
}

/*!
  Uncompress the version download
*/
void VersionUpdate::extractDownload()
{
	if (!checkFile()) {
		qDebug() << "UpdateManager: Error: cannot extract cause Download is not complet ";
		return;
	}
	QFile archiveFile(m_name);
	if (m_name.right(4) != ".biz") {
		qDebug() << "UpdateManager: Error: Bad name for the archive: " << m_name;
		return;
	}
	if (!archiveFile.open(QIODevice::ReadOnly)) {
		qDebug() << "UpdateManager: Error: Cannot open in read: " << m_name;
		return;
	}

	qDebug() << "UpdateManager: Reading archive: " << m_name;
	QByteArray compress = archiveFile.readAll();

	if (compress.isEmpty()) {
		qDebug() << "UpdateManager: Error: The download is empty: " << m_name;
		return;
	}

	QString execName = m_name;
	execName.chop(4);
	QFile exec(execName);
	if (!exec.open(QIODevice::WriteOnly)) {
		qDebug() << "UpdateManager: Error: Cannot open in write: " << execName;
		return;
	}

	qDebug() << "UpdateManager: Uncompress archive: " << m_name;
	QByteArray unCompress = qUncompress(compress);

	if (unCompress.isEmpty()) {
		qDebug() << "UpdateManager: Error: Cannot uncompress: " << m_name;
	}

	qDebug() << "UpdateManager: write file: " << execName;
	exec.write(unCompress);

	if (!exec.setPermissions(QFile::ExeUser | QFile::WriteUser)) {
		qDebug() << "UpdateManager: Error: Cannot set Permissions " << (QFile::ExeUser | QFile::WriteUser);
	}

	qDebug() << "UpdateManager: Archive uncompress sucesfuly";
	emit downloadFinished(execName);
}
