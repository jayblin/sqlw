#ifndef CMAKE_VARS_CONFIG_H_
#define CMAKE_VARS_CONFIG_H_

/**
 * The path to the top level of the current build tree.
 */
#define CMAKE_BINARY_DIR 			"C:/Users/JIORM/Projects/sqlw/build"

/**
 * The path to the binary directory currently being processed.
 * Each directory added by add_subdirectory() will create a binary directory in the build tree, and as it is being processed this variable will be set.
 */
#define CMAKE_CURRENT_BINARY_DIR 	"C:/Users/JIORM/Projects/sqlw/build"

/**
 * Full directory of the listfile currently being processed.
 */
#define CMAKE_CURRENT_LIST_DIR 		"C:/Users/JIORM/Projects/sqlw"

/**
 * The path to the source directory currently being processed.
 */
#define CMAKE_CURRENT_SOURCE_DIR 	"C:/Users/JIORM/Projects/sqlw"

/**
 * The path to the top level of the source tree.
 */
#define CMAKE_SOURCE_DIR 			"C:/Users/JIORM/Projects/sqlw"

/**
 * Full path to build directory for project.
 * This is the binary directory of the most recent project() command.
 */
#define PROJECT_BINARY_DIR 			"C:/Users/JIORM/Projects/sqlw/build"

/**
 * This is the source directory of the last call to the project() command made in the current directory scope or one of its parents
 */
#define PROJECT_SOURCE_DIR 			"C:/Users/JIORM/Projects/sqlw"

/**
 * Install directory used by install().
 * The CMAKE_INSTALL_PREFIX may be defined when configuring a build tree to set its installation prefix. Or, when using the cmake(1) command-line tool's --install mode, one may specify a different prefix using the --prefix option.
 */
#define CMAKE_INSTALL_PREFIX 		"C:/Program Files (x86)/sqlw"

#endif // CMAKE_VARS_CONFIG_H_
