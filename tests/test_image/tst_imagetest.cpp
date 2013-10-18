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
};

ImageTest::ImageTest()
{

}

void ImageTest::testConstructors()
{
    Image img;
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(ImageTest)

#include "tst_imagetest.moc"
