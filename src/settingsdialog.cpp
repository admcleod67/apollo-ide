#include "settingsdialog.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_compilerEdit(new QLineEdit(this))
    , m_vmEdit(new QLineEdit(this))
{
    setWindowTitle(tr("Preferences"));
    resize(520, 140);

    auto *compilerBrowse = new QPushButton(tr("Browse…"), this);
    auto *vmBrowse = new QPushButton(tr("Browse…"), this);
    connect(compilerBrowse, &QPushButton::clicked, this, &SettingsDialog::browseCompiler);
    connect(vmBrowse, &QPushButton::clicked, this, &SettingsDialog::browseVm);

    auto *compilerRow = new QHBoxLayout;
    compilerRow->addWidget(m_compilerEdit);
    compilerRow->addWidget(compilerBrowse);

    auto *vmRow = new QHBoxLayout;
    vmRow->addWidget(m_vmEdit);
    vmRow->addWidget(vmBrowse);

    auto *form = new QFormLayout;
    form->addRow(tr("Apollo compiler (apolloc)"), compilerRow);
    form->addRow(tr("Gemini VM (gemini-vm)"), vmRow);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

QString SettingsDialog::compilerPath() const
{
    return m_compilerEdit->text().trimmed();
}

QString SettingsDialog::vmPath() const
{
    return m_vmEdit->text().trimmed();
}

void SettingsDialog::setCompilerPath(const QString &path)
{
    m_compilerEdit->setText(path);
}

void SettingsDialog::setVmPath(const QString &path)
{
    m_vmEdit->setText(path);
}

void SettingsDialog::browseCompiler()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Select Apollo compiler"),
                                                      m_compilerEdit->text());
    if (!path.isEmpty()) {
        m_compilerEdit->setText(path);
    }
}

void SettingsDialog::browseVm()
{
    const QString path =
        QFileDialog::getOpenFileName(this, tr("Select Gemini VM"), m_vmEdit->text());
    if (!path.isEmpty()) {
        m_vmEdit->setText(path);
    }
}
