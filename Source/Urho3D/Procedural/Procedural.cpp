/// Urho3D related headers
#include "../Precompiled.h"
#include "../Core/Context.h"
#include "../Resource/Decompress.h"
#include "../IO/File.h"
#include "../IO/FileSystem.h"
#include "../IO/Log.h"
#include "../Core/Profiler.h"

/// Procedural class related
#include "Interp.h"
#include "Procedural.h"
#include "Rules.h"

/// C++/C classes
#include <cstring>
#include <vector>
#include <iostream>

/// STB
#include "../../ThirdParty/STB/stb_perlin.h"

#include "OlsenNoise2D.h"
#include "RandomNumberGenerator.h"

/// Might not be needed
using namespace std;
using namespace procedural;

Procedural::Procedural()
{
    //ctor
}

Procedural::~Procedural()
{
    //dtor
}

bool Procedural::Initialize(const int &width, const int &height)
{
    /// Set class variables
    width_=width;
    height_=height;
    offsetx_=0;
    offsety_=0;
    components_=4;
    depth_=1;

    /// calculate memory needed to match pixelData and perlininput1
    unsigned int dataSizeInt = (width_*height_)*sizeof(unsigned int);
    unsigned int dataSizeFloats =  (width_*height_)*sizeof(float);

    pixelData = (unsigned char *) malloc( (width_*height_*components_)*sizeof(unsigned int));

    /// Allocate perlininput1
    perlininput1 = ( float *) malloc(dataSizeFloats);
    perlininput2 = ( float *) malloc(dataSizeFloats);
    controlinput1 = ( float *) malloc(dataSizeFloats);
    controloutput1= ( float *) malloc(dataSizeFloats);
    diamondinput1 = ( float *) malloc(dataSizeFloats);

    perlinOutput = (unsigned int *) malloc(dataSizeInt);

    return true;
}

/// Set offsets (future use)
bool Procedural::SetOffSets(const int &offsetx, const int &offsety)
{
    /// Set class offset variables
    offsetx_=offsetx;
    offsety_=offsety;

    return true;
}

bool Procedural::SetOctaves(const unsigned &octaves, const float &opersistence, const bool &ooverride, const float &o1, const float &o2, const float &o3, const  float &o4, const float &o5, const float &o6, const float &o7, const float &o8)
{
    /// Set general octave information
    oct_override=ooverride;
    oct_persistence=opersistence;
    oct_octaves=octaves;

    /// Set octave values overrided
    oct_o1=o1;
    oct_o2=o2;
    oct_o3=o3;
    oct_o4=o4;
    oct_o5=o5;
    oct_o6=o6;
    oct_o7=o7;
    oct_o8=o8;
}


/// Standard perlin generation
bool Procedural::GenerateProceduralPerlin( const float &scale)
{

    /// Generate noise - Currently 8 octaves
    generatePerlinNoise1(perlininput1,oct_octaves,offsetx_, offsety_,0,0,0,0,oct_o1,oct_o2,oct_o3,oct_o4,oct_o5,oct_o6,oct_o7,oct_o8);

    /// Scale perlin
    GenerateScale(perlininput1,scale);

    /// Build image
    GenerateBuild(perlininput1,perlinOutput); /// Produce finals 0 to 1 for image - converting to RGBA

    /// Point pixelData to perlininput1 memory
    pixelData = (unsigned char *) perlinOutput;

    return true;
}


/// Standard perlin generation
bool Procedural::GenerateProceduralOlsen(const float &iterations)
{

    /// Use Olsen method
    GenerateOrlen2DNoise1(perlininput1, offsetx_, offsety_, iterations);

    /// Build image
    GenerateBuild(perlininput1,perlinOutput); /// Produce finals 0 to 1 for image - converting to RGBA

    /// Point pixelData to perlininput1 memory
    pixelData = (unsigned char *) perlinOutput;

    return true;
}


// standard perlin generation
bool Procedural::GenerateProceduralDiamond( const float &scale)
{
    /// Generate noise - Currently 8 octaves
    generateDiamondMethod1(diamondinput1,1, 0);

    /// Scale perlin
    GenerateScale(diamondinput1,scale);

    /// Build image
    GenerateBuild(diamondinput1,perlinOutput); /// Produce finals 0 to 1 for image - converting to RGBA

    /// Point pixelData to perlininput1 memory
    pixelData = (unsigned char *) perlinOutput;

    return true;
}

