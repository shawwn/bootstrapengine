# Common
SUBDIR=Common

MATH=MAABox.cpp MCone.cpp MLine.cpp MLineSegment.cpp MMat3x3.cpp        \
	MMat4x4.cpp MMat4x4Array.cpp MOBox.cpp MPlane.cpp MQuat.cpp	\
	MRay.cpp MSphere.cpp MTransform.cpp MVec2.cpp MVec3.cpp		\
	MVec4.cpp

MATH_H=MAABox.h MCone.h MLine.h MLineSegment.h MMat3x3.h MMat4x4.h	\
	MMat4x4Array.h MOBox.h MPlane.h MQuat.h MRay.h MSphere.h	\
	MTransform.h MVec2.h MVec3.h MVec4.h

UTIL=UBitArray.cpp UBuffer.cpp UExpr.cpp UExprArray.cpp			\
	UExprArrayRef.cpp UExprCallback.cpp UExprNumber.cpp		\
	UExprTerm.cpp UFixed.cpp ULexer.cpp UPath.cpp URand.cpp		\
	UReader.cpp URect.cpp URefCount.cpp UTable.cpp UTime.cpp	\
	UTimer.cpp UUTF8.cpp UValue.cpp UWriter.cpp UXMLTree.cpp	\
	UBitStream.cpp

UTIL_H=UBitArray.h UBuffer.h UConstRef.h UExpr.h UExprArray.h		\
	UExprArrayRef.h UExprCallback.h UExprNumber.h UExprTerm.h	\
	UFastArray.h UFixed.h ULexer.h UPath.h URand.h UReader.h	\
	URect.h URef.h URefCount.h UStateMachine.h UTable.h UTime.h	\
	UTimer.h UUTF8.h UValue.h UWriter.h UXMLTree.h UBitStream.h

IRRXML=irrXML.cpp
IRRXML_H=CXMLReaderImpl.h fast_atof.h heapsort.h irrArray.h	\
	irrString.h irrTypes.h irrXML.h


RESOURCE=RFile.cpp RFileMgr.cpp RFileStream.cpp RRsrc.cpp RRsrcMgr.cpp
RESOURCE_H=RConfig.h RFile.h RFileMgr.h RFileStream.h RRsrc.h RRsrcMgr.h

BSCRIPT=BscArrayTable.cpp BscExprLib.cpp BscLexer.cpp BscParseLib.cpp	\
	BscVarTable.cpp
BSCRIPT_H=BscArrayTable.h BscExprLib.h BscLexer.h BscParseLib.h	\
	BscVarTable.h

AFX=common_afx.cpp
ADFX_H=common_afx.h

LIBCOMMON_SRC:=$(MATH) $(UTIL) $(IRRXML) $(RESOURCE) $(BSCRIPT) $(AFX)

LIB_TARGETS+=$(LIBDIR)libcommon.a
#prof: lib/libcommon_pg.a

$(LIBDIR)libcommon.a: $(LIBCOMMON_SRC:%.cpp=$(BUILDDIR)Common/%.o)
	@if [ ! -d $(@D) ] ; then mkdir --parents $(@D); fi
	ar cruv $@ $^

#$(BUILDDIR)Common:
#	mkdir $@

include $(LIBCOMMON_SRC:%.cpp=$(DEPDIR)Common/%.d)