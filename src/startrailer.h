//#ifndef STARTRAILER_H
//#define STARTRAILER_H
//#include <Magick++.h>
//#include <QByteArray>
//#include <QList>
//#include <QStringList>
//#include <QModelIndexList>
//#include <QFileSystemModel>
//#include <QMimeData>
//#include <QMimeDatabase>
//#include <QMimeType>
//#include "libraw/libraw.h"

////typedef QList<QModelIndex> QModelIndexList;

//namespace Magick
//{
//using MagickCore::DivideDstCompositeOp;
//  using MagickCore::DistortCompositeOp;
//  using MagickCore::BlurCompositeOp;
//  using MagickCore::PegtopLightCompositeOp;
//  using MagickCore::VividLightCompositeOp;
//  using MagickCore::PinLightCompositeOp;
//  using MagickCore::LinearDodgeCompositeOp;
//  using MagickCore::LinearBurnCompositeOp;
//  using MagickCore::MathematicsCompositeOp;
//  using MagickCore::DivideSrcCompositeOp;
//  using MagickCore::MinusSrcCompositeOp;
//  using MagickCore::DarkenIntensityCompositeOp;
//  using MagickCore::LightenIntensityCompositeOp;
//}

//class StarTrailer
//{
//public:
//    StarTrailer();
//    virtual ~StarTrailer();

//    Magick::Image *read_image(const std::string &file);

////    void compose(const Image::Image &one, const Image::Image &another, CompositeOperator method);
//    Magick::Image *compose_first_with_second(Magick::Image *first, Magick::Image *second, const Magick::CompositeOperator compose_op=Magick::LightenCompositeOp);

//    const Magick::Image *compose(const Magick::Image &image_one_path, const Magick::Image &image_another_path, const Magick::CompositeOperator compose_op=Magick::LightenCompositeOp);
//    const Magick::Image *compose(const Magick::Image &one, const std::string &another_image_path, const Magick::CompositeOperator compose_op=Magick::LightenCompositeOp);
//    const Magick::Image *compose(const std::string &image_one_path, const std::string &another_image_path, const Magick::CompositeOperator compose_op=Magick::LightenCompositeOp);

//    const QByteArray *image_to_qbyte_array(Magick::Image *image);
//    const QByteArray *image_to_qbyte_array(Magick::Image &image);

//    Magick::Image *compose_list(QStringList files, const Magick::CompositeOperator compose_op=Magick::LightenCompositeOp);

//    const QByteArray *q_compose(const std::string &image_one_path, const std::string &image_another_path, const Magick::CompositeOperator compose_op=Magick::LightenCompositeOp);
//    const QByteArray *q_compose_list_and_return_qbyte_array(QStringList files, const Magick::CompositeOperator compose_op=Magick::LightenCompositeOp);
//    const QByteArray *q_compose_model_list(const QFileSystemModel *model, QModelIndexList list, const Magick::CompositeOperator compose_op=Magick::LightenCompositeOp);

//private:
//    QMimeDatabase mimeDatabase;
//    QMimeType mimeType;
//    LibRaw *iProcessor;
//};

//#endif // STARTRAILER_H