/// Create procedual terrain based on rules
bool Procedural::GenerateProceduralTerrain(const terrain_rule &terrainrule)
{

    /// Allocate flatplane
    float * flatplane;

    /// Size float
    unsigned int dataSizeFloats =  (width_*height_)*sizeof(float);
    flatplane = ( float *) malloc(dataSizeFloats);

    for(unsigned int x=0; x<width_; x++)
    {
        for(unsigned int y=0; y<height_; y++)
        {
            flatplane[x+(y*width_)]=0.5f;
        }
    }

    /// Replacement random generator here
    RandomNumberGenerator RandomGenerator;

    RandomGenerator.SetRandomSeed(terrainrule.creationtime+1);

    /// Temporary use rule seed to choose octaves - set base values
    int octaves= 0;

    int baseoctave=0;
    int basepersistencerandom =0;
    float basepersistence=0;

    /// Set base values of Octaves
    float octave1 = 0;
    float octave2 = 0;
    float octave3 = 0;
    float octave4 = 0;
    float octave5 = 0;
    float octave6 = 0;
    float octave7 = 0;
    float octave8 = 0;

    /// Advane create terrain base on
    switch (terrainrule.worldtype)
    {

    case WORLD_DESERT:
        /// Produce different valllles for corols
        octaves= RandomGenerator.RandRange(2)+6;
        baseoctave=RandomGenerator.RandRange(20);
        basepersistencerandom = RandomGenerator.RandRange(20)+10;
        basepersistence=(float)basepersistencerandom/100;

        /// Set base values of Octaves
        octave1 = (float)(50+baseoctave)/100;
        octave2 = (float) octave1*basepersistence;
        octave3 = (float) octave2*basepersistence;
        octave4 = (float) octave3*basepersistence;
        octave5 = (float) octave4*basepersistence;
        octave6 = (float) octave5*basepersistence;
        octave7 = (float) octave6*basepersistence;
        octave8 = (float) octave7*basepersistence;

        /// Generate noise - Currently 8 octaves
        generatePerlinNoise1(perlininput1, oct_octaves,octaves, offsetx_,offsety_,0,0,0,oct_o1,oct_o2,oct_o3,oct_o4,oct_o5,oct_o6,oct_o7,oct_o8);
        generateControlPerlinNoise1(controlinput1,octaves,octaves, offsetx_,offsety_,0,0,0,oct_o1,oct_o2,oct_o3,oct_o4,oct_o5,oct_o6,oct_o7,oct_o8);
        GenerateOrlen2DNoise1(diamondinput1, offsetx_,offsety_, 7);

        /// Scale perlin
        GenerateScale(perlininput1,0.1f);
        GenerateScale(diamondinput1,0.50f);

        controloutput1= GenerateSelectBuild2(.4f, 1.0f, 0.3f,perlininput1,diamondinput1,controlinput1);



        break;

    default:
        /// Produce different valllles for corols
        octaves= RandomGenerator.RandRange(2)+6;
        baseoctave=RandomGenerator.RandRange(20);
        basepersistencerandom = RandomGenerator.RandRange(20)+10;
        basepersistence=(float)basepersistencerandom/100;

        /// Set base values of Octaves
        octave1 = (float)(10+baseoctave)/100;
        octave2 = (float) octave1*basepersistence;
        octave3 = (float) octave2*basepersistence;
        octave4 = (float) octave3*basepersistence;
        octave5 = (float) octave4*basepersistence;
        octave6 = (float) octave5*basepersistence;
        octave7 = (float) octave6*basepersistence;
        octave8 = (float) octave7*basepersistence;


        /// Generate noise - Currently 8 octaves
        generatePerlinNoise1(perlininput1, oct_octaves,offsetx_,offsety_,0.0f, 0,0,0,oct_o1,oct_o2,oct_o3,oct_o4,oct_o5,oct_o6,oct_o7,oct_o8);
        generateControlPerlinNoise1(controlinput1,octaves, offsetx_,offsety_,0.0f, 0,0,0,octave1,octave2,octave3,octave4,octave5,octave6,octave7,octave8);
        GenerateOrlen2DNoise1(diamondinput1, offsetx_,offsety_, 7);

        /// Scale perlin
        GenerateScale(perlininput1,0.35f);

        //controloutput1= GenerateSelectBuild2(0.0f, 1.0f, 0.15f,GenerateAdd(flatplane,perlininput1),diamondinput1,controlinput1);
        controloutput1= GenerateSelectBuild2(0.3f, 1.0f, 0.25f,perlininput1,diamondinput1,controlinput1);

        break;

    }

    /// Erode
    for (int i = 0; i < 10; i++ )
    {
        Erode(controloutput1,16.0f);
    }

    for (int i = 0; i < 9; i++ )
    {

        Smoothen(controloutput1);
    }


    GenerateBuild(controloutput1,perlinOutput); /// Produce finals 0 to 1 for image - converting to RGBA

    /// Point pixelData to perlininput1 memo
    pixelData = (unsigned char *) perlinOutput;

    free(flatplane);

    return true;
}


