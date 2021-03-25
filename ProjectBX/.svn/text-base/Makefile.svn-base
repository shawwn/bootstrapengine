SUBDIRS=Common Graphics Editor UberTexture

BUILD=o2

GPP=g++-sjlj
GCC=gcc-sjlj

#-wno-attributes to get rid of Wx warnings
CPPFLAGS_COMMON=-gstabs+ -mfpmath=sse -msse2 -mthreads -Wno-attributes -fno-omit-frame-pointer # -Wall -Wno-sign-compare -Wno-comment -Wno-reorder -Wno-unused # -Weffc++ -Wextra
CPPFLAGS_g:=$(CPPFLAGS_COMMON)
CPPFLAGS_o2:=$(CPPFLAGS_COMMON) -O20
CPPFLAGS_pg:=$(CPPFLAGS_COMMON) -pg
CPPFLAGS_pgo2:=$(CPPFLAGS_COMMON) -pg -O20
CPPFLAGS:=$(CPPFLAGS_$(BUILD))
LDFLAGS_g=
LDFLAGS_o2=
LDFLAGS_pg=-pg
LDFLAGS_pgo2=-pg

LIBDIR=build/$(BUILD)/lib/
BINDIR=build/$(BUILD)/bin/
DEPDIR=build/$(BUILD)/.d/
BUILDDIR=build/$(BUILD)/


all:	libs bins

include $(SUBDIRS:%=%/files.mk)

libs:	$(LIB_TARGETS)
bins:	 $(BIN_TARGETS)

.PHONY: all libs bins
# := to evaluate immediately instead of deferring to each use
# ( possibly should cache the substituded version?)
CPP_O := $(CPP_SOURCE:%.cpp=build/$(BUILD)/%.o)

#$(DEPDIR)%.d: %.cpp
#	@if [ ! -d $(@D) ] ; then mkdir --parents $(@D); fi
#	@echo "generating dependencies for :" $< ; $(GPP) -MM $(CPPFLAGS) $(INCLUDES) $< -MF $@ -MP -MG -MT $@ -MT '$(BUILDDIR)'$(<:%.cpp=%.o)
#
#$(BUILDDIR)%.o: %.cpp
#	@if [ ! -d $(@D) ] ; then mkdir --parents $(@D); fi
#	@echo "compiling" $< "->" $@
#	@$(GPP) -c $(CPPFLAGS) $(INCLUDES) $< -o $@

#todo: figure out how to set a dependency on dirs instead of trying to
#recreate them every time

$(DEPDIR)%.d $(BUILDDIR)%.o: %.cpp
	@if [ ! -d $(dir $(DEPDIR)$*.cpp) ] ; then mkdir --parents $(dir $(DEPDIR)$*.cpp) ; fi
	@if [ ! -d $(dir $(BUILDDIR)$*.cpp) ] ; then mkdir --parents $(dir $(BUILDDIR)$*.cpp) ; fi
	@echo "compiling" $< "->" $@
	@$(GPP) -c $(CPPFLAGS) $(INCLUDES) $< -o $(BUILDDIR)$*.o -MMD -MF $(DEPDIR)$*.d -MP -MT $(DEPDIR)$*.d
# -MT '$(BUILDDIR)'$(<:%.cpp=%.o) # -MG ?
#	@if [ ! -d $(dir $(DEPDIR)$*.cpp) ] ; then mkdir --parents $(dir $(DEPDIR)$*.cpp) ; fi
#	@if [ ! -d $(dir $(BUILDDIR)$*.cpp) ] ; then mkdir --parents $(dir $(BUILDDIR)$*.cpp) ; fi



