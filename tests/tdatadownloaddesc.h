#ifndef TDATADOWNLOADDESC_H
#define TDATADOWNLOADDESC_H

#include <QObject>

namespace Test {

class TDataDownloadDesc : public QObject {
	Q_OBJECT

private slots:

	void check_noexcept();

	void check_returnType_GetAPIQuery();

};

} // namespace Test

#endif // TDATADOWNLOADDESC_H
