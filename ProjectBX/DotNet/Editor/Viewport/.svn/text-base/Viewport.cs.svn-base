using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using System.Reflection;
using Syncfusion.Windows.Forms;
using Syncfusion.Windows.Forms.Tools;
using System.Drawing.Imaging;
using Editor.Dialogs;
using Syncfusion.Windows.Forms.Tools.Win32API;

namespace Editor
{
	public partial class Viewport : RibbonForm
	{
		//=======================
		// Types
		//=======================
		public enum UserMode
		{
			Create,
			Manipulate,
			Paint,
			Material,
			Animate,
			Count,
		}

		public struct WidgetColors
		{
			public Bootstrap.Color	SelectedEntityColor;
			public Bootstrap.Color	ModelColor;
			public Bootstrap.Color	MeshInstColor;
			public float			LightColorScale;
		}

		//=======================
		// Member Variables
		//=======================

		static Viewport _viewport;

		// huzzah look state!
		LookState _lookState;
		bool _resizingBrush;
		bool _painting;
		bool _done;
		GizmoMgr _gizmoMgr;
		SceneManager _sceneManager;
		UberTexturePainter _painter;
		UserMode _currentMode;
		DateTime _prevFPSTime;
		int _fps;
		Timer _viewportResizeTimer;
		float _brushRoll;
        float _cameraSpeed;

		// keeps track of whether the user has any unsaved changes.
		bool _unbakedChanges;
		bool _unsavedChanges;

		// selected items.
		Bootstrap.SceneEntity _selectedEntity;
		Bootstrap.Material _selectedMaterial;
		Bootstrap.Brush _selectedBrush;

		// dialogs.
		MaterialSelectionDialog _materialSelectionDlg;
		BrushSelectionDialog _brushSelectionDlg;
		ModelSelectionDialog _modelSelectionDlg;
		OpenFileDialog _loadSceneDlg;
		ImportSceneDialog _importSceneDlg;
		ImportModelDialog _importModelDlg;
		TextureSelectionDialog _uberTextureDlg;

		// editors.
		Editors.LightEditor _lightEditor;
		Editors.ModelEditor _modelEditor;
		Editors.MaterialEditor _materialEditor;
		Editors.BrushPaletteEditor _brushPaletteEditor;
		Controls.BrushAttributes _brushAttributes;

		// random for creating lights.
		System.Random _random;

		//=======================
		// constructor & finalizer
		//=======================

		//----------------------------------------------------------
		public Viewport()
		{
			_viewport = this;

			InitializeComponent();

			rcDock.MinimizePanel = true;
			this.FormClosing += new FormClosingEventHandler( Viewport_FormClosing );
			_lookState = new LookState();
			_gizmoMgr = new GizmoMgr( btnMove, btnRotate, btnScale, btnSelect );
			_sceneManager = new SceneManager();
			_painter = new UberTexturePainter();
			_viewportResizeTimer = new Timer();
			//_viewportResizeTimer.Interval = 1000; // TODO: use this.
			_viewportResizeTimer.Interval = 1;
			_viewportResizeTimer.Tick += new EventHandler( _viewportResizeTimer_Tick );
			_resizingBrush = false;

			// initialize dialogs.
			_materialSelectionDlg = new MaterialSelectionDialog();
			_brushSelectionDlg = new BrushSelectionDialog();
			_modelSelectionDlg = new ModelSelectionDialog();

			// initialize editors.
			_lightEditor = new Editor.Editors.LightEditor();
			_modelEditor = new Editor.Editors.ModelEditor();
			_materialEditor = new Editor.Editors.MaterialEditor();
			_brushPaletteEditor = new Editor.Editors.BrushPaletteEditor();
			_brushAttributes = new Controls.BrushAttributes();

			// register for deletion callback.
			_modelEditor.SelectionDeleted += new Editors.ModelEditor.ObjectDeleted( ObjectDeleted );

			// tag each stage selector.
			bnDiffuseEnable.Tag = Bootstrap.Brush.Stage.Diffuse;
			bnSpecularEnable.Tag = Bootstrap.Brush.Stage.Specular;
			bnBumpEnable.Tag = Bootstrap.Brush.Stage.Bump;
			bnNormalEnable.Tag = Bootstrap.Brush.Stage.Normal;
			bnEmissiveEnable.Tag = Bootstrap.Brush.Stage.Emissive;
			bnAmbientEnable.Tag = Bootstrap.Brush.Stage.Ambient;
			bnUserEnable.Tag = Bootstrap.Brush.Stage.User;

			// init the brush controls to their default settings.
			UpdateBrushControls();

			// attach some event handlers.
			_brushPaletteEditor.SelectionChanged += new EventHandler( brushPalette_SelectionChanged );
			_brushAttributes.BrushSizeChanged += new EventHandler( _brushAttributes_BrushSizeChanged );
			_View.MouseEnter += new EventHandler( _View_MouseEnter );
			_View.KeyDown += new KeyEventHandler( _View_KeyDown );
			_View.KeyUp += new KeyEventHandler( _View_KeyUp );
			_View.MouseLeave += new EventHandler( _View_MouseLeave );
			_View.SizeChanged += new EventHandler( _View_SizeChanged );

			// attach an event handler to each brush stage button.
			foreach ( ToolStripItem item in pnBrushStages.Items )
			{
				if ( item is ToolStripButton )
				{
					ToolStripButton stageButton = ( ToolStripButton )item;
					stageButton.Click += new EventHandler( bnEnable_Click );
				}
			}

			// initialize the BlendMode dropdown.
			cbBlendMode.SelectedIndex = 0;

			// clear the status text.
			lblStatus.Text = "";
			lblFPS.Text = "";
			_prevFPSTime = DateTime.Now;

            // intialize the camera speed to one.
            _cameraSpeed = 1.0f;

			// initialize the random number generator.
			_random = new Random();
		}

		//=======================
		// Attributes
		//=======================

		// -----------------------
		public bool _Done
		{
			get { return _done; }
			set { _done = value; }
		}

		// -----------------------
		public bool _Painting
		{
			get { return _painting; }
		}

		// -----------------------
		public Control _View
		{
			get { return pnView; }
		}

		// -----------------------
		public UserMode _UserMode
		{
			get { return _currentMode; }
		}

		//----------------------------------------------------------
		Point _UICursor
		{
			get { return _View.PointToClient( Cursor.Position ); }
			set
			{
				Point pos = _View.PointToScreen( value );
				if ( !Cursor.Position.Equals( pos ) )
					Cursor.Position = pos;
			}
		}

		//----------------------------------------------------------
		Point _Center
		{
			get { return new Point( _View.Width / 2, _View.Height / 2 ); }
		}

		//----------------------------------------------------------
		Point _CursorAnchor;

