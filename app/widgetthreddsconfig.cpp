#include "widgetthreddsconfig.h"
#include "ui_widgetthreddsconfig.h"

/***********************************************************************************/
WidgetThreddsConfig::WidgetThreddsConfig(QWidget* parent) : QWidget{parent},
															m_ui{new Ui::WidgetThreddsConfig} {
	m_ui->setupUi(this);
}

/***********************************************************************************/
WidgetThreddsConfig::~WidgetThreddsConfig() {
	delete m_ui;
}
