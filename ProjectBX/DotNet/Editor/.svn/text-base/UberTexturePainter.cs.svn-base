using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using Bootstrap;

namespace Editor
{
	class UberTexturePainter
	{
        // variables and such.
        UberTextureRasterizer _rasterizer;
        Camera _sourceCamera;
        Camera _maskCamera;
        UberTexture _target;
        MaskMode _maskPaintMode;
        ushort[] _visibleTriangles;

		public enum PaintMode
		{
			Brush,
            Mask,
			Fill
		}

        public enum MaskMode
        {
            Add,
            Subtract,
        }

		//-----------------------
		public UberTexturePainter()
		{
			this.Plane = new Plane();
			IsRestrictedToPlane = false;

			// create the cameras.
			_maskCamera   = new Camera( new Vector3(), new Matrix3x3(), new Projection( -1.0f, 1.0f, -1.0f, 1.0f, 10.0f ) );
			_sourceCamera = new Camera( new Vector3(), new Matrix3x3(), new Projection( -1.0f, 1.0f, -1.0f, 1.0f, 10.0f ) );

			// create the rasterizer.
			_rasterizer = new UberTextureRasterizer();

			// allocate the index array.
			_visibleTriangles = new ushort[ 65536 ];

            // initialize to additive.
            _maskPaintMode = MaskMode.Add;
		}

		// -----------------------
		// transformations.

		//-----------------------
		public void Move( Vector3 position )
		{
			_maskCamera.Position = position;

			// don't allow the source camera to move as we're painting.
			if( _target == null )
				_sourceCamera.Position = position;
		}

		//-----------------------
		public void Orient( Camera camera, Vector3 position, float roll )
		{
            // calculate the rotation for the cameras.
            Bootstrap.Matrix3x3 cameraRotation = camera.Rotation;
            cameraRotation = cameraRotation * Bootstrap.Matrix3x3.MakeZRotation(roll);

            // set the transform on the mask camera.
            Vector3 offset = -camera.Forward;
            _maskCamera.Rotation = cameraRotation;
            _maskCamera.Position = position + offset;

			// don't allow the source camera to move as we're painting.
            if (_target == null)
            {
                // set the transform on the source camera if we're just
                // starting to paint.
                _sourceCamera.Rotation = cameraRotation;
                _sourceCamera.Position = position + offset;
            }
		}

		//-----------------------
		public void Orient( Vector3 forward, Vector3 side )
		{
			// don't allow the painter to rotate as we're painting.
			if( _target != null )
				return;

			// normalize our vectors.
			forward = forward.Normalized;
			side = side.Normalized;

			// if forward == side, fail.
			if( Math.Abs( forward.Dot( side ) ) > 0.999f )
				return;

			// calculate the rotation of the brush.
			Vector3 up = forward.Cross( side );
			side = up.Cross( forward );

			// compute the basis vectors of the new rotation.
			Vector3 XAxis = side.Normalized;
			Vector3 YAxis = up.Normalized;
			Vector3 ZAxis = forward.Normalized;

			// set the rotation of the brush.
			Matrix3x3 rot = new Matrix3x3();
			rot.XAxis = XAxis;
			rot.YAxis = YAxis;
			rot.ZAxis = ZAxis;
			rot = rot.Transpose;

			_sourceCamera.Rotation = rot;
			_maskCamera.Rotation = rot;
		}

		//-----------------------
		public void Orient( Vector3 forward, Camera camera )
		{
			Orient( forward, camera.Side );
		}

		//-----------------------
		public SizeF Size
		{
			get { return new SizeF( _sourceCamera.Projection.Width, _sourceCamera.Projection.Height ); }
			set 
			{
				Projection proj = _sourceCamera.Projection;
				proj.Left		= -0.5f * value.Width;
				proj.Right		=  0.5f * value.Width;
				proj.Bottom		= -0.5f * value.Height;
				proj.Top		=  0.5f * value.Height;
				_sourceCamera.Projection = proj;
				_maskCamera.Projection = proj;
			}
		}

		//-----------------------
		public Bootstrap.Brush Brush
		{
			get { return _rasterizer.Brush; }
			set { _rasterizer.Brush = value; }
		}

