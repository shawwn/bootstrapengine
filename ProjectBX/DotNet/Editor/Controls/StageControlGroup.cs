using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using Syncfusion.Windows.Forms;

namespace Editor.Controls
{
    public partial class StageControlGroup : UserControl
    {
        Bootstrap.MaterialPass _materialPass;
        float _bumpScale;
        float _bumpOffset;
        bool _skipBumpUpdate;

        const float kBumpScaleMin = -0.025f;
        const float kBumpScaleMax = 0.025f;
        const float kBumpOffsetMin = -1.0f;
        const float kBumpOffsetMax = 1.0f;

        public StageControlGroup()
        {
            InitializeComponent();
            _skipBumpUpdate = false;
            _bumpScale = 0.0f;
            _bumpOffset = 0.0f;
        }

        ~StageControlGroup()
        {

        }

        public void SetPass( Bootstrap.MaterialPass pass )
        {
            _materialPass = pass;
            UpdateControls();
        }

        public Bootstrap.MaterialPass GetPass()
        {
            return _materialPass;
        }

        private void UpdateControls()
        {
            // setup checkboxes.
            cbAlphaTestEnable.Checked = (_materialPass.RenderStates & Bootstrap.MaterialPass.RenderState.AlphaTest) != 0;
            cbDepthEnable.Checked = (_materialPass.RenderStates & Bootstrap.MaterialPass.RenderState.DepthTest) != 0;
            cbCullingEnable.Checked = (_materialPass.RenderStates & Bootstrap.MaterialPass.RenderState.CullFace) != 0;
            cbBlendEnable.Checked = (_materialPass.RenderStates & Bootstrap.MaterialPass.RenderState.Blend) != 0;
            cbDepthBiasEnable.Checked = (_materialPass.RenderStates & Bootstrap.MaterialPass.RenderState.DepthBias) != 0;

            // set the alpha test reference value.
            tbAlphaTestRef.Text = _materialPass.AlphaTestRef.Value.ToString();

            // set the culling mode.
            if (_materialPass.CullFace == Bootstrap.MaterialPass.Face.Back)
                cbCullMode.Text = "Back";
            else if (_materialPass.CullFace == Bootstrap.MaterialPass.Face.Front)
                cbCullMode.Text = "Front";

            // set the alpha test mode.
            if (_materialPass.AlphaTestMode == Bootstrap.MaterialPass.AlphaTest.Greater)
                cbAlphaTest.Text = "Greater";
            else if (_materialPass.AlphaTestMode == Bootstrap.MaterialPass.AlphaTest.GreaterEqual)
                cbAlphaTest.Text = "GreaterEqual";
            else if (_materialPass.AlphaTestMode == Bootstrap.MaterialPass.AlphaTest.Less)
                cbAlphaTest.Text = "Less";
            else if (_materialPass.AlphaTestMode == Bootstrap.MaterialPass.AlphaTest.LessEqual)
                cbAlphaTest.Text = "LessEqual";
            else if (_materialPass.AlphaTestMode == Bootstrap.MaterialPass.AlphaTest.Equal)
                cbAlphaTest.Text = "Equal";
            else if (_materialPass.AlphaTestMode == Bootstrap.MaterialPass.AlphaTest.NotEqual)
                cbAlphaTest.Text = "NotEqual";

            // set the alpha blend mode.
            if (_materialPass.AmbientSrcColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.One &&
                _materialPass.AmbientSrcAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.One &&
                _materialPass.AmbientDstColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.Zero &&
                _materialPass.AmbientDstAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.Zero &&
                _materialPass.LightSrcColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.One &&
                _materialPass.LightSrcAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.Zero &&
                _materialPass.LightDstColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.One &&
                _materialPass.LightDstAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.One)
            {
                cbBlendMode.Text = "Accumulate";
            }
            else if (_materialPass.AmbientSrcColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.One &&
                _materialPass.AmbientSrcAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.One &&
                _materialPass.AmbientDstColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.Zero &&
                _materialPass.AmbientDstAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.Zero &&
                _materialPass.LightSrcColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.OneMinusDstAlpha &&
                _materialPass.LightSrcAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.Zero &&
                _materialPass.LightDstColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.One &&
                _materialPass.LightDstAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.One)
            {
                cbBlendMode.Text = "Blend Base";
            }
            else if (_materialPass.AmbientSrcColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.SrcAlpha &&
                _materialPass.AmbientSrcAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.One &&
                _materialPass.AmbientDstColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.OneMinusSrcAlpha &&
                _materialPass.AmbientDstAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.Zero &&
                _materialPass.LightSrcColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.SrcAlpha &&
                _materialPass.LightSrcAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.Zero &&
                _materialPass.LightDstColorBlendFactor == Bootstrap.MaterialPass.BlendFactor.One &&
                _materialPass.LightDstAlphaBlendFactor == Bootstrap.MaterialPass.BlendFactor.One)
            {
                cbBlendMode.Text = "Blend Decal";
            }
            else
            {
                cbBlendMode.Text = "Advanced...";
            }

            // update bump controls.
            _skipBumpUpdate = true;
            {
                // calculate the local bump scale.
                float scale = _materialPass.BumpStage.TexColorMod.R;

                // calculate the local scale offset.
                float offset = 0.0f;
                if (scale > 0.0001f)
                    offset = _materialPass.BumpStage.TexColorAdd.R / scale;

                // calculate scroll bar positions.
                float scaleT = (scale - kBumpScaleMin) / (kBumpScaleMax - kBumpScaleMin);
                float offsetT = (offset - kBumpOffsetMin) / (kBumpOffsetMax - kBumpOffsetMin);
                if (scaleT > 1.0f)
                    scaleT = 1.0f;
                else if (scaleT < 0.0f)
                    scaleT = 0.0f;

                // store the value on the bump controls.
                tbBumpScale.Text = scale.ToString();
                tbarBumpScale.Value = tbarBumpScale.Minimum + (int)(scaleT * (tbarBumpScale.Maximum - tbarBumpScale.Minimum - 1));
                tbBumpOffset.Text = offset.ToString();
                tbarBumpOffset.Value = tbarBumpOffset.Minimum + (int)(offsetT * (tbarBumpOffset.Maximum - tbarBumpOffset.Minimum - 1));
            }
            _skipBumpUpdate = false;

            // make sure all tint previews are valid.
            UpdateTintPreviews();
            UpdateHighlightPreviews();
        }