		// -----------------------
		public static string StatusText
		{
			get { return _statusText; }
			set
			{
				if ( _statusText == value )
					return;
				_statusText = value;
				_viewport.lblStatus.Text = value;
			}
		}
		static string _statusText;

		//----------------------------------------------------------
		public PointF UIRelative( Point mousePos )
		{
			float rx = mousePos.X / (float)_View.Width;
			float ry = mousePos.Y / (float)_View.Height;

			// invert Y.
			ry = 1.0f - ry;
			return new PointF( rx * 2.0f - 1.0f, ry * 2.0f - 1.0f );
		}

		// -----------------------
		public static void OverrideStatusText( string text )
		{
			_viewport.lblStatus.Text = text;
		}

		// -----------------------
		public static void DoneOverridingStatusText( string text )
		{
			_viewport.lblStatus.Text = _statusText;
		}

		//=======================
		// public functionality
		//=======================

		//----------------------------------------------------------
		public void EditorUpdate()
		{
			Bootstrap.Engine engine = Program.GameEngine;
			Bootstrap.Camera camera = Program.GameCamera;

			// update the FPS info.
			DateTime now = DateTime.Now;
			if ( ( now - _prevFPSTime ).Seconds > 1 )
			{
				_prevFPSTime = now;
				lblFPS.Text = _fps + " FPS";
				_fps = 0;
			}

			if ( _UserMode == UserMode.Paint )
			{
				// pick against the scene, ignoring lights and stamps.
				Bootstrap.Ray ray = BuildPickRay( _UICursor );
				Bootstrap.Scene.PickResult pickResult = engine.Scene.Pick( ray, 100.0f,
					Bootstrap.Scene.Entities.All 
					& ~Bootstrap.Scene.Entities.Lights
					& ~Bootstrap.Scene.Entities.Stamps );

				if ( bnPaintType_Brush.Checked || bnPaintType_Mask.Checked )
				{
					// determine if we have a paintable surface.
					switch ( pickResult.PickedType )
					{
						// we can paint on any geometric surface.
						case Bootstrap.Scene.Entities.BSP:
						case Bootstrap.Scene.Entities.Models:
						case Bootstrap.Scene.Entities.Terrain:
							{
								if( !_resizingBrush )
								{
									// orient the painter.
									_painter.Orient( camera, pickResult.HitLocation, _brushRoll );

									// show the normal at the mouse position.
									//engine.DrawLine( pickResult.HitLocation, pickResult.HitLocation + pickResult.HitNormal, 
									//new Bootstrap.Color( 1.0f, 0.0f, 0.0f, 1.0f ) );

									if( !_painter.Painting )
									{
										// before painting begins, if the painter is restricted to a plane, 
										// use the pick result's normal / hitPos as the plane.
										_painter.IsRestrictedToPlane = cbRestrictBrushToPlane.Checked;
										if( cbRestrictBrushToPlane.Checked )
											_painter.Plane = new Bootstrap.Plane( pickResult.HitNormal, pickResult.HitLocation );
									}

									// TODO: Clean this up.
									if( _painting )
									{
										// if the user is holding down the ctrl key, fill the entire MeshInst.
										UberTexturePainter.PaintMode paintMode = UberTexturePainter.PaintMode.Brush;
										if( bnPaintType_Mask.Checked )
											paintMode = UberTexturePainter.PaintMode.Mask;
										else if( ( ModifierKeys & Keys.Control ) != 0 )
											paintMode = UberTexturePainter.PaintMode.Fill;

										// paint onto every range in the picked meshinst.
										for( int i = 0; i < pickResult.PickedMeshInst.Mesh.RangeCount; ++i )
											_painter.Paint( pickResult.PickedMeshInst, i, null, paintMode );

										// mark the unbaked changes flag to true.
										_unbakedChanges = true;
									}
									else
									{
										_painter.EndPaint();
									}
								}
							}
							break;
					}
				}
			}

			// increment the FPS counter.
			++_fps;
		}

		//----------------------------------------------------------
		public void EditorRender()
		{
			Bootstrap.Engine engine = Program.GameEngine;
			Bootstrap.Camera camera = Program.GameCamera;

			// if no scene is loaded, abort.
			if ( !engine.Scene.Loaded )
				return;

			// render the scene.
			engine.BeginRender( camera );

			WidgetColors colors = new WidgetColors();

			// disable depth testing and render widgets darker than normal.
			float kColorScale = 0.5f;
			engine.SetupStates( camera, true, false );
			colors.SelectedEntityColor = kColorScale * new Bootstrap.Color( 1.0f, 1.0f, 0.0f, 1.0f );
			colors.ModelColor = kColorScale * new Bootstrap.Color( 0.0f, 0.0f, 1.0f, 1.0f );
			colors.MeshInstColor = kColorScale * new Bootstrap.Color( 1.0f, 0.4f, 0.0f, 1.0f );
			colors.LightColorScale = kColorScale;
			RenderWidgets( colors );

			// enable depth testing and render the widgets with normal colors.
			engine.SetupStates( camera, true, true );
			colors.SelectedEntityColor = new Bootstrap.Color( 1.0f, 1.0f, 0.0f, 1.0f );
			colors.ModelColor = new Bootstrap.Color( 0.0f, 0.0f, 1.0f, 1.0f );
			colors.MeshInstColor = new Bootstrap.Color( 1.0f, 0.4f, 0.0f, 1.0f );
			colors.LightColorScale = 1.0f;
			RenderWidgets( colors );

			// disable depth testing and draw the gizmos and other widgets.
			engine.SetupStates( camera, true, false );
			if ( _UserMode == UserMode.Manipulate )
			{
				// render any gizmos.
				_gizmoMgr.Render();
			}
			else if ( _UserMode == UserMode.Paint )
			{
				if ( bnPaintType_Brush.Checked || bnPaintType_Mask.Checked )
				{
					// render the painter widget.
					_painter.Render();
				}
			}

			if ( _UserMode == UserMode.Paint && bnPaintType_Stamp.Checked || _UserMode == UserMode.Manipulate )
			{
				// render the stamps.
				Bootstrap.Stamp.RenderAllStamps( Program.GameEngine );
			}

			// end drawing widgets / gizmos.
			engine.EndRender();

			// present the backbuffer.
			engine.Present();
		}

		//----------------------------------------------------------
		void RenderWidgets( WidgetColors colors )
		{
			Bootstrap.Engine engine = Program.GameEngine;
			Bootstrap.Camera camera = Program.GameCamera;

			// calculate a list of all visible MeshInsts.
			IList<Bootstrap.MeshInst> visibleMeshInsts = engine.Scene.GetVisibleMeshInsts( camera );

			// are we in manipulate mode?
			if ( _UserMode == UserMode.Manipulate )
			{
				// render any light widgets.
				foreach ( Bootstrap.Light light in engine.Scene.Lights )
					engine.DrawSphere( light.Position, 1.0f, colors.LightColorScale * light.Color, 3 );

				// render any model volumes.
				Bootstrap.Model sceneRoot = engine.Scene.Root;
				if ( sceneRoot != null )
				{
					for ( int i = 0; i < sceneRoot.ChildCount; ++i )
					{
						Bootstrap.Model child = sceneRoot.GetChild( i );
						RenderModelHierarchy( engine, child, colors );
					}
				}
			}
		}

