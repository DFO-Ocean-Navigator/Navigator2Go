#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "datadownloaddesc.h"
#include "preferences.h"
#include "servermanager.h"

#include <QJsonObject>
#include <QMainWindow>
#include <QPointer>
#include <QProcess>
#include <QSettings>
#include <QTimer>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class MainWindow;
}

class WidgetConfigEditor;
class WidgetDataOrder;
class WidgetThreddsConfig;
class WidgetLocalData;
class QNetworkReply;

/***********************************************************************************/
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

  /// Show the progress bar and label with the given labelText. Starts at 0%
  void showProgressBar(const char *labelText);
  /// Updates the progress bar percent value. Value is a percentage from 0 to
  /// 100.
  void updateProgressBar(const int value);
  /// Hide progress bar and label
  void hideProgressBar();

private slots:
  ///
  void refreshRequestHandler();
  ///
  void showStatusBarMessageHandler(const char *message);

  // Callbacks

  /// About Qt menu item
  void on_actionAbout_Qt_triggered();
  /// Program close
  void on_actionClose_triggered();
  /// Preferences menu item
  void on_actionPreferences_triggered();
  /// About menu item
  void on_actionAbout_triggered();
  /// Update check menu item
  void on_actionCheck_for_Updates_triggered();
  /// Open Navigator2Go manual menu item
  void on_actionNavigator2Go_Manual_triggered();
  /// Launch web viewer button
  void on_pushButtonLaunchViewer_clicked();
  /// Data order button
  void on_pushButtonOpenDataOrder_clicked();
  /// Import NetCDF files button
  void on_pushButtonImportNCFiles_clicked();
  /// View local data button
  void on_pushButtonViewLocalData_clicked();
  /// Clear Python cache menu item
  void on_actionClear_Python_cache_triggered();
  /// Refresh Navigator2Go button
  void on_pushButtonRefresh_clicked();
  /// Fires when the current tab changes
  void on_tabWidget_currentChanged(int index);

  void on_actionReport_Issue_triggered();

private:
  /// Display a message in the window's status bar
  void showStatusBarMessage(const char *text) const;
  /// Queries the RemoteURL in m_prefs for a connection
  void checkRemoteConnection();
  /// Instructs m_widgetThreddsConfig to rebuild the THREDDS table widgets
  void updateTHREDDSConfigTable();
  /// Initialize widgets for each tab
  void initWidgets();
  /// Sets initial layout of program on start
  void setInitialLayout();
  /// Calls both update functions for convenience
  void checkForUpdates();
  /// Checks for updates for Navigator2Go.
  void checkForAppUpdate();
  /// Updates Ocean Navigator code from git and rebuilds necessary code.
  void checkForONUpdates();
  /// Show first run configuration dialog before main window pops up.
  void showFirstRunConfiguration();

  Ui::MainWindow *m_ui{nullptr}; ///< Pointer to UI widgets

  ServerManager m_serverManager{
      this}; ///< Server manager to handle gunicorn and THREDDS servers

  // Tab widgets
  QPointer<WidgetLocalData> m_widgetLocalData; ///< Local data tab
  QPointer<WidgetConfigEditor>
      m_widgetConfigEditor; ///< Dataset config tab (advanced mode)
  QPointer<WidgetDataOrder> m_widgetDataOrder; ///< Data order tab
  QPointer<WidgetThreddsConfig>
      m_widgetThreddsConfig; ///< THREDDS config tab (advanced mode)

  QPointer<QNetworkReply> m_updateReply; ///< Pointer to update response

  Preferences m_prefs{this}; ///< Serializes user settings

  QTimer m_uplinkTimer{this};   ///< Timer to check for a remote navigator
                                ///< connection every 5 minutes.
  bool m_hasRemoteUplink{true}; ///< Does this client have a connection to the
                                ///< remote Navigator server
};

#endif // MAINWINDOW_H
