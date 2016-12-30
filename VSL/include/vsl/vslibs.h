/** ----------------------------------------------------------
 * Very Simple Libraries
 *
 * Lighthouse3D
 *
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 *
 * These libs requires:
 *	GLEW (http://glew.sourceforge.net/)
 *	Assimp (http://assimp.sourceforge.net/)
 *	Devil (http://openil.sourceforge.net/)
 *	TinyXML (http://www.grinninglizard.com/tinyxml/)
 *		(only for fonts)
 ---------------------------------------------------------------*/



#include "vslConfig.h"


#include "vsGeometryLib.h"
#include "vsGLInfoLib.h"
#include "vsLogLib.h"
#include "vsMathLib.h"
#include "vsModelLib.h"
#include "vsProfileLib.h"
#include "vsResourceLib.h"
#include "vsShaderLib.h"
#include "vsSurfRevLib.h"

#if (__VSL_TEXTURE_LOADING__ == 1)
#include "vsFontLib.h"
#endif