		//----------------------------------------------------------
		void RenderModelHierarchy( Bootstrap.Engine engine, Bootstrap.Model model, WidgetColors colors )
		{
			// if the current model contains nodes, draw the model's bounding box, then
			// draw each node.
			if ( model.RootNode != null )
			{
				// select a color based on whether the current model is selected.
				Bootstrap.Color color = colors.ModelColor;
				if ( model.Equals( _selectedEntity ) )
					color = colors.SelectedEntityColor;

				// draw the bounding box.
				engine.DrawAABox( model.Bounds, color );

				// draw each node.
				RenderModelNodeHierarchy( engine, model.RootNode, colors );
			}

			// draw each child model.
			for ( int i = 0; i < model.ChildCount; ++i )
			{
				Bootstrap.Model child = model.GetChild( i );
				RenderModelHierarchy( engine, child, colors );
			}
		}

		//----------------------------------------------------------
		void RenderModelNodeHierarchy( Bootstrap.Engine engine, Bootstrap.ModelNode modelNode, WidgetColors colors )
		{
			// select a color based on whether the current model node is selected.
			Bootstrap.Color color = colors.MeshInstColor;
			if ( modelNode.Equals( _selectedEntity ) )
				color = colors.SelectedEntityColor;

			// if the current model node contains mesh instances, draw the node's bounding box.
			if ( modelNode.MeshInstCount > 0 )
				engine.DrawAABox( modelNode.BoundsExact, color );

			// draw each child model node.
			for ( int i = 0; i < modelNode.ChildCount; ++i )
			{
				Bootstrap.ModelNode child = modelNode.GetChild( i );
				RenderModelNodeHierarchy( engine, child, colors );
			}
		}

		//----------------------------------------------------------
		// renders a MeshInst in wireframe.
		static void RenderWireframe( Bootstrap.MeshInst meshInst, Bootstrap.Color color )
		{
			Bootstrap.Engine engine		= Program.GameEngine;
			Bootstrap.Mesh mesh		= meshInst.Mesh;
			Bootstrap.Vector3[] positions	= mesh.VertexBuffer.Positions;
			UInt16[] indices		= mesh.IndexBuffer.Indices;
			Bootstrap.Matrix transform	= meshInst.Transform;
			for ( int idx = 0; idx < indices.Length; )
			{
				// retrieve the triangle vertices.
				Bootstrap.Vector3 p0 = positions[ indices[ idx++ ] ];
				Bootstrap.Vector3 p1 = positions[ indices[ idx++ ] ];
				Bootstrap.Vector3 p2 = positions[ indices[ idx++ ] ];

				// transform them into worldspace.
				p0 = transform.TransformCoord( p0 );
				p1 = transform.TransformCoord( p1 );
				p2 = transform.TransformCoord( p2 );

				// render the triangle.
				engine.DrawLine( p0, p1, color );
				engine.DrawLine( p1, p2, color );
				engine.DrawLine( p2, p0, color );
			}
		}

		//----------------------------------------------------------
		// renders the midpoint of each triangle in a MeshInst.
		static void RenderTriMidpoints( Bootstrap.Camera camera, Bootstrap.MeshInst meshInst, int rangeIdx, Bootstrap.Color color )
		{
			Bootstrap.Engine engine		= Program.GameEngine;
			Bootstrap.Mesh mesh		= meshInst.Mesh;
			Bootstrap.Vector3[] positions	= mesh.VertexBuffer.Positions;
			UInt16[] indices		= mesh.IndexBuffer.Indices;
			Bootstrap.Vector3 camPos		= camera.Position;
			Bootstrap.Matrix transform	= camera.ViewProjMatrix * meshInst.Transform;

			// grab the meshInst's range.
			Bootstrap.RangeInfo range;
			if ( !mesh.GetRange( out range, rangeIdx ) )
				return;

			// store one midpoint per triangle.
			Bootstrap.Vector3[] midpoints = new Bootstrap.Vector3[ indices.Length/3 ];

			float invThird = 1.0f / 3.0f;
			uint midpointCount = 0;
			for ( uint idx = range.Start; idx < range.Start+range.Count; )
			{
				// get the triangle's vertex positions.
				Bootstrap.Vector3 p0 = positions[ indices[ idx++ ] ];
				Bootstrap.Vector3 p1 = positions[ indices[ idx++ ] ];
				Bootstrap.Vector3 p2 = positions[ indices[ idx++ ] ];

				// calculate the midpoint.
				Bootstrap.Vector3 midpoint = invThird * ( p0 + p1 + p2 );

				// skip backfaces.
				Bootstrap.Vector3 normal = Util.CalcNormal( p0, p1, p2 );
				Bootstrap.Vector3 camDir = ( camPos - midpoint ).Normalized;
				if ( normal.Dot( camDir ) < 0 )
					continue;

				// transform it into projection space.
				midpoint = transform.TransformCoord( midpoint );

				// if the midpoint is outside the frustum, ignore it.
				if ( midpoint.X < -1.0f || midpoint.X > 1.0f 
				 || midpoint.Y < -1.0f || midpoint.Y > 1.0f
				 || midpoint.Z <  0.0f || midpoint.Z > 1.0f )
				{
					continue;
				}

				midpoints[ midpointCount ] = midpoint;

				++midpointCount;
			}

			// now display each midpoint.
			Bootstrap.Render.Points( midpoints, color, 2, 0, midpointCount );
		}

		//----------------------------------------------------------
		public void AfterEngineInitialized()
		{
			// after the engine has initialized, try to load the default brush palette.
			_brushPaletteEditor.LoadDefaultPalette();

			Bootstrap.Settings settings = Bootstrap.Settings.Get();
			if ( settings != null )
			{
				string strPaintType = settings.GetUserSetting( "selectedpainttype" );
				if ( strPaintType.Length > 0 )
				{
					// uncheck all PaintTypes.
					foreach ( ToolStripItem item in pnPaintTypes.Items )
					{
						if ( item is ToolStripButton )
						{
							ToolStripButton button = ( ToolStripButton )item;
							button.Checked = false;
						}
					}

					// check the appropriate PaintType.
					switch ( strPaintType )
					{
						case "brush": bnPaintType_Brush.Checked = true; break;
						case "mask": bnPaintType_Mask.Checked = true; break;
						case "select": bnPaintType_Select.Checked = true; break;
						case "stamp": bnPaintType_Stamp.Checked = true; break;
					}

					settings.SetUserSetting( "selectedpainttype", strPaintType );
				}
			}
		}

