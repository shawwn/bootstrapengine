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
	[DefaultEvent( "ValueChanged" )]
	public partial class ToolStripTrackBar : ToolStripControlHost
	{
		public ToolStripTrackBar()
			: base( CreateControlInstance() )
		{
			AutoSize = false;
			IncreaseButtonSize = new Size( 0, 0 );
			DecreaseButtonSize = new Size( 0, 0 );
			ShowButtons = false;
		}

		/// <summary>
		/// Create a strongly typed property called TrackBarEx - handy to prevent casting everywhere.
		/// </summary>
		public TrackBarEx TrackBarEx
		{
			get
			{
				return Control as TrackBarEx;
			}
		}

		/// <summary>
		/// Create the actual control, note this is static so it can be called from the
		/// constructor.
		///
		/// </summary>
		/// <returns></returns>
		private static Control CreateControlInstance()
		{
			TrackBarEx t = new TrackBarEx();
			t.AutoSize = false;
			t.Height = 16;

			// Add other initialization code here.
			return t;
		}

		[DefaultValue( 0 )]
		public int Value
		{
			get { return TrackBarEx.Value; }
			set 
			{ 
				if( TrackBarEx.IsHandleCreated )
					TrackBarEx.Value = value; 
			}
		}

		[DefaultValue( 0 )]
		public int Minimum
		{
			get { return TrackBarEx.Minimum; }
			set { TrackBarEx.Minimum = value; }
		}

		[DefaultValue( 100 )]
		public int Maximum
		{
			get { return TrackBarEx.Maximum; }
			set { TrackBarEx.Maximum = value; }
		}

		public bool ShowButtons
		{
			get { return TrackBarEx.ShowButtons; }
			set { TrackBarEx.ShowButtons = value; }
		}

		public Size IncreaseButtonSize
		{
			get { return TrackBarEx.IncreaseButtonSize; }
			set { TrackBarEx.IncreaseButtonSize = value; }
		}

		public Size DecreaseButtonSize
		{
			get { return TrackBarEx.DecreaseButtonSize; }
			set { TrackBarEx.DecreaseButtonSize = value; }
		}

		public Orientation Orientation
		{
			get { return TrackBarEx.Orientation; }
			set { TrackBarEx.Orientation = value; }
		}


		/// <summary>
		/// Attach to events we want to re-wrap
		/// </summary>
		/// <param name="control"></param>
		protected override void OnSubscribeControlEvents( Control control )
		{
			base.OnSubscribeControlEvents( control );
			TrackBarEx trackBar = control as TrackBarEx;
			trackBar.ValueChanged += new EventHandler( trackBar_ValueChanged );
		}

		/// <summary>
		/// Detach from events.
		/// </summary>
		/// <param name="control"></param>
		protected override void OnUnsubscribeControlEvents( Control control )
		{
			base.OnUnsubscribeControlEvents( control );
			TrackBarEx trackBar = control as TrackBarEx;
			trackBar.ValueChanged -= new EventHandler( trackBar_ValueChanged );

		}


		/// <summary>
		/// Routing for event
		/// TrackBarEx.ValueChanged -> ToolStripTrackBar.ValueChanged
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		void trackBar_ValueChanged( object sender, EventArgs e )
		{
			// when the trackbar value changes, fire an event.
			if( this.ValueChanged != null )
			{
				ValueChanged( sender, e );
			}
		}

		// add an event that is subscribable from the designer.
		public event EventHandler ValueChanged;


		// set other defaults that are interesting
		protected override Size DefaultSize
		{
			get
			{
				return new Size( 200, 16 );
			}
		}

	}
}
