using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Imaging;
using Syncfusion.Windows.Forms;

namespace Editor.Dialogs
{
    public partial class HDRColorDialog : Office2007Form
    {
        // standard RGB values.
        private float _hue;
        private float _saturation;
        private float _intensity;
        private float _alpha;
        private const float maxIntensity = 4.0f;
        private bool _localUpdate;
		private bool _colorChanging;
        private bool _hideOnClose;


        //================================
        // ctor & ftor.
        //================================

        //--------------------------------
        public HDRColorDialog()
        {
            InitializeComponent();

            // create bitmaps for the image boxes.
            hueImageBox.Image = new Bitmap(hueImageBox.Size.Width,
                hueImageBox.Size.Height, PixelFormat.Format32bppArgb);
            saturationImageBox.Image = new Bitmap(saturationImageBox.Size.Width,
                saturationImageBox.Size.Height, PixelFormat.Format32bppArgb);
            finalColorImageBox.Image = new Bitmap(finalColorImageBox.Size.Width,
                 finalColorImageBox.Size.Height, PixelFormat.Format32bppArgb);

            _localUpdate = false;
            _hideOnClose = false;

            // generate the hue bitmap.
            GenHueBitmap();

            // configure the current color to red.
            _hue = 0.0f;
            _intensity = 1.0f;
            _saturation = 1.0f;
            _alpha = 1.0f;

            // update all of the controls.
            UpdateControls();
        }


        //================================
        // public interface.
        //================================

		//--------------------------------
		public event EventHandler ColorChanged;

		//--------------------------------
		public Bootstrap.Color Color
		{
			get 
			{
				float r, g, b;
				CalcRGB( out r, out g, out b,
					_hue, _saturation, _intensity );
				return new Bootstrap.Color( r, g, b, _alpha );
			}
			set
			{
				_alpha = value.A;
				CalcHSV( out _hue, out _saturation, out _intensity, 
					value.R, value.G, value.B );
	            GenSaturationBitmap();
	            GenFinalColorBitmap();
                UpdateControls();
				if( !_colorChanging )
					RaiseColorChangedEvent();
			}
		}

        //--------------------------------
        public bool HideOnClose
        {
            get { return _hideOnClose; }
            set { _hideOnClose = value; }
        }

        
        //================================
        // event callbacks.
        //================================
        
        private void hueImageBox_MouseEvent(object sender, MouseEventArgs e)
        {
			if( e.Button == MouseButtons.Left )
			{
	            // figure out where in the image box the user clicked.  That
	            // will let us select the hue.
	            MouseEventArgs args = e as MouseEventArgs;
	            _hue = 1.0f - Util.WhereInRange( args.Y, 0, hueImageBox.Height );
	            hueTextBox.Text = _hue.ToString();

	            // update the saturation image box with the new hue.
	            GenSaturationBitmap();
	            GenFinalColorBitmap();
	            UpdateRGBControls();
				RaiseColorChangedEvent();
			}
        }

        private void saturationImageBox_MouseEvent(object sender, MouseEventArgs e)
        {
			if( e.Button == MouseButtons.Left )
			{
	            // figure out where in the image box the user clicked.  That
	            // will let us select the saturation.
	            MouseEventArgs args = e as MouseEventArgs;
	            _saturation = Util.WhereInRange( args.Y, 0, saturationImageBox.Height );
	            saturationTextBox.Text = _saturation.ToString();
	            
	            // update the RGB color bitmap.
	            UpdateRGBControls();
	            GenFinalColorBitmap();
				RaiseColorChangedEvent();
			}
        }

        private void hueTextBox_TextChanged(object sender, EventArgs e)
        {
            // ignore any events that this class triggers internally.
            if (_localUpdate)
                return;

            // get the value from the text box.
            _hue = GetTextBoxValue(hueTextBox, 0.0f, 1.0f, 0.0f);
            UpdateRGBControls();
            GenSaturationBitmap();
            GenFinalColorBitmap();
			RaiseColorChangedEvent();
        }