		//=======================
		// private functionality
		//=======================

		//----------------------------------------------------------
		void UpdateBrushControls()
		{
			// update the 'enabled' status of some brush controls.
			//			pnPaintActions.Enabled = (_selectedBrush != null);
			//			pnBrushOptions.Enabled = (_selectedBrush != null);
			_brushAttributes.Enabled = ( _selectedBrush != null );

			// init the brush size.
			_brushAttributes.BrushSize = _painter.Size.Width;
		}

		//----------------------------------------------------------
		private void UpdateAttributeEditor()
		{
			pnAttributeEditor.Controls.Clear();
			if ( _currentMode == UserMode.Manipulate )
			{
				// make sure the Scale tool is in the correct state.
				if ( _selectedEntity != null )
				{
					btnScale.Enabled = _selectedEntity.Scalable;
					btnMove.Enabled = true;
					btnRotate.Enabled = true;
				}
				else
				{
					btnScale.Enabled = false;
					btnMove.Enabled = false;
					btnRotate.Enabled = false;
				}

				// setup the attribute editor.
				if ( _selectedEntity is Bootstrap.Light )
				{
					// set the light on the attribute editor.
					Bootstrap.Light light = _selectedEntity as Bootstrap.Light;
					_lightEditor.Light = light;

					// show the light editor in the attribute panel.
					dockingManager1.SetDockLabel( pnAttributeEditor, "Light Editor" );
					pnAttributeEditor.Controls.Add( _lightEditor );
				}
				else if ( _selectedEntity is Bootstrap.Model )
				{
					Bootstrap.Model model = _selectedEntity as Bootstrap.Model;
					_modelEditor.Model = model;

					// show the light editor in the attribute panel.
					dockingManager1.SetDockLabel( pnAttributeEditor, "Model Editor" );
					pnAttributeEditor.Controls.Add( _modelEditor );
				}
			}
			else if ( _currentMode == UserMode.Material )
			{
				// get the currently selected material.
				if ( _selectedMaterial != null )
				{
					// set the material we're editing to the material editor.
					_materialEditor.Material = _selectedMaterial;

					// show the material editor in the attribute panel.
					_materialEditor.Dock = DockStyle.Left;
					dockingManager1.SetDockLabel( pnAttributeEditor, "Material Editor" );
					pnAttributeEditor.Controls.Add( _materialEditor );
				}
			}
			else if ( _currentMode == UserMode.Paint )
			{
				// show the brush palette editor.
				_brushPaletteEditor.Dock = DockStyle.Left;
				dockingManager1.SetDockLabel( pnAttributeEditor, "Brush Palette" );
				pnAttributeEditor.Controls.Add( _brushPaletteEditor );

				// show the brush attributes.
				_brushAttributes.Dock = DockStyle.Top;
				pnAttributeEditor.Controls.Add( _brushAttributes );
			}
		}

		//----------------------------------------------------------
		private Bootstrap.Ray BuildPickRay( Point pos )
		{
			PointF uiPos = UIRelative( pos );
			return (Bootstrap.Ray)Program.GameCamera.BuildRay( uiPos.X, uiPos.Y );
		}

		//----------------------------------------------------------
		private void ToggleButton( ToolStripButton button )
		{
			// set the state on the button.
			button.Checked = !button.Checked;

			// figure out which button sent it and enable or disable
			// the channel.
			Bootstrap.UberTextureStage stage = Bootstrap.UberTextureStage.User;
			if ( button == this.bnDiffuseEnable )
				stage = Bootstrap.UberTextureStage.Diffuse;
			else if ( button == this.bnSpecularEnable )
				stage = Bootstrap.UberTextureStage.Specular;
			else if ( button == this.bnBumpEnable )
				stage = Bootstrap.UberTextureStage.Bump;
			else if ( button == this.bnNormalEnable )
				stage = Bootstrap.UberTextureStage.Normal;
			else if ( button == this.bnEmissiveEnable )
				stage = Bootstrap.UberTextureStage.Emissive;
			else if ( button == this.bnAmbientEnable )
				stage = Bootstrap.UberTextureStage.Ambient;
			else if ( button == this.bnUserEnable )
				stage = Bootstrap.UberTextureStage.User;
			Bootstrap.UberTexture.SetStageEnabled( stage, button.Checked );
		}

		//----------------------------------------------------------
		private void ObjectDeleted()
		{
			_selectedEntity = null;
			_gizmoMgr.UserSelection = null;
		}

		//----------------------------------------------------------
		private void SelectEntity( Bootstrap.SceneEntity entity )
		{
			// store the selected entity and notify the gizmo
			// manager.
			_selectedEntity = entity;
			_gizmoMgr.UserSelection = entity;

			// update the manipulation UI.
			if( _selectedEntity != null )
				UpdateAttributeEditor();
		}

		//----------------------------------------------------------
		private void DeleteEntity( Bootstrap.SceneEntity entity )
		{
			if( entity is Bootstrap.Model )
			{
				// remove the model from the scene.
				Bootstrap.Model model = entity as Bootstrap.Model;
				Bootstrap.Model parent = model.Parent;
				if( parent != null )
					parent.RemoveChild( model );
			}
			else if( entity is Bootstrap.Light )
			{
				// remove the light from the scene.
				Bootstrap.Light light = entity as Bootstrap.Light;
				Program.GameEngine.Scene.RemoveLight( light );
			}

			// deselect the removed entity, if required.
			if ( _selectedEntity == entity )
			{
				_selectedEntity = null;
				_gizmoMgr.UserSelection = null;

				// update the manipulation UI.
				UpdateAttributeEditor();
			}
		}

		//=======================
		// events
		//=======================

		//----------------------------------------------------------
		void Viewport_FormClosing( object sender, FormClosingEventArgs e )
		{
			// if the user has unsaved changes, ask whether they should be saved. 
			if ( _unsavedChanges )
			{
				DialogResult result = MessageBox.Show( "Save changes to scene?", "Unsaved Changes", MessageBoxButtons.YesNoCancel );
				if ( result == DialogResult.Yes )
				{
					btnSaveScene.PerformClick();
				}
				else if ( result == DialogResult.Cancel )
				{
					e.Cancel = true;
					return;
				}
			}

			// if the user has unbaked changes, ask whether they should be baked.
			if ( _unbakedChanges )
			{
				DialogResult result = MessageBox.Show( "Bake?", "Unbaked Painting", MessageBoxButtons.YesNoCancel );
				if ( result == DialogResult.Yes )
				{
					bnBake.PerformClick();
				}
				else if ( result == DialogResult.Cancel )
				{
					e.Cancel = true;
					return;
				}

			}

			// if we've already shut things down, simply return.
			if ( Program.GameEngine == null )
				return;

			// make sure everything gets disposed before shutting down
			// the engine.
			_gizmoMgr = null;
			GC.Collect();

			// make sure all thumbnails are disposed of.
			Program.ThumbnailMgr.Dispose();

			// free all bootstrap resources.
			//_painter.Dispose();

			// free the engine's resources.
			Program.GameEngine.Dispose();
			Program.GameEngine = null;
			_Done = true;
			Application.Exit();
		}

