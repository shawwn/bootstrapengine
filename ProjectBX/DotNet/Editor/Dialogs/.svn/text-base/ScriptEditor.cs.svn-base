using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Syncfusion.Windows.Forms;

namespace Editor.Dialogs
{
    public partial class ScriptEditor : Office2007Form
    {
        Bootstrap.Expr _expr;

        public ScriptEditor(Bootstrap.Expr expr)
        {
            InitializeComponent();
            _expr = expr;
            if (_expr != null)
                tbScript.Text = _expr.Decompile();
        }

        public Bootstrap.Expr Expression
        {
            get { return _expr; }
            set
            {
                // store the expression.
                _expr = value;
                if (_expr != null)
                    tbScript.Text = _expr.Decompile();
                else
                    tbScript.Text = "";

                // make sure the error text is clear.
                tbErrors.Text = "";
            }
        }

        private void btnCompile_Click(object sender, EventArgs e)
        {
            // grab the text from the text box amd compile it.
            Bootstrap.Expr expr = new Bootstrap.Expr(tbScript.Text);

            // check for errors.
            if (expr.Errors != null)
            {
                tbErrors.Text = expr.Errors;
                return;
            }
            else
                tbErrors.Text = "";
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            // grab the text from the text box amd compile it.
            Bootstrap.Expr expr = new Bootstrap.Expr(tbScript.Text);

            // check for errors.
            if (expr.Errors != null)
            {
                tbErrors.Text = expr.Errors;
                return;
            }

            // store the new expression.
            _expr = expr;
        }
    }
}