        private void saturationTextBox_TextChanged(object sender, EventArgs e)
        {
            // ignore any events that this class triggers internally.
            if (_localUpdate)
                return;

            _saturation = GetTextBoxValue(saturationTextBox, 0.0f, 1.0f, 1.0f);
            UpdateRGBControls();
            GenFinalColorBitmap();
			RaiseColorChangedEvent();
        }

        private void redTextBox_TextChanged(object sender, EventArgs e)
        {
            // ignore any events that this class triggers internally.
            if (_localUpdate)
                return;

            // calculate the current color to RGB, alter the red component,
            // and then change back.
            float r, g, b;
            CalcRGB(out r, out g, out b, _hue, _saturation, _intensity);

            // get the new value and convert back.
            r = GetTextBoxValue(redTextBox, 0.0f, maxIntensity, 1.0f);
            CalcHSV(out _hue, out _saturation, out _intensity, r, g, b);

            // update HSV controls.
            UpdateHSVControls();
            GenSaturationBitmap();
            GenFinalColorBitmap();
			RaiseColorChangedEvent();
        }

        private void greenTextBox_TextChanged(object sender, EventArgs e)
        {
            // ignore any events that this class triggers internally.
            if (_localUpdate)
                return;

            // calculate the current color to RGB, alter the green component,
            // and then change back.
            float r, g, b;
            CalcRGB(out r, out g, out b, _hue, _saturation, _intensity);

            // get the new value and convert back.
            g = GetTextBoxValue(greenTextBox, 0.0f, maxIntensity, 0.0f);
            CalcHSV(out _hue, out _saturation, out _intensity, r, g, b);

            // update HSV controls.
            UpdateHSVControls();
            GenSaturationBitmap();
            GenFinalColorBitmap();
			RaiseColorChangedEvent();
        }

        private void blueTextBox_TextChanged(object sender, EventArgs e)
        {
            // ignore any events that this class triggers internally.
            if (_localUpdate)
                return;

            // calculate the current color to RGB, alter the blue component,
            // and then change back.
            float r, g, b;
            CalcRGB(out r, out g, out b, _hue, _saturation, _intensity);

            // get the new value and convert back.
            b = GetTextBoxValue(blueTextBox, 0.0f, maxIntensity, 0.0f);
            CalcHSV(out _hue, out _saturation, out _intensity, r, g, b);

            // update HSV controls.
            UpdateHSVControls();
            GenSaturationBitmap();
            GenFinalColorBitmap();
			RaiseColorChangedEvent();
        }

        private void intensityTextBox_TextChanged(object sender, EventArgs e)
        {
            // ignore any events that this class triggers internally.
            if (_localUpdate)
                return;

            // calculate the new intensity.
            _intensity = GetTextBoxValue(intensityTextBox, 0.0f, maxIntensity, 1.0f);
            UpdateRGBControls();
            UpdateIntensityTrackBar();
            GenFinalColorBitmap();
			RaiseColorChangedEvent();
        }

        private void intensityTrackBar_Scroll(object sender, EventArgs e)
        {
            // ignore any events that this class triggers internally.
            if (_localUpdate)
                return;

            // calculate the new intensity.
            int localMin = intensityTrackBar.Minimum;
            int localMax = intensityTrackBar.Maximum - localMin;
            int localValue = intensityTrackBar.Value - localMin;
            _intensity = maxIntensity * localValue / localMax;

            // update the controls.
            _localUpdate = true;
            intensityTextBox.Text = _intensity.ToString();
            _localUpdate = false;
            UpdateRGBControls();
            GenFinalColorBitmap();
			RaiseColorChangedEvent();
        }

        private void alphaTextBox_TextChanged(object sender, EventArgs e)
        {
            _alpha = GetTextBoxValue(alphaTextBox, 0.0f, 4.0f, 1.0f);
			RaiseColorChangedEvent();
        }


        //================================
        // private methods.
        //================================

		private void RaiseColorChangedEvent()
		{
			_colorChanging = true;
			if( ColorChanged != null )
				ColorChanged( this, new EventArgs() );
			_colorChanging = false;
		}