		//----------------------------------------------------------
		private void btnLoadScene_Click( object sender, EventArgs e )
		{
			if ( _loadSceneDlg == null )
			{
				_loadSceneDlg = new OpenFileDialog();
				_loadSceneDlg.Filter = "Scene Descriptor File (*.sdf)|*.sdf|All files (*.*)|*.*";
				_loadSceneDlg.RestoreDirectory = true;
				_loadSceneDlg.CheckFileExists = true;
			}

			if ( _loadSceneDlg.ShowDialog() == DialogResult.OK )
			{
				// clear the selected entity.
				_selectedEntity = null;

				// load the scene.
				_sceneManager.Load( _loadSceneDlg.FileName );

				// reset the game camera's orientation.
				Program.GameCamera.Position = new Bootstrap.Vector3( 0.0f, 7.0f, 0.0f );
				Program.GameCamera.Rotation = Bootstrap.Matrix3x3.Identity;
			}
		}

		//----------------------------------------------------------
		private void btnSaveScene_Click( object sender, EventArgs e )
		{
			_sceneManager.Save();
			Bootstrap.Brush.SaveBrushes();
			_unsavedChanges = false;
		}

		//----------------------------------------------------------
		private void bnPlaceModel_Click( object sender, EventArgs e )
		{
			// if no scene is loaded, abort.
			Bootstrap.Scene scene = Program.GameEngine.Scene;
			if ( scene == null )
				return;

			DialogResult result = _modelSelectionDlg.ShowDialog();
			if ( result != DialogResult.OK )
				return;

			Bootstrap.Model lastModel = null;
			foreach ( string modelPath in _modelSelectionDlg.Selection )
			{
				// ask the user to name this instance.
				TextEntryDialog getInstanceName = new TextEntryDialog();
				getInstanceName.Title = "Name instance of " + modelPath;
				getInstanceName.Caption = "Enter a name for this model instance.";
				getInstanceName.DefaultText = "default";
				if ( getInstanceName.ShowDialog() != DialogResult.OK )
					break;
				string instanceName = getInstanceName.TextResult.Trim();

				Bootstrap.Model model = Bootstrap.Model.Create( modelPath, instanceName, true );
				if ( model == null )
				{
					MessageBox.Show( "Failed to load model " + modelPath + " with instance name " + instanceName );
					break;
				}

				// position the model in front of the camera.  (TODO-FIX: this seems to not work very well.)
				model.Position = Program.GameCamera.Position + ( 2.0f + 20.0f*model.Bounds.HalfExtents.Mag )*Program.GameCamera.Forward;

				// add it to the scene.
				scene.Root.AddChild( model, null );

				// track the last model that was added.
				lastModel = model;
			}

			// select the last placed model and switch to the 'manipulate' tab.
			if ( lastModel != null )
			{
				_selectedEntity = lastModel;
				_gizmoMgr.UserSelection = lastModel;
				_gizmoMgr.ActiveButton = btnMove;

				// update the manipulation UI.
				if ( _selectedEntity != null )
					UpdateAttributeEditor();

				rpManipulate.PerformClick();
			}

			_unsavedChanges = true;
		}

		//----------------------------------------------------------
		private void btnCreateLight_Click( object sender, EventArgs e )
		{
			// create the light.
			Bootstrap.Light light = Bootstrap.Light.Create( _random.Next().ToString() );

			// move it to the camera's position.
			light.Position = Program.GameCamera.Position;

			// add it to the scene.
			Program.GameEngine.Scene.AddLight( light );

			_unsavedChanges = true;
		}

		//----------------------------------------------------------
		private void btnMoveRotateScale_Click( object sender, EventArgs e )
		{
			_gizmoMgr.ActiveButton = sender as ToolStripButton;
		}

		//----------------------------------------------------------
		private void btnAddChannel_ButtonClick( object sender, EventArgs e )
		{
			//btnAddChannel.DropDown.Visible = true;
		}

		//----------------------------------------------------------
		private void pnView_MouseMove( object sender, MouseEventArgs e )
		{
			Bootstrap.Engine engine = Program.GameEngine;

			// make sure we update the look state modifiers on every mouse move
			// event.
			_lookState._Modifiers = Control.ModifierKeys & Keys.Alt;

			// check to see if we're in a move state.
			if ( _lookState._LookMode != LookState.LookMode.None )
			{
				if ( _lookState._LookMode == LookState.LookMode.Rotate )
				{
					// calculate the rotation.
					float yawInDegrees = _UICursor.X - _Center.X;
					float pitchInDegrees = _UICursor.Y - _Center.Y;
                    yawInDegrees *= 0.15f;
                    pitchInDegrees *= 0.15f;

					// rotate the camera.
					Program.GameCamera.RotateRelative( yawInDegrees, pitchInDegrees );

					// reset the cursor to the center of the screen.
					_UICursor = _Center;
				}
				else if ( _lookState._LookMode == LookState.LookMode.Pan )
				{
					// calculate the movement.
					float horz = _Center.X - _UICursor.X;
					float vert = _UICursor.Y - _Center.Y;
                    horz *= 0.015f * _cameraSpeed;
                    vert *= 0.015f * _cameraSpeed;
					Program.GameCamera.Position += horz * Program.GameCamera.Side;
					Program.GameCamera.Position += vert * Program.GameCamera.Up;

					// reset the cursor to the center of the screen.
					_UICursor = _Center;
				}
				else if ( _lookState._LookMode == LookState.LookMode.Move )
				{
					// calculate the movement.
					float movement = _UICursor.X - _Center.X;
					movement += _UICursor.Y - _Center.Y; // this matches Maya.  (unfortunately; the opposite is more intuitive IMO.)
                    movement *= 0.015f * _cameraSpeed;
					Program.GameCamera.Position += movement * Program.GameCamera.Forward;

					// reset the cursor to the center of the screen.
					_UICursor = _Center;
				}

				return;
			}
			else
			{
				if ( ( ModifierKeys & Keys.Shift ) != 0 )
				{
					// if the shift key is held down, resize the brush.
					if ( _UserMode == UserMode.Paint )
					{
						// calculate the brush size delta.
						int dx = _UICursor.X - _CursorAnchor.X;
						int dy = _UICursor.Y - _CursorAnchor.Y;

						// resize.
						//tsBrushSize.Value += (dx - dy);
						_brushAttributes.AddBrushSizeUIDelta( dx - dy );

						// reset the cursor to the cursor anchor.
						_UICursor = _CursorAnchor;
					}
				}
			}

			// calculate the picking ray.
			Bootstrap.Ray ray = BuildPickRay( e.Location );

			// branch based on the current UI mode.
			if ( _currentMode == UserMode.Manipulate )
			{
				_gizmoMgr.MouseMove( ray, _UICursor.X, _UICursor.Y );
				return;
			}
		}

