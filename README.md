# StarTrailer
## helps you to make star trails from your photos


> **This software are still at very early stage.**


### Benefits

* Can read and composite RAW files with libraw. Useful for making previews of your shots before you start processing RAWs in your favourite processing software.
* Reads many formats (with ImageMagick).
* Can composite images with different methods (which ImageMagick support).
* Ability to take fast previews of star trails or time lapse.
* Cross platform. Should works on Linux, Mac and Windows.


![StarTrailer screenshot][1]

### Built with

* ImageMagick - http://www.imagemagick.org/
* Libraw - http://www.libraw.su/
* Qt - http://qt-project.org/

### License

Copyright (c) 2013 Egor Vakhromtsev MIT Licensed, see [LICENSE-MIT] for details.

## FAQ

#### How to make star trails without this software

1. Write your own script that use `convert -composite` from ImageMagick. Like this one: https://gist.github.com/zuf/6825447
2. If you are on windows try Startrails application: http://www.startrails.de/html/software.html


  [1]: http://zuf.github.io/startrailer/StarTrailer_20131008.png
