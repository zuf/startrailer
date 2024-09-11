#ifndef ICONPROXY_H
#define ICONPROXY_H

#include <QIdentityProxyModel>
#include <QFileSystemModel>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>
#include <QCache>
#include <QDebug>

/// A thread-safe function that returns an icon for an item with a given path.
/// If the icon is not known, a null icon is returned.
QIcon getThumbnailIcon(const QString & path);

class IconProxy : public QIdentityProxyModel {
    Q_OBJECT
    // QMap<QString, QIcon> m_icons;
    QCache<QString, const QIcon> m_icons;
    Q_SIGNAL void hasIcon(const QString&, const QIcon&, const QPersistentModelIndex& index) const;

    void onIcon(const QString& path, const QIcon& icon, const QPersistentModelIndex& index) {
        if (!icon.isNull()) {
            m_icons.insert(path, new QIcon(icon));
            emit dataChanged(index, index, QVector<int>{QFileSystemModel::FileIconRole});
        }
    }
    QThreadPool *runnersPool=0;

public:
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override {
        // QIdentityProxyModel::data(index, QFileSystemModel::FileIconRole);

        if (role == QFileSystemModel::FileIconRole && index.column() == 0) {
            auto path = index.data(QFileSystemModel::FilePathRole).toString();
            const QIcon *it = m_icons[path];
            if (it != nullptr) {
                return *it;
            } else {
                QPersistentModelIndex pIndex{index};
                QtConcurrent::run(runnersPool, [this,path,pIndex]{
                    emit hasIcon(path, getThumbnailIcon(path), pIndex);
                });
            }

            /*f (it != m_icons.end()) {
                if (! it->isNull()) return *it;
                return QIdentityProxyModel::data(index, role);
            }*/

            // QPersistentModelIndex pIndex{index};
            // QtConcurrent::run(runnersPool, [this,path,pIndex]{
            //     emit hasIcon(path, getThumbnailIcon(path), pIndex);
            // });
            // return QIcon();
            // return QVariant{};
            return QIdentityProxyModel::data(index, role);
        }
        return QIdentityProxyModel::data(index, role);
    }

    IconProxy(QObject * parent = nullptr) : QIdentityProxyModel{parent} {
        m_icons.setMaxCost(8192);
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
