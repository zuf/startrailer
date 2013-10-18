#include <QString>
#include <QtTest>

#include "image.h"

class ImageTest : public QObject
{
    Q_OBJECT

public:
    ImageTest();

private Q_SLOTS:
    void testConstructors();
    void BenchmarkJPEG();
    void BenchmarkFullPreview();
    void BenchmarkHalfRaw();
    void BenchmarkFullRaw();
};

ImageTest::ImageTest()
{

}

void ImageTest::testConstructors()
{
    Image img;
    img.read("../../images/jpeg/20130906_015110_IMG_8470-preview3.jpg");
    img.read("../../images/cr2/20130906_003758_IMG_8398.CR2");
    img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", Image::FullPreview);
    img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", Image::HalfRaw);
    img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", Image::FullRaw);

    try {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", Image::SmallPreview);
        QFAIL("Exception not thrown");
    }
    catch (...) {
    }

    try {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", Image::TinyPreview);
        QFAIL("Exception not thrown");
    }
    catch (...) {
    }
}

void ImageTest::BenchmarkJPEG()
{
    Image img;
    QBENCHMARK {
        img.read("../../images/jpeg/20130906_015110_IMG_8470-preview3.jpg");
    }
}

void ImageTest::BenchmarkFullPreview()
{
    Image img;

    QBENCHMARK {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", Image::FullPreview);
    }
}

void ImageTest::BenchmarkHalfRaw()
{
    Image img;

    QBENCHMARK {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", Image::HalfRaw);
    }
}

void ImageTest::BenchmarkFullRaw()
{
    Image img;

    QBENCHMARK {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", Image::FullRaw);
    }
}

QTEST_APPLESS_MAIN(ImageTest)

#include "tst_imagetest.moc"
