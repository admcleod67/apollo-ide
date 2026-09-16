#include "pascalhighlighter.h"

#include <QColor>
#include <QFont>

namespace {

const QStringList kKeywords = {
    QStringLiteral("and"),       QStringLiteral("array"),     QStringLiteral("begin"),
    QStringLiteral("case"),      QStringLiteral("const"),     QStringLiteral("div"),
    QStringLiteral("do"),        QStringLiteral("downto"),    QStringLiteral("else"),
    QStringLiteral("end"),       QStringLiteral("file"),      QStringLiteral("for"),
    QStringLiteral("function"),  QStringLiteral("goto"),      QStringLiteral("if"),
    QStringLiteral("in"),        QStringLiteral("label"),     QStringLiteral("mod"),
    QStringLiteral("nil"),       QStringLiteral("not"),       QStringLiteral("of"),
    QStringLiteral("or"),        QStringLiteral("packed"),    QStringLiteral("procedure"),
    QStringLiteral("program"),   QStringLiteral("record"),    QStringLiteral("repeat"),
    QStringLiteral("set"),       QStringLiteral("then"),      QStringLiteral("to"),
    QStringLiteral("type"),      QStringLiteral("until"),     QStringLiteral("var"),
    QStringLiteral("while"),     QStringLiteral("with"),
};

} // namespace

PascalHighlighter::PascalHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    setupFormats();
    setupRules();
}

void PascalHighlighter::setupFormats()
{
    m_keywordFormat.setFontWeight(QFont::Bold);
    m_keywordFormat.setForeground(QColor(0x00, 0x00, 0xA0));

    m_commentFormat.setFontItalic(true);
    m_commentFormat.setForeground(QColor(0x80, 0x80, 0x80));

    m_stringFormat.setForeground(QColor(0x00, 0x80, 0x00));

    m_numberFormat.setForeground(QColor(0x80, 0x00, 0x80));
}

void PascalHighlighter::setupRules()
{
    QStringList keywordAlternatives = kKeywords;

    Rule keywordRule;
    keywordRule.pattern = QRegularExpression(
        QStringLiteral("\\b(?:%1)\\b").arg(keywordAlternatives.join(QLatin1Char('|'))),
        QRegularExpression::CaseInsensitiveOption);
    keywordRule.format = m_keywordFormat;
    m_rules.append(keywordRule);

    Rule numberRule;
    numberRule.pattern = QRegularExpression(
        QStringLiteral("\\b\\d+(?:\\.\\d+)?(?:[Ee][+-]?\\d+)?\\b"));
    numberRule.format = m_numberFormat;
    m_rules.append(numberRule);
}

void PascalHighlighter::highlightBlock(const QString &text)
{
    setCurrentBlockState(static_cast<int>(BlockState::Normal));

    int startIndex = 0;
    BlockState state = static_cast<BlockState>(previousBlockState());
    if (state != BlockState::BraceComment && state != BlockState::ParenStarComment) {
        state = BlockState::Normal;
    }

    while (startIndex <= text.length()) {
        if (state == BlockState::BraceComment) {
            const int end = text.indexOf(QLatin1Char('}'), startIndex);
            if (end < 0) {
                setFormat(startIndex, text.length() - startIndex, m_commentFormat);
                setCurrentBlockState(static_cast<int>(BlockState::BraceComment));
                return;
            }
            setFormat(startIndex, end - startIndex + 1, m_commentFormat);
            startIndex = end + 1;
            state = BlockState::Normal;
            continue;
        }

        if (state == BlockState::ParenStarComment) {
            const int end = text.indexOf(QStringLiteral("*)"), startIndex);
            if (end < 0) {
                setFormat(startIndex, text.length() - startIndex, m_commentFormat);
                setCurrentBlockState(static_cast<int>(BlockState::ParenStarComment));
                return;
            }
            setFormat(startIndex, end - startIndex + 2, m_commentFormat);
            startIndex = end + 2;
            state = BlockState::Normal;
            continue;
        }

        // Normal: find next string or comment start.
        int brace = text.indexOf(QLatin1Char('{'), startIndex);
        int parenStar = text.indexOf(QStringLiteral("(*"), startIndex);
        int quote = text.indexOf(QLatin1Char('\''), startIndex);

        int next = -1;
        enum class Kind { None, Brace, ParenStar, String } kind = Kind::None;
        auto consider = [&](int pos, Kind k) {
            if (pos >= 0 && (next < 0 || pos < next)) {
                next = pos;
                kind = k;
            }
        };
        consider(brace, Kind::Brace);
        consider(parenStar, Kind::ParenStar);
        consider(quote, Kind::String);

        const int segmentEnd = next < 0 ? text.length() : next;
        const QString segment = text.mid(startIndex, segmentEnd - startIndex);
        for (const Rule &rule : m_rules) {
            auto it = rule.pattern.globalMatch(segment);
            while (it.hasNext()) {
                const QRegularExpressionMatch match = it.next();
                setFormat(startIndex + match.capturedStart(), match.capturedLength(), rule.format);
            }
        }

        if (kind == Kind::None) {
            return;
        }

        if (kind == Kind::Brace) {
            startIndex = next;
            state = BlockState::BraceComment;
            continue;
        }
        if (kind == Kind::ParenStar) {
            startIndex = next;
            state = BlockState::ParenStarComment;
            continue;
        }

        // String literal with '' escapes; does not span lines in Apollo Pascal.
        int i = next + 1;
        bool closed = false;
        while (i < text.length()) {
            if (text.at(i) == QLatin1Char('\'')) {
                if (i + 1 < text.length() && text.at(i + 1) == QLatin1Char('\'')) {
                    i += 2;
                    continue;
                }
                closed = true;
                ++i;
                break;
            }
            ++i;
        }
        setFormat(next, i - next, m_stringFormat);
        startIndex = i;
        if (!closed) {
            // Unclosed string: still coloured to EOL; stay in Normal for next line.
            return;
        }
    }
}
