using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace Editor
{
    class LookState
    {        
        public enum LookMode
        {
            None = 0,
            Move = 1,
            Rotate = 2,
            Pan = 4,
        };

        //=======================
        // constructor and finalizer
        //=======================

        //-----------------------
        public LookState()
        {
            _buttonsDown = 0;
            _mouseHidden = false;
        }


        //=======================
        // Attributes
        //=======================

        //-----------------------
        public MouseButtons _MouseButtons
        {
            get
            {
                return _buttonsDown;
            }

            set
            {
                _buttonsDown = value;
                CheckCursorState();
            }
        }

        //-----------------------
        public Keys _Modifiers
        {
            get
            {
                return _modifiers;
            }

            set
            {
                _modifiers = value;
                CheckCursorState();
            }
        }

        //-----------------------
        public LookMode _LookMode
        {
            get
            {
                return _lookMode;
            }
        }

        
        //=======================
        // Private Methods
        //=======================

        //-----------------------
        private void CheckCursorState()
        {
            // check the buttons.
            const MouseButtons buttonMask = MouseButtons.Right | MouseButtons.Middle | MouseButtons.Left;

            // check the modifiers keys and the mouse button states.
            bool shouldHide = ((_modifiers & Keys.Alt) != 0) && ((_buttonsDown & buttonMask) != 0);
            if (shouldHide && !_mouseHidden)
                Cursor.Hide();
            if (!shouldHide && _mouseHidden)
                Cursor.Show();
            _mouseHidden = shouldHide;

            // if the mouse is hidden, then we're in a look state.
            if (shouldHide)
            {
                MouseButtons moveButtonMask = (MouseButtons.Left | MouseButtons.Right);
                if ((_buttonsDown & moveButtonMask) == moveButtonMask || (_buttonsDown & MouseButtons.Middle) != 0)
                    _lookMode = LookMode.Pan;
                else if ((_buttonsDown & MouseButtons.Left) != 0)
                    _lookMode = LookMode.Rotate;
                else if ((_buttonsDown & MouseButtons.Right) != 0)
                    _lookMode = LookMode.Move;
            }
            else
            {
                _lookMode = LookMode.None;
            }
        }


        //=======================
        // Member Variables
        //=======================
        MouseButtons _buttonsDown;
        Keys _modifiers;
        LookMode _lookMode;
        bool _mouseHidden;
    }
}
