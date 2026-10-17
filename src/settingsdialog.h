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
    [[nodiscard]] QString modulesPath() const;

    void setCompilerPath(const QString &path);
    void setVmPath(const QString &path);
    void setModulesPath(const QString &path);

private:
    void browseCompiler();
    void browseVm();
    void browseModules();

    QLineEdit *m_compilerEdit = nullptr;
    QLineEdit *m_vmEdit = nullptr;
    QLineEdit *m_modulesEdit = nullptr;
};
