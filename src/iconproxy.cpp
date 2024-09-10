#include "iconproxy.h"
#include "qiconthumbextractor.h"

QIcon getThumbnailIcon(const QString & path){
    QIconThumbExtractor ex;
    return ex.icon(path);
}


IconProxy::~IconProxy() {
    if (runnersPool) {
        delete runnersPool;
    }
}
