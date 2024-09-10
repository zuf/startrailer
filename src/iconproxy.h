#ifndef ICONPROXY_H
#define ICONPROXY_H

#include <QIdentityProxyModel>
#include <QFileSystemModel>
#include <QtWidgets>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>
#include <QDebug>

/// A thread-safe function that returns an icon for an item with a given path.
/// If the icon is not known, a null icon is returned.
QIcon getThumbnailIcon(const QString & path);

class IconProxy : public QIdentityProxyModel {
    Q_OBJECT
    QMap<QString, QIcon> m_icons;
    Q_SIGNAL void hasIcon(const QString&, const QIcon&, const QPersistentModelIndex& index) const;

    void onIcon(const QString& path, const QIcon& icon, const QPersistentModelIndex& index) {
        m_icons.insert(path, icon);
        emit dataChanged(index, index, QVector<int>{QFileSystemModel::FileIconRole});
    }
    QThreadPool *runnersPool=0;

public:
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override {
        // QIdentityProxyModel::data(index, QFileSystemModel::FileIconRole);

        if (role == QFileSystemModel::FileIconRole && index.column() == 0) {
            auto path = index.data(QFileSystemModel::FilePathRole).toString();
            auto it = m_icons.find(path);
            if (it != m_icons.end()) {
                if (! it->isNull()) return *it;
                return QIdentityProxyModel::data(index, role);
            }
            QPersistentModelIndex pIndex{index};
            QtConcurrent::run(runnersPool, [this,path,pIndex]{
                emit hasIcon(path, getThumbnailIcon(path), pIndex);
            });
            return QVariant{};
        }
        return QIdentityProxyModel::data(index, role);
    }

    IconProxy(QObject * parent = nullptr) : QIdentityProxyModel{parent} {
        connect(this, &IconProxy::hasIcon, this, &IconProxy::onIcon);

        runnersPool = new QThreadPool();
        int half_of_cpus = (QThread::idealThreadCount() / 2);
        int threads_count = half_of_cpus > 1 ? half_of_cpus : 1;
        qDebug() << "Thread pool size for thumbnails creation: " << threads_count;
        runnersPool->setMaxThreadCount(threads_count);
    }

    virtual ~IconProxy();

protected:
    std::atomic_bool cancelled;
};

#endif // ICONPROXY_H
