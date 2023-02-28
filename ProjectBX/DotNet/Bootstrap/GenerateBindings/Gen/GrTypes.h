namespace Bootstrap
{
	public value class SVec3
	{
	
	public:
		float		Dist( SVec3^ vec );
		float		DistSqr( SVec3^ vec );
		float		Mag();
		float		MagSqr();
		float		Dot( SVec3^ vec );
		float		Dot( MVec3^ vec );
		float		AbsMax();
		void		Normalize();
		SVec3^		Normalized();
		void		NormalizeFast();
		SVec3^		NormalizedFast();
		SVec3^		Cross( SVec3^ v );
	private:
	
	};
}
