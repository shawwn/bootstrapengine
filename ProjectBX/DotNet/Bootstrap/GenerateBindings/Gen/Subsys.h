namespace Bootstrap
{
    public ref class GrSubsys
    {
        BOOTOBJECT( GrSubsys, GrSubsys );
    
    public:
        ~GrSubsys();
        !GrSubsys();
    
        unsigned int        GetScreenWidth();
        unsigned int        GetScreenHeight();
        void        SetTexturingQuality( EGRQUALITY^ quality );
        void        SetShadingQuality( EGRQUALITY^ quality );
        void        SetHDRQuality( EGRQUALITY^ quality );
        void        SetHDREnable( bool^ enabled );
        void        SetUberDXTCompression( bool^ enabled );
        void        SetUberOverlaySupport( bool^ enabled );
        EGRQUALITY^     GetTexturingQuality();
        EGRQUALITY^     GetShadingQuality();
        EGRQUALITY^     GetHDRQuality();
        bool^       GetHDREnable();
        bool^       GetUberDXTCompression();
        bool^       GetUberOverlaySupport();
        void        SetGamma( float gamma );
        void        SetBrightness( float brightness );
        void        TakeScreenshot( String^ fileName );
        void        Update( unsigned int timeDelta, unsigned int sysTimeDelta );
        void        Render( GrCamera *^ camera );
        void        Resize( unsigned int width, unsigned int height );
        void        Reset( unsigned int width, unsigned int height );
        void        Reset();
        void        PreContextReset();
        void        PostContextReset( unsigned int width, unsigned int height );
        bool^       IssueCommand( String^ command );
        void        AddCommandHandler( String^ command, bool (*)( tstring ) *^ handler );
    private:
    
    };
}
