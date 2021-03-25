using System;
using System.Collections.Generic;
using System.Text;

namespace Editor
{
    class SceneManager
    {
        string activeScene = "";

        //=======================
        // constructor & finalizer
        //=======================

        //-----------------------
        public SceneManager()
        {

        }

        //-----------------------
        ~SceneManager()
        {

        }


        //=======================
        // public methods
        //=======================

        //-----------------------
        public void Load( string fileName )
        {
            // store the file name.
            activeScene = fileName;
            if (fileName == null)
            {
                Close();
                return;
            }

            // if the file name specified was valid, then try to load it.
            Program.GameEngine.Scene.Load( fileName );
			Program.Output.AddLine( "Scene " + fileName + " loaded." );
        }

        //-----------------------
        public void Save()
        {
            // simply return if no scene is loaded.
            if (activeScene == "")
                return;

            // save the current scene.
            Program.GameEngine.Scene.Save();
			Program.Output.AddLine( "Scene " + activeScene + " saved." );
        }

        //-----------------------
        public void Close()
        {
			if( activeScene != "" )
			{
	            activeScene = "";
	            Program.GameEngine.Scene.Clear();
				Program.Output.AddLine( "Scene " + activeScene + " closed." );
			}
        }
    };
}
