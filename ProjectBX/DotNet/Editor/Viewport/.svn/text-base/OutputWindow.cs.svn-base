using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Syncfusion.Windows.Forms;
using System.Runtime.InteropServices;

namespace Editor
{
	public partial class OutputWindow : Office2007Form
	{
		// SetScrollPos
		[DllImport( "user32.dll" )]
		static extern int SetScrollPos( IntPtr hWnd, int nBar,
									   int nPos, bool bRedraw );

		// GetScrollPos
		[DllImport( "user32.dll" )]
		static extern int GetScrollPos( IntPtr hWnd, int nBar );

		// SendMessage
		[DllImport( "user32.dll" )]
		static extern int SendMessage( IntPtr hWnd, int wMsg,
									   int wParam, int lParam );

		const int EM_LINESCROLL = 0x00B6;

		public OutputWindow()
		{
			InitializeComponent();

			// clear the default output text.
			tbOutput.Text = "";
		}

		public void Add( string text )
		{
			// if the text is empty, don't do anything.
			if( text == "" )
				return;

			// replace \r\n with \n, then \n with \r\n.  
			// (This replaces any lone \n's with \r\n's, without affecting any \r\n's.)
			text = text.Replace( "\r\n", "\n" ).Replace( "\n", "\r\n" );

			// pump any engine messages.
			Bootstrap.Engine.PumpOutputMessages();

			// add a timestamp.
			text = Timestamp + ": " + text;

			// add the text.
			WriteText( text );
		}

		public void AddLine( string line )
		{
			// if the line is empty, don't do anything.
			if( line == "" )
				return;

			Add( line + "\n" );
		}

		internal void AddEngineMessage( string message )
		{
			// if the message is empty, don't do anything.
			if( message == "" )
				return;

			// add each line in the message.
			foreach( string line in message.Split( new char[] { '\n' } ) )
			{
				// append a timestamp and add the line.
				// TODO: figure out why this is so slow!
				//WriteText( Timestamp + ": " + line + "\r\n" );
			}
		}

		protected override void OnFormClosing( FormClosingEventArgs e )
		{
			e.Cancel = true;
		}

		void WriteText( string text )
		{
			// update the textbox.
			tbOutput.AppendText( text );
		}

		string Timestamp
		{
			get
			{
				// get a timestamp like "8/4/2008 10:35:52 PM"
				string timestamp = DateTime.Now.ToString();

				// erase the date, so that we are left with "10:35:52 PM"
				if( timestamp.Contains( " " ) )
					timestamp = timestamp.Substring( timestamp.IndexOf( ' ' )+1 );

				// return the formatted timestamp.
				return timestamp;
			}
		}

		private void tbConsole_KeyDown( object sender, KeyEventArgs e )
		{
			if( e.KeyCode == Keys.Enter )
			{
				btnSubmit.PerformClick();
				e.Handled = true;
			}
		}

		private void btnSubmit_Click( object sender, EventArgs e )
		{
            // get the text from the console and make sure it doesn't
            // have a newline or white space prepended to the front.
            int strStart = 0;
            string text = tbConsole.Text;
            for ( int i = 0; i < text.Length; ++i )
            {
                if ( text[ i ] == ' ' || text[ i ] == '\t' || text[ i ] == '\n' ||
                     text[ i ] == '\r' )
                {
                    ++strStart;
                }
                else
                {
                    break;
                }
            }

            if ( strStart < text.Length )
            {
                // get only the text we're interested in.  This makes
                // sure that there are no leading whitespace
                // characters.
                text = text.Substring( strStart );

                // echo the text back to the console.
                WriteText( text + "\n\r" );

                // handle the console message.
                Program.GameEngine.ProcessConsoleMessage( tbConsole.Text );

                // clear the text box.
                tbConsole.Text = "";
            }
		}
	}
}