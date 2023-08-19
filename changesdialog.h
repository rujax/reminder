#ifndef CHANGESDIALOG_H
#define CHANGESDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QTextBrowser;
QT_END_NAMESPACE

class ChangesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangesDialog(QWidget *parent = nullptr);

private:
    // UI
    QTextBrowser *_version02X;
    QTextBrowser *_version01X;
    QTextBrowser *_version00X;

    // Methods
    void _buildUI();
    void _displayChanges();
};

#endif // CHANGESDIALOG_H