		//----------------------------------------------------------
		private void pnView_MouseDown( object sender, MouseEventArgs e )
		{
			// update the mouse button.
			_lookState._MouseButtons |= e.Button;
			_lookState._Modifiers = Control.ModifierKeys & Keys.Alt;

			// check to see if the current user state is a look state.  If so,
			// then we're done here.
			if ( _lookState._LookMode != LookState.LookMode.None )
			{
				_UICursor = _Center;
				return;
			}

			// if we're not in a look state, then we should check to see what
			// the current UI state is so that we know how to interpret the
			// mouse click.

			if ( bnPaintType_Select.Checked )
			{
				// if we're selecting triangles, simply record the mousedown pos.
			}

			// calculate the picking ray.
			Bootstrap.Ray ray = BuildPickRay( e.Location );

			// deal with a mouse button press.
			if ( ( e.Button & MouseButtons.Left ) != 0 )
			{
				// what mode are we in?
				if ( _currentMode == UserMode.Manipulate )
				{
					// pick against the scene.
					Bootstrap.Scene.PickResult pickResult = Program.GameEngine.Scene.Pick( ray, 100.0f,
						Bootstrap.Scene.Entities.All );

					// Give any gizmos a chance to update the position of the
					// current selection.  If they don't need to do anything,
					// the check to see if there is a new selection.
					if ( _gizmoMgr.MouseDown( ray, e.Location.X, e.Location.Y ) )
					{
						_unsavedChanges = true;
					}
					else
					{
						// pick an entity from the scene.
						Bootstrap.SceneEntity entity = pickResult.PickedEntity;

						// if the current selection changed, then update the current
						// selection.
						if ( _selectedEntity == null || entity == null || !_selectedEntity.Equals( entity ) ) // TODO: This Equals check may not be doing what you think it's doing.
						{
							SelectEntity( entity );
							return;
						}

					}
				}
				else if ( _currentMode == UserMode.Paint )
				{
					// pick against the scene, ignoring lights and other stamps.
					Bootstrap.Scene.PickResult pickResult = Program.GameEngine.Scene.Pick( ray, 100.0f,
						Bootstrap.Scene.Entities.All & 
						~Bootstrap.Scene.Entities.Lights & 
						~Bootstrap.Scene.Entities.Stamps );

					if ( _selectedBrush != null )
					{
						// if we are stamping a brush onto the world, do so.  
						if ( bnPaintType_Stamp.Checked )
						{
							Bootstrap.Matrix transform = new Bootstrap.Matrix( pickResult.HitLocation + 0.5f*pickResult.HitNormal );
							transform.Scale = new Bootstrap.Vector3( _brushAttributes.BrushSize, _brushAttributes.BrushSize, 1.0f );
							transform.SetOrientation( Program.GameCamera.Side, pickResult.HitNormal );
							if ( _selectedBrush != null )
								Bootstrap.Stamp.Create( transform, _selectedBrush );
							else
								MessageBox.Show( "Select a brush." );
						}
						else
						{
							// otherwise, paint onto the world.
							_painter.Brush = _selectedBrush;
							_painting = true;
						}
					}
				}
			}
		}

		//----------------------------------------------------------
		private void pnView_MouseUp( object sender, MouseEventArgs e )
		{
			// clear the mouse buttons.
			_lookState._MouseButtons &= ~e.Button;
			_lookState._Modifiers = Control.ModifierKeys & Keys.Alt;

			// check to see if the current user state is a look state.  If so,
			// then we're done here.
			if ( _lookState._LookMode != LookState.LookMode.None )
				return;

			// calculate the picking ray.
			Bootstrap.Ray ray = BuildPickRay( e.Location );

			// deal with a left mouse button press.
			if ( ( e.Button & MouseButtons.Left ) != 0 )
			{
				_painting = false;
				if ( _gizmoMgr != null )
					_gizmoMgr.MouseUp( ray, e.Location.X, e.Location.Y );
			}
		}

		//----------------------------------------------------------
		private void btnImportArt_DropDownItemClicked( object sender, ToolStripItemClickedEventArgs e )
		{
			rcDock.OfficeMenu.Close();

			if ( e.ClickedItem == btnImportArtScene )
			{
				// display the "import scene" dialog.
				if ( _importSceneDlg == null )
				{
					_importSceneDlg = new ImportSceneDialog();
					_importSceneDlg.InitialDirectory = Bootstrap.Settings.Get().GetWorkingFolder();
				}
				if ( _importSceneDlg.ShowDialog() != DialogResult.OK )
					return;

				Program.Output.AddLine( "Importing scene " + _importSceneDlg.SceneFileName + " ... " );
				if ( !Program.GameEngine.ImportArtScene(
					_importSceneDlg.SceneName, _importSceneDlg.SceneFileName,
					true, true,
					_importSceneDlg.OverwriteMaterials, _importSceneDlg.OverwriteLights ) )
				{
					Program.Output.AddLine( "Failed to import the scene!" );
				}
				else
				{
					// now load the scene.
					string scenePath = Bootstrap.Settings.Get().GetWorkingFolder() + "\\data\\" + _importSceneDlg.SceneName + "\\scene.sdf";
					_sceneManager.Load( scenePath );
				}
			}
			else if ( e.ClickedItem == btnImportArtModel )
			{
				// display the "import model" dialog.
				if ( _importModelDlg == null )
				{
					_importModelDlg = new ImportModelDialog();
					_importModelDlg.InitialDirectory = Bootstrap.Settings.Get().GetWorkingFolder();
				}
				if ( _importModelDlg.ShowDialog() != DialogResult.OK )
					return;

				Program.Output.AddLine( "Importing model " + _importModelDlg.ModelFileName + " ... " );
				if ( !Program.GameEngine.Scene.ImportArtModel(
					_importModelDlg.ModelName, _importModelDlg.PackageName, _importModelDlg.ModelFileName,
					true, _importModelDlg.ImportAnimations ) )
				{
					Program.Output.AddLine( "Failed to _importModelDlg the model!" );
				}
			}
			MaterialSelectionDialog.NeedPopulate = true;
		}

		//----------------------------------------------------------
		private void Viewport_DragEnter( object sender, DragEventArgs e )
		{
			if ( e.Data.GetDataPresent( DataFormats.Dib ) )
				e.Effect = DragDropEffects.All;
		}

		//----------------------------------------------------------
		private void Viewport_DragDrop( object sender, DragEventArgs e )
		{
			// look for a file name.
			if ( e.Data.GetDataPresent( DataFormats.Dib ) )
			{

			}
		}

