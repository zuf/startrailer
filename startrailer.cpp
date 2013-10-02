#include "startrailer.h"

#include <QDebug>
#include <omp.h>


StarTrailer::StarTrailer()
{    
#if defined(_OPENMP)
   qDebug() << "Compiled by an OpenMP-compliant implementation.";
   omp_set_num_threads(4);
   qDebug() << "The result of omp_get_num_threads: " << omp_get_num_threads();
#endif
}

Magick::Image *StarTrailer::compose_first_with_second(Magick::Image *first, Magick::Image *second)
{
    first->composite(*second, 0,0, Magick::LightenCompositeOp);
    return first;
}

const Magick::Image *StarTrailer::compose(const Magick::Image &one, const Magick::Image &another)
{
    Magick::Image *copy_of_one = new Magick::Image(one);
    copy_of_one->composite(another, 0,0, Magick::LightenCompositeOp);
    return copy_of_one;
}

const Magick::Image *StarTrailer::compose(const Magick::Image &one, const std::string &another_image_path)
{
    Magick::Image *copy_of_one = new Magick::Image(one);
    Magick::Image *another= new Magick::Image(another_image_path); // do not hope that it can be placed into stack
    copy_of_one->composite(*another, 0,0, Magick::LightenCompositeOp);
    delete another;
    return copy_of_one; // now it not the copy of one but result one
}

const Magick::Image *StarTrailer::compose(const std::string &image_one_path, const std::string &another_image_path)
{
    Magick::Image *one = new Magick::Image(image_one_path);
    Magick::Image *another= new Magick::Image(another_image_path); // still not hope that it can be placed into stack ;)
    one->composite(*another, 0,0, Magick::LightenCompositeOp);
    delete another;
    return one;
}

const QByteArray *StarTrailer::q_compose(const std::string &image_one_path, const std::string &image_another_path)
{    
    Magick::Image *img = (Magick::Image *) compose(image_one_path, image_another_path);
    img->magick("BMP");
    Magick::Blob *blob = new Magick::Blob();
    img->write(blob);
    delete img;
    const QByteArray *imageData = new QByteArray((char*)(blob->data()), blob->length());
    delete blob;
    return imageData;
}

const QByteArray *StarTrailer::image_to_qbyte_array(Magick::Image *image)
{
    return image_to_qbyte_array(*image);
}

const QByteArray *StarTrailer::image_to_qbyte_array(Magick::Image &image)
{
    image.magick("BMP");
    Magick::Blob *blob = new Magick::Blob();
    image.write(blob);
    const QByteArray *image_data = new QByteArray((char*)(blob->data()), blob->length());
    delete blob;
    return image_data;
}

const QByteArray *StarTrailer::q_compose_list(QStringList files)
{
    QStringList::const_iterator constIterator;
    Magick::Image *out_image = new Magick::Image((*files.constBegin()).toStdString());
    Magick::Image *tmp_image = new Magick::Image(*out_image);
    for (constIterator = files.constBegin(); constIterator != files.constEnd(); ++constIterator)
    {
        // TODO: Skip first iteration
        //std::out << "Trail file: " << (*constIterator).toStdString();
        tmp_image->read((*constIterator).toStdString());
        compose_first_with_second(out_image, tmp_image);
    }

    delete tmp_image;
    const QByteArray *image_data = image_to_qbyte_array(out_image);
    delete out_image;

    return image_data;
}


const QByteArray *StarTrailer::q_compose_model_list(const QFileSystemModel *model, QModelIndexList list)
{
    QModelIndexList::const_iterator constIterator;

    Magick::Image *out_image = new Magick::Image(model->filePath(*list.constBegin()).toStdString());
    Magick::Image *tmp_image = new Magick::Image(*out_image);
    for (constIterator = list.constBegin(); constIterator != list.constEnd(); ++constIterator)
    {
        // TODO: Skip first iteration
        //std::out << "Trail file: " << (*constIterator).toStdString();
        qDebug() << "Trail file: " << (model->filePath((*constIterator)));;
        tmp_image->read(model->filePath((*constIterator)).toStdString());
        compose_first_with_second(out_image, tmp_image);
    }

    delete tmp_image;
    const QByteArray *image_data = image_to_qbyte_array(out_image);
    delete out_image;

    return image_data;
}
