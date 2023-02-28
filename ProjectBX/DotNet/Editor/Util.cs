using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Runtime.InteropServices; 

namespace Editor
{
    public static class Util
    {
        // -----------------------
        public static bool ValidateFileCharacter( char letter )
        {
            char c = Char.ToUpper( letter );
            return ( c >= 'A' && c <= 'Z' )
                || ( c >= '0' && c <= '9' )
                || c == '-'
                || c == '_';
        }

        // -----------------------
        public static float GetTrackBarValue( Syncfusion.Windows.Forms.Tools.TrackBarEx trackBar, float fMin, float fMax )
        {
            float alpha = WhereInRange( trackBar.Value, trackBar.Minimum, trackBar.Maximum );
            return Lerp( alpha, fMin, fMax );
        }

        // -----------------------
        public static void SetTrackBarValue( Syncfusion.Windows.Forms.Tools.TrackBarEx trackBar, float fMin, float fMax, float val )
        {
            float alpha = WhereInRange( val, fMin, fMax );
            trackBar.Value = (int)Lerp( alpha, trackBar.Minimum, trackBar.Maximum );
        }

        // -----------------------
        public static class Drawing
        {
            // -----------------------
            public static void FillPictureBox(PictureBox box, Color fillColor)
            {
                // create a row of the image, filled with the fillColor.
                int rowSize = 4 * box.Width;
                byte[] row = new byte[rowSize];
                for (int i = 0; i < box.Width; ++i)
                {
                    row[4 * i + 0] = fillColor.B;
                    row[4 * i + 1] = fillColor.G;
                    row[4 * i + 2] = fillColor.R;
                    row[4 * i + 3] = 255;
                }

                // create a new bitmap and fill it.
                Bitmap newImage = new Bitmap(box.Width, box.Height, PixelFormat.Format32bppArgb);
                BitmapData data = newImage.LockBits(new Rectangle(0, 0, box.Width, box.Height),
                    ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
                IntPtr ptr = data.Scan0;
                for (int i = 0; i < box.Height; ++i)
                {
                    // copy the data and advance the pointer by the stride.
                    Marshal.Copy(row, 0, ptr, rowSize);
                    ptr = new IntPtr(ptr.ToInt64() + data.Stride);
                }
                newImage.UnlockBits(data);

                // now finish by assigning the bitmap to the picture box.
                box.Image = newImage;
            }

            // -----------------------
            public static void FillDisabled( Graphics g, RectangleF rect )
            {
                using( HatchBrush brush = new HatchBrush( HatchStyle.LightDownwardDiagonal, Color.DarkGray, Color.Gray ) )
                {
                    g.FillRectangle( brush, rect );
                }
            }
        }

        // -----------------------
        // -- Misc. Programming Utilities

        public delegate bool ArrayFindCallback<T>(T item);

        // -----------------------
        public static T ArrayFind<T>(IEnumerable<T> collection, ArrayFindCallback<T> callback)
        {
            foreach (T item in collection)
                if (callback(item))
                    return item;
            return default( T );
        }

        // -----------------------
        public static float Lerp( float alpha, float x, float y )
        {
            return (y-x)*alpha + x;
        }

        // -----------------------
        public static T Clamp< T >( T value, T min, T max )
            where T : IComparable
        {
            if( value.CompareTo( min ) < 0 )
                return min;
            if( value.CompareTo( max ) > 0 )
                return max;
            return value;
        }

        // -----------------------
        // returns a [0 .. 1] value.
        public static float WhereInRange( float value, float min, float max )
        {
            return ( Clamp( value, min, max ) - min ) / ( max - min );
        }
        
        // -----------------------
        public delegate TTo TransformCallback< TFrom, TTo >( TFrom item );
        public static TTo[] Transform< TFrom, TTo >( TFrom[] collection, TransformCallback< TFrom, TTo > callback )
        {
          TTo[] results = new TTo[ collection.Length ];
          for( int i = 0; i < collection.Length; ++i )
            results[ i ] = callback( collection[ i ] );
          return results;
        }

        // -----------------------
        public static void DisposeCollection( IDisposable[] collection )
        {
          foreach( IDisposable disposable in collection )
            disposable.Dispose();
        }

        // -----------------------
        // -----------------------
        // -- Painting Utilities

        // -----------------------
        public static void DrawBorder( Graphics g, Pen borderPen )
        {
          RectangleF r = g.VisibleClipBounds;

          // top.
          g.DrawLine( Pens.Black, 0, 0, r.Width-1, 0 );

          // bottom.
          g.DrawLine( Pens.Black, 0, r.Height-1, r.Width-1, r.Height-1 );

          // left.
          g.DrawLine( Pens.Black, 0, 0, 0, r.Height-1 );

          // right.
          g.DrawLine( Pens.Black, r.Width-1, 0, r.Width-1, r.Height-1 );
        }

        // -----------------------
        public static void FillDropshadow( Graphics g, Brush shadowBrush, float shadowOffsetX, float shadowOffsetY, Region shadowCaster )
        {
          using( Region shadowCasterCopy = shadowCaster.Clone() )
          {
            // translate the region.
            shadowCasterCopy.Translate( shadowOffsetX, shadowOffsetY );

            // draw the shadow.
            g.FillRegion( shadowBrush, shadowCasterCopy);
          }
        }

        // -----------------------
        public static void FillDropshadow( Graphics g, Brush shadowBrush, float shadowOffsetX, float shadowOffsetY, RectangleF shadowCaster )
        {
          // translate the rect.
          shadowCaster.Location = new PointF( 
            shadowOffsetX + shadowCaster.Location.X, 
            shadowOffsetY + shadowCaster.Location.Y );

          // draw the shadow.
          g.FillRectangle( shadowBrush, shadowCaster );
        }

        // -----------------------
        public static void FillDropshadow( Graphics g, Brush shadowBrush, float shadowOffsetX, float shadowOffsetY, Control shadowCaster )
        {
          if( shadowCaster.Region != null )
            FillDropshadow( g, shadowBrush, shadowOffsetX, shadowOffsetY, shadowCaster.Region );
          else
            FillDropshadow( g, shadowBrush, shadowOffsetX, shadowOffsetY, shadowCaster.Bounds );
        }

        // -----------------------
        public static void FillDropshadows( Graphics g, Brush shadowBrush, float shadowOffsetX, float shadowOffsetY, IEnumerable< Control > shadowCasters )
        {
          foreach( Control control in shadowCasters )
            FillDropshadow( g, shadowBrush, shadowOffsetX, shadowOffsetY, control );
        }

        // -----------------------
        public static Bootstrap.Vector3 CalcNormal( Bootstrap.Vector3 p0, Bootstrap.Vector3 p1, Bootstrap.Vector3 p2 )
        {
            return ( p1 - p0 ).Cross( p2 - p0 ).Normalized;
        }

        // -----------------------
        public static float DegToRad( float deg )
        {
            return deg * (float)( Math.PI / 180.0 );
        }

        // -----------------------
        public static float RadToDeg( float rad )
        {
            return rad * (float)( 180.0 / Math.PI );
        }
    }
}