        private void UpdateTintPreviews()
        {
            Util.Drawing.FillPictureBox(pbDiffuseTint, _materialPass.DiffuseStage.TexColorMod);
            Util.Drawing.FillPictureBox(pbSpecularTint, _materialPass.SpecularStage.TexColorMod);
            Util.Drawing.FillPictureBox(pbEmissiveTint, _materialPass.EmissiveStage.TexColorMod);
            Util.Drawing.FillPictureBox(pbAmbientTint, _materialPass.AmbientStage.TexColorMod);
        }

        private void UpdateHighlightPreviews()
        {
            Util.Drawing.FillPictureBox(pbDiffuseHighlight, _materialPass.DiffuseStage.TexColorAdd);
            Util.Drawing.FillPictureBox(pbSpecularHighlight, _materialPass.SpecularStage.TexColorAdd);
            Util.Drawing.FillPictureBox(pbEmissiveHighlight, _materialPass.EmissiveStage.TexColorAdd);
            Util.Drawing.FillPictureBox(pbAmbientHighlight, _materialPass.AmbientStage.TexColorAdd);
        }

        private void UpdateBumpParams()
        {
            float scale = _bumpScale;
            float offset = _bumpOffset * _bumpScale;
            _materialPass.BumpStage.TexColorMod = new Bootstrap.Color(scale, scale, scale, scale);
            _materialPass.BumpStage.TexColorAdd = new Bootstrap.Color(offset, offset, offset, offset);
        }

        private void btnUberTexture_Click(object sender, EventArgs e)
        {
            // TODO: present some sort of output letting the user know
            // that they need to have a texture selected.
            if (_materialPass == null)
                return;

            // present the texture dialog to the user.
            Dialogs.TextureSelectionDialog dialog = new Dialogs.TextureSelectionDialog(true);
            DialogResult result = dialog.ShowDialog();
            if (result == DialogResult.Cancel)
                return;

            // get the current selection (or possibly none).
            string selection = null;
            int count = dialog.Selection.Count;
            if (count > 0)
                selection = dialog.Selection[0];
            else
                selection = "";

            // set the selection to the ubertexture.
            _materialPass.UberTexture = selection;
        }

