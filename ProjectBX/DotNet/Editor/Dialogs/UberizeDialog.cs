using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Syncfusion.Windows.Forms.Tools;
using Bootstrap;

namespace Editor.Dialogs
{
    public partial class UberizeDialog : BaseDialog
    {
        string _uberTexture;
        static Dialogs.TextureSelectionDialog _textureSelector;
        Dictionary<MeshInstRange, int> _uberizeItems;
        Dictionary<string, Model> _models;
        Matrix _viewMatrix;
        Matrix _transform;

        class MeshInstRange
        {
            public MeshInst inst;
            public int range;

            public override bool Equals(object obj)
            {
                MeshInstRange rhs = obj as MeshInstRange;
                return inst.Equals( rhs.inst ) && range == rhs.range;
            }

            public override int GetHashCode()
            {
                return inst.GetHashCode() ^ range.GetHashCode();
            }

            public new static bool Equals(object objA, object objB)
            {
                MeshInstRange lhs = objA as MeshInstRange;
                MeshInstRange rhs = objB as MeshInstRange;
                return lhs.inst.Equals( rhs.inst ) && lhs.range == rhs.range;
            }
        }

        //================================
        // ctor & ftor.
        //================================

        //--------------------------------
        public UberizeDialog()
        {
            InitializeComponent();

            _uberizeItems = new Dictionary<MeshInstRange, int>();
            _models = new Dictionary<string, Model>();
            _transform = new Bootstrap.Matrix(1.0f);
            _viewMatrix = new Bootstrap.Matrix(1.0f);
            _uberTexture = "";

            // initialize the transform widgets.
            tcTransform.ScaleEnable = true;
            tcTransform.Position = _transform.Translate;
            tcTransform.Rotation = _transform.Rotate.EulerAngles;
            tcTransform.ScaleXYZ = _transform.Scale;
        }


        //================================
        // public interface.
        //================================

        //--------------------------------
        public string UberTexture { get { return _uberTexture; } }

        //================================
        // private methods.
        //================================

