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
    void testComposite();
    void testToBuffer();

    void benchmarkJPEG();
    void benchmarkFullPreview();
    void benchmarkHalfRaw();
    void benchmarkFullRaw();
};

ImageTest::ImageTest()
{

}

void ImageTest::testConstructors()
{
    StarTrailer::Image img;
    img.read("../../images/jpeg/20130906_015110_IMG_8470-preview3.jpg");
    img.read("../../images/cr2/20130906_003758_IMG_8398.CR2");
    img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::FullPreview);
    img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::HalfRaw);
    img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::FullRaw);

    try {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::SmallPreview);
        QFAIL("Exception not thrown");
    }
    catch (...) {
    }

    try {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::TinyPreview);
        QFAIL("Exception not thrown");
    }
    catch (...) {
    }
}

void ImageTest::benchmarkJPEG()
{
    StarTrailer::Image img;
    QBENCHMARK {
        img.read("../../images/jpeg/20130906_015110_IMG_8470-preview3.jpg");
    }
}

void ImageTest::benchmarkFullPreview()
{
    StarTrailer::Image img;

    QBENCHMARK {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::FullPreview);
    }
}

void ImageTest::benchmarkHalfRaw()
{
    StarTrailer::Image img;

    QBENCHMARK {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::HalfRaw);
    }
}

void ImageTest::benchmarkFullRaw()
{
    StarTrailer::Image img;

    QBENCHMARK {
        img.read("../../images/cr2/20130906_003859_IMG_8399.CR2", StarTrailer::Image::FullRaw);
    }
}

void ImageTest::testComposite()
{
    const StarTrailer::Image empty;
    const StarTrailer::Image img0("../../images/cr2/20130906_003859_IMG_8399.CR2");
    StarTrailer::Image img1("../../images/cr2/20130906_003859_IMG_8399.CR2");

    QVERIFY2(img0!=empty, "Image should not eq to empty one");

    QCOMPARE(img0, img0);
    QVERIFY2(img0==img1, "Image should be eq when loaded frome one file");

    StarTrailer::Image img2("../../images/jpeg/20130906_015110_IMG_8470-preview3.jpg");

    QVERIFY2(img0!=img2, "Image should not be eq when loaded frome different files");

    img1.composite(img2);

    QVERIFY2(img0!=img1, "Image should changed after compose");
}

void ImageTest::testToBuffer()
{

    StarTrailer::Image empty;
    StarTrailer::Image img0("../../images/cr2/20130906_003859_IMG_8399.CR2");
    StarTrailer::Image img1("../../images/jpeg/20130906_015110_IMG_8470-preview3.jpg");

    void *buf = new char[3*img0.width()*img0.height()];
    size_t buf_length;
    buf_length = img0.to_buffer(buf);

    Magick::Image m(img0.width(), img0.height(), "RGB", Magick::CharPixel, buf);
    StarTrailer::Image from_m(m);

    QCOMPARE(img0, from_m);

    img0.to_buffer(buf);
    // touch the image
    ((char*)buf)[1]+=1;
    ((char*)buf)[2]+=1;
    ((char*)buf)[3]+=1;
    Magick::Image m2(img0.width(), img0.height(), "RGB", Magick::CharPixel, buf);
    StarTrailer::Image from_m2(m2);

    QVERIFY2(img0!=from_m2, "Image should be different");
    delete[] buf;
}

QTEST_APPLESS_MAIN(ImageTest)

#include "tst_imagetest.moc"
