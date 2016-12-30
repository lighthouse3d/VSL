/* --------------------------------------------------

Lighthouse3D

VSMathLib - Very Simple Font Library

http://www.lighthouse3d.com/very-simple-libs

----------------------------------------------------*/
#include "vsFontLib.h"

#if (__VSL_FONT_LOADING__ == 1)	


// Constructor
VSFontLib::VSFontLib(): VSResourceLib(),
		mHeight(0), 
		mNumChars(0), 
		mPrevDepth(false),
		mFixedSize(false)
{
	mMaterial.emissive[0] = 1.0f;
	mMaterial.emissive[1] = 1.0f;
	mMaterial.emissive[2] = 1.0f;
	mMaterial.emissive[3] = 1.0f;
	mMaterial.texCount = 1;
	mVSML = VSMathLib::getInstance();
}


// Clear chars info, and sentences
VSFontLib::~VSFontLib()
{
	mChars.clear();

	// Free resources for each sentence
	std::vector<VSFLSentence>::iterator iter = mSentences.begin();

	for ( ; iter != mSentences.end(); ++iter) {
		(*iter).clear();
	}
}


// Generate slots for sentences
// If there are deleted slots use them, otherwise
// create a new slot
// returns the index of the new slot
unsigned int
VSFontLib::genSentence() 
{
	unsigned int index;
	VSFLSentence aSentence;

	// Are there deleted slots?
	if (mDeletedSentences.size()) {
		// use the last deleted slot
		index = mDeletedSentences[mDeletedSentences.size()-1];
		// remove the slot from the deleted list
		mDeletedSentences.pop_back();
	}
	// if not create a new slot
	else {
		index = (int)mSentences.size();
		// add a slot
		mSentences.push_back(aSentence);
	}
	// return the index of the slot
	return index;
}


// Delete a Sentence
void 
VSFontLib::deleteSentence(unsigned int index)
{
	// if the index refers to a valid slot
	// i.e. the slot is within range and it has a sentence
	if (index < mSentences.size() && mSentences[index].getVAO()) {
		// clear deletes the VAO and buffers
		mSentences[index].clear();
		// add the index of the deleted slot to the list
		mDeletedSentences.push_back(index);
	}
}


// A font is specified by two files: a TGA file with the rendered 
// chars for the font, and a XML file which contains global info 
// about the font and the texture coordinates and width of each char
// The parameter fontName is the filename without extension. 
// It is assumed that the files are "fontName.xml" and "fontName.tga"
bool
VSFontLib::load(std::string fontName) 
{
	// Test if image file exists
	FILE *fp;
	std::string s;
	
	s = fontName + ".tga";
	fp = fopen(s.c_str(),"r");
	if (fp == NULL) {
		VSResourceLib::sLogError.addMessage("Unable to find font texture: %s", s.c_str());
		return false;
	}
	
	mFontTex = VSResourceLib::loadRGBATexture(s);

	s = fontName + ".xml";
	TiXmlDocument doc(s.c_str());
	bool loadOK = doc.LoadFile();

	if (!loadOK) {
		VSResourceLib::sLogError.addMessage("Problem reading the XML font definition file: %s", s.c_str());
		return false;
	}
	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);
	TiXmlElement *pElem;

	pElem = hDoc.FirstChildElement().Element();
	if (0 == pElem)
		return false;

	hRoot = TiXmlHandle(pElem);
	
	pElem->QueryIntAttribute("numchars",&mNumChars);

	if (mNumChars == 0)
		return false;

	hRoot = hRoot.FirstChild("characters");
	pElem = hRoot.FirstChild("chardata").Element();
	if (pElem)
		pElem->QueryIntAttribute("hgt",&mHeight);
	VSFLChar aChar;
	int charCode, numChars = 0;
	for(; 0 != pElem; pElem = pElem->NextSiblingElement(), ++numChars) {

		pElem->QueryIntAttribute("char",&charCode);
		pElem->QueryIntAttribute("wid",&(aChar.width));
		pElem->QueryFloatAttribute("X1", &(aChar.x1));
		pElem->QueryFloatAttribute("X2", &(aChar.x2));
		pElem->QueryFloatAttribute("Y1", &(aChar.y1));
		pElem->QueryFloatAttribute("Y2", &(aChar.y2));
		pElem->QueryIntAttribute("A", &(aChar.A));
		pElem->QueryIntAttribute("C", &(aChar.C));
		mChars[(unsigned char)charCode] = aChar;
	}
	VSResourceLib::sLogInfo.addMessage("Font has %d chars", numChars);
	return true;
}


