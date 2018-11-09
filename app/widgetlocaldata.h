#ifndef WIDGETLOCALDATA_H
#define WIDGETLOCALDATA_H

#include <QWidget>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class WidgetLocalData;
}
class QString;

/***********************************************************************************/
class WidgetLocalData : public QWidget {
	Q_OBJECT

public:
	explicit WidgetLocalData(const QString* m_ONInstallDir, QWidget *parent = nullptr);
	~WidgetLocalData() override;

	/// Reads the offline dataset config file and updates
	/// the tree widget with dataset names and variables
	void updateTreeWidget();

signals:
	void refreshRequested();

private slots:
	void buttonHandler();

private:
	void deleteDataset(const QString& datasetName);

	Ui::WidgetLocalData* m_ui{nullptr};
	const QString* const m_ONInstallDir{nullptr}; ///< Pointer to Navigator installation directory in User settings
};

#endif // WIDGETLOCALDATA_H
