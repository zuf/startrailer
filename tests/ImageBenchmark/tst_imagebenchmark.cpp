#include <QString>
#include <QtTest>

#include "image.h"

class ImageBenchmark : public QObject
{
    Q_OBJECT

public:
    ImageBenchmark();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void benchmarkJPEG();
    void benchmarkFullPreview();
    void benchmarkHalfRaw();
    void benchmarkFullRaw();
};

ImageBenchmark::ImageBenchmark()
{
}

void ImageBenchmark::initTestCase()
{
}

void ImageBenchmark::cleanupTestCase()
{
}

void ImageBenchmark::benchmarkJPEG()
{
    StarTrailer::Image img;
    QBENCHMARK {
        img.read("../../images/jpeg/20130906_015110_IMG_8470-preview3.jpg");
    }
}

void ImageBenchmark::benchmarkFullPreview()
{
    StarTrailer::Image img;

    QBENCHMARK {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::FullPreview);
    }
}

void ImageBenchmark::benchmarkHalfRaw()
{
    StarTrailer::Image img;

    QBENCHMARK {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::HalfRaw);
    }
}

void ImageBenchmark::benchmarkFullRaw()
{
    StarTrailer::Image img;

    QBENCHMARK {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::FullRaw);
    }
}

QTEST_APPLESS_MAIN(ImageBenchmark)

#include "tst_imagebenchmark.moc"
