using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace Editor
{
    class GizmoMgr
    {
        public enum Gizmo
        {
            Select = 0,
            Move = 1,
            Rotate = 2,
            Scale = 3,
        };

        //=======================
        // constructor & finalizer
        //=======================

        //-----------------------
        public GizmoMgr( ToolStripButton move, ToolStripButton rotate,
                           ToolStripButton scale, ToolStripButton select)
        {
            _btnMove = move;
            _btnRotate = rotate;
            _btnScale = scale;
            _btnSelect = select;

            _gizmos = new Bootstrap.IGizmo[4];
            _gizmos[0] = new Bootstrap.MoveGizmo(2.0f);
            _gizmos[(int)Gizmo.Move] = new Bootstrap.MoveGizmo(1.0f);
            _gizmos[(int)Gizmo.Rotate] = new Bootstrap.RotateGizmo(1.0f);
            _gizmos[3] = new Bootstrap.ScaleGizmo(1.0f);
            _currentGizmo = _gizmos[0];
        }

        //-----------------------
        ~GizmoMgr()
        {

        }


        //=======================
        // Attributes
        //=======================

        //-----------------------
        public Bootstrap.SceneEntity UserSelection
        {
            get { return _userSelection; }
            set
            {
                _userSelection = value;
                _currentGizmo.SetEntity(_userSelection);
            }
        }

        //-----------------------
        public Bootstrap.IGizmo CurrentGizmo
        {
            get { return _currentGizmo; }
        }

        //-----------------------
        public ToolStripButton ActiveButton
        {
            set
            {
                // set the active button.
                _activeButton = value;

                // determine which control needs to be checked.
                _btnMove.Checked = _activeButton == _btnMove;
                _btnRotate.Checked = _activeButton == _btnRotate;
                _btnScale.Checked = _activeButton == _btnScale;
                _btnSelect.Checked = _activeButton == _btnSelect;
                if (_activeButton == _btnMove)
                    _activeGizmo = Gizmo.Move;
                else if (_activeButton == _btnRotate)
                    _activeGizmo = Gizmo.Rotate;
                else if (_activeButton == _btnScale)
                    _activeGizmo = Gizmo.Scale;
                else if (_activeButton == _btnSelect)
                    _activeGizmo = Gizmo.Select;
                _currentGizmo = _gizmos[(int)_activeGizmo];
                _currentGizmo.SetEntity(_userSelection);
            }
        }


        //=======================
        // Methods
        //=======================

        //-----------------------
        // returns true if the current gizmo was selected.
        public bool MouseDown(Bootstrap.Ray ray, int mouseX, int mouseY)
        {
            // set the mouse down flag to true if we're handling the current
            // event.
            _mouseDown = _currentGizmo.MouseDown(ray, mouseX, mouseY);
            return _mouseDown;
        }

        //-----------------------
        public void MouseMove(Bootstrap.Ray ray, int mouseX, int mouseY)
        {
            _currentGizmo.MouseMove( ray, mouseX, mouseY, _mouseDown );

        }

        //-----------------------
        public void MouseUp(Bootstrap.Ray ray, int mouseX, int mouseY)
        {
            if( _mouseDown )
            {
                _currentGizmo.MouseUp(ray, mouseX, mouseY);
                _mouseDown = false;
            }
        }

        //-----------------------
        public void Render()
        {
            _currentGizmo.Render();
        }

        //=======================
        // Member Variables
        //=======================
        Bootstrap.SceneEntity _userSelection;
        ToolStripButton _btnMove;
        ToolStripButton _btnRotate;
        ToolStripButton _btnScale;
        ToolStripButton _btnSelect;
        ToolStripButton _activeButton;

        Bootstrap.IGizmo[] _gizmos;
        Bootstrap.IGizmo _currentGizmo;
        Gizmo _activeGizmo;
        bool _mouseDown;
    }
}
