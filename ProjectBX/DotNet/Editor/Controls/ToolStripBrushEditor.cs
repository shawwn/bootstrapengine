using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using Syncfusion.Windows.Forms.Tools;

namespace Editor
{
    [System.ComponentModel.DesignerCategory( "code" )]
    [ToolStripItemDesignerAvailability( ToolStripItemDesignerAvailability.ToolStrip | ToolStripItemDesignerAvailability.StatusStrip )]
    public partial class ToolStripBrushEditor : ToolStripControlHost
    {
        public ToolStripBrushEditor()
            : base( CreateControlInstance() )
        {
        }

        /// <summary>
        /// Create a strongly typed property - handy to prevent casting everywhere.
        /// </summary>
        public Editors.BrushEditor EditorUI
        {
            get { return Control as Editors.BrushEditor; }
        }

        [Browsable(false)]
        public Bootstrap.Brush Brush
        {
            get { return EditorUI.Brush; }
            set { EditorUI.Brush = value; }
        }

        /// <summary>
        /// Create the actual control, note this is static so it can be called from the
        /// constructor.
        ///
        /// </summary>
        /// <returns></returns>
        private static Control CreateControlInstance()
        {
            Editors.BrushEditor editor = new Editors.BrushEditor();

            // Add other initialization code here.
            return editor;
        }

        /// <summary>
        /// Attach to events we want to re-wrap
        /// </summary>
        /// <param name="control"></param>
        protected override void OnSubscribeControlEvents( Control control )
        {
            base.OnSubscribeControlEvents( control );
        }

        /// <summary>
        /// Detach from events.
        /// </summary>
        /// <param name="control"></param>
        protected override void OnUnsubscribeControlEvents( Control control )
        {
            base.OnUnsubscribeControlEvents( control );

        }

        // set other defaults that are interesting
        protected override Size DefaultSize
        {
            get
            {
                return new Size( 276, 259 );
            }
        }
    }
}

