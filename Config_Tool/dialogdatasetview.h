#ifndef DATASETVIEW_H
#define DATASETVIEW_H

#include <QDialog>
#include <QJsonObject>
#include <QString>
#include <QNetworkAccessManager>

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

public:
	explicit DialogDatasetView(QWidget* parent = nullptr);
	~DialogDatasetView();

	// Parse data from loaded config file
	void SetData(const QString& datasetKey, const QJsonObject& object);
	//
	void SetData(const QJsonObject& datasetObj, QNetworkAccessManager& nam);

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

	Ui::DatasetView* m_ui{nullptr};
	HTMLHighlighter* m_highlighter{nullptr};
};

#endif // DATASETVIEW_H
