#include "switchwidget.h"

#include <QObject>
#include <QPropertyAnimation>
#include <QPainter>
#include <QMouseEvent>

/***********************************************************************************/
SwitchWidget::SwitchWidget(QWidget* parent) : QAbstractButton{parent}, m_anim{new QPropertyAnimation{this, "offset", this}} {
	const auto offset = m_height / 2;
	setOffset(offset);
	m_y = offset;
	setBrush(QColor("009688"));
	setMaximumWidth(m_width);
	setCheckable(true);

	setMinimumWidth(100);
}

/***********************************************************************************/
QSize SwitchWidget::sizeHint() const {
	return {2 * (m_height + m_margin), m_height + 2 * m_margin};
}

/***********************************************************************************/
void SwitchWidget::setOffset(const int o) {
	m_x = o;
	update();
}

/***********************************************************************************/
void SwitchWidget::paintEvent(QPaintEvent* e) {
	QPainter p{this};
	p.fillRect(rect(), QColor(90, 102, 117));
	QRect switchRect;
	QColor switchColor;
	QString text;
	const auto halfWidth = width() / 2;

	if (isChecked()) {
		switchRect = QRect(halfWidth, 0, halfWidth, height());
		switchColor = QColor(61, 174, 233);
		text = tr("On");
	} else {
		switchRect = QRect(0, 0, halfWidth, height());
		switchColor = QColor(Qt::darkGray);
		text = tr("Off");
	}
	p.fillRect(switchRect, switchColor);
	p.drawText(switchRect, Qt::AlignCenter, text);
}

/***********************************************************************************/
void SwitchWidget::mouseReleaseEvent(QMouseEvent* e) {

	if (e->button() & Qt::LeftButton) {
		m_switch = m_switch ? false : true;
		m_thumb = m_switch ? m_brush : QBrush("#d5d5d5");
		if (m_switch) {
			m_anim->setStartValue(m_height / 2);
			m_anim->setEndValue(width() - m_height);
			m_anim->setDuration(120);
			m_anim->start();
		} else {
			m_anim->setStartValue(offset());
			m_anim->setEndValue(m_height / 2);
			m_anim->setDuration(120);
			m_anim->start();
		}
	}

	QAbstractButton::mouseReleaseEvent(e);

	emit toggled(isChecked());
}

/***********************************************************************************/
void SwitchWidget::enterEvent(QEvent* e) {
	setCursor(Qt::PointingHandCursor);
	QAbstractButton::enterEvent(e);
}
