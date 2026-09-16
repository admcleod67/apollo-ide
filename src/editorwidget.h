#pragma once

#include <QWidget>

class PascalHighlighter;
class QPlainTextEdit;

class EditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditorWidget(QWidget *parent = nullptr);

    [[nodiscard]] bool isDirty() const;
    [[nodiscard]] QString filePath() const;
    [[nodiscard]] QString toPlainText() const;

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

    QPlainTextEdit *m_editor = nullptr;
    PascalHighlighter *m_highlighter = nullptr;
    QString m_filePath;
};
