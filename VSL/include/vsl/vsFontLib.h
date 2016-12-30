/*/** ----------------------------------------------------------
 * \class VSFontLib
 *
 * Lighthouse3D
 *
 * VSFL - Very Simple Font Library
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 * This class aims at displaying text strings using
 * bitmap fonts for core versions of OpenGL.
 *
 * This lib requires:
 *
 * VSMathLib (http://www.lighthouse3d.com/very-simple-libs)
 *
 * and the following third party libs:
 *
 * GLEW (http://glew.sourceforge.net/),
 * TinyXML (http://sourceforge.net/projects/tinyxml/), and
 * DevIL (http://openil.sourceforge.net/)
 *
 * Note: The font files (xml and tga) are produced by an 
 * old software called FontStudio by Michael Pote from Nitrogen
---------------------------------------------------------------*/

#ifndef __VSFontLib__
#define __VSFontLib__

#include "vslConfig.h"

#if (__VSL_FONT_LOADING__ == 1)

#include <map>
#include <vector>
#include <string>

#ifdef __ANDROID_API__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

// include Resource Lib, from which it derives
#include "vsResourceLib.h"

// add tinyxml to parse font information file
#include <tinyxml.h>

class VSFontLib  : public VSResourceLib{

	public:
		VSFontLib();
		~VSFontLib();

		/** Loads the font data. It assumes that there is a file
		  * fontName.xml with the font data and a fontName.tga with the 
		  * font image
		  * \param fontName the file name without extension
		  * \return true if successfull, false otherwise
		*/
		virtual bool load(std::string fontName);
		
		/// Render the sentence
		virtual void render() {};

		/** Use this for fixed sized fonts
		*/
		void setFixedFont(bool fixed);

		/** Allocate a slot for a sentence
		  * \return a slot index
		*/
		unsigned int genSentence();

		/** Clear a slot 
		  * \param index a previously allocated slot index 
		*/
		void deleteSentence(unsigned int index);

		/** Prepares a sentence in a given slot index
		  * \param index the index where the sentence will be stored
		  * \param sentence the string to be displayed
		*/
		void prepareSentence(unsigned int index, std::string sentence);

		/** Render a sentence. Note that screen 
		  * coordinates (x,y), have (0,0)
		  * as the top left of the screen
		  * \param x the x screen cordinate
		  * \param y the y screen coordinate
		  * \param index a previously allocated slot index  
		*/
		void renderSentence(int x, int y, unsigned int index);

		/** Shortcut to render a string with a single function call
		  * This is equivalent to calling genSentence, 
		  * prepareSentence, renderSentence, and finally
		  * deleteSentence
		*/
		void renderAndDiscard(int x, int y, std::string sentence);

		/// Sets the emissive component of the font color
		void setColor(float *values);
		/// Sets the emissive component of the font color
		void setColor(float r, float g, float b, float a);
	protected:

		/// Fixed size?
		bool mFixedSize;

	/** This class contains information for individual chars
	  * x1,x2,y1,y2 are the texture coordinates, width, A, and C 
	  * the remaining char properties
	*/ 
	class VSFLChar {
	public:
		// TexCoords
		float x1,x2,y1,y2;
		/// Char width
		int width;
		int A,C;

	};	

	/** \brief Stores the information to render a string
	  * 
	  * A sentence stores the VAO index required to render the string
	  * and free the resources both the VAO and the attribute buffers).
	*/
	class VSFLSentence {

		private:
			/// VAO index
			GLuint mVAO;
			/// Vertex and texcoord buffers
			GLuint mBuffers[2];
			/// String size
			int mSize;

		public:
			VSFLSentence();
			~VSFLSentence();
			/// sets the instance variables
			void initSentence(GLuint vao, GLuint *buffers, int size);
			/// deletes the VAO and the buffers
			void clear();

			/// returns the VAO index
			GLuint getVAO();
			/// returns the vertex buffer index
			GLuint getVertexBuffer();
			/// returns the texCoord buffer index
			GLuint getTexCoordBuffer();
			/// returns the 
			int getSize();
	};

		/** Font char data, results from parsing 
		 * the XML file for the font */
		std::map<char, VSFLChar> mChars;
		/// font char height
		int mHeight;
		/// total chars parsed
		int mNumChars;
		/// a pointer for the VSML singleton
		//VSMathLib *pVSMathLib;

		// Sentence managment
		/// Contains all the sentences
		std::vector<VSFLSentence> mSentences;
		/// contains a list of the indexes of free slots
		std::vector<unsigned int> mDeletedSentences;

		// OpenGL Settings
		/// Font Texture index
		GLuint mFontTex;
		/// Previous DEPTH_TEST settings
		GLint mPrevDepth;
		/// Previous BLEND settings
		GLint mPrevBlend; 
		/// Previous BLEND_DST settings
		GLint mPrevBlendDst;
		/// Previous BLEND_SRC settings
		GLint mPrevBlendSrc;

		/** Prepare matrices for rendering at (x,y) window coordinates
		  * \param x the x window coordinate
		  * \param y the y window coordinate
		*/
		void prepareRender(float x, float y);

		/// Restore the original matrices prior to prepareRender
		void restoreRender();

		Material mMaterial;
};

#endif

#endif