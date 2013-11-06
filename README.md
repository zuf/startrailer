# StarTrailer
## helps you to make star trails from your photos
[![Build Status](https://api.travis-ci.org/zuf/startrailer.png)](https://travis-ci.org/zuf/startrailer)


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

### History or how StarTrailer was born

Few years ago I have got my first DSLR camera. After playing with it for some time I started to look at the skies.
Night skies are cool. I began to want to make shot for a whole night. I took attempt and discovered that
digital sensor makes too much noise. Oldschool film was unbeaten on this field. I have stop trying making
long night shots. Time goes on. And after year or something like this I were been in wild photo expedition.
Where one man says to me that combining lots of night shot to one is a good idea for star trails photo.

I start think about it and take the point that making single shot from hundreds are cheating and not
very interesting. I was calmed and stop thinking about that stuff again. But seed was placed in my head.

Time still passed. And one day I was found few posts on the internet about star trails and
how to make them. It was nice and cool. And I tried to make them. This posts told about photography
(which are out of the part of this story) and about startrails windows application which can compose photos to star trails.

I don't like windows and don't use it. I use linux (GNU/Linux if you wish).
I took a look on screenshots of this nice software and think that this software may be does not did any very special.
Just composing images with "lighten" method.

Ok, I say this for myself and... start googling for this kind of software for linux. Nothing was found.
After few attempts I discover [one post from Argentina][2].
I don't know spanish. But this page contains two important "keywords": ImageMagick and Ruby.
And even more - link to [gist][3] with ruby script for composing star trails!

I love ruby. I use this script, make my first amazing star trails... and look inside this script.
Script has one annoying issue. It save temporary results to jpeg file. So jpeg artifacts were accumulated in it.
This was not good. I made my version of this script (which use uncompressed format for temporary results).
I even trird to made parallel version (with [parallel][4] gem) of it.

I use this scripts several times last few years. They do the job. But there were two points that does not satisfy me.

Firstly, I love scripts. They can solve your task very vell. They highly configurable. But scripts are not interactive.
They cant gives fast previews of their work (when work still not completed). If you with to remove some files
from processing you must get result and then look on it. Then if you not satisfied with resilt you start work on separate files.
Then again composite files to star trails result and do all this stuff again... This may be annoying a bit.

Secondly, my scrips can't work with previews from RAW files. I want this fature to look on result of my night shots
as fast as posible. And I don't want to make RAW+JPG shots in camera because most of RAWs already has full-resolution jpeg preview inside.

So as you can clue, at one moment I finally saying that enough for me.

And start make StarTrailer - the software which can gives you fast and nice preview of your great startrails.
And you can just play with your night shots, even they stored in complicated RAW format.

At the end of this story I want to send very big thanks to great authors of cool libs without which StarTrails can not were builded:

* ImageMagick - http://www.imagemagick.org/
* Libraw - http://www.libraw.su/
* Qt - http://qt-project.org/

Thanks a lot!


### License

Copyright (c) 2013 Egor Vakhromtsev MIT Licensed, see [LICENSE-MIT] for details.

## FAQ

#### How to make star trails without this software

1. Write your own script that use `convert -composite` from ImageMagick. Like [this][3] or [this one][5].
2. If you are on windows try [Startrails][6] application.


  [1]: http://zuf.github.io/startrailer/StarTrailer_20131008.png
  [2]: http://ktulu.com.ar/blog/2012/03/25/star-trails-con-imagemagick/
  [3]: https://gist.github.com/luisparravicini/2194335
  [4]: https://github.com/grosser/parallel
  [5]: https://gist.github.com/zuf/6825447
  [6]: http://www.startrails.de/html/software.html
