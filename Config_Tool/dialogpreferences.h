#ifndef DIALOGPREFERENCES_H
#define DIALOGPREFERENCES_H

#include "defines.h"

#include "preferences.h"

#include <QDialog>
#include <QSettings>

/***********************************************************************************/
// Forward Declarations
namespace Ui {
class DialogPreferences;
}

/***********************************************************************************/
class DialogPreferences : public QDialog {
	Q_OBJECT

public:
	explicit DialogPreferences(QWidget* parent = nullptr);
	~DialogPreferences();

	void SetPreferences(Preferences& settings);
	NODISCARD auto GetPreferences() const noexcept -> Preferences;

private slots:
	void on_pushButtonBrowseInstallDir_clicked();

	void on_pushButtonBroweseTHREDDS_clicked();

private:
	Ui::DialogPreferences* m_ui{nullptr};


};

#endif // DIALOGPREFERENCES_H
