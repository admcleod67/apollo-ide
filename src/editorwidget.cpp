#include "editorwidget.h"

#include <QFile>
#include <QFontDatabase>
#include <QPlainTextEdit>
#include <QTextDocument>
#include <QVBoxLayout>

EditorWidget::EditorWidget(QWidget *parent)
    : QWidget(parent)
    , m_editor(new QPlainTextEdit(this))
{
    m_editor->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);

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