        private void UpdateControls()
        {
            // generate the hue bitmap.
            GenSaturationBitmap();
            GenFinalColorBitmap();

            _localUpdate = true;

            // update HSV textboxes along with the intensity slider.
            hueTextBox.Text = ClampFloat(_hue).ToString();
            saturationTextBox.Text = ClampFloat(_saturation).ToString();
            intensityTextBox.Text = ClampFloat(_intensity).ToString();
            alphaTextBox.Text = ClampFloat(_alpha).ToString();
            UpdateIntensityTrackBar();

            // update the RGB controls.
            float r, g, b;
            CalcRGB(out r, out g, out b, _hue, _saturation, _intensity);
            redTextBox.Text = ClampFloat(r).ToString();
            greenTextBox.Text = ClampFloat(g).ToString();
            blueTextBox.Text = ClampFloat(b).ToString();

            _localUpdate = false;
        }

        private void UpdateHSVControls()
        {
            // recursion guard.
            if (_localUpdate)
                return;

            // update HSV textboxes along with the intensity slider.
            _localUpdate = true;
            hueTextBox.Text = ClampFloat(_hue).ToString();
            saturationTextBox.Text = ClampFloat(_saturation).ToString();
            intensityTextBox.Text = ClampFloat(_intensity).ToString();
            UpdateIntensityTrackBar();
            _localUpdate = false;
        }

        private void UpdateRGBControls()
        {
            // recursion guard.
            if (_localUpdate)
                return;

            // update the RGB controls.
            _localUpdate = true;
            float r, g, b;
            CalcRGB(out r, out g, out b, _hue, _saturation, _intensity);
            redTextBox.Text = ClampFloat(r).ToString();
            greenTextBox.Text = ClampFloat(g).ToString();
            blueTextBox.Text = ClampFloat(b).ToString();
            _localUpdate = false;
        }

        private void CalcHSV( out float hue, out float saturation, out float intensity,
            float r, float g, float b )
        {
            // calculate our rgb color.
            float[] rgb = { r, g, b };

            // intensity = max.
            intensity = 0.0f;
            int iC = 0;
            for (int i = 0; i < 3; ++i)
            {
                if (intensity < rgb[i])
                {
                    intensity = rgb[i];
                    iC = i;
                }
            }

            // if the intensity is 0, simply keep the hue the same.
            hue = 0.0f;
            saturation = 0.0f;
            if (intensity < 0.001f )
                return;

            // rescale the color components by the intensity.
            float invIntensity = 1.0f / intensity;
            for (int i = 0; i < 3; ++i)
                rgb[i] *= invIntensity;

            // saturation = 1 - min.
            float minC = 1.0f;
            int sC = 0;
            for (int i = 0; i < 3; ++i)
            {
                if (minC > rgb[i])
                {
                    minC = rgb[i];
                    sC = i;
                }
            }
            saturation = 1.0f - minC;

            // hue = remaining two colors.
            int r1 = ( sC + 1 ) % 3;
            int r2 = ( sC + 2 ) % 3;

            // red-green.
            if (rgb[r1] > rgb[r2])
                hue = ( ( 2 * r1 ) + rgb[r2] ) / 6.0f;
            else
                hue = ( ( 2 * r1 + 1 ) + 1.0f - rgb[r1] ) / 6.0f;
        }

