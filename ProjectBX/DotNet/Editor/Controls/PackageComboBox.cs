//----------------------------------------------------------
// File:        EdUberTexBuilder.cpp
// Author:      Chris Harvey
// Created:     6-24-08
//
// Purpose:     To select an engine package.
//
// Copyright À© 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Editor.Controls
{
    public class PackageComboBox : ComboBox
    {
        // member variables.
        List< string > _extraPackages = new List< string >();

        public PackageComboBox()
        {
            // do not allow the user to directly edit the package name.
            this.DropDownStyle = ComboBoxStyle.DropDownList;
        }

        public string DefaultSelectedPackage = "";

        public bool AddPackage( string package )
        {
            // do not add the package unless it contains a 'textures' folder.
            string packageTextureFolder = Bootstrap.Settings.Get().GetWorkingFolder() + "\\data\\" + package + "\\textures";
            if( !Directory.Exists( packageTextureFolder ) )
                return false;

            // add the package.
            this.Items.Add( package.ToLower() );
            _extraPackages.Add( package.ToLower() );
            return true;
        }

        void Populate()
        {
            // iterate through all folders under data.
            string packageFolder = Bootstrap.Settings.Get().GetPackageFolder();
            DirectoryInfo dir = new DirectoryInfo(packageFolder);

            // each folder with a 'textures' subfolder represents a package.
            this.Items.Clear();
            DirectoryInfo[] categories = dir.GetDirectories();
            foreach (DirectoryInfo package in categories)
            {
                string texturePath = package.FullName + "\\textures";
                if( Directory.Exists( texturePath ) )
                {
                    switch( package.Name.ToLower() )
                    {
                            // skip the following special directories.
                        case "system":
                        case "editor":
                            break;

                        default:
                            this.Items.Add( package.Name );
                            break;
                    }
                }
            }

            // add any extra packages.
            foreach( string package in _extraPackages )
                this.Items.Add( package );

            if( this.Items.Count > 0 )
            {
                // set the default selected package.  If there isn't an 
                // explicit default, set selected package to be the 
                // active scene.
                if( DefaultSelectedPackage.Length > 0 )
                    this.SelectedItem = DefaultSelectedPackage;
                else if( Program.GameEngine != null && Program.GameEngine.Scene != null )
                    this.SelectedItem = Program.GameEngine.Scene.ShortName;
                else
                    this.SelectedItem = this.Items[0];
            }
        }

        protected override void OnHandleCreated( EventArgs e )
        {
            base.OnHandleCreated( e );
            if( DesignMode || _initialized  )
                return;
            _initialized = true;

            Populate();
        }
        bool _initialized;

        protected override void OnDropDownStyleChanged( EventArgs e )
        {
            // do not allow the dropdown style to change away from "DropDownList".
        }

        protected override void OnSelectedIndexChanged( EventArgs e )
        {
            // if there are packages, do not allow "no selection".
            if( this.SelectedIndex < 0 && this.Items.Count > 0 )
            {
                this.SelectedIndex = 0;
            }
            else
            {
                // refresh the editor before raising the "SelectedIndexChanged" event.
                // (The texture selector takes awhile to generate all texture thumbnails for a package.)
                Refresh();
                base.OnSelectedIndexChanged( e );
            }
        }

        protected override void WndProc( ref Message m )
        {
            // do not handle mouse wheel messages.
            switch( m.Msg )
            {
                case 0x020A: // WM_MOUSEWHEEL
                    return;
            }

            base.WndProc( ref m );
        }
    }
}