/// Get a image from the terrain
unsigned char * Procedural::GetImage(void)
{
    return pixelData;
}

unsigned Procedural::rgba32ToUInt(unsigned r, unsigned g, unsigned b, unsigned a)
{
    return (r&255) + ((g&255) << 8) + ((b&255) << 16) + ((a&255) << 24);
}


bool Procedural::generatePerlinNoise1(float * inputData1, const unsigned &octaves, const int &xOffset, const int &yOffset, const float &zOffset,const int &xWrap,const int &yWrap,const int &zWrap,const float &o1, const float &o2,const float &o3, const float &o4, const float &o5, const float &o6, const float &o7, const float &o8)
{
    float mag[] = {o1, o2, o3, o4, o5, o6, o7, o8};

    /// allocate memory
    float * tempdata_;
    tempdata_ = (float *) malloc ((width_*height_)*sizeof(float));

    /// set range keeping scale
    float NewMax = 1.0f;
    float NewMin = 0.0f;
    float NewRange = (NewMax - NewMin);

    float lowfloat=0.0f;
    float highfloat=0.0f;


    for (unsigned o = 0; o<octaves; o++)
    {
        float oSize = (o+1) << o;

        float oxDiv = oSize / width_;
        float oyDiv = oSize / height_;

        for(unsigned x = 0; x<width_; x++)
        {
            for(unsigned y = 0; y<height_; y++)
            {
                /// scale perlin based on octave size
                float hx = (float)x / (float)width_;
                float hy = (float)y / (float)height_;
                float noise = stb_perlin_noise3((x*oxDiv)+xOffset,(y*oyDiv)+yOffset,zOffset,xWrap,yWrap,zWrap);

                int index = x+(y*width_);

                noise *= mag[o];

                if(o)
                {
                    /// Add additinal noise
                    /// Determine high point and low point
                    tempdata_[index]= tempdata_[index]+noise;
                    if(tempdata_[index]>highfloat)
                    {
                        highfloat=tempdata_[index];
                    }
                    if(tempdata_[index]<lowfloat)
                    {
                        lowfloat=tempdata_[index];
                    }

                }
                else
                {
                    /// Set initial noise
                    /// Set high and low point
                    tempdata_[index]= noise;
                    if(tempdata_[index]>highfloat)
                    {
                        highfloat=tempdata_[index];
                    }
                    if(tempdata_[index]<lowfloat)
                    {
                        lowfloat=tempdata_[index];
                    }
                }
            }
        }

    }

    float OldRange = (highfloat-lowfloat);

    /// loop through all the values then scale down by number of octaves
    for(unsigned x = 0; x<width_; x++)
    {
        for(unsigned y = 0; y<height_; y++)
        {
            int index = x+(y*width_);
            /// slower but scales the value range from 0.0f to 1.0f
            tempdata_[index]= (((tempdata_[index] - lowfloat) * NewRange) / OldRange) + NewMin;
        }
    }

    /// copy memory
    memcpy (inputData1, tempdata_,  (width_*height_)*sizeof(float));

    /// free memory
    free(tempdata_);

    return true;
}

/// generate perlin output
bool Procedural::GenerateBuild(float * inputData1, unsigned * output)
{
    /// loop through all the floats then convert to grayscale setting the color basis to .5 (forcing values 0 to 1)
    for(unsigned y = 0; y<height_; y++)
    {
        for(unsigned x = 0; x<width_; x++)
        {

            /// incremennt memory which seems to work
            int index = x+(y*height_);

            unsigned col = inputData1[index]* 255;  /// create color value

            /// Compute precision
            float precision = ((float)inputData1[index]* 255)-col;

            col = rgba32ToUInt(col,precision*255, 0, 255);

            output[index] = col;      /// set grayscale - rgba is not needed. it seems to be screwy with this type of code.
        }
    }

    return true;
}

