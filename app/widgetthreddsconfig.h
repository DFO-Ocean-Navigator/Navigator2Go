#ifndef WIDGETTHREDDSCONFIG_H
#define WIDGETTHREDDSCONFIG_H

#include <QWidget>

/***********************************************************************************/
// Forward declarations
namespace Ui {
class WidgetThreddsConfig;
}

/***********************************************************************************/
class WidgetThreddsConfig : public QWidget {
	Q_OBJECT

public:
	explicit WidgetThreddsConfig(QWidget* parent = nullptr);

	~WidgetThreddsConfig();

private:
	Ui::WidgetThreddsConfig* m_ui;
};

#endif // WIDGETTHREDDSCONFIG_H
