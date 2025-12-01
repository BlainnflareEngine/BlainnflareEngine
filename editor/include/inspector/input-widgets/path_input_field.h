//
// Created by gorev on 17.11.2025.
//

#pragma once


#include <QWidget>


class QPushButton;
class QLabel;
class QLineEdit;
namespace editor
{

class path_input_field : public QWidget
{
    Q_OBJECT

public:
    explicit path_input_field(const QString &name, const QStringList &acceptedExtensions = {},
                              QWidget *parent = nullptr);

    void SetExtensions(const QStringList &extensions);
    QStringList GetExtensions() const;

    QString GetPath() const;
    QString GetAbsolutePath() const;
    void SetPath(const QString &relativePath);

    /**
     * Set path, do not trigger PathChanged signal.
     */
    void SetPathSilent(const QString &relativePath) const;

signals:
    void PathChanged(const QString &oldPath, const QString &newPath);

private slots:
    void OnBrowse();
    void OnDelete();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    bool IsFileAccepted(const QString &filePath) const;

    QLineEdit *m_input;
    QLabel *m_name;
    QPushButton *m_browse;
    QPushButton *m_delete;

    QStringList m_acceptedExtensions;
};

} // namespace editor