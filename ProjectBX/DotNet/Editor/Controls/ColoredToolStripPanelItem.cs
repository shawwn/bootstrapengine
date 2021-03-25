using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms.Design;

namespace Editor.Controls
{
	[ToolStripItemDesignerAvailability( ToolStripItemDesignerAvailability.ToolStrip | ToolStripItemDesignerAvailability.StatusStrip )]
	public class ColoredToolStripPanelItem : Syncfusion.Windows.Forms.Tools.ToolStripPanelItem
	{
		protected override void OnPaint( System.Windows.Forms.PaintEventArgs e )
		{
			using( Brush brush = new SolidBrush( this.BackColor ) )
				e.Graphics.FillRectangle( brush, e.Graphics.VisibleClipBounds );
		}
	}
}
