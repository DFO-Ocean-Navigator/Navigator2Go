#ifndef FILECOPYWORKER_H
#define FILECOPYWORKER_H

#include <QObject>

namespace IO {

class FileCopyWorker : public QObject {
	Q_OBJECT
public:
	explicit FileCopyWorker(const QStringList& fileList, QObject* parent = nullptr);

public slots:
	void copyFiles();

signals:
	void progress(const std::size_t percentDone);
	void finished();
	void error(const QString& error);

private:
	Q_DISABLE_COPY(FileCopyWorker)

	const QStringList m_fileList;
};

} // namespace IO

#endif // FILECOPYWORKER_H
