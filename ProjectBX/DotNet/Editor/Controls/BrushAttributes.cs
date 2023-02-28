using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Editor.Controls
{
    public partial class BrushAttributes : UserControl
    {
        readonly float kBrushSizeMin = 0.01f; // meters
        readonly float kBrushSizeMax = 10.0f; // meters
        float _brushSize;

        public BrushAttributes()
        {
            InitializeComponent();
            tbBrushSize.MinValue = kBrushSizeMin;
            tbBrushSize.MaxValue = kBrushSizeMax;
            tbBrushSize.AllowMaxOverride = true;
            tbBrushSize.AllowMinOverride = false;
            tbBrushSize.Value = 1.0f;
            _brushSize = 1.0f;
            tbBrushSize.ValueChanged += new EventHandler( tbBrushSize_ValueChanged );
        }

        public event EventHandler BrushSizeChanged;

        public float BrushSize
        {
            get { return _brushSize; }
            set
            {
                if( _brushSize == value )
                    return;
                _brushSize = value;
                tbBrushSize.Value = value;
            }
        }

        public void AddBrushSizeUIDelta( int delta )
        {
            tbBrushSize.Value += delta;
        }

        void tbBrushSize_ValueChanged( object sender, EventArgs e )
        {
            _brushSize = tbBrushSize.Value;
            if( BrushSizeChanged != null )
                BrushSizeChanged( this, new EventArgs() );
        }
    }
}
