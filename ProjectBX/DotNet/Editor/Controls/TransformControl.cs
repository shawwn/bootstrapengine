using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Editor.Controls
{
    public partial class TransformControl : UserControl
    {
        Bootstrap.SceneEntity _sceneEntity;

        //================================
        // ctor & ftor.
        //================================

        //--------------------------------
        public TransformControl()
        {
            InitializeComponent();
        }

        //================================
        // public interface.
        //================================

        public event EventHandler TranslateChanged;
        public event EventHandler RotateChanged;
        public event EventHandler ScaleChanged;

        //--------------------------------
        public bool ScaleEnable
        {
            get { return !veScale.ReadOnly; }
            set { veScale.ReadOnly = value; }
        }

        //--------------------------------
        public Bootstrap.Vector3 Position
        {
            get { return veTranslate.Vector; }
            set { veTranslate.Vector = value; }
        }

        //--------------------------------
        public Bootstrap.Vector3 Rotation
        {
            get { return veRotate.Vector; }
            set { veRotate.Vector = value; }
        }

        //--------------------------------
        public Bootstrap.Vector3 ScaleXYZ
        {
            get { return veScale.Vector; }
            set { veScale.Vector = value; }
        }

        //--------------------------------
        public void SetSceneEntity( Bootstrap.SceneEntity entity )
        {
            if ( entity != _sceneEntity )
            {
                // unregister our scene entity's transform changed handler.
                if ( _sceneEntity != null )
                {
                    _sceneEntity.TransformChanged -= new Bootstrap.SceneEntity.TransformChangedHandler( sceneEntity_TransformChanged );
                }

                // store the new scene entity.
                _sceneEntity = entity;

                if ( _sceneEntity != null )
                {
                    // be notified whenever the new scene entity's transform is changed externally.
                    _sceneEntity.TransformChanged += new Bootstrap.SceneEntity.TransformChangedHandler( sceneEntity_TransformChanged );

                    // enable the controls.
                    veTranslate.Enabled = true;
                    veRotate.Enabled = true;
                    veScale.Enabled = true;

                    // store the values of the new scene entity's transform.
                    veTranslate.Vector = _sceneEntity.Position;
                    veRotate.Vector = _sceneEntity.Rotation;
                    veScale.Vector = _sceneEntity.Scale;


                    // if the new scene entity isn't scalable, then disable the scale editor.
                    veScale.ReadOnly = !entity.Scalable;
                }
                else
                {
                    // disable the controls.
                    veTranslate.Enabled = false;
                    veRotate.Enabled = false;
                    veScale.Enabled = false;
                }
            }
        }


        //================================
        // private methods.
        //================================

        //--------------------------------
        private void sceneEntity_TransformChanged( Bootstrap.SceneEntity sender )
        {
            veTranslate.Vector = _sceneEntity.Position;
            veRotate.Vector = _sceneEntity.Rotation;
            veScale.Vector = _sceneEntity.Scale;
        }

        private void veTranslate_VectorChanged( object sender, EventArgs e )
        {
            // don't do unnecessary work.
            if ( _sceneEntity == null || _sceneEntity.Position.Equals( veTranslate.Vector ) )
                return;

            // apply the new values to the target scene entity.
            _sceneEntity.Position = veTranslate.Vector;

            // notify listeners.
            if ( TranslateChanged != null )
                TranslateChanged( this, new EventArgs() );
        }

        private void veRotate_VectorChanged( object sender, EventArgs e )
        {
            // don't do unnecessary work.
            if ( _sceneEntity == null || _sceneEntity.Rotation.Equals( veRotate.Vector ) )
                return;

            // apply the new values to the target scene entity.
            _sceneEntity.Rotation = veRotate.Vector;

            // notify listeners.
            if ( RotateChanged != null )
                RotateChanged( this, new EventArgs() );
        }

        private void veScale_VectorChanged( object sender, EventArgs e )
        {
            // don't do unnecessary work.
            if ( _sceneEntity == null || _sceneEntity.Scale.Equals( veScale.Vector ) )
                return;

            // apply the new values to the target scene entity.
            _sceneEntity.Scale = veScale.Vector;

            // notify listeners.
            if ( ScaleChanged != null )
                ScaleChanged( this, new EventArgs() );
        }
    }
}
