using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Editor.Controls
{
    public partial class ThumbnailSelector : UserControl, IEnumerable<Thumbnail>
    {
        //=======================
		// constants
        //=======================
		int kRetypeInterval = 1500; // interval after which the user's "typing text" is reset.  (See OnKeyPress.)

        //=======================
		// member variables
        //=======================

        Size _thumbnailSize;
        List<Thumbnail> _selection;
        bool _multiSelect;
        int _count;
		Color _backgroundColor;
		Timer _clearUserTyping;

        //=======================
		// constructor & finalizer
        //=======================

        //-----------------------
        public ThumbnailSelector()
        {
            InitializeComponent();

            _selection = new List<Thumbnail>();
            _multiSelect = true;
            _thumbnailSize = new Size(64, 64);
			_backgroundColor = this.BackColor;

			// this control can receive focus (for navigational keypresses).
			SetStyle( ControlStyles.Selectable, true );

			_clearUserTyping = new Timer();
			_clearUserTyping.Interval = kRetypeInterval;
			_clearUserTyping.Tick += new EventHandler( _clearUserTyping_Tick );
        }


        //=======================
        // Events
        //=======================

        //-----------------------
        public event EventHandler SelectionChanged;
        public event EventHandler SelectionMade;
		public event MouseEventHandler ThumbnailClick;
		public event EventHandler ThumbnailDeleted;

        //=======================
        // Attributes
        //=======================

        //-----------------------
        IEnumerator IEnumerable.GetEnumerator() { return GetEnumerator(); }
        public IEnumerator<Thumbnail> GetEnumerator()
        {
            foreach (Control control in this.Controls)
                if (control is Thumbnail)
                    yield return control as Thumbnail;
        }

        //-----------------------
        public int ThumbnailCount
        {
            get { return _count; }
        }

        //-----------------------
		public bool ShowDeleteButtons
		{
			get { return _showDeleteButtons; }
			set 
			{ 
				if( _showDeleteButtons == value ) 
					return;
				_showDeleteButtons = value;
			}
		}
		bool _showDeleteButtons;


        //-----------------------
        public List<Thumbnail> Selection
        {
            get 
			{ 
				List< Thumbnail > sel = new List<Thumbnail>();
				foreach( Thumbnail thumbnail in _selection )
				{
					if( this.Controls.Contains( thumbnail ) )
						sel.Add( thumbnail );
				}
				return sel; 
			}
        }

        //-----------------------
		[Browsable(false)]
		public Thumbnail Selected
		{
			get
			{
	            // get the current selection (the last thing that was clicked on).
	            List< Controls.Thumbnail > selection = this.Selection;
				if( selection.Count > 0 ) 
					return selection[ selection.Count-1 ];
				else
					return null;
			}
			set
			{
				SelectThumbnail( value, false );
			}
		}

        //-----------------------
		[Browsable(false)]
		public string SelectedName
		{
			get
			{
				Thumbnail selected = this.Selected;
				if( selected != null )
					return selected.Name;
				else
					return "";
			}
			set
			{
				if( value != "" )
				{
					foreach( Thumbnail thumbnail in this )
					{
						if( thumbnail.Text == value )
						{
							SelectThumbnail( thumbnail, false );
							return;
						}
					}
				}
				SelectThumbnail( null, false );
			}
		}

        //-----------------------
        public Size ThumbnailSize
        {
            get { return _thumbnailSize; }
            set
            {
                _thumbnailSize = value;
                foreach (Thumbnail thumbnail in this)
                    thumbnail.ImageSize = value;
            }
        }

        //-----------------------
        public void Add(string text, Bitmap image)
        {
            // create the new thumbnail.
            Thumbnail thumbnail = new Thumbnail();
            thumbnail.Text = text;
            thumbnail.Image = image;
            thumbnail.ImageSize = _thumbnailSize;
			thumbnail.ShowDeleteButton = _showDeleteButtons;
            thumbnail.MouseClick += new MouseEventHandler(OnThumbnailClick);
            thumbnail.DoubleClick += new EventHandler(OnThumbnailDoubleClick);
			thumbnail.Deleted += new EventHandler(OnThumbnailDeleted);

            // add the thumbnail to our list of images.
            this.Controls.Add(thumbnail);
            ++_count;
        }

        //-----------------------
        public void Add(string text, Bitmap image, string userData)
        {
            // create the new thumbnail.
            Thumbnail thumbnail = new Thumbnail();
            thumbnail.Text = text;
            thumbnail.UserData = userData;
            thumbnail.Image = image;
			thumbnail.ShowDeleteButton = _showDeleteButtons;
            thumbnail.MouseClick += new MouseEventHandler(OnThumbnailClick);
            thumbnail.DoubleClick += new EventHandler(OnThumbnailDoubleClick);
			thumbnail.Deleted += new EventHandler(OnThumbnailDeleted);

            // add the thumbnail to our list of images.
            this.Controls.Add(thumbnail);
            ++_count;
        }

        //-----------------------
        public void Remove(string text)
        {
            // find the thumbnail by it's id.
            Thumbnail item = Find(text);
            if (item != null)
            {
                this.Controls.Remove(item);
                --_count;
            }
        }

        //-----------------------
        public void Clear()
        {
            foreach (Thumbnail thumbnail in this)
            {
                thumbnail.Dispose();
            }

            // all child controls are thumbnails.
            this.Controls.Clear();
            _count = 0;
        }

        //-----------------------
        public bool AllowMultiSelect
        {
            get { return _multiSelect; }
            set { _multiSelect = value; }
        }

        //=======================
        // internal methods
        //=======================

        //-----------------------
		internal void EnsureFocused()
		{
			this.Focus();

			// clear the partial "user typed" string.
			_userTyped = "";
		}

        //=======================
        // private methods
        //=======================

        //-----------------------
		string UserTyped
		{
			get { return _userTyped; }
			set
			{
				if( _userTyped == value )
					return;
				// remove newline characters.
				_userTyped = value.Replace("\n","").Replace("\r","");
				Viewport.OverrideStatusText( _userTyped );
			}
		}

        //-----------------------
		Thumbnail FindPartial( string startsWithText )
		{
			foreach( Thumbnail thumbnail in this )
			{
				if( thumbnail.Text.ToLower().StartsWith( startsWithText ) )
					return thumbnail;
			}
			return  null;
		}

        //-----------------------
        Thumbnail Find(string text)
        {
            foreach (Thumbnail thumbnail in this)
            {
                if (text == thumbnail.Text)
                    return thumbnail;
            }
            return null;
        }

        //-----------------------
		void SelectThumbnail( Thumbnail thumbnail, bool appendToSelection )
		{
			if( thumbnail != null )
			{
				// if that thumbnail isn't in this collection, complain loudly.
				if( !this.Controls.Contains( thumbnail ) )
					throw new InvalidEnumArgumentException( "That thumbnail is not in this ThumbnailSelector." );

	            // make sure the appendToSelection flag takes into account _multiSelect.
	            appendToSelection = appendToSelection && _multiSelect;
	            if (!appendToSelection)
	            {
	                // clear the current highlight.
	                foreach (Thumbnail thumb in _selection)
	                    thumb.SetHighlight(false);

	                // clear the current selection.
	                _selection.Clear();
	            }
	            else
	            {
	                // check to see if the item is already selected.
	                int idx = _selection.FindIndex(delegate(Thumbnail cur) { return (cur == thumbnail); });
	                if (idx >= 0)
	                {
	                    // if the item is already selected, de-select it.
	                    _selection[idx].SetHighlight(false);
	                    _selection.RemoveAt(idx);
	                    return;
	                }
	            }

	            // store the current selection.
	            thumbnail.SetHighlight(true);
	            _selection.Add(thumbnail);

				// scroll so that the newly selected thumbnail is visible.
				this.ScrollControlIntoView( thumbnail );
			}
			else
			{
				// clear the current highlight.
				foreach( Thumbnail thumb in _selection )
					thumb.SetHighlight( false );

				// clear the current selection.
				_selection.Clear();
			}

            // notify any subscribers of the pending event.
			RaiseSelectionChangedEvent();
		}

        //-----------------------
		void MoveSelection( int dx, int dy )
		{
			// don't do anything unless there are thumbnails.
			if( this.Controls.Count == 0 )
				return;

			// if no thumbnail is selected, simply select the first one in view.
			Thumbnail selected = this.Selected;
			if( selected == null )
			{
				foreach( Thumbnail thumbnail in this )
				{
					/*
					// grab the client (viewing) rectangle.
					Rectangle scrolledRect = this.ClientRectangle;

					// translate it by the display rectangle's offset (to account for scrolling).
					scrolledRect.Location = new Point( 
						scrolledRect.Location.X + this.DisplayRectangle.X,
						scrolledRect.Location.Y + this.DisplayRectangle.Y );
					 */

					// select the first thumbnail contained within the viewing area.
					if( this.ClientRectangle.Contains( thumbnail.Bounds ) )
					{
						SelectThumbnail( thumbnail, false );
						return;
					}
				}
			}

			// compute the thumbnail layout offset along the X and Y axes.
			int offX = selected.Width + selected.Margin.Right + selected.Margin.Left - 1;
			int offY = selected.Height + selected.Margin.Bottom + selected.Margin.Top - 1;

			// find a thumbnail along both axes whose position delta (from the selected thumbnail)
			// is [dx] multiples of [offX] or [dy] multiples of [offY].
			foreach( Thumbnail thumbnail in this )
			{
				if( ((thumbnail.Location.X - selected.Location.X) / offX == dx) && 
					((thumbnail.Location.Y - selected.Location.Y) / offY == dy) )
				{
					SelectThumbnail( thumbnail, false );
					return;
				}
			}
		}

        //=======================
        // events
        //=======================

        //-----------------------
		protected override bool ProcessDialogKey( Keys keyData )
		{
			switch( keyData )
			{
				// whenever the 'enter' or 'escape' keys are pressed,
				// clear the text the user has entered.
				case Keys.Enter:
				case Keys.Escape:
					{
						UserTyped = "";
					}
					// allow our parent to process the enter & escape keys.
					return base.ProcessDialogKey( keyData );

					// when the user presses backspace, remove a character 
					// from the text the user has entered.
				case Keys.Back:
					{
						if( UserTyped.Length > 0 )
							UserTyped = UserTyped.Substring( 0, UserTyped.Length-1 );
					}
					return true;

				// move the selection whenever the arrow keys are pressed.
				// (also clear the text the user has entered.)
				case Keys.Left:
				case Keys.Up:
				case Keys.Right:
				case Keys.Down:
					{
						if( keyData == Keys.Left )
							MoveSelection( -1,  0 );
						else if( keyData == Keys.Up )
							MoveSelection(  0, -1 );
						else if( keyData == Keys.Right )
							MoveSelection(  1,  0 );
						else if( keyData == Keys.Down )
							MoveSelection(  0,  1 );

						UserTyped = "";
					}
					return true;
			}

			return false;
		}

        //-----------------------
		protected override void OnKeyPress(KeyPressEventArgs e)
		{
			char charCode = e.KeyChar;
			charCode = charCode.ToString().ToLower()[0];
			
			// skip newlines.
			if( charCode == '\n' || charCode == '\r' )
				return;

			// reset the "clear what the user is typing" timer.
			_clearUserTyping.Stop();
			_clearUserTyping.Start();

			// add the character to the "user is typing" string.
			UserTyped = UserTyped + charCode;
			Console.WriteLine( UserTyped );

			// try to find a matching thumbnail.
			Thumbnail match = FindPartial( UserTyped );

			// if there is a match, select it.  Otherwise clear what the user has typed.
			if( match != null )
				SelectThumbnail( match, false );
		}
		string _userTyped;

        //-----------------------
		protected override bool IsInputKey( Keys keyData )
		{
			// if the key is A-Z, it's an input key.
			if( keyData.CompareTo( Keys.A ) >= 0 && keyData.CompareTo( Keys.Z ) <= 0 )
				return true;

			// if the key is 0-9, it's an input key.
			if( keyData.CompareTo( Keys.D0 ) >= 0 && keyData.CompareTo( Keys.D9 ) <= 0 )
				return true;

			// neither 'enter' nor 'backspace' are input keys.
			if( keyData == Keys.Enter || keyData == Keys.Back )
				return false;

			return base.IsInputKey( keyData );
		}

        //-----------------------
		void _clearUserTyping_Tick( object sender, EventArgs e )
		{
			UserTyped = "";
		}
        //-----------------------
		void RaiseSelectionChangedEvent()
		{
			if( SelectionChanged != null )
				SelectionChanged( this, new EventArgs() );
		}

        //-----------------------
		protected override void OnPaintBackground( PaintEventArgs e )
		{
			Graphics g = e.Graphics;

			if( Enabled )
			{
				base.OnPaintBackground( e );
			}
			else
			{
				// if the thumbnail selector is disabled, just fill with a "disabled" pattern.
				Util.Drawing.FillDisabled( g, g.VisibleClipBounds );
			}
		}

        //-----------------------
        private void OnThumbnailClick( object sender, MouseEventArgs e )
        {
            // determine whether or not we need to append.
            bool appendSel = ( Control.ModifierKeys & Keys.Control ) != 0 ||
                ( Control.ModifierKeys & Keys.Shift ) != 0;

			SelectThumbnail( (Thumbnail)sender, appendSel );

			if( ThumbnailClick != null )
				ThumbnailClick( sender, e );
        }

        //-----------------------
        private void OnThumbnailDoubleClick( object sender, EventArgs e )
        {
            // get the thumbnail object.
            Thumbnail thumbnail = sender as Thumbnail;

            // clear the current highlight.
            foreach (Thumbnail thumb in _selection)
                thumb.SetHighlight(false);

            // clear the current selection.
            _selection.Clear();

            // store the current selection.
            thumbnail.SetHighlight(true);
            _selection.Add(thumbnail);

            // notify any subscribers of the pending event.
			RaiseSelectionChangedEvent();

            // notify any subscribers that the user tried to make a
            // selection with a double click.
            if (SelectionMade != null)
                SelectionMade(this, new EventArgs());
        }

        //-----------------------
		void OnThumbnailDeleted( object sender, EventArgs e )
		{
			if( ThumbnailDeleted != null )
				ThumbnailDeleted( sender, e );
		}
    }
}