/// generate perlin select
float * Procedural::GenerateSelectBuild(float * inputData1,  float * inputData2, float * controldata1)
{

    /// create new memory
    float * output_;
    output_ = (float *) malloc ((width_*height_)*sizeof(float));

    /// loop through all the floats then convert to grayscale setting the color basis to .5 (forcing values 0 to 1)
    /// loop through all the floats then convert to grayscale setting the color basis to .5 (forcing values 0 to 1)
    for(unsigned x = 0; x<width_; x++)
    {
        for(unsigned y = 0; y<height_; y++)
        {
            /// incremennt memory which seems to work
            int index = x+(y*width_);

            if((controldata1[index]-.5)<0)
            {
                output_[index]=inputData1[index];
            }
            else
            {
                output_[index]=inputData2[index]*.7;
            }

        }
    }

    return output_;
}


/// Cold to create noise through the Diamond method. Requires offset and better hash table to create random height_maps but repeatable
bool Procedural::generateDiamondMethod1 (float * inputData1, const float &maxYcoords,const float &minYcoords)
{
    //an initial seed value for the corners of the data
    float SEED = 0.4f;
    //static const unsigned int DATA_SIZE=width_+1;
    static const unsigned int DATA_SIZE=width_;
    std::vector< std::vector<float> > diamond( DATA_SIZE, std::vector<float>(DATA_SIZE) );

    //initialise the values of the corners++
    diamond[0][0] = SEED;
    diamond[0][DATA_SIZE-1] = SEED;
    diamond[DATA_SIZE-1][0] = SEED;
    diamond[DATA_SIZE-1][DATA_SIZE-1] = SEED;

    float h =300; 	//the range (-h -> h) for the average offset
    ///srand(256);		//seed the random generator

    //side length is the distance of a single square side
    //or distance of diagonal in diamond
    //each iteration we are looking at smaller squares and diamonds, we decrease the variation of the offset
    for (int sideLength = DATA_SIZE-1; sideLength >= 2; sideLength /= 2, h /= 2.0)
    {

        int halfSide = sideLength/2;

        //generate new square values
        for(int x=0; x<DATA_SIZE-1; x+=sideLength)
        {
            for(int y=0; y<DATA_SIZE-1; y+=sideLength)
            {

                //x,y is upper left corner of the square
                //calculate average of existing corners
                float avg = diamond[x][y] + 				//top left
                            diamond[(x+sideLength)%DATA_SIZE][y]   +				//top right
                            diamond[x][ (y+sideLength)%DATA_SIZE]   + 				//lower left
                            diamond[(x+sideLength)%DATA_SIZE][(y+sideLength)%DATA_SIZE]; 	//lower right

                avg /= 4.0;

                //center is average plus random offset in the range (-h, h)
                float offset = (-h) + (float)rand() * (h - (-h))  / RAND_MAX;

                diamond[x+halfSide][y+halfSide] = avg + offset;

            } //for y
        } /// for x

        //Generate the diamond values
        //Since diamonds are staggered, we only move x by half side
        //NOTE: if the data shouldn't wrap the x < DATA_SIZE and y < DATA_SIZE
        for (int x=0; x<DATA_SIZE; x+=halfSide)
        {
            for (int y=(x+halfSide)%sideLength; y<DATA_SIZE; y+=sideLength)
            {

                //x,y is center of diamond
                //we must use mod and add DATA_SIZE for subtraction
                //so that we can wrap around the array to find the corners

                float avg =
                    diamond[(x-halfSide+DATA_SIZE)%DATA_SIZE][y] +	//left of center
                    diamond[(x+halfSide)%DATA_SIZE][y]				+	//right of center
                    diamond[x][(y+halfSide)%DATA_SIZE]				+	//below center
                    diamond[x][(y-halfSide+DATA_SIZE)%DATA_SIZE];	//above center

                avg /= 4.0;

                //new value = average plus random offset
                //calc random value in the range (-h,+h)
                float offset = (-h) + (float)rand() * (h - (-h))  / RAND_MAX;

                avg = avg + offset;

                //update value for center of diamond
                diamond[x][y] = avg;

                //wrap values on the edges
                //remove this and adjust loop condition above
                //for non-wrapping values
                //if (x == 0) diamond[DATA_SIZE-1][y] = avg;
                //if (y == 0) diamond[x][DATA_SIZE-1] = avg;
            } //for y
        } //for x
    } //for sideLength


    /// Set maxY and minY to 0.0f
    float maxY = diamond[1][1];
    float minY = diamond[1][1];

    for (int x = 0; x<DATA_SIZE; x++)
    {
        for(int y = 0; y<DATA_SIZE; y++)
        {
            if ((float)diamond[x][y] > maxY)
            {
                maxY = diamond[x][y];
            }
            if ((float)diamond[x][y] < minY)
            {
                minY = diamond[x][y];
            }
        }
    }

    /// Calculate height_ from 0 to 1
    for(int x=0; x < DATA_SIZE; x++)
    {
        for(int y=0; y < DATA_SIZE; y++)
        {
            //change range to 0..1
            diamond[x][y] = (diamond[x][y] - minY) / (maxY - minY);
        }
    }

    /// Copy color float from create texture
    for(unsigned y = 0; y<width_; y++)
    {
        for(unsigned x = 0; x<height_; x++)
        {
            /// incremennt memory which seems to work
            int index = (y*width_)+x;

            inputData1[index]=diamond[x][y];
        }
    }

    return true;
}