        private void CalcRGB(out float r, out float g, out float b,
            float hue, float saturation, float intensity)
        {
            // figure out what colors to use.  Lets use a lookup table =).
            float blueRed = 4.0f / 6.0f;
            float greenBlue = 2.0f / 6.0f;
            float oneMinusSaturation = 1.0f - saturation;
            if (hue > blueRed)
            {
                float lerp = 6.0f * (hue - blueRed);
                b = saturation * Math.Min(2.0f - lerp, 1.0f) + oneMinusSaturation;
                r = saturation * Math.Min(lerp, 1.0f) + oneMinusSaturation;
                g = oneMinusSaturation;
            }
            else if (hue > greenBlue)
            {
                float lerp = 6.0f * (hue - greenBlue);
                r = oneMinusSaturation;
                g = saturation * Math.Min(2.0f - lerp, 1.0f) + oneMinusSaturation;
                b = saturation * Math.Min(lerp, 1.0f) + oneMinusSaturation;
            }
            else // red-green.
            {
                float lerp = 6.0f * hue;
                r = saturation * Math.Min(2.0f - lerp, 1.0f) + oneMinusSaturation;
                g = saturation * Math.Min(lerp, 1.0f) + oneMinusSaturation;
                b = oneMinusSaturation;
            }

            // make sure the color is the right intensity.
            r *= intensity;
            g *= intensity;
            b *= intensity;
        }

        private void GenSaturationBitmap()
        {
            // lock the entire bitmap.
            int imageWidth = saturationImageBox.Size.Width;
            int imageHeight = saturationImageBox.Size.Height;
            Bitmap bitmap = saturationImageBox.Image as Bitmap;
            Rectangle rect = new Rectangle(new Point(0, 0), new Size(imageWidth, imageHeight));
            BitmapData bitmapData = bitmap.LockBits(rect, ImageLockMode.WriteOnly,
                PixelFormat.Format32bppArgb);

            // here we use unsafe code to modify the bitmap data.
            unsafe
            {
                // get the destination pointer.
                uint* pDst = (uint*)bitmapData.Scan0.ToPointer();

                // calculate the color for each line.
                float r = 0.0f;
                float g = 0.0f;
                float b = 0.0f;
                float fy = 0.0f;
                float fInvHeight = 1.0f / imageHeight;
                uint* dst = pDst;
                for (int y = 0; y < imageHeight; ++y, dst = (uint*)((byte*)dst + bitmapData.Stride), fy += 1)
                {
                    float saturation = fInvHeight * fy;
                    CalcRGB(out r, out g, out b, _hue, saturation, 1.0f);

                    // calculate the color for the current row.
                    uint color = 0xFF000000;
                    color |= (uint)(b * 255.0f);
                    color |= (uint)(g * 255.0f) << 8;
                    color |= (uint)(r * 255.0f) << 16;

                    // write out the current row.
                    for (int x = 0; x < imageWidth; ++x)
                        dst[x] = color;
                }
            }

            // unlock the bitmap.
            bitmap.UnlockBits(bitmapData);
            saturationImageBox.Invalidate();
        }

        private void GenHueBitmap()
        {
            // lock the entire bitmap.
            int imageWidth = hueImageBox.Size.Width;
            int imageHeight = hueImageBox.Size.Height;
            Bitmap bitmap = hueImageBox.Image as Bitmap;
            Rectangle rect = new Rectangle(new Point(0, 0), new Size(imageWidth, imageHeight));
            BitmapData bitmapData = bitmap.LockBits(rect, ImageLockMode.WriteOnly,
                PixelFormat.Format32bppArgb);

            // here we use unsafe code to modify the bitmap data.
            unsafe
            {
                // get the destination pointer.
                uint* pDst = (uint*)bitmapData.Scan0.ToPointer();

                // calculate the color for each line.
                float r = 0.0f;
                float g = 0.0f;
                float b = 0.0f;
                float fy = 0.0f;
                float fInvHeight = 1.0f / imageHeight;
                uint* dst = pDst;
                for (int y = 0; y < imageHeight; ++y, dst = (uint*)((byte*)dst + bitmapData.Stride), fy += 1)
                {
                    float hue = 1.0f - fInvHeight * fy;
                    CalcRGB(out r, out g, out b, hue, 1.0f, 1.0f);

                    // calculate the RGB color value for the current row.
                    uint color = 0xFF000000;
                    color |= (uint)(b * 255.0f);
                    color |= (uint)(g * 255.0f) << 8;
                    color |= (uint)(r * 255.0f) << 16;

                    // write out the current row.
                    for (int x = 0; x < imageWidth; ++x)
                        dst[x] = color;
                }
            }

            // unlock the bitmap.
            bitmap.UnlockBits(bitmapData);
            hueImageBox.Invalidate();
        }

