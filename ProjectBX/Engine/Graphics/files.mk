SUBDIR=Graphics
RENDERER=GrAfterEffect.cpp GrCamera.cpp GrColor.cpp GrDeformer.cpp	\
	GrDeformMorph.cpp GrDeformWave.cpp GrFramebuffer.cpp		\
	GrFrustum.cpp GrGLShader.cpp grglshadermgr.cpp GrImage.cpp	\
	GrLight.cpp GrLightMgr.cpp GrMaterial.cpp GrMaterialMgr.cpp	\
	GrMaterialPass.cpp GrMaterialUserParams.cpp GrMesh.cpp		\
	GrMeshBufferMgr.cpp GrMeshCombiner.cpp GrMeshIB.cpp		\
	GrMeshInst.cpp GrMeshVB.cpp GrPick.cpp GrProjection.cpp		\
	GrQuery.cpp GrQueryMgr.cpp GrRadianceElement.cpp		\
	GrRenderAmbience.cpp GrRenderbuffer.cpp GrRenderer.cpp		\
	GrRendererGL20.cpp GrRendererImpl.cpp GrRenderLight.cpp		\
	GrRenderList.cpp GrRenderShadow.cpp grrendertarget.cpp		\
	GrRenderTargetMgr.cpp GrShadowBuffer.cpp GrSkin.cpp		\
	GrSkinInst.cpp GrStreamingIB.cpp GrTexture.cpp GrTexture3D.cpp	\
	GrTextureBase.cpp GrTextureCube.cpp GrTextureMgr.cpp		\
	GrTextureRect.cpp GrTextureStage.cpp GrUtil.cpp			\
	GrVirtualShadowPage.cpp GrVirtualShadowSegmentSet.cpp		\
	GrRenderUberTexture.cpp GrUberTilePacker.cpp GrTextureSet.cpp	\
	GrUberTile.cpp GrUberTileFetcher.cpp GrUberTileLayer.cpp	\
	GrUberTileLayerMgr.cpp



RENDERER_H=GrAfterEffect.h GrCamera.h GrColor.h GrDeformer.h		\
	GrDeformMorph.h GrDeformWave.h GrFramebuffer.h GrFrustum.h	\
	GrGLShader.h GrGLShaderMgr.h GrImage.h GrLight.h GrLightMgr.h	\
	GrMaterial.h GrMaterialMgr.h GrMaterialPass.h			\
	GrMaterialUserParams.h GrMesh.h GrMeshBufferMgr.h		\
	GrMeshCombiner.h GrMeshIB.h GrMeshInst.h GrMeshVB.h GrPick.h	\
	GrProjection.h GrQuery.h GrQueryMgr.h GrRadianceElement.h	\
	GrRenderAmbience.h GrRenderbuffer.h GrRenderer.h		\
	GrRendererGL20.h GrRendererImpl.h GrRenderLight.h		\
	GrRenderList.h GrRenderShadow.h GrRenderTarget.h		\
	GrRenderTargetMgr.h GrShadowBuffer.h GrSkin.h GrSkinInst.h	\
	GrStreamingIB.h GrTexture.h GrTexture3D.h GrTextureBase.h	\
	GrTextureCube.h GrTextureMgr.h GrTextureRect.h			\
	GrTextureStage.h GrUtil.h GrVirtualShadowPage.h			\
	GrVirtualShadowSegmentSet.h GrRenderUberTexture.h		\
	GrUberTilePacker.h GrTextureSet.h GrUberTile.h			\
	GrUberTileFetcher.h GrUberTileLayer.h GrUberTileLayerMgr.h

SCENE=GrAttachment.cpp GrBounds.cpp GrBSPArea.cpp GrBSPSplit.cpp	\
	GrBSPTree.cpp GrFrustumSet.cpp GrLightSet.cpp GrModel.cpp	\
	GrModelMgr.cpp GrModelNode.cpp GrModelSkinController.cpp	\
	GrScene.cpp GrSceneLightMgr.cpp GrSceneRender.cpp grtexgen.cpp 	\
	GrTerrain.cpp GrLOctree.cpp

