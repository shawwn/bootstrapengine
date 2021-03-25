using System;
using System.Collections.Generic;
using System.Windows.Forms;
using Bootstrap;

namespace Editor
{
	static class Program
	{
		public static Editor.OutputWindow Output;
		public static UberPainter _Painter;

        public static Engine GameEngine;
        public static Camera GameCamera;

        public static UberPainter Painter
		{
			get { return _Painter; }
		}

		// managers.
        public static Utils.ThumbnailMgr ThumbnailMgr;

		[STAThread]
		static void Main()
		{
            // start the module and check the engine.
            Bootstrap.Module.Startup();

			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault( false );

			// initialize the managers.
            ThumbnailMgr = new Utils.ThumbnailMgr(64, 64);

			// create the editor's windows.
			Output = new Editor.OutputWindow();
			Viewport viewport = new Viewport();

			// show the editor's windows.
			Output.Show();
			viewport.Show();

            // ask the user to choose the working folder.
            Settings settings = Settings.Get();
            if (settings.GetWorkingFolder() == "" || settings.GetBrushFolder() == "")
            {
				Output.AddLine( "Choose the game's working folder.  (Its working folder should contain a 'data' directory.)" );
                Dialogs.Settings settingsDlg = new Dialogs.Settings("Choose working folders...");
                DialogResult result = settingsDlg.ShowDialog();
				if( result != DialogResult.OK )
				{
					Application.Exit();
					return;
				}
				Output.AddLine( "You chose " + settingsDlg.WorkingFolder );
            }

            Engine engine = null;
			while( engine == null )
			{
	            try
	            {
	                // try to start up the engine.
	                engine = Engine.Startup( viewport._View, OutputEditorMessage, null );
	            }
	            catch( System.Exception )
	            {
	                // show the working folder dialog.
					Output.AddLine( "Engine initialization failed.  Please reset your working directory." );
	                Dialogs.Settings settingsDlg = new Dialogs.Settings( "Choose a different project..." );
	                DialogResult result = settingsDlg.ShowDialog();
					if( result != DialogResult.OK )
					{
						Application.Exit();
						return;
					}
	            }
			}
			Output.AddLine( "Session start." );

			// notify the viewport that the engine has been initialized.
			viewport.AfterEngineInitialized();

			// create the game camera and init its position, FOV, and aspect ratio.
            Camera camera			= new Camera();
			camera.Position			= new Vector3( 0.0f, 7.0f, 0.0f );
			Projection proj			= camera.Projection;
			proj.FOV				= Util.DegToRad( 60.0f );
			proj.AspectRatio		= viewport._View.Width / (float)viewport._View.Height;
			camera.Projection		= proj;

			GameEngine = engine;
			GameCamera = camera;
			_Painter = new UberPainter();

			// set the background 'clear color'.
            engine.Scene.ClearColor = new Bootstrap.Color(viewport.BackColor.R / 512.0f, viewport.BackColor.G / 512.0f, viewport.BackColor.B / 512.0f, 1);

			while( !viewport._Done )
			{
#if !DEBUG 
				if( Form.ActiveForm != viewport )
					System.Threading.Thread.Sleep( 50 );
#endif

                // windows message pump.
				Application.DoEvents();

                // only update if the viewport is visible.
                if (!viewport.Visible || viewport._View.Width == 0 || viewport._View.Height == 0)
                    continue;

                // if the engine isn't ready, don't update it.
                if (engine == null || !engine.IsReady())
                    continue;

				// update the engine.
				engine.Update();

				// update the viewport.
				viewport.EditorUpdate();

				// render the scene.
				viewport.EditorRender();
			}
		}

		static void OutputEditorMessage( string text )
		{
			Output.AddEngineMessage( text );
		}
	}
}
