#ifndef FILECOPYWORKER_H
#define FILECOPYWORKER_H

#include <QObject>

namespace IO {

class FileCopyWorker : public QObject {
	Q_OBJECT

public:
	explicit FileCopyWorker(QObject* parent = nullptr);

	void copyFiles();

public slots:

signals:
	void copyComplete();
};

} // namespace IO

#endif // FILECOPYWORKER_H
