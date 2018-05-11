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

	// Parse data from loaded config file
	void SetData(const QString& datasetKey, const QJsonObject& object);
	// Given a dataset key from the Dory list, send API calls to Dory
	// to populate UI components
	void SetData(const QString& datasetKey);

	// Serializes values from the UI into a JSON object.
	// Returns the dataset key, and JSON object.
	std::pair<QString, QJsonObject> GetData() const;

private slots:
	void on_tableWidgetVariables_cellDoubleClicked(int row, int column);
	void on_pushButtonAddVariable_clicked();
	void on_pushButtonDeleteVariable_clicked();
	void on_lineEditKey_editingFinished();
	void on_lineEditName_editingFinished();
	void on_lineEditURL_editingFinished();

private:
	void addEmptyVariable();
	void checkInputEmpty(const QString& inputLabel, const QString& inputText);

	Ui::DatasetView* m_ui;
	HTMLHighlighter* m_highlighter;
};

#endif // DATASETVIEW_H
