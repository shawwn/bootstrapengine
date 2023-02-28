namespace Bootstrap
{
	public ref class Camera
	{
		BOOTOBJECT( Camera, GrCamera );
	
	public:
		~Camera();
		!Camera();
	
		void		SetPos( MVec3^ pos );
		void		SetRot( MMat3x3^ rot );
		void		Look( MVec3^ direction );
		void		LookAt( MVec3^ pos, MVec3^ target );
		void		SetFarCull( float farCull );
		void		SetProj( GrProjection^ proj );
		void		SetProjFOV( float fov );
		void		SetProjNearClip( MPlane^ nearPlane );
		void		SetProjFarClip( float farDist );
		void		SetProjAspectRatio( float ratio );
		void		BuildRay( MRay^ ray, float x, float y );
		GrCamera^		BuildReflectedCamera( MPlane^ plane );
		MVec3^		GetPos();
		MMat3x3^		GetRot();
		GrProjection^		GetProj();
		MMat4x4^		BuildWorldMatrix();
		MMat4x4^		GetViewMatrix();
		MMat4x4^		GetInvViewMatrix();
		MMat4x4^		GetProjMatrix();
		MMat4x4^		GetViewProjMatrix();
		MMat3x3^		GetNormalMatrix();
		GrFrustum^		GetFrustum();
		MVec3^		GetSideDir();
		MVec3^		GetUpDir();
		MVec3^		GetLookDir();
		void		AddUserFrustumPlane( MPlane^ plane );
	private:
	
	};
}