/// generate perlin select
bool Procedural::GenerateScale(float *inputData1,const float scale)
{
    /// loop through all the floats then convert to grayscale setting the color basis to .5 (forcing values 0 to 1)
    for(unsigned int x = 0; x<width_; x++)
    {
        for(unsigned int y = 0; y<height_; y++)
        {
            int index=x+(y*width_);
            inputData1[index]=inputData1[index]*scale;
        }
    }

    return true;
}

/// generate perlin select
float * Procedural::GenerateAdd(float *inputData1, float *inputData2)
{
    float * OutputData ;

    /// calculate memory needed to match pixelData and perlininput1
    unsigned int dataSizeFloats =  (width_*height_)*sizeof(float);

    OutputData = ( float *) malloc(dataSizeFloats);;


    /// loop through all the floats then convert to grayscale setting the color basis to .5 (forcing values 0 to 1)
    for(unsigned int x = 0; x<width_; x++)
    {
        for(unsigned int y = 0; y<height_; y++)
        {
            int index=x+(y*width_);
            OutputData[index]=inputData1[index]+inputData2[index];
        }
    }

    return OutputData;
}


/// generate perlin select
float * Procedural::GenerateSubtract(float *inputData1, float *inputData2)
{
    float * OutputData ;

    /// calculate memory needed to match pixelData and perlininput1
    unsigned int dataSizeFloats =  (width_*height_)*sizeof(float);

    OutputData = ( float *) malloc(dataSizeFloats);;


    /// loop through all the floats then convert to grayscale setting the color basis to .5 (forcing values 0 to 1)
    for(unsigned int x = 0; x<width_; x++)
    {
        for(unsigned int y = 0; y<height_; y++)
        {
            int index=x+(y*width_);
            OutputData[index]=inputData1[index]-inputData2[index];
        }
    }

    return OutputData;
}