        private void texButton_Click(object sender, EventArgs e)
        {
            // TODO: present some sort of output letting the user know
            // that they need to have a texture selected.
            if (_materialPass == null)
                return;

            // present the texture dialog to the user.
            Dialogs.TextureSelectionDialog dialog = new Dialogs.TextureSelectionDialog(false);
            DialogResult result = dialog.ShowDialog();
            if (result == DialogResult.Cancel)
                return;

            // get the current selection (or possibly none).
            string selection = null;
            int count = dialog.Selection.Count;
            if (count > 0)
                selection = dialog.Selection[0];
            else
                return;

            // figure out what stage to bind the texture to.
            Bootstrap.TextureStage stage = null;
            ButtonAdv button = sender as ButtonAdv;
            if (button == btnDiffuseTexture)
                stage = _materialPass.DiffuseStage;
            else if (button == btnSpecularTexture)
                stage = _materialPass.SpecularStage;
            else if (button == btnNormalTexture)
                stage = _materialPass.NormalStage;
            else if (button == btnBumpTexture)
                stage = _materialPass.BumpStage;
            else if (button == btnEmissiveTexture)
                stage = _materialPass.EmissiveStage;
            else if (button == btnAmbientTexture)
                stage = _materialPass.AmbientStage;
            else
                return;

            // finish by binding the texture to the stage.
            stage.Texture = selection;
        }

        private void texTint_Click(object sender, EventArgs e)
        {
            // figure out which stage we're modifying.
            Bootstrap.TextureStage stage = null;
            ButtonAdv button = sender as ButtonAdv;
            if (button == btnDiffuseTint)
                stage = _materialPass.DiffuseStage;
            else if (button == btnSpecularTint)
                stage = _materialPass.SpecularStage;
            else if (button == btnEmissiveTint)
                stage = _materialPass.EmissiveStage;
            else if (button == btnAmbientTint)
                stage = _materialPass.AmbientStage;
            else
                return;

            // crate a new color dialog and show it.
            Dialogs.HDRColorDialog colorDialog = new Dialogs.HDRColorDialog();
            colorDialog.Color = stage.TexColorMod;
            colorDialog.ShowDialog();
            stage.TexColorMod = colorDialog.Color;

            // make sure the tint picture boxes are updated.
            UpdateTintPreviews();
        }

        private void texHighlight_Click(object sender, EventArgs e)
        {
            // figure out which stage we're modifying.
            Bootstrap.TextureStage stage = null;
            ButtonAdv button = sender as ButtonAdv;
            if (button == btnDiffuseHighlight)
                stage = _materialPass.DiffuseStage;
            else if (button == btnSpecularHighlight)
                stage = _materialPass.SpecularStage;
            else if (button == btnEmissiveHighlight)
                stage = _materialPass.EmissiveStage;
            else if (button == btnAmbientHighlight)
                stage = _materialPass.AmbientStage;
            else
                return;

            // crate a new color dialog and show it.
            Dialogs.HDRColorDialog colorDialog = new Dialogs.HDRColorDialog();
            colorDialog.Color = stage.TexColorAdd;
            colorDialog.ShowDialog();
            stage.TexColorAdd = colorDialog.Color;
            
            // make sure the highlight picture boxes are updated.
            UpdateHighlightPreviews();
        }

        private void cbDepthEnable_CheckedChanged(object sender, EventArgs e)
        {
            Bootstrap.MaterialPass.RenderState states = _materialPass.RenderStates;
            if (cbDepthEnable.Checked)
                states |= Bootstrap.MaterialPass.RenderState.DepthTest;
            else
                states &= ~Bootstrap.MaterialPass.RenderState.DepthTest;
            _materialPass.RenderStates = states;
        }

        private void cbDepthBiasEnable_CheckedChanged(object sender, EventArgs e)
        {
            Bootstrap.MaterialPass.RenderState states = _materialPass.RenderStates;
            if (cbDepthEnable.Checked)
                states |= Bootstrap.MaterialPass.RenderState.DepthBias;
            else
                states &= ~Bootstrap.MaterialPass.RenderState.DepthBias;
            _materialPass.RenderStates = states;
        }

