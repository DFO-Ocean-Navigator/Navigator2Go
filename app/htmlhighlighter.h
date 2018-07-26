#ifndef HTMLHIGHLIGHTER_H
#define HTMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class HTMLHighlighter : public QSyntaxHighlighter {
	Q_OBJECT

public:
	explicit HTMLHighlighter(QTextDocument* parent = nullptr);

protected:
	void highlightBlock(const QString& text) Q_DECL_OVERRIDE;

private:
	// Status highlighting, which is a text box at the time of its closure
	enum States {
		None,
		Tag,
		Comment,
		Quote
	};

	struct HighlightingRule {
		QRegExp pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> m_startTagRules;    // Formatting rules for opening tag
	QVector<HighlightingRule> m_endTagRules;      // Formatting rules for closing tags

	QRegExp m_openTag;                            // opening tag symbol - "<"
	QRegExp m_closeTag;                           // closing symbol tag - ">"
	QTextCharFormat m_edgeTagFormat;              // character formatting of openTag and closeTag
	QTextCharFormat m_insideTagFormat;            // Formatting text inside the tag

	QRegExp m_commentStartExpression;             // Regular expression of start comment
	QRegExp m_commentEndExpression;               // Redular expression of end comment
	QTextCharFormat m_multiLineCommentFormat;     // Format text inside a comment

	QRegExp m_quotes;                             // Regular Expression for text in quotes inside the tag
	QTextCharFormat m_quotationFormat;            // Formatting text in quotes inside the tag
	QTextCharFormat m_tagsFormat;                 // Formatting tags themselves
};

#endif // HTMLHIGHLIGHTER_H