bool Procedural::generateControlPerlinNoise1(float * inputData1,const unsigned &octaves, const int  &xOffset, const int  &yOffset, const float &zOffset,const int &xWrap,const int &yWrap,const int &zWrap,const float &o1, const float &o2,const float &o3, const float &o4, const float &o5, const float &o6, const float &o7, const float &o8)
{
    float mag[] = {o1, o2, o3, o4, o5, o6, o7, o8};

    /// allocate memory
    float * tempdata_;
    tempdata_ = (float *) malloc ((width_*height_)*sizeof(float));

    /// set range keeping scale
    float NewMax = 1.0f;
    float NewMin = 0.0f;
    float NewRange = (NewMax - NewMin);

    float lowfloat=0.0f;
    float highfloat=0.0f;

    int newoctaves=0;

    if(octaves<3)
    {
        newoctaves=3;
    }
    else if(octaves>4)
    {
        newoctaves=4;
    }
    else
    {
        newoctaves=octaves;
    }


    for (unsigned o = 0; o<newoctaves; o++)
    {
        float oSize = (o+1) << o;

        float oxDiv = oSize / width_;
        float oyDiv = oSize / height_;


        for(unsigned x = 0; x<width_; x++)
        {
            for(unsigned y = 0; y<height_; y++)
            {
                /// scale perlin based on octave size
                //float hx = (float)x / (float)width_;
                //float hy = (float)y / (float)height_;
                float noise = stb_perlin_noise3((float)(x*oxDiv)+xOffset,(float)(y*oyDiv)+yOffset, zOffset,xWrap,yWrap,zWrap);


                int index = x+(y*width_);

                noise *= mag[o];

                if(o)
                {
                    /// Add additinal noise
                    /// Determine high point and low point
                    tempdata_[index]= tempdata_[index]+noise;
                    if(tempdata_[index]>highfloat)
                    {
                        highfloat=tempdata_[index];
                    }
                    if(tempdata_[index]<lowfloat)
                    {
                        lowfloat=tempdata_[index];
                    }

                }
                else
                {
                    //Set initial noise
                    //Set high and low point
                    tempdata_[index]= noise;
                    if(tempdata_[index]>highfloat)
                    {
                        highfloat=tempdata_[index];
                    }
                    if(tempdata_[index]<lowfloat)
                    {
                        lowfloat=tempdata_[index];
                    }
                }
            }
        }

    }

    float OldRange = (highfloat-lowfloat);

    /// loop through all the values then scale down by number of octaves
    for(unsigned x = 0; x<width_; x++)
    {
        for(unsigned y = 0; y<height_; y++)
        {
            int index = x+(y*width_);
            /// slower but scales the value range from 0.0f to 1.0f
            tempdata_[index]= (((tempdata_[index] - lowfloat) * NewRange) / OldRange) + NewMin;
            //tempdata_[index]-=0.5f;
        }
    }

    /// copy memory
    memcpy (inputData1, tempdata_,  (width_*height_)*sizeof(float));

    /// free memory
    free(tempdata_);

    return true;
}


/// test code to add better select functionality
float * Procedural::GenerateSelectBuild2(const float &lowerBound, const float &upperBound, const float &edgeFalloff , float * inputData1, float * inputData2, float *controldata1)
{
    /// create new memory
    float * output_;
    output_ = (float *) malloc ((width_*height_)*sizeof(float));

    float m_lowerBound = lowerBound;
    float m_upperBound = upperBound;

    /// swap values of upperBounds
    if (m_lowerBound>m_upperBound)
    {
        float temp_m_upperBound=m_lowerBound;

        /// swap here
        m_lowerBound=m_upperBound;
        m_upperBound=temp_m_upperBound;

    }

    /// m_upperBound to maximum value of 1
    if (m_upperBound>1.0f)
    {
        m_upperBound=1.0f;
    }

    /// m_minimum to mininum value of 0
    if (m_lowerBound<0.0f)
    {
        m_lowerBound=0.0f;
    }

    /// get Bound size
    float BoundSize = m_upperBound - m_lowerBound;

    //set Bound edge falleof
    float m_edgeFalloff = (edgeFalloff > BoundSize / 2)? BoundSize / 2: edgeFalloff;
    float newFloat=0.0f;

    for(unsigned int x = 0; x<width_; x++)
    {
        for(unsigned int y = 0; y<height_; y++)
        {
            int index=x+(y*width_);

            /// set control value and alpha
            float controlValue = controldata1[index];
            float alpha;

            if (m_edgeFalloff > 0.0)
            {
                if (controlValue < (m_lowerBound - m_edgeFalloff))
                {
                    /// The output value from the control module is below the selector
                    /// threshold; return the output value from the first source module.
                    newFloat=inputData1[index];

                }
                else if (controlValue < (m_lowerBound + m_edgeFalloff))
                {
                    /// The output value from the control module is near the lower end of the
                    /// selector threshold and within the smooth curve. Interpolate between
                    /// the output values from the first and second source modules.
                    float lowerCurve = (m_lowerBound - m_edgeFalloff);
                    float upperCurve = (m_lowerBound + m_edgeFalloff);

                    alpha = SCurve3 ((controlValue - lowerCurve) / (upperCurve - lowerCurve));
                    newFloat= LinearInterp (inputData1[index],inputData2[index],alpha);

                }
                else if (controlValue < (m_upperBound - m_edgeFalloff))
                {
                    /// The output value from the control module is within the selector
                    /// threshold; return the output value from the second source module.
                    newFloat = inputData2[index];

                }
                else if (controlValue < (m_upperBound + m_edgeFalloff))
                {
                    /// The output value from the control module is near the upper end of the
                    /// selector threshold and within the smooth curve. Interpolate between
                    /// the output values from the first and second source modules.
                    float lowerCurve = (m_upperBound - m_edgeFalloff);
                    float upperCurve = (m_upperBound + m_edgeFalloff);
                    alpha = SCurve3 ((controlValue - lowerCurve) / (upperCurve - lowerCurve));
                    newFloat= LinearInterp (inputData2[index],inputData1[index],alpha);

                }
                else
                {
                    /// Output value from the control module is above the selector threshold;
                    /// return the output value from the first source module.
                    newFloat = inputData1[index];
                }
            }
            else
            {
                if (controlValue < m_lowerBound || controlValue > m_upperBound)
                {
                    newFloat = inputData1[index];
                }
                else
                {
                    //return m_pSourceModule[1]->GetValue (x, y, z);
                    newFloat = inputData2[index];
                }
            }

            /// create new float
            output_[index]=newFloat;
        }
    }

    return output_;
}