		//----------------------------------------------------------
		private void tabSelect_Create( object sender, EventArgs e )
		{
			// enter creation mode.
			_currentMode = UserMode.Create;
			UpdateAttributeEditor();
		}

		//----------------------------------------------------------
		private void tabSelect_Manipulate( object sender, EventArgs e )
		{
			// enter manipulation mode.
			_currentMode = UserMode.Manipulate;
			UpdateAttributeEditor();
		}

		//----------------------------------------------------------
		private void tabSelect_Paint( object sender, EventArgs e )
		{
			// enter paint mode.
			_currentMode = UserMode.Paint;
			UpdateAttributeEditor();
		}

		//----------------------------------------------------------
		private void tabSelect_Material( object sender, EventArgs e )
		{
			// enter script mode.
			_currentMode = UserMode.Material;
			UpdateAttributeEditor();
		}

		//----------------------------------------------------------
		private void tabSelect_Animate( object sender, EventArgs e )
		{
			_currentMode = UserMode.Animate;
			UpdateAttributeEditor();
		}

		//----------------------------------------------------------
		private void bnEnable_Click( object sender, EventArgs e )
		{
			// get the button that sent the message.
			ToolStripButton button = sender as ToolStripButton;
			ToggleButton( button );
		}

		//----------------------------------------------------------
		private void OnKeyDown( object sender, KeyEventArgs e )
		{
			// check to see if we're handling a hotkey or not.
			if ( e.Alt )
			{
				switch ( e.KeyValue )
				{
					case '1':
						ToggleButton( bnDiffuseEnable );
						break;
					case '2':
						ToggleButton( bnSpecularEnable );
						break;
					case '3':
						ToggleButton( bnBumpEnable );
						break;
					case '4':
						ToggleButton( bnNormalEnable );
						break;
					case '5':
						ToggleButton( bnEmissiveEnable );
						break;
					case '6':
						ToggleButton( bnAmbientEnable );
						break;
					case '7':
						ToggleButton( bnUserEnable );
						break;
				}
			}
		}

		//----------------------------------------------------------
		void _View_KeyDown( object sender, KeyEventArgs e )
		{
			if ( _currentMode == UserMode.Manipulate )
			{
				switch ( e.KeyCode )
				{
					// manipulate mode + W key = Move
					case Keys.W:
						{
							btnMove.PerformClick();
						}
						break;

					// manipulate mode + E key = Rotate
					case Keys.E:
						{
							btnRotate.PerformClick();
						}
						break;

					// manipulate mode + R key = Scale
					case Keys.R:
						{
							btnScale.PerformClick();
						}
						break;
				}

				// ctrl + D = duplicate selected.
				if ( e.Control && e.KeyCode == Keys.D )
				{
					if ( _selectedEntity is Bootstrap.Model )
					{
						Bootstrap.Model model = _selectedEntity as Bootstrap.Model;

						// duplicate the selected model.
						Bootstrap.Model clone = model.Clone( model.InstanceName + "Copy", true );

						// move the duplicate to the same location.
						clone.LocalTransform = model.LocalTransform;

						// add the duplicate to the scene.
						model.Parent.AddChild( clone, model.ParentNode );

						// select the cloned model.
						SelectEntity( clone );
					}
					else if ( _selectedEntity is Bootstrap.Light )
					{
						Bootstrap.Light light = _selectedEntity as Bootstrap.Light;

						// duplicate the selected light.
						Bootstrap.Light clone = light.Clone( light.Name + "Copy" );

						// move the duplicate to the same location.
						clone.Transform = light.Transform;

						// add the duplicate to the scene.
						// TODO: figure out how to attach the light to the parent properly.
						Program.GameEngine.Scene.AddLight( clone );

						// select the cloned light.
						SelectEntity( clone );
					}
				}

				// delete = delete selected entity.
				if ( e.KeyCode == Keys.Delete )
				{
					if ( _selectedEntity != null )
						DeleteEntity( _selectedEntity );
				}
			}
			else if ( _currentMode == UserMode.Paint )
			{
				/*
				if( e.KeyCode == Keys.ShiftKey )
				{
					// start resizing the brush.
					_resizingBrush = true;
					_View.Capture = true;
					_CursorAnchor = _UICursor;
				}
				 */
			}
            if ( e.KeyCode == Keys.Add )
                _cameraSpeed *= 2.0f;
            if ( e.KeyCode == Keys.Subtract )
                _cameraSpeed *= 0.5f;
            if ( e.KeyCode == Keys.Enter )
                _cameraSpeed = 1.0f;
		}

		//----------------------------------------------------------
		void _View_KeyUp( object sender, KeyEventArgs e )
		{
		}

		//----------------------------------------------------------
		void _View_MouseEnter( object sender, EventArgs e )
		{
		}

		//----------------------------------------------------------
		void _View_MouseLeave( object sender, EventArgs e )
		{
			if ( ( ModifierKeys & Keys.ShiftKey ) != 0 )
			{
				// reset the cursor pos to the cursor anchor.
				_UICursor = _CursorAnchor;
			}
		}

		// -----------------------
		void _View_SizeChanged( object sender, EventArgs e )
		{
			// delay resizing the game engine's backbuffer for a few seconds 
			// (so that the user can drag the window edge without constantly
			// resizing the backbuffer).

			_viewportResizeTimer.Stop();
			_viewportResizeTimer.Start();
		}

		//----------------------------------------------------------
		void _viewportResizeTimer_Tick( object sender, EventArgs e )
		{
			_viewportResizeTimer.Stop();

			if ( Program.GameEngine != null )
			{
				// resize the backbuffer.
				Program.GameEngine.Resize( ( uint )_View.Width, ( uint )_View.Height );
			}

			if ( Program.GameCamera != null )
			{
				// adjust the camera's aspect ratio.
				Bootstrap.Projection proj = Program.GameCamera.Projection;
				proj.AspectRatio = _View.Width / ( float )_View.Height;
				Program.GameCamera.Projection = proj;
			}
		}

		//----------------------------------------------------------
		private void bnBake_OnClick( object sender, EventArgs e )
		{
			Bootstrap.UberTexture.BakeLayers();
			_unbakedChanges = false;
		}

		//----------------------------------------------------------
		private void bnClear_Click( object sender, EventArgs e )
		{
			Bootstrap.UberTexture.FreeLayers();
			_unbakedChanges = false;
		}

		//----------------------------------------------------------
		private void btnSettings_Click( object sender, EventArgs e )
		{
			// change the settings.
			Dialogs.Settings settingsDlg = new Dialogs.Settings( "Select working folders..." );
			settingsDlg.ShowDialog();

			// restart the engine.
		}

		//----------------------------------------------------------
		private void bnBrush_Click( object sender, EventArgs e )
		{
			// present the brush selector to the user.
			DialogResult result = _brushSelectionDlg.ShowDialog();
			if ( result == DialogResult.OK )
				_selectedBrush = _brushSelectionDlg.Selected;
			else if ( result == DialogResult.None )
				_selectedBrush = null;

			UpdateBrushControls();
		}

