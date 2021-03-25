using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using Syncfusion.Windows.Forms;

namespace Editor.Dialogs
{
    public partial class BrushSelectionDialog : Office2007Form
    {
        string _filter;
        List< Bootstrap.Brush > _selection;
		bool _loaded;

        public BrushSelectionDialog()
        {
            InitializeComponent();

			_selection = new List<Bootstrap.Brush>();

            // set the filter.
            _filter = "*.*";

			// initialize the exclusion.
			_exclude = new SortedList< string, Bootstrap.Brush >();

			// initialize the multi-select tip label.
			lblMultiSelectTip.Visible = thumbnailSelector.AllowMultiSelect;

            // add some event handlers.
            this.VisibleChanged += new EventHandler(OnShow);
			brushEditor.BrushEdited += new EventHandler( brushEditor_BrushEdited );

			// focus the thumbnail selector.
			thumbnailSelector.Focus();
        }

        //=======================
        // public attributes
        //=======================

        //-----------------------
		public bool AllowMultiSelect
		{
			get { return thumbnailSelector.AllowMultiSelect; }
			set 
			{ 
				thumbnailSelector.AllowMultiSelect = value; 
				lblMultiSelectTip.Visible = value;
			}
		}

        //-----------------------
        public List< Bootstrap.Brush > Selection
        {
            get { return _selection; }
        }

        //-----------------------
        public Bootstrap.Brush Selected
        {
            get 
			{ 
	            // get the current selection (the last thing that was clicked on).
				List< Bootstrap.Brush > selection = new List< Bootstrap.Brush>( this.Selection );
				if( selection.Count > 0 )
					return selection[ selection.Count-1 ];
				else
					return null;
			}
        }

        //-----------------------
		public SortedList< string, Bootstrap.Brush > Exclude
		{
			get { return new SortedList< string, Bootstrap.Brush >( _exclude ); }
			set 
			{
				if( value == null )
					return;

				// set the new exclusion.
				_exclude = value;

				// if the brush selector has been populated at least once,
				// reload the brushes to reflect the new exclusion.
				if( _loaded )
				{
					ReloadScripts();
				}
			}
		}
        SortedList< string, Bootstrap.Brush > _exclude;

        private void OnNewBrush(object sender, EventArgs e)
        {
            // create a new brush.
            BrushCreationDialog brushCreationDlg = new BrushCreationDialog();
            DialogResult result = brushCreationDlg.ShowDialog();
            if (result != DialogResult.OK)
				return;

            // create a new brush.
            string name = brushCreationDlg.tbName.Text;
            string category = brushCreationDlg.cbCategory.Text;
			Bootstrap.Brush selection = Bootstrap.Brush.Create( name, category );

            // make sure we contain the new brush.
            ReloadScripts();

			if( selection != null )
			{
				// select the new brush.
				thumbnailSelector.SelectedName = selection.Name;
			}

			// save the new brush.
			Bootstrap.Brush.SaveBrushes();
        }

        private void ReloadScripts()
        {
            // get info about the current folder.
            DirectoryInfo dir = new DirectoryInfo(Bootstrap.Settings.Get().GetBrushFolder());

            // each folder represents a category.
            string errors = "";
            DirectoryInfo[] categories = dir.GetDirectories(_filter);
            foreach (DirectoryInfo category in categories)
            {
                Bootstrap.Engine.Get().LoadBrushScript(category.FullName, errors);
            }

            // add all thumbnails.
            PopulateThumbnails();

			_loaded = true;
        }

        private void thumbnailSelector_SelectionChanged(object sender, EventArgs e)
        {
			// repopulate the selected brushes.
			_selection.Clear();
			foreach( Controls.Thumbnail thumbnail in thumbnailSelector.Selection )
			{
				_selection.Add( new Bootstrap.Brush( thumbnail.UserData ) );
			}

			// enable the brush editor if exactly one brush is selected.
			if( _selection.Count == 1 )
				brushEditor.Brush = _selection[0];
			else
				brushEditor.Brush = null;
        }

        private void PopulateThumbnails()
        {
            // remove existing brushes.
            thumbnailSelector.Clear();

            // get all brushes from the loaded scripts.
            string[] brushNames = Bootstrap.Brush.GetBrushNameList();

            // iterate through each brush and add a thumbnail.
			string selecting = "";
            foreach (string name in brushNames)
            {
				// if this brush is being excluded, skip it.
				if( _exclude.ContainsKey( name ) )
					continue;

                // create a manipulator for the current brush.
                using( Bootstrap.Brush brush = new Bootstrap.Brush(name) )
				{
	                // add the thumbnail to the selection dialog.
	                thumbnailSelector.Add( brush.Name, Program.ThumbnailMgr.GetThumbnail( brush ), brush.Name );

					// if the brush editor's brush matches the current brush, 
					// flag the current brush for selection.
					if( brushEditor.Brush != null && brushEditor.Brush.Name == name )
					{
						selecting = name;;
					}
				}
            }

			// select the appropriate brush.
			thumbnailSelector.SelectedName = selecting;
			if( selecting == "" )
				brushEditor.Brush = null;
        }

        private void OnShow(object sender, EventArgs e)
        {
            // simply return if we're being hidden.
            if( !this.Visible )
                return;

            // if there isn't anything in the thumbnail selector, try to
            // populate it.
            if( thumbnailSelector.ThumbnailCount == 0 )
                ReloadScripts();
        }

        private void thumbnailSelector_SelectionMade(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

		private void bnReload_Click( object sender, EventArgs e )
		{
			// reload all brushes.
			ReloadScripts();
		}

		void brushEditor_BrushEdited( object sender, EventArgs e )
		{
			// reload all brushes whenever a brush is edited.
			ReloadScripts();
		}
    }
}