        private void cbCullingEnable_CheckedChanged(object sender, EventArgs e)
        {
            Bootstrap.MaterialPass.RenderState states = _materialPass.RenderStates;
            if (cbDepthEnable.Checked)
                states |= Bootstrap.MaterialPass.RenderState.CullFace;
            else
                states &= ~Bootstrap.MaterialPass.RenderState.CullFace;
            _materialPass.RenderStates = states;
        }

        private void cbBlendEnable_CheckedChanged(object sender, EventArgs e)
        {
            Bootstrap.MaterialPass.RenderState states = _materialPass.RenderStates;
            if (cbDepthEnable.Checked)
                states |= Bootstrap.MaterialPass.RenderState.Blend;
            else
                states &= ~Bootstrap.MaterialPass.RenderState.Blend;
            _materialPass.RenderStates = states;
        }

        private void cbCullMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            string value = cbCullMode.SelectedItem as string;
            if (value == "Front")
                _materialPass.CullFace = Bootstrap.MaterialPass.Face.Front;
            else if (value == "Back")
                _materialPass.CullFace = Bootstrap.MaterialPass.Face.Back;
        }

        private void cbAlphaTest_SelectedIndexChanged(object sender, EventArgs e)
        {
            string value = cbAlphaTest.SelectedItem as string;
            if (value == "Less")
                _materialPass.AlphaTestMode = Bootstrap.MaterialPass.AlphaTest.Less;
            else if (value == "LessEqual")
                _materialPass.AlphaTestMode = Bootstrap.MaterialPass.AlphaTest.LessEqual;
            else if (value == "Greater")
                _materialPass.AlphaTestMode = Bootstrap.MaterialPass.AlphaTest.Greater;
            else if (value == "GreaterEqual")
                _materialPass.AlphaTestMode = Bootstrap.MaterialPass.AlphaTest.GreaterEqual;
            else if (value == "Equal")
                _materialPass.AlphaTestMode = Bootstrap.MaterialPass.AlphaTest.Equal;
            else if (value == "NotEqual")
                _materialPass.AlphaTestMode = Bootstrap.MaterialPass.AlphaTest.NotEqual;
        }

        private void tbAlphaTestRef_TextChanged(object sender, EventArgs e)
        {
            string text = tbAlphaTestRef.Text;
            try
            {
                float value = Convert.ToSingle(text);
                _materialPass.AlphaTestRef = new Bootstrap.Expr(value);
            }
            catch (SystemException)
            {

            }
        }