// Matrix and OpenGL settings for rendering
void 
VSFontLib::prepareRender( float x, float y)
{
	// get previous depth test setting 
	glGetIntegerv(GL_DEPTH_TEST,&mPrevDepth);
	// disable depth testing
	glDisable(GL_DEPTH_TEST);

	// get previous blend settings
	glGetIntegerv(GL_BLEND, &mPrevBlend);
	glGetIntegerv(GL_BLEND_DST, &mPrevBlendDst);
	glGetIntegerv(GL_BLEND_SRC, &mPrevBlendSrc);
	// set blend for transparency
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// get viewport
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	// prepare projection matrix so that there is a 1:1 mapping
	// between window and vertex coordinates
	mVSML->pushMatrix(VSMathLib::PROJECTION);
	mVSML->loadIdentity(VSMathLib::PROJECTION);
	mVSML->ortho((float)vp[0], (float)vp[0] + (float)vp[2], (float)vp[1] + (float)vp[3], (float)vp[1]);
	
	// set model and view = identity matrix
	mVSML->pushMatrix(VSMathLib::MODEL);
	mVSML->loadIdentity(VSMathLib::MODEL);

	mVSML->pushMatrix(VSMathLib::VIEW);
	mVSML->loadIdentity(VSMathLib::VIEW);

	//// translate to cursor position
	mVSML->translate((float)x,(float)y,0.0f);

}


void
VSFontLib::restoreRender()
{
	// restore previous depth test settings
	if (mPrevDepth)
		glEnable(GL_DEPTH_TEST);

	// restore previous blend settings
	if (!mPrevBlend)
		glDisable(GL_BLEND);

	glBlendFunc(mPrevBlendSrc, mPrevBlendDst);

	// restore previous projection matrix
	mVSML->popMatrix(VSMathLib::PROJECTION);

	// restore previous model and view matrices
	mVSML->popMatrix(VSMathLib::MODEL);
	mVSML->popMatrix(VSMathLib::VIEW);
}


void 
VSFontLib::prepareSentence(unsigned int index, std::string sentence)
{
	float *positions, *texCoords;
	float hDisp = 0.0f, vDisp = 0.0f;
	GLuint vao, buffer[2];

	// if index is not within range
	// this should never happen if using genSentence
	if (index >= mSentences.size())
		return;

	// clear previous sentence data if reusing
	mSentences[index].clear();

	// allocate temporary arrays for vertex and texture coordinates
	int size = (int)sentence.length();
	positions = (float *)malloc(sizeof(float) * size * 6 * 3);
	texCoords = (float *)malloc(sizeof(float) * size * 6 * 2);

	int i = 0;
	for (int count = 0; count < size; count++) {
	
		// get char at position count
		char c = sentence[count];
		// if char exists in the font definition
		if (mChars.count(c)) {
			positions[18 * i + 0] = hDisp;
			positions[18 * i + 1] = vDisp + mHeight;
			positions[18 * i + 2] = 0.0f;

			positions[18 * i + 3] = hDisp + mChars[c].width;
			positions[18 * i + 4] = vDisp + 0.0f;
			positions[18 * i + 5] = 0.0f;

			positions[18 * i + 6] = hDisp;
			positions[18 * i + 7] = vDisp + 0.0f;
			positions[18 * i + 8] = 0.0f;

			positions[18 * i + 9] = hDisp + mChars[c].width;
			positions[18 * i + 10] = vDisp + 0.0f;
			positions[18 * i + 11] = 0.0f;

			positions[18 * i + 12] = hDisp;
			positions[18 * i + 13] = vDisp + mHeight;
			positions[18 * i + 14] = 0.0f;

			positions[18 * i + 15] = hDisp + mChars[c].width;
			positions[18 * i + 16] = vDisp + mHeight;
			positions[18 * i + 17] = 0.0f;

			texCoords[12 * i + 0] = mChars[c].x1;
			texCoords[12 * i + 1] = 1-mChars[c].y2;

			texCoords[12 * i + 2] = mChars[c].x2;
			texCoords[12 * i + 3] = 1-mChars[c].y1;

			texCoords[12 * i + 4] = mChars[c].x1;
			texCoords[12 * i + 5] = 1-mChars[c].y1;

			texCoords[12 * i + 6] = mChars[c].x2;
			texCoords[12 * i + 7] = 1-mChars[c].y1;

			texCoords[12 * i + 8] = mChars[c].x1;
			texCoords[12 * i + 9] = 1-mChars[c].y2;

			texCoords[12 * i + 10] = mChars[c].x2;
			texCoords[12 * i + 11] = 1-mChars[c].y2;

			if (mFixedSize)
				hDisp += mChars[c].C + mChars[c].A; 
			else
				hDisp += mChars[c].C;
			i++;
		}
		// newline
		else if (c == '\n') {
			vDisp += mHeight;
			hDisp = 0.0f;
		}
	}
	// real number of chars (excluding '\n')
	size = i;

	// create VAO
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	// create vertex buffers
	glGenBuffers(2,buffer);
	
	// positions
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size * 6 * 3, positions,GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VSShaderLib::VERTEX_COORD_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

	// texCoords
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size * 6 * 2, texCoords,GL_STATIC_DRAW);
	glEnableVertexAttribArray(VSShaderLib::TEXTURE_COORD_ATTRIB);
	glVertexAttribPointer(VSShaderLib::TEXTURE_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);

	glBindVertexArray(0);

	// init the sentence
	mSentences[index].initSentence(vao, buffer,size);

	// delete temporary arrays
	delete positions;
	delete texCoords;

}


