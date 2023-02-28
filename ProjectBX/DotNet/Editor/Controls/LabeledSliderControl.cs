using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Editor.Controls
{
    [DefaultEvent("ValueChanged")]
    public partial class LabeledSliderControl : UserControl
    {
        float _value;
        float _minValue;
        float _maxValue;
        bool _allowMaxOverride;
        bool _allowMinOverride;
        bool _skipControlUpdate;

        //================================
        // ctor & ftor.
        //================================

        //--------------------------------
        public LabeledSliderControl()
        {
            InitializeComponent();

            _value = 0.0f;
            _minValue = 0.0f;
            _maxValue = 1.0f;
            _allowMaxOverride = false;
            _skipControlUpdate = false;
        }

        //================================
        // public interface.
        //================================

        //--------------------------------
        public event EventHandler ValueChanged;

        //--------------------------------
        public string Label
        {
            get { return lbLabel.Text; }
            set { lbLabel.Text = value; }
        }

        //--------------------------------
        public float Value
        {
            get { return _value; }
            set
            {
                _skipControlUpdate = true;
                _value = value;
                tbValue.Text = _value.ToString();
                CalcSliderValue();
                _skipControlUpdate = false;
            }
        }

        //--------------------------------
        public float MinValue
        {
            get { return _minValue; }
            set { _minValue = value; }
        }

        //--------------------------------
        public float MaxValue
        {
            get { return _maxValue; }
            set { _maxValue = value; }
        }

        //--------------------------------
        public bool AllowMaxOverride
        {
            get { return _allowMaxOverride; }
            set { _allowMaxOverride = value; }
        }

        //--------------------------------
        public bool AllowMinOverride
        {
            get { return _allowMinOverride; }
            set { _allowMinOverride = value; }
        }

        //--------------------------------
        public int SliderResolution
        {
            get { return tbSlider.Maximum - tbSlider.Minimum; }
            set { tbSlider.Maximum = tbSlider.Minimum + value; }
        }

        //================================
        // private methods.
        //================================

        //--------------------------------
        private void CalcSliderValue()
        {
            // check to see if we should clamp the value.
            if (!_allowMaxOverride && _value > _maxValue)
                _value = _maxValue;
            if (!_allowMinOverride && _value < _minValue)
                _value = _minValue;

            // calculate the interpolant between the min and max values on
            // the slider.
            float t = (_value - _minValue) / (_maxValue - _minValue);
            if (t > 1.0f)
                t = 1.0f;
            else if (t < 0.0f)
                t = 0.0f;

            // update the slider.
            tbSlider.Value = (int)(tbSlider.Minimum + t * (tbSlider.Maximum - tbSlider.Minimum - 1));
        }

        //--------------------------------
        private void tbValue_TextChanged(object sender, EventArgs e)
        {
            if (_skipControlUpdate)
                return;

            _skipControlUpdate = true;
            try
            {
                // get the value from the text box.
                _value = Convert.ToSingle(tbValue.Text);

                // calculate the slider's value.
                CalcSliderValue();

                // call the value changed event.
                ValueChanged(this, new EventArgs());
            }
            catch (SystemException)
            {

            }
            _skipControlUpdate = false;
        }

        //--------------------------------
        private void tbSlider_Scroll(object sender, EventArgs e)
        {
            // if we're updating the control, then simply return.
            if (_skipControlUpdate)
                return;

            _skipControlUpdate = true;
            try
            {
                // get the value from the slider.
                float t = (tbSlider.Value - tbSlider.Minimum) / (float)(tbSlider.Maximum - tbSlider.Minimum);
                _value = _minValue + t * (_maxValue - _minValue);

                // update the text box.
                tbValue.Text = _value.ToString();

                // call the value changed event.
                ValueChanged(this, new EventArgs());
            }
            catch (SystemException)
            {

            }
            _skipControlUpdate = false;
        }
    }
}
