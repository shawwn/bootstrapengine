using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Editor.Editors
{
    public partial class BrushPaletteEditor : UserControl, IEnumerable< Bootstrap.Brush >
    {
        Editor.Dialogs.BrushSelectionDialog _brushSelectionDlg;
        Editor.Dialogs.BrushPaletteSelectionDialog _brushPaletteSelectionDlg;
        Bootstrap.BrushPalette _palette;

        //-----------------------
        public BrushPaletteEditor()
        {
            InitializeComponent();

            _brushSelectionDlg = new Editor.Dialogs.BrushSelectionDialog();
            _brushPaletteSelectionDlg = new Editor.Dialogs.BrushPaletteSelectionDialog();

            // allow the user to select multiple brushes from the brush selection dialog.
            _brushSelectionDlg.AllowMultiSelect = true;

            // don't allow the user to select more than one brush from the palette at a time.
            thumbnailSelector.AllowMultiSelect = false;

            // disable some controls.
            UpdatePaletteControls();

            // attach some event handlers.
            thumbnailSelector.SelectionChanged += new EventHandler( thumbnailSelector_SelectionChanged );
            thumbnailSelector.ThumbnailClick += new MouseEventHandler( thumbnailSelector_ThumbnailClick );
            popupEditBrush.Closing += new ToolStripDropDownClosingEventHandler( popupEditBrush_Closing );
            brushEditor.BrushEdited += new EventHandler( brushEditor_BrushEdited );
        }

        //-----------------------
        void popupEditBrush_Closing( object sender, ToolStripDropDownClosingEventArgs e )
        {
            // allow the popup to close if it was explicitly closed.
            if( e.CloseReason == ToolStripDropDownCloseReason.CloseCalled )
                return;

            // always allow the user to close a popup with the keyboard (ESC key).
            if( e.CloseReason == ToolStripDropDownCloseReason.Keyboard )
                return;

            // otherwise, don't allow the popup to close.
            e.Cancel = true;
        }

        //-----------------------
        public event EventHandler SelectionChanged;

        //-----------------------
        [Browsable(false)]
        public Bootstrap.BrushPalette Palette 
        {
            get { return _palette; }
            set 
            { 
                if( _palette == value )
                    return;
                _palette = value;
                UpdatePaletteLabel();
                UpdatePaletteControls(); 
                PopulateThumbnails();
            }
        }

        //-----------------------
        public Bootstrap.Brush Selection
        {
            get 
            {
                // if a brush is selected, return a new brush manipulator.
                Controls.Thumbnail thumbnail = thumbnailSelector.Selected;
                if( thumbnail != null )
                    return new Bootstrap.Brush( thumbnail.UserData );
                else
                    return null;
            }
        }

        //-----------------------
        BrushEditor brushEditor
        {
            get { return tsBrushEditor.EditorUI; }
        }

        //-----------------------
        public void LoadDefaultPalette()
        {
            _loadingSettings = true;
            Bootstrap.Settings settings = Bootstrap.Settings.Get();
            if( settings != null )
            {
                // try to load the palette the user previously selected.
                string palette = settings.GetUserSetting( "selectedbrushpalette" );
                if( palette.Length > 0 )
                    Palette = Bootstrap.BrushPalette.TryLoad( palette );

                // try to load the brush the user previously selected.
                string brush = settings.GetUserSetting( "selectedbrush" );
                if( brush.Length > 0 )
                    thumbnailSelector.SelectedName = brush;
            }
            _loadingSettings = false;
        }
        bool _loadingSettings;

        //-----------------------
        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() { return GetEnumerator(); }
        public IEnumerator< Bootstrap.Brush > GetEnumerator()
        {
            foreach( Controls.Thumbnail thumbnail in thumbnailSelector )
            {
                yield return new Bootstrap.Brush( thumbnail.UserData );
            }
        }

        //-----------------------
        void UpdatePaletteControls()
        {
            // if the editor is enabled then enable the editor controls 
            // and the thumbnail selector, else disable them.
            if( _palette != null )
            {
                pnEdit.Enabled = true;
                thumbnailSelector.Enabled = true;
            }
            else
            {
                pnEdit.Enabled = false;
                thumbnailSelector.Enabled = false;
            }
        }

        //-----------------------
        void PopulateThumbnails()
        {
            // remove existing brushes.
            thumbnailSelector.Clear();

            if( _palette != null )
            {
                // iterate through each brush and add a thumbnail.
                string selecting = "";
                foreach( Bootstrap.Brush brush in _palette.GetBrushes() )
                {
                    // add the thumbnail to the selection dialog.
                    thumbnailSelector.Add( brush.Name, Program.ThumbnailMgr.GetThumbnail( brush ), brush.Name );

                    // if the current brush matches the brush editor's brush, 
                    // flag it for selection.
                    if( brushEditor.Brush != null && brushEditor.Brush.Name == brush.Name )
                    {
                        selecting = brush.Name;
                    }
                }
                thumbnailSelector.SelectedName = selecting;
            }
        }

        //-----------------------
        void UpdatePaletteLabel()
        {
            if( _palette != null )
                pnPalette.Text = _palette.Name + " brushes";
            else
                pnPalette.Text = "Brushes";
        }

        //-----------------------
        void brushEditor_BrushEdited( object sender, EventArgs e )
        {
            PopulateThumbnails();
        }

        //-----------------------
        void thumbnailSelector_SelectionChanged( object sender, EventArgs e )
        {
            if( !_loadingSettings )
            {
                // save the brush selection (if we're not currently loading settings).
                Bootstrap.Settings settings = Bootstrap.Settings.Get();
                if( settings != null )
                {
                    Bootstrap.Brush selection = this.Selection;
                    string brush = "";
                    if( selection != null )
                        brush = selection.Name;
                    settings.SetUserSetting( "selectedbrush", brush );
                    settings.Save();
                }
            }

            // notify others that the selection has changed.
            if( SelectionChanged != null )
                SelectionChanged( sender, e );

            // close the "edit brush" popup.
            popupEditBrush.Close();
        }

        //-----------------------
        void thumbnailSelector_ThumbnailClick( object sender, MouseEventArgs e )
        {
            // if the user right-clicked on a brush thumbnail,
            // pop up the brush editor beneath it.
            if( e.Button == MouseButtons.Right )
            {
                Controls.Thumbnail thumbnail = (sender as Controls.Thumbnail);
                string brushName = thumbnail.UserData;
                brushEditor.Brush = new Bootstrap.Brush( brushName );
                popupEditBrush.Show( thumbnailSelector, new Point( thumbnail.Left, thumbnail.Bottom ) );
            }
        }

        //-----------------------
        private void bnAdd_Click( object sender, EventArgs e )
        {
            // exclude the brushes in this palette from the brush selector.
            SortedList< string, Bootstrap.Brush > brushes = new SortedList< string, Bootstrap.Brush >();
            foreach( Bootstrap.Brush brush in this )
            {
                brushes.Add( brush.Name, brush );
            }
            _brushSelectionDlg.Exclude = brushes;

            // present the brush selector to the user.
            DialogResult result = _brushSelectionDlg.ShowDialog();
            if( result != DialogResult.OK || _brushSelectionDlg.Selection == null )
                return;

            // add each selected brush to the palette.
            foreach( Bootstrap.Brush brush in _brushSelectionDlg.Selection )
            {
                _palette.AddBrush( brush );
            }

            // save the palette.
            Bootstrap.BrushPalette.SaveBrushPalettes();

            // repopulate the editor.
            PopulateThumbnails();

            // select the brush that was added last.
            Bootstrap.Brush selected = _brushSelectionDlg.Selected;
            if( selected != null )
                thumbnailSelector.SelectedName = selected.Name;
        }

        //-----------------------
        private void bnRemove_Click( object sender, EventArgs e )
        {
            // remove any selected brush.
            foreach( Controls.Thumbnail thumbnail in thumbnailSelector.Selection )
            {
                _palette.RemoveBrush( thumbnail.UserData );
            }

            // save the palette.
            Bootstrap.BrushPalette.SaveBrushPalettes();

            // repopulate the editor.
            PopulateThumbnails();
        }

        //-----------------------
        private void bnOpen_Click( object sender, EventArgs e )
        {
            // present the palette selector to the user.
            DialogResult result = _brushPaletteSelectionDlg.ShowDialog();
            if( result != DialogResult.OK || _brushPaletteSelectionDlg.Selection == null )
                return;

            // select the new palette.
            Palette = _brushPaletteSelectionDlg.Selection;

            // save the fact that the user selected this palette.
            Bootstrap.Settings settings = Bootstrap.Settings.Get();
            if( settings != null )
            {
                string paletteName = "";
                if( Palette != null )
                    paletteName = Palette.Name;
                settings.SetUserSetting( "selectedbrushpalette", paletteName );
                settings.Save();
            }
        }

        //-----------------------
        private void bnNew_Click( object sender, EventArgs e )
        {
            Palette = _brushPaletteSelectionDlg.NewPalette();
        }

        //-----------------------
        private void popupEditBrush_bnClose_Click( object sender, EventArgs e )
        {
            popupEditBrush.Close();
        }
    }
}
