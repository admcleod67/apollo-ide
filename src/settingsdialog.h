#pragma once

#include <QDialog>

class QLineEdit;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    [[nodiscard]] QString compilerPath() const;
    [[nodiscard]] QString vmPath() const;

    void setCompilerPath(const QString &path);
    void setVmPath(const QString &path);

private:
    void browseCompiler();
    void browseVm();

    QLineEdit *m_compilerEdit = nullptr;
    QLineEdit *m_vmEdit = nullptr;
};
