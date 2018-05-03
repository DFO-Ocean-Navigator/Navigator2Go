#ifndef DATASETVIEW_H
#define DATASETVIEW_H

#include <QDialog>
#include <QJsonValueRef>
#include <QString>

/***********************************************************************************/
// Forward Declarations
namespace Ui {
class DatasetView;
}

/***********************************************************************************/
class DialogDatasetView : public QDialog {
	Q_OBJECT
public:
	explicit DialogDatasetView(QWidget* parent = nullptr);
	~DialogDatasetView();

	void SetData(const QString& datasetKey, const QJsonValueRef object);

private:
	Ui::DatasetView* m_ui;
};

#endif // DATASETVIEW_H