        private void cbBlendMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            string value = cbBlendMode.SelectedItem as string;
            if (value == "Accumulate")
            {
                // overwrite destination pixels.
                _materialPass.AmbientSrcColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
                _materialPass.AmbientSrcAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
                _materialPass.AmbientDstColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.Zero;
                _materialPass.AmbientDstAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.Zero;

                // accumulate color, preserve alpha.
                _materialPass.LightSrcColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
                _materialPass.LightSrcAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.Zero;
                _materialPass.LightDstColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
                _materialPass.LightDstAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
            }
            else if (value == "Blend Base")
            {
                // overwrite destination pixels.
                _materialPass.AmbientSrcColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
                _materialPass.AmbientSrcAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
                _materialPass.AmbientDstColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.Zero;
                _materialPass.AmbientDstAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.Zero;

                // accumulate color multiplied with the inverse decal alpha.
                _materialPass.LightSrcColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.OneMinusDstAlpha;
                _materialPass.LightSrcAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.Zero;
                _materialPass.LightDstColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
                _materialPass.LightDstAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
            }
            else if (value == "Blend Decal")
            {
                // alpha blend with destination pixels.
                _materialPass.AmbientSrcColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.SrcAlpha;
                _materialPass.AmbientSrcAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
                _materialPass.AmbientDstColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.OneMinusSrcAlpha;
                _materialPass.AmbientDstAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.Zero;

                // accumulate color multiplied with decal alpha.
                _materialPass.LightSrcColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.SrcAlpha;
                _materialPass.LightSrcAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.Zero;
                _materialPass.LightDstColorBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
                _materialPass.LightDstAlphaBlendFactor = Bootstrap.MaterialPass.BlendFactor.One;
            }
        }

        private void cbAlphaTestEnable_CheckedChanged(object sender, EventArgs e)
        {
            Bootstrap.MaterialPass.RenderState states = _materialPass.RenderStates;
            if (cbAlphaTestEnable.Checked)
                states |= Bootstrap.MaterialPass.RenderState.AlphaTest;
            else
                states &= ~Bootstrap.MaterialPass.RenderState.AlphaTest;
            _materialPass.RenderStates = states;
        }

        private void tbBumpScale_TextChanged(object sender, EventArgs e)
        {
            if (_skipBumpUpdate)
                return;

            // ignore any events.
            _skipBumpUpdate = true;
            try
            {
                // calculate the new value and update the slider.
                string text = tbBumpScale.Text;
                float value = Convert.ToSingle(text);
                if (value > kBumpScaleMax)
                {
                    tbBumpScale.Text = kBumpScaleMax.ToString();
                    value = kBumpScaleMax;
                }
                else if (value < kBumpScaleMin)
                {
                    tbBumpScale.Text = kBumpScaleMin.ToString();
                    value = kBumpScaleMin;
                }
                float t = (value - kBumpScaleMin) / (kBumpScaleMax - kBumpScaleMin);
                int range = tbarBumpScale.Maximum - tbarBumpScale.Minimum - 1;
                tbarBumpScale.Value = tbarBumpScale.Minimum + (int)(t * range);

                // store the value.
                _bumpScale = value;
                UpdateBumpParams();
            }
            catch (SystemException)
            {

            }
            _skipBumpUpdate = false;
        }

        private void tbBumpOffset_TextChanged(object sender, EventArgs e)
        {
            if (_skipBumpUpdate)
                return;

            // ignore any events.
            _skipBumpUpdate = true;
            try
            {
                // calculate the new value and update the slider.
                string text = tbBumpOffset.Text;
                float value = Convert.ToSingle(text);
                if (value > kBumpOffsetMax)
                {
                    tbBumpOffset.Text = kBumpOffsetMax.ToString();
                    value = kBumpOffsetMax;
                }
                else if (value < kBumpOffsetMin)
                {
                    tbBumpOffset.Text = kBumpOffsetMin.ToString();
                    value = kBumpOffsetMin;
                }
                float t = (value - kBumpOffsetMin) / (kBumpOffsetMax - kBumpOffsetMin);
                int range = tbarBumpOffset.Maximum - tbarBumpOffset.Minimum - 1;
                tbarBumpOffset.Value = tbarBumpOffset.Minimum + (int)(t * range);

                // store the value.
                _bumpOffset = value;
                UpdateBumpParams();
            }
            catch (SystemException)
            {

            }
            _skipBumpUpdate = false;
        }

        private void tbarBumpScale_Scroll(object sender, EventArgs e)
        {
            if (_skipBumpUpdate)
                return;

            // ignore any events.
            _skipBumpUpdate = true;
            {
                // calculate t.
                int tvalue = (tbarBumpScale.Value - tbarBumpScale.Minimum);
                int range = (tbarBumpScale.Maximum - tbarBumpScale.Minimum);
                float t = tvalue / (float)range;

                // convert to a valid value and update the text control.
                float value = kBumpScaleMin + t * (kBumpScaleMax - kBumpScaleMin);
                tbBumpScale.Text = value.ToString();

                // store the value.
                _bumpScale = value;
                UpdateBumpParams();
            }
            _skipBumpUpdate = false;
        }

        private void tbarBumpOffset_Scroll(object sender, EventArgs e)
        {
            if (_skipBumpUpdate)
                return;

            // ignore any events.
            _skipBumpUpdate = true;
            {
                // calculate t.
                int tvalue = (tbarBumpOffset.Value - tbarBumpOffset.Minimum);
                int range = (tbarBumpOffset.Maximum - tbarBumpOffset.Minimum);
                float t = tvalue / (float)range;

                // convert to a valid value and update the text control.
                float value = kBumpOffsetMin + t * (kBumpOffsetMax - kBumpOffsetMin);
                tbBumpOffset.Text = value.ToString();

                // store the value.
                _bumpOffset = value;
                UpdateBumpParams();
            }
            _skipBumpUpdate = false;
        }

        private void btnBlendMode_Click(object sender, EventArgs e)
        {
            // todo: show the advanced alpha blend dialog.
        }
    }
}
