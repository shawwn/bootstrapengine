
/*
 * Copy and paste these functions into any control to see 
 * when each keyboard event is raised.
 * 
		protected override void OnKeyDown(KeyEventArgs e)
		{
			Console.WriteLine("OnKeyDown");
			return base.OnKeyDown( e );
		}

		protected override bool ProcessCmdKey( ref Message msg, Keys keyData )
		{
			Console.WriteLine("ProcessCmdKey");
			return base.ProcessCmdKey( ref msg, keyData );
		}

		protected override bool ProcessDialogChar( char charCode )
		{
			Console.WriteLine("ProcessDialogChar");
			return base.ProcessDialogChar( charCode );
		}

		protected override bool ProcessDialogKey( Keys keyData )
		{
			Console.WriteLine("ProcessDialogKey");
			return base.ProcessDialogKey( keyData );
		}

		protected override bool ProcessKeyEventArgs( ref Message m )
		{
			Console.WriteLine("ProcessKeyEventArgs");
			return base.ProcessKeyEventArgs( ref m );
		}

		protected override bool ProcessKeyMessage( ref Message m )
		{
			Console.WriteLine("ProcessKeyMessage");
			return base.ProcessKeyMessage( ref m );
		}

		protected override bool ProcessKeyPreview( ref Message m )
		{
			Console.WriteLine("ProcessKeyPreview");
			return base.ProcessKeyPreview( ref m );
		}

		protected override bool ProcessMnemonic( char charCode )
		{
			Console.WriteLine("ProcessMnemonic");
			return base.ProcessMnemonic( charCode );
		}

		public override bool PreProcessMessage( ref Message msg )
		{
			Console.WriteLine("PreProcessMessage");
			return base.PreProcessMessage( ref msg );
		}

		protected override bool ProcessTabKey( bool forward )
		{
			Console.WriteLine("ProcessTabKey");
			return base.ProcessTabKey( forward );
		}

		protected override bool IsInputKey( Keys keyData )
		{
			// if the key is A-Z, it's an input key.
			if( keyData.CompareTo( Keys.A ) >= 0 && keyData.CompareTo( Keys.Z ) <= 0 )
				return true;

			Console.WriteLine("IsInputKey");
			return base.IsInputKey( keyData );
		}

		protected override void OnKeyPress( KeyPressEventArgs e )
		{
			Console.WriteLine("OnKeyPress");
			base.OnKeyPress( e );
		}

		protected override void OnKeyUp( KeyEventArgs e )
		{
			Console.WriteLine("OnKeyUp");
			base.OnKeyUp( e );
		}

		protected override void OnPreviewKeyDown( PreviewKeyDownEventArgs e )
		{
			Console.WriteLine("OnPreviewKeyDown");
			base.OnPreviewKeyDown( e );
		}
 */



/*
 * Dialog Codes
#define DLGC_WANTARROWS     0x0001      // Control wants arrow keys         
#define DLGC_WANTTAB        0x0002      // Control wants tab keys           
#define DLGC_WANTALLKEYS    0x0004      // Control wants all keys           
#define DLGC_WANTMESSAGE    0x0004      // Pass message to control          
#define DLGC_HASSETSEL      0x0008      // Understands EM_SETSEL message    
#define DLGC_DEFPUSHBUTTON  0x0010      // Default pushbutton               
#define DLGC_UNDEFPUSHBUTTON 0x0020     // Non-default pushbutton           
#define DLGC_RADIOBUTTON    0x0040      // Radio button                     
#define DLGC_WANTCHARS      0x0080      // Want WM_CHAR messages            
#define DLGC_STATIC         0x0100      // Static item: don't include       
#define DLGC_BUTTON         0x2000      // Button item: can be checked      
 */