// Render a previously prepared sentence at (x,y) window coords
// (0,0) is the top left corner of the window
void
VSFontLib::renderSentence(int x, int y, unsigned int index)
{
	if (mSentences[index].getVAO()) {

		prepareRender((float)x,(float)y);

		// set the material
		setMaterial(mMaterial);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mFontTex);

		mVSML->matricesToGL();		
		glBindVertexArray(mSentences[index].getVAO());
		glDrawArrays(GL_TRIANGLES, 0, mSentences[index].getSize()*6);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D,0);

		restoreRender();
	}
}


/* This is a shortcut to easily render a string once */
void
VSFontLib::renderAndDiscard(int x, int y, std::string sentence)
{
	unsigned int s = genSentence();
	prepareSentence(s, sentence);
	renderSentence(x,y,s);
	deleteSentence(s);
}


void 
VSFontLib::setFixedFont(bool fixed) {

	mFixedSize = fixed;
}


void 
VSFontLib::setColor(float *v) {

	mMaterial.emissive[0] = v[0];
	mMaterial.emissive[1] = v[1];
	mMaterial.emissive[2] = v[2];
	mMaterial.emissive[3] = v[3];
}


void 
VSFontLib::setColor(float r, float g, float b, float a) {

	mMaterial.emissive[0] = r;
	mMaterial.emissive[1] = g;
	mMaterial.emissive[2] = b;
	mMaterial.emissive[3] = a;
}

/* ---------------------------------------------------------------------------

	                        VSFLSentence (inner class)

----------------------------------------------------------------------------*/

// Init mVAO and mSize
VSFontLib::VSFLSentence::VSFLSentence()
{
	mVAO = 0;
	mSize = 0;
}


VSFontLib::VSFLSentence::~VSFLSentence()
{
	if (mVAO) {
		glDeleteVertexArrays(1, &mVAO);
		glDeleteBuffers(2, mBuffers);
		mVAO = 0;
	}
}


void
VSFontLib::VSFLSentence::clear()
{
	if (mVAO) {
		glDeleteVertexArrays(1, &mVAO);
		glDeleteBuffers(2, mBuffers);
		mVAO = 0;
	}
}


void
VSFontLib::VSFLSentence::initSentence(GLuint vao, GLuint *buffers, int size)
{
	mVAO = vao;
	mSize = size;
	mBuffers[0] = buffers[0];
	mBuffers[1] = buffers[1];
}


GLuint
VSFontLib::VSFLSentence::getVAO()
{
	return mVAO;
}


int
VSFontLib::VSFLSentence::getSize()
{
	return mSize;
}


GLuint
VSFontLib::VSFLSentence::getVertexBuffer()
{
	return mBuffers[0];
}


GLuint
VSFontLib::VSFLSentence::getTexCoordBuffer()
{
	return mBuffers[1];
}

#endif