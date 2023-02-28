using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.IO;

namespace Editor.Utils
{
    class ThumbnailMgr : IDisposable
    {
        // list of thumbnails.
        List<ThumbnailPair> _thumbnails;
        Bitmap _errorImage;
        int _thumbWidth;
        int _thumbHeight;

        public class ThumbnailPair
        {
            public ThumbnailPair(string name, Bitmap image)
            {
                _Name = name;
                _Image = image;
            }

            public static string _FindName;
            public string _Name;
            public Bitmap _Image;
        };

        public ThumbnailMgr(int thumbWidth, int thumbHeight)
        {
            _thumbnails = new List<ThumbnailPair>();
            _thumbHeight = thumbHeight;
            _thumbWidth = thumbWidth;

            // create the error bitmap.
            Bitmap colorBmp = new Bitmap(2, 2);
            colorBmp.SetPixel(0, 0, Color.FromArgb(255, 255, 0, 0));
            colorBmp.SetPixel(1, 0, Color.FromArgb(255, 255, 255, 255));
            colorBmp.SetPixel(1, 1, Color.FromArgb(255, 255, 0, 0));
            colorBmp.SetPixel(0, 1, Color.FromArgb(255, 255, 255, 255));
            _errorImage = new Bitmap(colorBmp, 64, 64);
        }

        ~ThumbnailMgr()
        {
            
        }

        public void Dispose()
        {
            // dispose of all cached thumbnails.
            foreach (ThumbnailPair thumbnail in _thumbnails)
            {
                thumbnail._Image.Dispose();
            }
            _thumbnails.Clear();

            // dispose of the error image.
            _errorImage.Dispose();
            _errorImage = null;
        }

        public Bitmap GetThumbnail(string filePath)
        {
            // try to find the image.
            ThumbnailPair._FindName = filePath;
            ThumbnailPair imagePair = _thumbnails.Find(
                delegate(ThumbnailPair item) { return item._Name.CompareTo(ThumbnailPair._FindName) == 0; }
                );
            if (imagePair != null)
                return imagePair._Image;

            // make sure the current image type is supported.
            FileInfo file = new FileInfo(filePath);
            string ext = file.Extension.ToLower();
            if (ext != ".tga" && ext != ".dds" && ext != ".utx")
                return null;

            // check to see if we have an ubertexture.
            Bitmap bmpThumbnail = null;
            if (ext == ".utx")
            {
                Bootstrap.UberTexture uberTexture = new Bootstrap.UberTexture(filePath);
                if (uberTexture.Valid)
                {
                    // make a bitmap.
                    bmpThumbnail = uberTexture.MakeThumbnail();
                }
            }
            else
            {
                // load the current file and generate a thumbnail.
                Bootstrap.Image thumbnail = null;
                using (Bootstrap.Image image = new Bootstrap.Image(filePath))
                {
                    if (!image.IsValid())
                    {
                        return null;
                    }

                    // add the thumbnail to the selection dialog.
                    thumbnail = image.BuildThumbnail(64);
                }
                
                // convert the image to a thumbnail.
                bmpThumbnail = thumbnail.ToBitmap(-1, -1);
            }

            // make sure we store the thumbnail.
            ThumbnailPair pair = new ThumbnailPair(filePath, bmpThumbnail);
            _thumbnails.Add(pair);

            // remove the oldest thumbnails if we have too many thumbnails.
            if (_thumbnails.Count > 1024)
            {
                _thumbnails[0]._Image.Dispose();
                _thumbnails.RemoveAt(0);
            }

            // return the thumbnail.
            return bmpThumbnail;
        }

        public Bitmap GetThumbnail( Bootstrap.Brush brush )
        {
            // get the diffuse image and use that to display the brush.
            Bitmap image = null;
            string imageFile = brush.GetImageName( Bootstrap.Brush.Stage.Diffuse );

            // if there is no diffuse image, try to use the mask as our image.
            if (imageFile == "")
                imageFile = brush.GetMaskName();

            // now try to create a valid thumbnail.
            if (imageFile != "")    
                image = GetThumbnail( imageFile );

            // if we couldn't find a decent thumbnail, give it an error
            // image.
            if( image == null )
                image = GetErrorThumbnail();

            return image;
        }

        public Bitmap GetThumbnail( Bootstrap.BrushPalette palette )
        {
            // if there are no brushes, use the error thumbnail.
            if( palette.GetBrushCount() == 0 )
            {
                return GetErrorThumbnail();
            }

            // otherwise return the thumbnail of the first brush.
            return GetThumbnail( palette.GetBrush( 0 ) );
        }

        public Bitmap GetErrorThumbnail()
        {
            return _errorImage;
        }
    }
}
