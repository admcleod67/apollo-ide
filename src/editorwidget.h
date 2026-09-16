#pragma once

#include <QPlainTextEdit>
#include <QWidget>

class LineNumberArea;
class PascalHighlighter;
class QEvent;
class QLineEdit;

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
    [[nodiscard]] QString selectedText() const;
    [[nodiscard]] int lineNumberAreaWidth() const;

    void newFile();
    bool loadFile(const QString &path);
    bool saveToFile(const QString &path);
    bool save();

    /** Set editor content. When markDirty is true, the document is marked modified. */
    void setContent(const QString &text, bool markDirty = true);

    void showFind();
    void hideFind();
    void setFindQuery(const QString &query);
    bool findNext();
    bool findPrevious();

signals:
    void documentChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setFilePath(const QString &path);
    void setModified(bool modified);
    void emitDocumentChanged();
    bool findWithWrap(bool backward);

    CodeEditor *m_editor = nullptr;
    PascalHighlighter *m_highlighter = nullptr;
    QWidget *m_findBar = nullptr;
    QLineEdit *m_findEdit = nullptr;
    QString m_filePath;
};
