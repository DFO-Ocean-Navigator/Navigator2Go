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
/// Local data widget in the Local Data tab.
class WidgetLocalData : public QWidget {
  Q_OBJECT

public:
  WidgetLocalData(const QString *m_ONInstallDir, QWidget *parent = nullptr);
  ~WidgetLocalData() override;

  /// Reads the offline dataset config file and updates the tree widget with
  /// dataset names and variables
  void updateTreeWidget();

signals:
  /// Signal emitted to MainWindow to refresh the Ocean Navigator servers and
  /// cache.
  void refreshRequested();

private slots:
  /// Delete button callback
  void buttonHandler();

private:
  /// Removes a dataset from the Ocean Navigator given a dataset name.
  void deleteDataset(const QString &datasetName);

  Ui::WidgetLocalData *m_ui{nullptr}; ///< Pointer to UI widgets
  const QString *const m_ONInstallDir{
      nullptr}; ///< Pointer to Navigator installation directory in User
                ///< settings
};

#endif // WIDGETLOCALDATA_H