		//----------------------------------------------------------
		private void brushPalette_SelectionChanged( object sender, EventArgs e )
		{
			_selectedBrush = _brushPaletteEditor.Selection;
			UpdateBrushControls();
		}

		//----------------------------------------------------------
		private void selectMaterialButton_Click( object sender, EventArgs e )
		{
			DialogResult result = _materialSelectionDlg.ShowDialog();
			if ( result == DialogResult.OK )
				_selectedMaterial = _materialSelectionDlg.Selection;
			else if ( result == DialogResult.None )
				_selectedMaterial = null;

			UpdateAttributeEditor();
		}

		//----------------------------------------------------------
		private void btnPlay_Click( object sender, EventArgs e )
		{
			btnPlay.Checked = true;
			btnPause.Checked = false;
			Program.GameEngine.TimeResume();

		}

		//----------------------------------------------------------
		private void btnPause_Click( object sender, EventArgs e )
		{
			btnPlay.Checked = false;
			btnPause.Checked = true;
			Program.GameEngine.TimePause();
		}

		//----------------------------------------------------------
		void _brushAttributes_BrushSizeChanged( object sender, EventArgs e )
		{
			_painter.Size = new SizeF( _brushAttributes.BrushSize, _brushAttributes.BrushSize );
		}

		//----------------------------------------------------------
		private void bnEdit_Click( object sender, EventArgs e )
		{
			// initialize the ubertexture selection dialog.
			if ( _uberTextureDlg == null )
			{
				_uberTextureDlg = new TextureSelectionDialog( true );
				_uberTextureDlg.ShowClearSelection = false;
			}

			// show the ubertexture selection dialog.
			DialogResult result = _uberTextureDlg.ShowDialog();
			if ( result != DialogResult.OK )
				return;
			if ( _uberTextureDlg.Selection.Count == 0 )
				return;

			// get the main selection.
			string uberTextureName = _uberTextureDlg.Selection[ 0 ];
			Bootstrap.UberTexture texture = new Bootstrap.UberTexture( uberTextureName );

			// initialize the editor.
			UberTextureEditDialog editor = new UberTextureEditDialog();
			editor.UberTexture = texture;

			// show the ubertexture editor.
			bool editing = true;
			editor.Show();
			editor.FormClosing += new FormClosingEventHandler(
					delegate( object sendingObj, FormClosingEventArgs eventArgs )
					{
						editing = false;
					}
				);
			while ( editing )
			{
				Application.DoEvents();
				editor.Render();
			}
		}

		//----------------------------------------------------------
		private void bnPaintType_Click( object sender, EventArgs e )
		{
			// if the incoming PaintType is checked, disregard this click.
			ToolStripButton paintType = ( ToolStripButton )sender;
			if ( paintType.Checked )
				return;

			// uncheck all PaintTypes.
			foreach ( ToolStripItem item in pnPaintTypes.Items )
			{
				if ( item is ToolStripButton )
				{
					ToolStripButton button = ( ToolStripButton )item;
					button.Checked = false;
				}
			}

			// check the incoming PaintType.
			paintType.Checked = true;

			// store which PaintType is checked.
			Bootstrap.Settings settings = Bootstrap.Settings.Get();
			if ( settings != null )
			{
				string strPaintType = "";
				if ( paintType == bnPaintType_Brush )
					strPaintType = "brush";
				else if ( paintType == bnPaintType_Mask )
					strPaintType = "mask";
				else if ( paintType == bnPaintType_Select )
					strPaintType = "select";
				else if ( paintType == bnPaintType_Stamp )
					strPaintType = "stamp";

				settings.SetUserSetting( "selectedpainttype", strPaintType );
				settings.Save();
			}
		}

		private void toolStripMenuItem_Click( object sender, EventArgs e )
		{
			ToolStripMenuItem item = sender as ToolStripMenuItem;
			if ( item == this.wireframeToolStripMenuItem )
			{
				// enable wireframe?
				Program.GameEngine.RenderWireframe = item.Checked;
			}
		}

		private void miCast_Click( object sender, EventArgs e )
		{
			// show the file dialog.
			DetailCastDialog castOptions = new DetailCastDialog();
			if ( castOptions.ShowDialog() == DialogResult.OK )
			{
				DetailCastProgressDialog progress = new DetailCastProgressDialog( castOptions );
				progress.Show();
				progress.Cast();
			}
		}

		private void bnClearMask_Click( object sender, EventArgs e )
		{
			Bootstrap.UberTexture.ClearLayerMasks();
		}

		private void cbBlendMode_SelectedIndexChanged( object sender, EventArgs e )
		{
			if ( cbBlendMode.SelectedIndex == 0 )
				this._painter.MaskPaintMode = UberTexturePainter.MaskMode.Add;
			else
				this._painter.MaskPaintMode = UberTexturePainter.MaskMode.Subtract;
		}

		private void miUberize_Click( object sender, EventArgs e )
		{
			// show the uberize dialog.
			UberizeDialog dialog = new UberizeDialog();
			if ( dialog.ShowDialog() == DialogResult.OK )
				_sceneManager.Save();
		}

		private void btnImportArt_Click( object sender, EventArgs e )
		{

		}

		private void pnBrushOptions_ItemClicked( object sender, ToolStripItemClickedEventArgs e )
		{

		}

		private void pnBrushTransform_Click( object sender, EventArgs e )
		{

		}

		private void tbRoll_TextChanged( object sender, EventArgs e )
		{
			try
			{
				// get the rotation.
				float rotateNormalized = Convert.ToSingle( tbRoll.Text ) / 360.0f;
				float fraction = rotateNormalized - ( float )Math.Floor( rotateNormalized );

				// check the sign.
				if ( Math.Sign( rotateNormalized ) < 0 )
					fraction = 1.0f + fraction;

				// calculate the brush roll in radians.
				_brushRoll = fraction * 2.0f * ( float )Math.PI;

				// update the trackbar.
				int range = tbarRoll.Maximum - tbarRoll.Minimum;
				tbarRoll.Value = tbarRoll.Minimum + ( int )( fraction * ( float )range );
			}
			catch ( SystemException )
			{

			}
		}

		private void tbarRoll_ValueChanged( object sender, EventArgs e )
		{
			// get the trackbar value.
			int value = tbarRoll.Value - tbarRoll.Minimum;
			int range = tbarRoll.Maximum - tbarRoll.Minimum;
			float rotateNormalized = ( float )value / ( float )range;

			// calculate the brush roll in radians.
			_brushRoll = rotateNormalized * 2.0f * ( float )Math.PI;

			// assign the new text to the text box.
			tbRoll.Text = Convert.ToString( rotateNormalized * 360.0f );
		}
	}
}
