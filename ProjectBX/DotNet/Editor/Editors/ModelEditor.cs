using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using Syncfusion.Windows.Forms.Tools;
using Bootstrap;

namespace Editor.Editors
{
	public partial class ModelEditor : UserControl
	{
		Model		_model;
		bool		_populating;

		//================================
		// ctor & ftor.
		//================================

		//--------------------------------
		public ModelEditor()
		{
			InitializeComponent();
			this.SetStyle( ControlStyles.Selectable, true );
		}

		//--------------------------------
		public Bootstrap.Model Model
		{
			get { return _model; }
			set
			{
				// simply ignore any "clearing" of the light.
				if ( value == null )
					return;

				// set the light's value and update the controls.
				_model = value;
				UpdateControls();
			}
		}


		//================================
		// public delegates.
		//================================

		//--------------------------------
		public delegate void ObjectDeleted();
		public ObjectDeleted SelectionDeleted;

		//================================
		// private methods.
		//================================

		//--------------------------------
		private void UpdateControls()
		{
			// protect against unintentional changes to settings.
			_populating = true;

			tcTransform.SetSceneEntity( _model );

			tbFileName.Text = _model.FileName;
			tbInstanceName.Text = _model.InstanceName;

			// fill out the child list box.
			int childCount = _model.ChildCount;
			for ( int i = 0; i < childCount; ++i )
			{
				// get the current child and add it to the list box.
				_model.GetChild( i );
				lbChildren.Items.Add( _model.InstanceName );
			}

			// fill out the node list.
			TreeNodeAdv root = new TreeNodeAdv( _model.InstanceName );
			WalkNodeHierarchy( root, _model.RootNode );

			tvNodeHierarchy.Nodes.Clear();
			tvNodeHierarchy.Nodes.Add( root );

			cbSkybox.Checked = _model.InSkybox;

			_populating = false;
		}

		//--------------------------------
		private void WalkNodeHierarchy( TreeNodeAdv parentTreeNode, ModelNode node )
		{
			// add the current node to the tree.
			TreeNodeAdv curTreeNode = new TreeNodeAdv( node.Name );
			parentTreeNode.Nodes.Add( curTreeNode );

			// now add mesh instances to the tree.
			int meshInstCount = node.MeshInstCount;
			for ( int i = 0; i < meshInstCount; ++i )
			{
				// add the instance.
				MeshInst inst = node.GetMeshInst( i );
				TreeNodeAdv curMeshNode = new TreeNodeAdv( "< mesh >" );
				curTreeNode.Nodes.Add( curMeshNode );

				// add the instance's ranges.
				Mesh mesh = inst.Mesh;
				for ( int j = 0; j < inst.RangeCount; ++j )
				{
					// get the range info.
					RangeInfo range;
					if ( !mesh.GetRange( out range, j ) )
						continue;

					// get the material assignend to that range.
					Material material = inst.GetMaterial( j );
					if ( material == null )
						continue;

					// add the name of the range.
					TreeNodeAdv curRangeNode = new TreeNodeAdv( range.Name );
					curMeshNode.Nodes.Add( curRangeNode );

					// add the range's material.
					TreeNodeAdv curMatNode = new TreeNodeAdv( material.Name );
					curRangeNode.Nodes.Add( curMatNode );

					// add statistics.
					TreeNodeAdv curTriCountNode = new TreeNodeAdv( range.Count.ToString() + " tris" );
					curRangeNode.Nodes.Add( curTriCountNode );
				}
			}

			// recurse children.
			int childCount = node.ChildCount;
			for ( int i = 0; i < childCount; ++i )
				WalkNodeHierarchy( curTreeNode, node.GetChild( i ) );
		}

		//--------------------------------
		private void lblModelHierarchy_Click( object sender, EventArgs e )
		{

		}

		private void lbChildren_DoubleClick( object sender, EventArgs e )
		{

		}

		private void treeViewAdv1_MouseClick( object sender, MouseEventArgs e )
		{
			// draw a transform indicator where the node is.
		}

		private void btnDelete_Click( object sender, EventArgs e )
		{
			// if there is no model, abort.
			if ( _model == null )
				return;

			// remove the model from the scene.
			Bootstrap.Model parent = _model.Parent;
			if ( parent != null )
				parent.RemoveChild( _model );

			if ( SelectionDeleted != null )
				SelectionDeleted();
		}

		private void cbSkybox_CheckedChanged( object sender, EventArgs e )
		{
			if ( _populating )
				return;

			if ( cbSkybox.Checked )
			{
				_model.AddToSkybox();
			}
			else
			{
				// if no scene is loaded, abort.
				Bootstrap.Scene scene = Program.GameEngine.Scene;
				if ( scene == null )
					return;

				// if there is no root model, abort.
				Model root = scene.Root;
				if ( root == null )
					return;

				// remove the model from the scene.
				Model parent = _model.Parent;
				if ( parent != null )
					parent.RemoveChild( _model );

				// add the model to the scene.
				root.AddChild( _model, null );
			}
		}
	}
}
