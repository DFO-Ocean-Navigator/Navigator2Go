#ifndef DATASETVIEW_H
#define DATASETVIEW_H

#include "constants.h"
#include "datadownloaddesc.h"

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
	~DialogDatasetView() override;

	// Parse data from loaded config file
	void SetData(const QString& datasetKey, const QJsonObject& object);
	//
	void SetData(const QJsonObject& datasetObj, QNetworkAccessManager& nam);
	//
	void SetData(const DataDownloadDesc& data);

	// Serializes values from the UI into a JSON object.
	// Returns the dataset key, and JSON object.
	NODISCARD std::pair<QString, QJsonObject> GetData() const;

	NODISCARD DataDownloadDesc GetDownloadData() const;

private slots:
	void on_tableWidgetVariables_cellDoubleClicked(int row, int column);
	void on_pushButtonAddVariable_clicked();
	void on_pushButtonDeleteVariable_clicked();
	void on_lineEditKey_editingFinished();
	void on_lineEditName_editingFinished();
	void on_lineEditURL_editingFinished();

	void keyPressEvent(QKeyEvent* e) override;
	void on_pushButtonMagicScan_clicked();

private:
	void addEmptyVariable();
	void checkInputEmpty(const QString& inputLabel, const QString& inputText);
	void setReadOnlyUI();

	Ui::DatasetView* m_ui{nullptr};
	HTMLHighlighter* m_highlighter{nullptr};

	QHash<QString, QString> m_variableMap;
};

#endif // DATASETVIEW_H
