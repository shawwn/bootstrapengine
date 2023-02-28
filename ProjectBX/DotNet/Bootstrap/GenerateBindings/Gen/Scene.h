namespace Bootstrap
{
    public ref class GrScene
    {
        BOOTOBJECT( GrScene, GrScene );
    
    public:
        ~GrScene();
        !GrScene();
    
        UPath^      GetSceneDefPath();
        bool^       LoadScene( UPath^ path, tstring^ errors );
        void        Reset( UPath^ sceneDefPath, String^ name, GrBSPTree *^ bspTree, bool^ zUp );
        bool^       GetZUp();
        void        Update( unsigned int gameTimeDelta, unsigned int sysTimeDelta );
        void        SetName( String^ name );
        String^     GetName();
        void        SetClearColor( GrColor^ color );
        GrColor^        GetClearColor();
        unsigned int        GetSceneTime();
        unsigned int        GetTimeDelta();
        unsigned int        GetFrameId();
        unsigned int        Pick( GrModel *^ model, GrModelNode *^ modelNode, GrMeshInst *^ meshInst, GrMeshSTriCollision^ hit, MRay^ ray, unsigned int flags );
        GrBSPTree *^        GetBSPTree();
        GrModel *^      GetRootModel();
        GrModel *^      GetSkyboxModel();
        void        AddLight( URef<GrLight>^ light );
        URef<GrLight>^      FindLight( String^ id, int cmpLen );
        bool^       RemoveLight( String^ id );
        bool^       RemoveLight( GrLight *^ light );
        unsigned int        GetLightCount();
        URef<GrLight>^      GetLight( unsigned int idx );
        void        SetGlobalAmbience( GrColor^ ambience );
        GrColor^        GetGlobalAmbience();
        void        SetLightEnable( bool^ enabled );
        void        DbgSetWireframe( bool^ wireframe );
        bool^       DbgGetWireframe();
        void        DbgDrawHierarchy();
        void        ShowAffected( GrLight *^ light );
        void        Cache();
        void        Evict();
        void        Render( GrCamera *^ camera );
        void        RenderTexture( GrCamera *^ camera, MVec3^ pvsEyePos );
        void        Clear();
        void        Traverse( GrSceneTraverser *^ traverser, unsigned int elems );
        void        FindMeshInstsInFrustum( GrRenderList^ meshInsts, GrFrustum^ frustum );
    private:
    
    };
}
