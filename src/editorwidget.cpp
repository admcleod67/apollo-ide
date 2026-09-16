#include "editorwidget.h"

#include "pascalhighlighter.h"

#include <QColor>
#include <QFile>
#include <QFontDatabase>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTextBlock>
#include <QTextDocument>
#include <QVBoxLayout>

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor)
        : QWidget(editor)
        , m_editor(editor)
    {
    }

    [[nodiscard]] QSize sizeHint() const override
    {
        return QSize(m_editor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        m_editor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *m_editor = nullptr;
};

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_lineNumberArea = new LineNumberArea(this);

    connect(document(), &QTextDocument::blockCountChanged, this,
            &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);

    updateLineNumberAreaWidth(0);
}

int CodeEditor::lineNumberAreaWidth() const
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    const int digitWidth = fontMetrics().horizontalAdvance(QLatin1Char('9'));
    return 3 + digitWidth * digits + 4;
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::updateLineNumberAreaWidth(int /*newBlockCount*/)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy != 0) {
        m_lineNumberArea->scroll(0, dy);
    } else {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(0xf0, 0xf0, 0xf0));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    painter.setPen(QColor(0x78, 0x78, 0x78));
    painter.setFont(font());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, m_lineNumberArea->width() - 4, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

EditorWidget::EditorWidget(QWidget *parent)
    : QWidget(parent)
    , m_editor(new CodeEditor(this))
{
    m_editor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_highlighter = new PascalHighlighter(m_editor->document());

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_editor);

    connect(m_editor->document(), &QTextDocument::modificationChanged, this,
            [this](bool) { emitDocumentChanged(); });

    newFile();
}

bool EditorWidget::isDirty() const
{
    return m_editor->document()->isModified();
}

QString EditorWidget::filePath() const
{
    return m_filePath;
}

QString EditorWidget::toPlainText() const
{
    return m_editor->toPlainText();
}

int EditorWidget::lineNumberAreaWidth() const
{
    return m_editor->lineNumberAreaWidth();
}

void EditorWidget::newFile()
{
    m_editor->clear();
    setFilePath(QString());
    setModified(false);
    emitDocumentChanged();
}

bool EditorWidget::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    const QByteArray bytes = file.readAll();
    if (file.error() != QFile::NoError) {
        return false;
    }

    m_editor->setPlainText(QString::fromUtf8(bytes));
    setFilePath(path);
    setModified(false);
    emitDocumentChanged();
    return true;
}

bool EditorWidget::saveToFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return false;
    }

    const QByteArray bytes = m_editor->toPlainText().toUtf8();
    if (file.write(bytes) != bytes.size()) {
        return false;
    }

    setFilePath(path);
    setModified(false);
    emitDocumentChanged();
    return true;
}

bool EditorWidget::save()
{
    if (m_filePath.isEmpty()) {
        return false;
    }
    return saveToFile(m_filePath);
}

void EditorWidget::setContent(const QString &text, bool markDirty)
{
    m_editor->setPlainText(text);
    setModified(markDirty);
    emitDocumentChanged();
}

void EditorWidget::setFilePath(const QString &path)
{
    m_filePath = path;
}

void EditorWidget::setModified(bool modified)
{
    m_editor->document()->setModified(modified);
}

void EditorWidget::emitDocumentChanged()
{
    emit documentChanged();
}
