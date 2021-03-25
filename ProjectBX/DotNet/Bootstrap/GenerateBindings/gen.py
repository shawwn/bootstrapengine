
# imports.
import os
import pygccxml
from pygccxml import parser
from pygccxml import declarations
from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies

import pdb

# set up paths.
root_dir = os.path.abspath( os.path.dirname( __file__ ) )
gccxml_bin = os.path.join( "c:", os.sep, "Program Files", "gccxml 0.9", "bin", "gccxml.exe" )

class CodeGen_Header:

    def __init__( self, namespace, namespaceName ):
	self._INDENT = "\t"
	self._namespace = namespace
	self._namespaceName = namespaceName

    def generate_code_file( self, path, enums, structs, classes ):
	result = self.generate_namespace( 0, self._namespaceName, enums, structs, classes )

	fp = open( path, "w" )
	fp.write( result )
	fp.close()

    def generate_namespace( self, depth, name, enums, structs, classes ):
	result = self.block( depth, "namespace " + name )

	for class_ in classes:
	    result += self.generate_class( depth+1, class_ )

	return self.block_end( depth, result )

    def generate_class( self, depth, class_ ):

	result = ""

	if class_._class_type == 'class':
	    result += self.block( depth, "public ref class " + class_.alias )

	    result += self.indent( depth+1, "BOOTOBJECT( " + class_.alias + ", " + class_.name + " );" )
	elif class_._class_type == 'struct':
	    result += self.block( depth, "public value class " + class_.alias )

	result += self.blank( depth )

	#################
	# public
	result += self.indent( depth, "public:" )

	if class_._class_type == 'class':
	    result += self.indent( depth+1, "~" + class_.alias + "();" )
	    result += self.indent( depth+1, "!" + class_.alias + "();" )
	    result += self.blank( depth )

	for fn in class_.member_functions().to_list():
	    if fn.access_type == 'public':
		result += self.generate_class_member_function( depth+1, class_, fn )

	#################
	# private
	result += self.indent( depth, "private:" )
	result += self.blank( depth )

	return self.block_end( depth, result, True )

    def generate_class_member_function( self, depth, class_, fn ):
	#if fn.return_type.decl_string != 'void':
	    #pdb.set_trace()

	# example of a return type string:	::GrFrustum const &
	rtype = self.to_managed_type_name( fn.return_type.decl_string )

	#if len( fn.arguments ) > 0:
	    #pdb.set_trace()

	result = rtype + "\t\t" + fn.name 

	result += "("
	if len( fn.arguments ) > 0:
	    result += ' '
	    result += ', '.join( [ self.format_arg( arg ) for arg in fn.arguments ] )
	    result += ' '
	result += ");"

	return self.indent( depth, result )

    def format_arg( self, arg ):
	return self.to_managed_type_name( arg.type.decl_string ) + " " + arg.name

    def to_managed_type_name( self, nativeType ):

	if nativeType == "char const *" or nativeType == "::tstring" or nativeType == "::tstring const &":
	    return "String^"

	nativeType = nativeType.replace( "::", "" )
	nativeType = nativeType.replace( " const", "" )
	nativeType = nativeType.replace( " &", "" )
	result = nativeType

	nt = nativeType.replace( "unsigned ", "" )

	if ( nt != 'void' and
	    nt != 'int' and
	    nt != 'float' and
	    nt != 'double' ):
	    result += "^"

	return result

    def block( self, depth, header ):
	result = self.indent( depth, header )
	result += self.indent( depth, "{" )
	return result

    def block_end( self, depth, block, semicolon=False ):
	str = "}"
	if semicolon:
	    str += ";"
	return block + self.indent( depth, str )

    def blank( self, depth ):
	return self.indent( depth, "" )

    def indent( self, depth, str ):
	return depth * self._INDENT + str + "\n"

def generate_code():

    code_tree = parser.create_cached_source_fc(
	    os.path.join( root_dir, "Engine.h" ),
	    "BootCache.xml" )

    defined_symbols = [ 'B_NO_INTERLOCKED_INTRINSICS' ]

    include_dirs = [
	    os.path.join( root_dir, "..", "..", "..", "Engine", "Common" ),
	    os.path.join( root_dir, "..", "..", "..", "Engine", "Graphics" ),
	    os.path.join( root_dir, "..", "..", "..", "Engine", "FileSystem" ) ]

    mb = module_builder.module_builder_t(
	    [ code_tree ],
	    gccxml_path		= gccxml_bin,
	    working_directory	= root_dir,
	    include_paths	= include_dirs,
	    define_symbols	= defined_symbols,
	    undefine_symbols	= [],
	    indexing_suite_version = 2,
	    cflags		= '' )

    #ns = mb.global_ns
    #ns.exclude()

    return mb

def main():

    mb = generate_code()

    ns = mb.global_ns

    pdb.set_trace()

    gen = CodeGen_Header( ns, "Bootstrap" )

    gen.generate_code_file( "Gen/Camera.h", [], [], [ ns.class_( "GrCamera" ) ] )
    gen.generate_code_file( "Gen/Scene.h", [], [], [ ns.class_( "GrScene" ) ] )
    gen.generate_code_file( "Gen/Subsys.h", [], [], [ ns.class_( "GrSubsys" ) ] )
    gen.generate_code_file( "Gen/GrTypes.h", [], [], [ ns.class_( "SVec3" ) ] )

if __name__ == '__main__':
    main()
