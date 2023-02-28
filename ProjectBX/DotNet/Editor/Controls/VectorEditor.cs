using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Editor.Controls
{
    [DefaultEvent("VectorChanged")]
    public partial class VectorEditor : UserControl
    {
        Bootstrap.Vector3   _vector;
        bool                _populating;
        bool                _readOnly;
        int                 _decimalPlaces;
        decimal             _increment;
        decimal             _minimum;
        decimal             _maximum;

        //================================
        // ctor & ftor.
        //================================

        //--------------------------------
        public VectorEditor()
        {
            InitializeComponent();

            // initialize our state with the defaults from one of our value editor controls.
            _readOnly = numVecX.ReadOnly;
            _decimalPlaces = numVecX.DecimalPlaces;
            _increment = numVecX.Increment;
            _minimum = numVecX.Minimum;
            _maximum = numVecX.Maximum;
        }

        //================================
        // public interface.
        //================================

        public event EventHandler VectorChanged;

        //--------------------------------
        [Category("Appearance")]
        public Bootstrap.Vector3 Vector
        {
            get { return _vector; }
            set 
            { 
                if ( !_vector.Equals( value ) )
                {
                    // store the new vector.
                    _vector = value;
                    UpdateControls();

                    // fire off the 'vector changed' event.
                    RaiseVectorChangedEvent();
                }
            }
        }

        //--------------------------------
        [Category("Behavior")]
        public bool ReadOnly
        {
            get { return _readOnly; }
            set
            {
                if ( value != _readOnly )
                {
                    _readOnly = value;
                    numVecX.ReadOnly = value;
                    numVecY.ReadOnly = value;
                    numVecZ.ReadOnly = value;
                }
            }
        }

        //--------------------------------
        [Category("Data")]
        public int DecimalPlaces
        {
            get { return _decimalPlaces; }
            set
            {
                if ( value != _decimalPlaces )
                {
                    _decimalPlaces = value;
                    numVecX.DecimalPlaces = value;
                    numVecY.DecimalPlaces = value;
                    numVecZ.DecimalPlaces = value;
                }
            }
        }

        //--------------------------------
        [Category("Data")]
        public decimal Increment
        {
            get { return _increment; }
            set
            {
                if ( value != _increment )
                {
                    _increment = value;
                    numVecX.Increment = value;
                    numVecY.Increment = value;
                    numVecZ.Increment = value;
                }
            }
        }

        //--------------------------------
        [Category("Data")]
        public decimal Minimum
        {
            get { return _minimum; }
            set
            {
                if ( value != _minimum )
                {
                    _minimum = value;
                    numVecX.Minimum = value;
                    numVecY.Minimum = value;
                    numVecZ.Minimum = value;
                }
            }
        }

        //--------------------------------
        [Category("Data")]
        public decimal Maximum
        {
            get { return _maximum; }
            set
            {
                if ( value != _maximum )
                {
                    _maximum = value;
                    numVecX.Maximum = value;
                    numVecY.Maximum = value;
                    numVecZ.Maximum = value;
                }
            }
        }

        //================================
        // private methods.
        //================================

        //--------------------------------
        void UpdateControls()
        {
            _populating = true;

            numVecX.Value = (decimal)_vector.X;
            numVecY.Value = (decimal)_vector.Y;
            numVecZ.Value = (decimal)_vector.Z;

            _populating = false;
        }

        //--------------------------------
        void RaiseVectorChangedEvent()
        {
            if ( VectorChanged != null )
                VectorChanged( this, new EventArgs() );
        }

        //================================
        // events.
        //================================

        //--------------------------------
        private void numVec_ValueChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            // get the vector values.
            _vector.X = (float)numVecX.Value;
            _vector.Y = (float)numVecY.Value;
            _vector.Z = (float)numVecZ.Value;

            // fire off the 'vector changed' event.
            RaiseVectorChangedEvent();
        }

        //--------------------------------
        private void numVec_Enter( object sender, EventArgs e )
        {
            // get the editor that just became active.
            Syncfusion.Windows.Forms.Tools.NumericUpDownExt numVec = (Syncfusion.Windows.Forms.Tools.NumericUpDownExt)sender;

            // select all of the editor's text, so that the user can enter
            // a new value immediately.
            numVec.Select( 0, numVec.MaxLength );
        }
    }
}
