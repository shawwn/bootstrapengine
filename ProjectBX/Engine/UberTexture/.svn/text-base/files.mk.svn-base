# UberTexture
SUBDIR=UberTexture


SERVER=UTServer.cpp UTX.cpp
SERVER_H=UTServer.h UTX.h

CLIENT=UTClient.cpp
CLIENT_H=UTClient.h

COMMON=UTMessage.cpp UTNetUtils.cpp
COMMON_H=UTConfig.h UTMessage.h UTNetUtiles.h

LIBUBERTEXTURE_SRC:=$(SERVER) $(CLIENT) $(COMMON)

LIB_TARGETS+=$(LIBDIR)libubertexture.a

$(LIBDIR)libubertexture.a: $(LIBUBERTEXTURE_SRC:%.cpp=$(BUILDDIR)$(SUBDIR)/%.o)
	@if [ ! -d $(@D) ] ; then mkdir --parents $(@D); fi
	ar cruv $@ $^


include $(LIBUBERTEXTURE_SRC:%.cpp=$(DEPDIR)$(SUBDIR)/%.d)