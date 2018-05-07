#ifndef DATASETVIEW_H
#define DATASETVIEW_H

#include <QDialog>
#include <QJsonObject>
#include <QString>

#include <utility>

/***********************************************************************************/
// Forward Declarations
namespace Ui {
class DatasetView;
}
class HTMLHighlighter;

/***********************************************************************************/
class DialogDatasetView : public QDialog {
	Q_OBJECT
	friend class MainWindow;
public:
	explicit DialogDatasetView(QWidget* parent = nullptr);
	~DialogDatasetView();

	void SetData(const QString& datasetKey, const QJsonObject& object);
	std::pair<QString, QJsonObject> GetData() const;

private slots:
	void on_tableWidgetVariables_cellDoubleClicked(int row, int column);
	void on_pushButtonAddVariable_clicked();

private:
	void addEmptyVariable();

	Ui::DatasetView* m_ui;
	HTMLHighlighter* m_highlighter;
};

#endif // DATASETVIEW_H
