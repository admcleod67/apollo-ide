#pragma once

#include <QPlainTextEdit>
#include <QWidget>

class LineNumberArea;
class PascalHighlighter;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
    friend class LineNumberArea;

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    [[nodiscard]] int lineNumberAreaWidth() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    LineNumberArea *m_lineNumberArea = nullptr;
};

class EditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditorWidget(QWidget *parent = nullptr);

    [[nodiscard]] bool isDirty() const;
    [[nodiscard]] QString filePath() const;
    [[nodiscard]] QString toPlainText() const;
    [[nodiscard]] int lineNumberAreaWidth() const;

    void newFile();
    bool loadFile(const QString &path);
    bool saveToFile(const QString &path);
    bool save();

    /** Set editor content. When markDirty is true, the document is marked modified. */
    void setContent(const QString &text, bool markDirty = true);

signals:
    void documentChanged();

private:
    void setFilePath(const QString &path);
    void setModified(bool modified);
    void emitDocumentChanged();

    CodeEditor *m_editor = nullptr;
    PascalHighlighter *m_highlighter = nullptr;
    QString m_filePath;
};
