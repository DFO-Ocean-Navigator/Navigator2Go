#ifndef DATASETVIEW_H
#define DATASETVIEW_H

#include <QDialog>
#include <QJsonObject>
#include <QString>

/***********************************************************************************/
// Forward Declarations
namespace Ui {
class DatasetView;
}
class HTMLHighlighter;

/***********************************************************************************/
class DialogDatasetView : public QDialog {
	Q_OBJECT
public:
	explicit DialogDatasetView(QWidget* parent = nullptr);
	~DialogDatasetView();

	void SetData(const QString& datasetKey, const QJsonObject& object);

private:
	Ui::DatasetView* m_ui;

	HTMLHighlighter* m_highlighter;
};

#endif // DATASETVIEW_H