		//-----------------------
		public bool Painting
		{
			get { return _target != null; }
		}

        public MaskMode MaskPaintMode
        {
            get { return _maskPaintMode;  }
            set { _maskPaintMode = value; }
        }

		// -----------------------
		// painting methods.

		//-----------------------
        public void CastGeometry(string objFilePath, float maxRayDist, bool normal, bool bump, float bumpScale, bool smooth)
        {
        }

		//-----------------------
		public void Paint( MeshInst meshInst, int rangeIdx, uint[] triangleMask, PaintMode paintMode )
		{
			if ( meshInst == null )
				return;

			Mesh mesh = meshInst.Mesh;
			if ( mesh == null )
				return;

			// get the material associated with the meshInst's range.
			Material material = meshInst.GetMaterial( rangeIdx );
			if ( material == null )
				return;

			// if the material isn't ubertextured, skip it.
			if( !material.Ubertextured )
				return;

            // determine if the current range and the paint frustum
            // intersect.
            if ( !_maskCamera.Frustum.IsOBoxInside( meshInst.GetRangeOBox( rangeIdx ) ) )
                return;

			// configure the ubertexture renderer for the current material's
			// targets.
			UberTexture uberTexture = new UberTexture( material.GetPass( 0 ).UberTexture );

			if( uberTexture == null )
				return;

			_target = uberTexture;

            // get the inverse of the mesh instance's transform.  This is
            // so we can move the cameras into the mesh instance's local
            // space for culling and rasterization purposes.
            Matrix invInstXForm = new Matrix();
            if (!meshInst.Transform.Inverse(out invInstXForm))
                return;

            // get the source camera's transform.
            Matrix sourceCamXForm = new Matrix(_sourceCamera.Rotation, _sourceCamera.Position);
            Matrix maskCamXForm = new Matrix(_maskCamera.Rotation, _maskCamera.Position);

            sourceCamXForm = invInstXForm * sourceCamXForm;
            maskCamXForm = invInstXForm * maskCamXForm;

            // build cameras that are local to the mesh instance.
            Camera localSourceCamera = new Camera(sourceCamXForm.Translate, sourceCamXForm.Rotate,
                _sourceCamera.Projection);
            Camera localMaskCamera = new Camera(maskCamXForm.Translate, maskCamXForm.Rotate,
                _maskCamera.Projection);

			// set the paint targets.
			_rasterizer.RenderTarget = _target;

			// set the camera info.
            _rasterizer.SetVertexConstants(0, localSourceCamera.ViewProjMatrix);
            _rasterizer.SetVertexConstants(4, localMaskCamera.ViewProjMatrix);

			// set the blending mode to accumulate.
            float blendingMode = 1.0f;
            if (_maskPaintMode == MaskMode.Subtract)
                blendingMode = 0.0f;

			// if the paint mode is 'fill' then disable the per-pixel clip.
			float perPixelClip = 1.0f;
			if( paintMode == PaintMode.Fill )
				perPixelClip = 0.0f;

            // set the mask-only value.
            float maskValue = 0.0f;
            if( paintMode == PaintMode.Mask )
                maskValue = 1.0f;

            // setup the shader modes.
            _rasterizer.SetFragmentConstant(0, new Vector4(blendingMode, perPixelClip, maskValue, 0.0f));

            // set the camera's rotation for normals.
            Matrix cameraRot = new Matrix(localSourceCamera.Rotation, new Vector3(0.0f, 0.0f, 0.0f));
            _rasterizer.SetFragmentConstants(1, cameraRot);

			// now grab the mesh's vertices and throw them at the
			// uber-texture renderer.
            uint triCount = CalculateVisibleTriangles(localMaskCamera, meshInst.Mesh, rangeIdx);

            // now render the visible triangles.
			_rasterizer.RenderTriangleList(meshInst.Mesh.VertexBuffer, _visibleTriangles,
				0, 3*triCount);
        }

		//-----------------------
		public void EndPaint()
		{
			_target = null;
		}