SCENE_H=GrAttachment.h GrBounds.h GrBSPArea.h GrBSPSplit.h GrBSPTree.h	\
	GrFrustumSet.h GrLightSet.h GrModel.h GrModelMgr.h		\
	GrModelNode.h GrModelSkinController.h GrScene.h			\
	GrSceneLightMgr.h GrSceneRender.h GrTexGen.h

GEOMETRY=GrCoplanarPolygonGroup.cpp GrPolygon.cpp GrPolygonBSPArea.cpp	\
	GrPolygonBSPPortal.cpp GrPolygonBSPPortalSet.cpp		\
	GrPolygonBSPSplit.cpp GrPolygonBSPTree.cpp GrPolygonGroup.cpp	\
	GrPolygonMesh.cpp GrPolygonNode.cpp GrPolygonUVAtlas.cpp	\
	GrPolygonUVMapper.cpp
GEOMETRY_H=GrCoplanarPolygonGroup.h GrPolygon.h GrPolygonBSPArea.h	\
	GrPolygonBSPPortal.h GrPolygonBSPPortalSet.h			\
	GrPolygonBSPSplit.h GrPolygonBSPTree.h GrPolygonGroup.h		\
	GrPolygonMesh.h GrPolygonNode.h GrPolygonUVAtlas.h		\
	GrPolygonUVMapper.h

ANIMATION=GrAnim.cpp GrAnimPlayer.cpp GrAnimXForm.cpp		\
	GrAnimXFormCache.cpp GrKFAnim.cpp GrKFAnimData.cpp	\
	GrKFAnimMgr.cpp GrKFInterpolator.cpp
ANIMATION_H=GrAnim.h GrAnimPlayer.h GrAnimXForm.h GrKFAnim.h	\
	GrKFAnimData.h GrKFAnimMgr.h GrKFInterpolator.h

NVTRISTRIP=NVTriStrip/NvTriStrip.cpp NVTriStrip/NvTriStripObjects.cpp
NVTRISTRIP_H=NVTriStrip/NvTriStrip.h NVTriStrip/NvTriStripObjects.h	\
	NVTriStrip/VertexCache.h

GL=GLSubsys.cpp
GL_H=GL.h GLAux.h glext.h GLSubsys.h GLU.h


TOOLS=GrBSPAmbOccBaker.cpp GrBSPAmbOccRaySet.cpp	\
	GrBSPAmbOccUVMapper.cpp GrBSPUniqueMapBaker.cpp	\
	GrSoftwareRasterizer.cpp GrAtlasPacker.cpp
TOOLS_H=GrBSPAmbOccBaker.h GrBSPAmbOccRaySet.h GrBSPAmbOccUVMapper.h	\
	GrBSPUniqueMapBaker.h GrAtlasPacker.h

MISC=graphics_afx.cpp GrDebug.cpp GrParse.cpp GrSubsys.cpp GrTypes.cpp
MISC_H=graphics_afx.h GrConfig.h GrDebug.h GrParse.h GrSubsys.h GrTypes.h

UBERTEXTURE= GrUberTexture.cpp GrUberTextureAtlas.cpp	\
	GrUberTextureCache.cpp GrUberTextureMgr.cpp	\
	GrUberTextureSet.cpp


UBERTEXTURE_H = GrUberTexture.h GrUberTextureAtlas.h			\
	GrUberTextureCache.h GrUberTextureMgr.h GrUberTextureSet.h


INCLUDES=-ICommon -IGraphics
LIB_TARGETS+=$(LIBDIR)libgraphics.a

LIBGRAPHICS_SOURCE=$(RENDERER) $(SCENE) $(GEOMETRY) $(ANIMATION) $(NVTRISTRIP) $(GL) $(TOOLS) $(MISC) $(UBERTEXTURE)

$(LIBDIR)libgraphics.a: $(LIBGRAPHICS_SOURCE:%.cpp=$(BUILDDIR)Graphics/%.o)
	@if [ ! -d $(@D) ] ; then mkdir --parents $(@D); fi
	ar cruv $@ $^

include $(LIBGRAPHICS_SOURCE:%.cpp=$(DEPDIR)$(SUBDIR)/%.d)