using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Editor
{
    class DialogMgr
    {
        [Flags]
        public enum Dialog
        {
            // creation dialogs.
            CreateModel,
            CreateHeightMap,
            CreateLight,

            // manipulate dialogs.
            EditLight,
            EditModel,
            EditHeightMap,

            // paint dialogs.
            SelectBrush,

            // shade dialogs.
            SelectMaterial,

            // total number of dialogs.
            Count,
        }

        //=======================
        // Member Variables
        //=======================
        Viewport.UserMode _userMode;    // viewport look state.
        int _dialogsVisible;
        int[] _dialogsActive;
        Form[] _dialogs;
        Form _parent;

        //=======================
        // constructor & finalizer
        //=======================

   		//-----------------------
		public DialogMgr(Form parent)
		{
            _parent = parent;

            _dialogsActive = new int[(int)Viewport.UserMode.Count];
            _dialogsActive[(int)Viewport.UserMode.Create] |= 1 << (int)Dialog.CreateModel;
            _dialogsActive[(int)Viewport.UserMode.Create] |= 1 << (int)Dialog.CreateHeightMap;
            _dialogsActive[(int)Viewport.UserMode.Create] |= 1 << (int)Dialog.CreateLight;

            _dialogsActive[(int)Viewport.UserMode.Manipulate] |= 1 << (int)Dialog.EditLight;
            _dialogsActive[(int)Viewport.UserMode.Manipulate] |= 1 << (int)Dialog.EditHeightMap;
            _dialogsActive[(int)Viewport.UserMode.Manipulate] |= 1 << (int)Dialog.EditModel;

            _dialogsActive[(int)Viewport.UserMode.Paint] |= 1 << (int)Dialog.SelectBrush;

            _dialogsActive[(int)Viewport.UserMode.Material] |= 1 << (int)Dialog.SelectMaterial;

            _dialogsActive[(int)Viewport.UserMode.Animate] = 0;

            _dialogs = new Form[(int)Dialog.Count];
        }


        //=======================
        // Attributes
        //=======================

        //-----------------------
        public Viewport.UserMode UserMode
        {
            get { return _userMode; }
            set { _userMode = value; AdjustDialogs(); }
        }


        //=======================
        // Functions
        //=======================

        //-----------------------
        public void SetDialog(Dialog dialog, Form form)
        {
            // only set the dialog if it's not initialized.
            if (_dialogs[(int)dialog] == null)
                _dialogs[(int)dialog] = form;
        }

        //-----------------------
        public Form GetDialog(Dialog dialog)
        {
            return _dialogs[(int)dialog];
        }

        //-----------------------
        public void ShowDialog(Dialog dialog)
        {
            // check to see if the dialog is already visible.
            int flag = (1 << (int)dialog);
            if ((_dialogsVisible & flag) != 0)
            {
                if (_dialogs[(int)dialog] != null)
                    _dialogs[(int)dialog].Activate();
                return;
            }

            // the dialog should be visible, but isn't.
            _dialogsVisible |= flag;
            int active = _dialogsVisible & _dialogsActive[(int)_userMode];
            if (active != 0)
            {
                // show the dialog.
                ShowDialogInternal(dialog, true);
            }
        }

        //-----------------------
        private void AdjustDialogs()
        {
            // iterate through the dialogs.
            int mask = _dialogsActive[(int)_userMode];
            for (int idx = 0; idx < (int)Dialog.Count; ++idx)
            {
                int flag = _dialogsVisible & (1 << idx);
                flag &= mask;
                if (flag > 0)
                {
                    // show the dialog.
                    ShowDialogInternal((Dialog)idx, false);
                }
                else
                {
                    HideDialogInternal((Dialog)idx);
                }
            }
            _parent.Activate();
        }

        //-----------------------
        private void ShowDialogInternal(Dialog dialog, bool focus)
        {
            // show the dialog.
            Form form = _dialogs[(int)dialog];
            if (form == null)
                return;

            // show the dialog.
            if (!focus)
                ShowWindow(form.Handle, (short)ShowWindowFlag.SW_SHOWNOACTIVATE);
            else
                form.Show();

            // hook into the dialog's hide procedure.
            form.VisibleChanged += new EventHandler(OnDialogClose);
        }

        //-----------------------
        private void HideDialogInternal(Dialog dialog)
        {
            // hide the dialog.
            Form form = _dialogs[(int)dialog];
            if (form == null)
                return;
            if (!form.Visible)
                return;

            // hook into the dialog's hide procedure.
            form.VisibleChanged -= new EventHandler(OnDialogClose);

            // show the dialog.
            form.Hide();
            form.SendToBack();
        }

        //-----------------------
        private void OnDialogClose(Object sender, EventArgs e)
        {
            // get the sending form.
            Form sendingForm = sender as Form;
            if (sendingForm == null)
                return;

            // track the closed state.
            int dialogIdx = 0;
            foreach (Form form in _dialogs)
            {
                // if we found the sending form, make sure we mark it as closed.
                if (form == sendingForm)
                    break;
                ++dialogIdx;
            }

            // make sure we found the dialog.
            if (dialogIdx >= (int)Dialog.Count)
                return;

            // now mark the dialog as closed, but only if we're not the one
            // closing it.
            int flag = 1 << dialogIdx;
            int mask = _dialogsActive[(int)_userMode];
            if ((flag & mask) > 0)
                _dialogsVisible &= ~flag;

            // remove ourselves from the event handler.
            sendingForm.VisibleChanged -= new EventHandler(OnDialogClose);
        }

        // Win32 imports.
        [DllImport("User32.dll")]
        public extern static int ShowWindow(System.IntPtr hWnd, short cmdShow);

        public enum ShowWindowFlag
        {
            SW_HIDE = 0,
            SW_SHOWNORMAL = 1,
            SW_NORMAL = 1,
            SW_SHOWMINIMIZED = 2,
            SW_SHOWMAXIMIZED = 3,
            SW_MAXIMIZE = 3,
            SW_SHOWNOACTIVATE = 4,
            SW_SHOW = 5,
            SW_MINIMIZE = 6,
            SW_SHOWMINNOACTIVE = 7,
            SW_SHOWNA = 8,
            SW_RESTORE = 9,
            SW_SHOWDEFAULT = 10,
            SW_FORCEMINIMIZE = 11,
            SW_MAX = 11
        }


    }
}