        //--------------------------------
        private void bnOK_Click(object sender, EventArgs e)
        {
            // make sure there is a selection!
            if (_uberTexture == "")
                return;

            // now bake all selected items.
            foreach (KeyValuePair<MeshInstRange, int> pair in _uberizeItems)
                Bootstrap.UberTexture.UberizeMeshRange(_uberTexture, _transform, pair.Key.inst, (uint)pair.Key.range);

            // now save all models.
            foreach (KeyValuePair<string, Model> pair in _models)
                pair.Value.Save();

            // clear the models list.
            _models.Clear();

            // handle the success case.
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        //--------------------------------
        private void btnUbertexture_Click(object sender, EventArgs e)
        {
            // present the texture dialog to the user.
            if( _textureSelector == null )
                _textureSelector = new Dialogs.TextureSelectionDialog(true);
            DialogResult result = _textureSelector.ShowDialog();
            if (result == DialogResult.Cancel)
                return;

            // get the current selection (or possibly none).
            string selection = null;
            int count = _textureSelector.Selection.Count;
            if (count > 0)
                selection = _textureSelector.Selection[0];
            else
                return;

            // set the selection to the ubertexture.
            _uberTexture = selection;

            // TODO: clip the selection to a reasonable length.
            string displayName = "(" + selection + ")";

            // replace the button's text with the current selection.
            btnUbertexture.Text = displayName;
        }

        //--------------------------------
        private void btnAddModels_Click(object sender, EventArgs e)
        {
            // load up the model browser.
            ModelSelectionDialog modelSelector = new ModelSelectionDialog();
            modelSelector.ShowDialog();

            // get the result.
            foreach (string selection in modelSelector.Selection)
            {
                // if we already contain the model, ignore it.
                if (_models.ContainsKey(selection))
                    continue;

                // add the model to our list.
                TreeNodeAdv modelNode = new TreeNodeAdv("Model " + selection);
                tvSelection.Nodes.Add(modelNode);
                modelNode.Tag = selection;

                // add all of the model's meshes to our list.
                Model model = Model.Create(selection, "uber", false);

                // clone the model so that we can mess with it's data.
                string cloneFileName = model.FileName;
                int loc = cloneFileName.LastIndexOf( '.' );
                if ( loc != -1 )
                    cloneFileName = cloneFileName.Substring(0, loc);
                cloneFileName += "_uber.mdl";

                // clone the file.
                Model uberModel = model.CloneFile(cloneFileName, model.InstanceName);
                foreach (MeshInst inst in uberModel.MeshInsts)
                    AddUberizeItem(modelNode, inst);

                // add the model to our models list.
                _models.Add(selection, uberModel);
            }
        }

        //--------------------------------
        private void btnAddMaterial_Click(object sender, EventArgs e)
        {
            // TODO: get all items using the specified material.
            MessageBox.Show("This feature is not yet implemented!", "Error...",
                MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        //--------------------------------
        private void btnAddBSP_Click(object sender, EventArgs e)
        {
            // add all items in the BSP.
            TreeNodeAdv bspNode = new TreeNodeAdv("BSP Tree");
            tvSelection.Nodes.Add(bspNode);

            IList<MeshInst> meshInsts = Program.GameEngine.Scene.GetBSPMeshInsts();
            foreach (MeshInst inst in meshInsts)
                AddUberizeItem(bspNode, inst);
        }

        //--------------------------------
        private void btnRemove_Click(object sender, EventArgs e)
        {
            if (tvSelection.SelectedNode != null)
                RemoveUberizeItem(tvSelection.SelectedNode);

            pbPreview.Invalidate();
        }

        //--------------------------------
        private void AddUberizeItem(TreeNodeAdv parent, MeshInst inst)
        {
            TreeNodeAdv meshNode = new TreeNodeAdv(inst.Mesh.Name);
            parent.Nodes.Add(meshNode);

            // iterate through mesh instance's ranges.
            int rangeIndex = 0;
            Mesh mesh = inst.Mesh;
            for ( int i = 0; i < inst.RangeCount; ++i )
            {
                // get the range info.
                RangeInfo range;
                if ( !mesh.GetRange( out range, i ) )
                    continue;

                TreeNodeAdv rangeNode = new TreeNodeAdv(range.Name);
                meshNode.Nodes.Add(rangeNode);

                // fill out a new key.
                MeshInstRange key = new MeshInstRange();
                key.inst = inst;
                key.range = rangeIndex;

                // check for the current mesh instance/range pair.
                if (_uberizeItems.ContainsKey(key))
                {
                    // increment the reference count if necessary.
                    _uberizeItems[key] += 1;
                }
                else
                {
                    // TODO: Calculate a valid default matrix.
                    _uberizeItems.Add(key, 1);
                    rangeNode.Tag = key;
                }
            }
        }

        //--------------------------------
        private void RemoveUberizeItem(TreeNodeAdv node)
        {
            // update the uberize item dictionary.
            MeshInstRange curKey = node.Tag as MeshInstRange;
            if (curKey != null)
            {
                int refCount = (_uberizeItems[curKey] -= 1);
                if (refCount == 0)
                {
                    _uberizeItems.Remove(curKey);
                }
            }

            // remove the model if necessary.
            string curModel = node.Tag as string;
            if (curModel != null)
            {
                if (_models.ContainsKey(curModel))
                    _models.Remove(curModel);
            }

            // recurse children.
            while (node.Nodes.Count > 0)
                RemoveUberizeItem(node.Nodes[0]);

            // remove the node.
            node.Remove();
        }

        //--------------------------------
        private void pbPreview_Paint(object sender, PaintEventArgs e)
        {
            // get all unique selections.
            Dictionary<MeshInstRange, int> items = new Dictionary<MeshInstRange, int>();
            foreach (TreeNodeAdv node in tvSelection.SelectedNodes)
                GetUniqueNodes(items, node);

            // fill the client rectangle with black.
            e.Graphics.FillRectangle(Brushes.Black, pbPreview.ClientRectangle);

            // calculate the UV to screen matrix.
            Matrix uvXForm = _viewMatrix * _transform;

            // draw everything that was selected.
            foreach (KeyValuePair<MeshInstRange, int> key in items)
            {
                // get the texture coordinates from the mesh.
                Vector2[] texCoords = key.Key.inst.Mesh.VertexBuffer.TexCoords;
                ushort[] indices = key.Key.inst.Mesh.IndexBuffer.Indices;
                if (texCoords == null)
                    continue;

                // TODO: INVERT THE UVs.  0,0 = bottom left. 1,1 = top right.
                if (indices != null)
                {
                    int indexCount = indices.Length;
                    for (int i = 0; i < indexCount; i += 3)
                        DrawTriangle(e, texCoords[indices[i + 0]], texCoords[indices[i + 1]], texCoords[indices[i + 2]], uvXForm);
                }
                else
                {
                    int vertexCount = texCoords.Length;
                    for (int i = 0; i < vertexCount; i += 3)
                        DrawTriangle(e, texCoords[i + 0], texCoords[i + 1], texCoords[i + 2], uvXForm);
                }
            }
        }

        private void GetUniqueNodes(Dictionary<MeshInstRange, int> items, TreeNodeAdv node)
        {
            // add the current node's key.
            MeshInstRange key = node.Tag as MeshInstRange;
            if (key != null)
                items.Add(key, 0);

            // recurse.
            foreach (TreeNodeAdv child in node.Nodes)
                GetUniqueNodes(items, child);

        }

        private void DrawTriangle(PaintEventArgs e, Vector2 p0, Vector2 p1, Vector2 p2, Matrix wvXForm)
        {
            // calculate the current coordinate.
            Vector3 xp0 = wvXForm.TransformCoord(new Vector3(p0.X, p0.Y, 0.0f));
            Vector3 xp1 = wvXForm.TransformCoord(new Vector3(p1.X, p1.Y, 0.0f));
            Vector3 xp2 = wvXForm.TransformCoord(new Vector3(p2.X, p2.Y, 0.0f));

            int targetWidth = pbPreview.Width;
            int targetHeight = pbPreview.Height;
            Point ppt0 = new Point((int)(xp0.X * (float)targetWidth), targetHeight - (int)(xp0.Y * (float)targetHeight));
            Point ppt1 = new Point((int)(xp1.X * (float)targetWidth), targetHeight - (int)(xp1.Y * (float)targetHeight));
            Point ppt2 = new Point((int)(xp2.X * (float)targetWidth), targetHeight - (int)(xp2.Y * (float)targetHeight));

            // draw the lines.
            e.Graphics.DrawLine(Pens.White, ppt0, ppt1);
            e.Graphics.DrawLine(Pens.White, ppt1, ppt2);
            e.Graphics.DrawLine(Pens.White, ppt2, ppt0);
        }

        private void tvSelection_Click(object sender, EventArgs e)
        {
            // invalidate the preview window.
            pbPreview.Invalidate();
        }

        private void tcTransform_TranslateChanged(object sender, EventArgs e)
        {
            // now apply the transform.
            _transform.Translate = tcTransform.Position;

            // invalidate the preview window.
            pbPreview.Invalidate();
        }

        private void tcTransform_ScaleChanged(object sender, EventArgs e)
        {
            // now apply the transform.
            _transform.Scale = tcTransform.ScaleXYZ;

            // invalidate the preview window.
            pbPreview.Invalidate();
        }

        private void tcTransform_RotateChanged(object sender, EventArgs e)
        {
            // now apply the transform.
            _transform.Rotate = new Matrix3x3(tcTransform.Rotation.Y, tcTransform.Rotation.X, tcTransform.Rotation.Z);

            // invalidate the preview window.
            pbPreview.Invalidate();
        }

        private void tvSelection_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete)
            {
                if (tvSelection.SelectedNode != null)
                    RemoveUberizeItem(tvSelection.SelectedNode);

                pbPreview.Invalidate();
            }
        }

        private void btnUVLayout_Click(object sender, EventArgs e)
        {
            // allocate an array for our mesh instances and populate it.
            int i = 0;
            KeyValuePair<Bootstrap.MeshInst, int>[] meshInsts = new KeyValuePair<Bootstrap.MeshInst, int>[_uberizeItems.Count];
            foreach (KeyValuePair<MeshInstRange, int> key in _uberizeItems)
            {
                meshInsts[i++] = new KeyValuePair<Bootstrap.MeshInst, int>( key.Key.inst, key.Key.range );
            }

            // calculate the uberize UV transform.
            _transform = Bootstrap.UberTexture.CalcUberizeDefaultUVXForm(_uberTexture, meshInsts, true);
            UpdateTransform();

            // invalidate the preview.
            pbPreview.Invalidate();
        }

        private void UpdateTransform()
        {
            tcTransform.Position = _transform.Translate;
            tcTransform.Rotation = _transform.Rotate.EulerAngles;
            tcTransform.ScaleXYZ = _transform.Scale;
        }
    }
}
