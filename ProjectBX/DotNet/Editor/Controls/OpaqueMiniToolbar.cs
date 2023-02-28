using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using Syncfusion.Windows.Forms.Tools;

namespace Editor.Controls
{
    public class OpaqueMiniToolbar : MiniToolBar
    {
        Color _backColor;

        public OpaqueMiniToolbar()
        {
            // match the black color scheme.
            _backColor = Color.FromArgb( 113, 113, 113 );
            this.BackColor = _backColor;
        }

        protected override void OnVisibleChanged( EventArgs e )
        {
            /* don't pass Syncfusion this event. */
        }

        protected override void OnPaintBackground( System.Windows.Forms.PaintEventArgs e )
        {
            //base.OnPaintBackground( e );
            using( Brush brush = new SolidBrush( _backColor ) )
                e.Graphics.FillRectangle( brush, e.Graphics.VisibleClipBounds );
        }
    }
}