        private void GenFinalColorBitmap()
        {
            // lock the entire bitmap.
            int imageWidth = finalColorImageBox.Size.Width;
            int imageHeight = finalColorImageBox.Size.Height;
            Bitmap bitmap = finalColorImageBox.Image as Bitmap;
            Rectangle rect = new Rectangle(new Point(0, 0), new Size(imageWidth, imageHeight));
            BitmapData bitmapData = bitmap.LockBits(rect, ImageLockMode.WriteOnly,
                PixelFormat.Format32bppArgb);

            // here we use unsafe code to fill out the bitmap.
            unsafe
            {
                // calculate the final color.
                float r = 0.0f;
                float g = 0.0f;
                float b = 0.0f;
                CalcRGB(out r, out g, out b, _hue, _saturation, Math.Min(_intensity, 1.0f));

                // calculate the value that we'll be writing.
                uint color = 0xFF000000;
                color |= (uint)(b * 255.0f);
                color |= (uint)(g * 255.0f) << 8;
                color |= (uint)(r * 255.0f) << 16;

                // write out the final color.
                uint* dst = (uint*)bitmapData.Scan0.ToPointer();
                for (int y = 0; y < imageHeight; ++y, dst = (uint*)((byte*)dst + bitmapData.Stride))
                    for (int x = 0; x < imageWidth; ++x)
                        dst[x] = color;
            }

            // unlock the bitmap.
            bitmap.UnlockBits(bitmapData);
            finalColorImageBox.Invalidate();
        }

        private float GetTextBoxValue(TextBox textBox, float min, float max, float def)
        {
            // get the value from the text box.
            float result = def;
            if (textBox.Text.Length == 0)
                return def;

            // try to parse the value into a float.  Simply return the default
            // on failure.
            try
            {
                result = float.Parse(textBox.Text);
            }
            catch (System.Exception)
            {
                return def;
            }
        
            // clamp the hue to [min..max].
            if (result > max)
            {
                result = max;
                textBox.Text = max.ToString();
            }
            else if (result < min)
            {
                result = min;
                hueTextBox.Text = min.ToString();
            }

            // return the result.
            return result;
        }

        void UpdateIntensityTrackBar()
        {
            int localMin = intensityTrackBar.Minimum;
            int localMax = intensityTrackBar.Maximum - localMin;
            intensityTrackBar.Value = (int)((_intensity / maxIntensity) * localMax) + localMin;
        }

        float ClampFloat(float v)
        {
            if (v < 0.001f)
                v = 0.0f;
            return v;
        }

		private void saturationImageBox_Paint( object sender, PaintEventArgs e )
		{
			// paint the ImageBox bitmap.
			base.OnPaint( e );

			// paint a black border.
			Util.DrawBorder( e.Graphics, Pens.Black );
		}

		private void hueImageBox_Paint( object sender, PaintEventArgs e )
		{
			// paint the ImageBox bitmap.
			base.OnPaint( e );

			// paint a black border.
			Util.DrawBorder( e.Graphics, Pens.Black );
		}

		private void finalColorImageBox_Paint( object sender, PaintEventArgs e )
		{
			// paint the ImageBox bitmap.
			base.OnPaint( e );

			// paint a black border.
			Util.DrawBorder( e.Graphics, Pens.Black );
		}

		protected override void OnPaintBackground( PaintEventArgs e )
		{
			// paint the necessities.
			base.OnPaintBackground( e );

			// paint some dropshadows.
			Graphics g = e.Graphics;
			Util.FillDropshadows( g, Brushes.Black, 2.0f, 2.0f,
			  new Control[] { 
	            saturationImageBox, hueImageBox, finalColorImageBox,
	            saturationTextBox, hueTextBox,
	            redTextBox, greenTextBox, blueTextBox, alphaTextBox } );
		}

        private void HDRColorDialog_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (_hideOnClose)
            {
                this.Hide();
                e.Cancel = true;
            }
        }
	}
}