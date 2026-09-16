#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

class PascalHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit PascalHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    enum class BlockState {
        Normal = 0,
        BraceComment = 1,
        ParenStarComment = 2,
    };

    struct Rule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    void setupFormats();
    void setupRules();

    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_numberFormat;
    QVector<Rule> m_rules;
};
