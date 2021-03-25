using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Editor.Controls
{
    public delegate void ThumbnailEvent( object sender, bool appendSel );

    public partial class Thumbnail : UserControl
    {
        string _userText;
        Bitmap _image;

        // events.
		public event EventHandler Deleted;

        public Thumbnail()
        {
            InitializeComponent();

			SetStyle( ControlStyles.Selectable, false );

			// hide the delete button.
			pDelete.Visible = false;

			// attach some event handlers that only affect the label and the image, 
			// but not the surrounding space.
			pImageTarget.MouseClick += new MouseEventHandler( HandleClick );
			lblCaption.MouseClick += new MouseEventHandler( HandleClick );
			pImageTarget.MouseDoubleClick += new MouseEventHandler( HandleDoubleClick );
			lblCaption.MouseDoubleClick += new MouseEventHandler( HandleDoubleClick );
        }

        ~Thumbnail()
        {

        }

        public override string Text
        {
            get { return lblCaption.Text; }
            set { lblCaption.Text = value; }
        }

        public string UserData
        {
            get { return _userText; }
            set { _userText = value; }
        }

		public bool ShowDeleteButton
		{
			get { return _showDeleteButton; }
			set 
			{ 
				if( _showDeleteButton == value ) 
					return;
				_showDeleteButton = value;
				pDelete.Visible = value;
			}
		}
		bool _showDeleteButton;

        public Size ImageSize
        {
            get { return pImageTarget.Size; }
            set { pImageTarget.Size = value; }
        }

        public void SetHighlight(bool highlight)
        {
            if (highlight)
                lblCaption.BackColor = Color.SteelBlue;
            else
                lblCaption.BackColor = Color.Transparent;
        }

        [Browsable(false)]
        public Bitmap Image
        {
            get { return _image; }
            set { _image = value; }
        }

		protected override void OnGotFocus( EventArgs e )
		{
			// if this thumbnail is a member of a ThumbnailSelector,
			// focus the ThumbnailSelector.
			if( this.Parent is ThumbnailSelector )
			{
				ThumbnailSelector thumbnailSelector = (ThumbnailSelector)this.Parent;
				thumbnailSelector.EnsureFocused();
			}
			base.OnGotFocus( e );
		}

		protected override void OnMouseClick( MouseEventArgs e )
		{
			// if this thumbnail is a member of a ThumbnailSelector,
			// focus the ThumbnailSelector.
			if( this.Parent is ThumbnailSelector )
			{
				ThumbnailSelector thumbnailSelector = (ThumbnailSelector)this.Parent;
				thumbnailSelector.EnsureFocused();
			}

			base.OnMouseClick( e );
		}

        private void HandleClick(object sender, MouseEventArgs e)
        {
            // issue the OnSelect event.
			this.OnMouseClick( e );
        }

        private void HandleDoubleClick(object sender, EventArgs e)
        {
			this.OnDoubleClick( e );
        }

        private void pImageTarget_Paint(object sender, PaintEventArgs e)
        {
			Graphics g = e.Graphics;

			if( this.Enabled )
			{
	            if( _image != null )
	                g.DrawImageUnscaled( _image, new Point(0, 0) );
	            else
	                g.Clear(Color.FromArgb(255, 0, 0, 0));
			}
			else
			{
				// if the Thumbnail is disabled, just fill with a "disabled" pattern.
				Util.Drawing.FillDisabled( g, g.VisibleClipBounds );
			}
        }

		private void pDelete_Click( object sender, EventArgs e )
		{
			if( Deleted != null )
				Deleted( this, new EventArgs() );
		}
    }
}