		//-----------------------
		// UI rendering.
		public void Render()
		{
			Engine engine = Program.GameEngine;

			float width  = 0.5f * _maskCamera.Projection.Width;
			float height = 0.5f * _maskCamera.Projection.Height;
			Matrix world = new Matrix( _maskCamera.Rotation, _maskCamera.Position );

			Bootstrap.Color red		= new Bootstrap.Color( 1.0f, 0.0f, 0.0f, 1.0f );
			Bootstrap.Color green	= new Bootstrap.Color( 0.0f, 1.0f, 0.0f, 1.0f );
			Bootstrap.Color blue	= new Bootstrap.Color( 0.0f, 0.0f, 1.0f, 1.0f );
			Bootstrap.Color yellow	= new Bootstrap.Color( 1.0f, 1.0f, 0.0f, 1.0f );
			Bootstrap.Color teal	= new Bootstrap.Color( 0.0f, 1.0f, 1.0f, 0.0f );

			// bottom.
			DrawWireframeTri( engine, world, 
				new Vector3( 0.0f, 0.0f, 0.0f ),
				new Vector3( -width, -height, -1.0f ),
				new Vector3(  width,  height, -1.0f ),
				green, green, green );

			// left.
			DrawWireframeTri( engine, world, 
				new Vector3( 0.0f, 0.0f, 0.0f ),
				new Vector3( -width, -height, -1.0f ),
				new Vector3( -width,  height, -1.0f ),
				green, green, green );

			// top.
			DrawWireframeTri( engine, world, 
				new Vector3( 0.0f, 0.0f, 0.0f ),
				new Vector3(  width,  height, -1.0f ),
				new Vector3( -width,  height, -1.0f ),
				green, yellow, yellow );

			// right.
			DrawWireframeTri( engine, world, 
				new Vector3( 0.0f, 0.0f, 0.0f ),
				new Vector3(  width,  height, -1.0f ),
				new Vector3(  width, -height, -1.0f ),
				green, teal, teal );
		}

		//-----------------------
		public Plane Plane;
		public bool  IsRestrictedToPlane;

		//-----------------------
		void DrawWireframeTri( Engine engine, Matrix mat, Vector3 p0, Vector3 p1, Vector3 p2, 
						   Bootstrap.Color c0, Bootstrap.Color c1, Bootstrap.Color c2 )
		{
			p0 = mat.TransformCoord( p0 );
			p1 = mat.TransformCoord( p1 );
			p2 = mat.TransformCoord( p2 );

			engine.DrawLine( p0, p1, c0 );
			engine.DrawLine( p1, p2, c1 );
			engine.DrawLine( p2, p0, c2 );
		}

		//-----------------------
		uint CalculateVisibleTriangles( Camera maskCamera, Mesh mesh, int rangeIdx )
		{
			// grab the camera's frustum.
			Frustum frustum = maskCamera.Frustum;

			// grab the mesh range.
			RangeInfo range;
			if( !mesh.GetRange( out range, rangeIdx ) )
				return 0;

			uint triCount = 0;

			Vector3[] positions = mesh.VertexBuffer.Positions;
			ushort[]  indices   = mesh.IndexBuffer.Indices;
			for( uint i = range.Start; i < range.Start+range.Count; i += 3 )
			{
				ushort  i0 = indices[ i   ];
				ushort  i1 = indices[ i+1 ];
				ushort  i2 = indices[ i+2 ];
				Vector3 p0 = positions[ i0 ];
				Vector3 p1 = positions[ i1 ];
				Vector3 p2 = positions[ i2 ];

				// if 'restrict brush to plane' is on, if the triangle 
				// is not coplanar then skip it.
				if( IsRestrictedToPlane )
				{
					Plane triPlane = new Plane( p0, p1, p2 );
					if( !this.Plane.IsCoplanar( triPlane, 0.001f, 0.01f ) )
						continue;
				}

				// if the triangle is not visible, skip it.
				if( !frustum.IsTriangleInside( p0, p1, p2 ) )
					continue;

				// otherwise add the triangle to the "visible tris" list.
				_visibleTriangles[ 3*triCount   ] = i0;
				_visibleTriangles[ 3*triCount+1 ] = i1;
				_visibleTriangles[ 3*triCount+2 ] = i2;
				++triCount;
			}

			return triCount;
		}
	}
}
