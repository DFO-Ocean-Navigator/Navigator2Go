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
	~WidgetLocalData();

	/// Reads the offline dataset config file and updates
	/// the tree widget with dataset names and variables
	void updateTreeWidget();

private:
	Ui::WidgetLocalData* m_ui{nullptr};
	const QString* const m_ONInstallDir{nullptr}; ///< Ptr Navigator installation directory in User settings
};

#endif // WIDGETLOCALDATA_H