bool Procedural::GenerateClamp(float * inputData1, const float &lowerBound, const float &upperBound)
{
    // switch bounds if case
    float m_lowerBound = lowerBound;
    float m_upperBound = upperBound;

    /// swap values of upperBounds
    if (m_lowerBound>m_upperBound)
    {
        float temp_m_upperBound=m_lowerBound;

        /// swap here
        m_lowerBound=m_upperBound;
        m_upperBound=temp_m_upperBound;

    }

    /// loop through all the floats then convert to grayscale setting the color basis to .5 (forcing values 0 to 1)
    for(unsigned x = 0; x<width_; x++)
    {
        for(unsigned y = 0; y<height_; y++)
        {
            /// incremennt memory which seems to work
            int index = x+(y*height_);

            if ( inputData1[index] < m_lowerBound)
            {
                inputData1[index]=m_lowerBound;
            }
            else if (inputData1[index] > m_upperBound)
            {
                inputData1[index]= m_upperBound;
            }
        }
    }
    return 1;
}

bool Procedural::GenerateOrlen2DNoise1(float * inputData1, int x, int y, int iterations)
{
    /// allocate memory
    float * tempdata_;
    tempdata_ = (float *) malloc ((width_*height_)*sizeof(float));

    int * generated_;
    generated_ = (int *) malloc ((width_*height_)*sizeof(int));

    /// Generate
    OlsenNoise2D orlenNoise;

    /// Generate
    generated_=orlenNoise.olsennoise(iterations, x,y, width_, height_);


    /// loop through all the values then scale down by number of octaves
    for(int x = 0; x<width_; x++)
    {
        for(int y = 0; y<height_; y++)
        {
            /// slower but scales the value range from 0.0f to 1.0f
            tempdata_[x+(y*width_)]=(float)generated_[x+(y*width_)]/255;
        }
    }


    /// copy memory
    memmove (inputData1, tempdata_,  (width_*height_)*sizeof(float));

    /// free memory
    free(tempdata_);
    free(generated_);

    return true;
}


/// generate perlin output
bool Procedural::Erode(float * inputData1, float smoothness)
{

    for (int y = 1; y < height_-1; y++)
    {
        for (int x = 1; x < width_-1; x++)
        {
            float d_max = 0.0f;
            int match[] = { 0, 0 };

            for (int u = -1; u <= 1; u++)
            {
                for (int v = -1; v <= 1; v++)
                {
                    if(abs(u) + abs(v) > 0)
                    {
                        float d_i = inputData1[x+(y*width_)]- inputData1[x + u + ((y + v)*width_)];
                        if (d_i > d_max)
                        {
                            d_max = d_i;
                            match[0] = u;
                            match[1] = v;
                        }
                    }
                }
            }

            if(0 < d_max && d_max <= (smoothness / (float)width_))
            {
                float d_h = 0.5f * d_max;
                inputData1[x+(y*width_)]-= d_h;
                inputData1[x + match[0]+ ((y + match[1])*width_)] += d_h;
            }
        }
    }

    return true;
}

bool Procedural::Smoothen(float * inputData1)
{
    for (int y = 1; y < width_ - 1; ++y)
    {
        for (int x = 1; x < width_ - 1; ++x)
        {
            float total = 0.0f;
            for (int u = -1; u <= 1; u++)
            {
                for (int v = -1; v <= 1; v++)
                {

                    total += inputData1[x + u + ((y + v)*width_)];
                }
            }

            inputData1[x+(y*width_)] = total / 9.0f;

        }
    }

    return 0;
}
