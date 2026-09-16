#include "pascalhighlighter.h"

#include <QFont>
#include <QTextDocument>
#include <QTextLayout>
#include <QtTest>

#include <functional>

class PascalHighlighterTest : public QObject
{
    Q_OBJECT

private slots:
    void keywordsAreBoldBlue();
    void braceCommentsAreGrayItalic();
    void parenStarCommentsSpanLines();
    void stringsAndNumbersAreColoured();
    void keywordsInsideCommentsAreNotKeywords();
};

namespace {

QList<QTextLayout::FormatRange> formatsFor(QTextDocument &doc, int blockNumber)
{
    QTextBlock block = doc.findBlockByNumber(blockNumber);
    if (!block.isValid() || block.layout() == nullptr) {
        return {};
    }
    return block.layout()->formats();
}

bool hasFormatAt(const QList<QTextLayout::FormatRange> &ranges, int position,
                 const std::function<bool(const QTextCharFormat &)> &predicate)
{
    for (const QTextLayout::FormatRange &range : ranges) {
        if (position >= range.start && position < range.start + range.length
            && predicate(range.format)) {
            return true;
        }
    }
    return false;
}

bool looksLikeKeyword(const QTextCharFormat &format)
{
    return format.fontWeight() >= QFont::Bold
        && format.foreground().color().blue() > format.foreground().color().red();
}

bool looksLikeComment(const QTextCharFormat &format)
{
    return format.fontItalic()
        && format.foreground().color().red() == format.foreground().color().green();
}

bool looksLikeString(const QTextCharFormat &format)
{
    const QColor c = format.foreground().color();
    return c.green() > c.red() && c.green() > c.blue();
}

bool looksLikeNumber(const QTextCharFormat &format)
{
    const QColor c = format.foreground().color();
    return c.red() > 0 && c.blue() > 0 && c.green() == 0;
}

} // namespace

void PascalHighlighterTest::keywordsAreBoldBlue()
{
    QTextDocument doc;
    PascalHighlighter highlighter(&doc);
    doc.setPlainText(QStringLiteral("Begin Hello;"));
    highlighter.rehighlight();

    const auto ranges = formatsFor(doc, 0);
    QVERIFY(hasFormatAt(ranges, 0, looksLikeKeyword));  // B of Begin
    QVERIFY(hasFormatAt(ranges, 4, looksLikeKeyword));  // n of Begin
    QVERIFY(!hasFormatAt(ranges, 6, looksLikeKeyword)); // space before Hello
}

void PascalHighlighterTest::braceCommentsAreGrayItalic()
{
    QTextDocument doc;
    PascalHighlighter highlighter(&doc);
    doc.setPlainText(QStringLiteral("x { comment } y"));
    highlighter.rehighlight();

    const auto ranges = formatsFor(doc, 0);
    QVERIFY(hasFormatAt(ranges, 2, looksLikeComment));
    QVERIFY(hasFormatAt(ranges, 6, looksLikeComment));
    QVERIFY(!hasFormatAt(ranges, 0, looksLikeComment));
}

void PascalHighlighterTest::parenStarCommentsSpanLines()
{
    QTextDocument doc;
    PascalHighlighter highlighter(&doc);
    doc.setPlainText(QStringLiteral("(* line one\nline two *) begin"));
    highlighter.rehighlight();

    QVERIFY(hasFormatAt(formatsFor(doc, 0), 0, looksLikeComment));
    QVERIFY(hasFormatAt(formatsFor(doc, 1), 0, looksLikeComment));
    QVERIFY(hasFormatAt(formatsFor(doc, 1), 12, looksLikeKeyword)); // begin
}

void PascalHighlighterTest::stringsAndNumbersAreColoured()
{
    QTextDocument doc;
    PascalHighlighter highlighter(&doc);
    doc.setPlainText(QStringLiteral("'O''Reilly' 42"));
    highlighter.rehighlight();

    const auto ranges = formatsFor(doc, 0);
    QVERIFY(hasFormatAt(ranges, 0, looksLikeString));
    QVERIFY(hasFormatAt(ranges, 5, looksLikeString));
    QVERIFY(hasFormatAt(ranges, 12, looksLikeNumber));
}

void PascalHighlighterTest::keywordsInsideCommentsAreNotKeywords()
{
    QTextDocument doc;
    PascalHighlighter highlighter(&doc);
    doc.setPlainText(QStringLiteral("{ begin } end"));
    highlighter.rehighlight();

    const auto ranges = formatsFor(doc, 0);
    QVERIFY(hasFormatAt(ranges, 2, looksLikeComment));
    QVERIFY(!hasFormatAt(ranges, 2, looksLikeKeyword));
    QVERIFY(hasFormatAt(ranges, 10, looksLikeKeyword));
}

QTEST_MAIN(PascalHighlighterTest)
#include "pascalhighlighter_test.moc"
