
vector<string> split(const string& s, const string& delim, const bool keep_empty)
{
    vector<string> result;
    if (delim.empty())
    {
        result.push_back(s);
        return result;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true)
    {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty())
        {
            result.push_back(temp);
        }
        if (subend == s.end())
        {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}


time_t ConvertStringToTime(const char * buff, time_t timeseed)
{
    /// set up needed variables
    int yy, mm, dd, hour, min, sec;
    struct tm when;
    time_t tme;

    /// return failure if it can't be converted
    if(!sscanf(buff, "%d:%d:%d:%d:%d:%d", &mm, &dd, &yy, &hour, &min, &sec))
    {
        return 0;
    }

    /// copy structure
    when.tm_year = yy;
    when.tm_mon = mm-1;
    when.tm_mday = dd;
    when.tm_hour = hour;
    when.tm_min = min;
    when.tm_sec = sec;

    return mktime(&when);
}

string GenerateName(char group, char subgroup);

string ConvertUIntToString(unsigned int val)
{
    char buff[32];
    sprintf(buff, "%d", val);
    string str = buff;
    return(str);
}

/// Fuction Cutoff
/// Take input from a slope for lerping to gradiant
float cutoff(float inputvalue, float pointmid, float range, bool debug)
{
    /// Create valuables to calculate
    float midpoint=pointmid;

    float midpoint_low=midpoint-range;
    float midpoint_high=midpoint+range;
    float midpoint_range=midpoint_range=midpoint_high-midpoint_low;

    float result;

    /// Double check for overaflow
    if(midpoint_low<0.0f)
    {
        midpoint_low=0.0f;
    }
    if(midpoint_high>1.0f)
    {
        midpoint_high=1.0f;
    }


    /// Calculate gradiant based on lerp results
    if(midpoint_low>inputvalue)
    {
        result=0;
    }
    else if(inputvalue>midpoint_high)
    {
        result=1;

    }
    else
    {
        result=(inputvalue-midpoint_low)/midpoint_range;
    }

    return result;
}

/// Function StringtoFloat
/// Strign to Float
float StringToFloat(string buffer)
{

    istringstream bufferinput(buffer);
    float temp;
    bufferinput >> temp;
    return temp;
}


// range make_range( float a, float b ) { return { std::min(a,b), std::max(a,b) } ; } // C++11
range make_range( float a, float b )
{
    return range( std::min(a,b), std::max(a,b) ) ;
}

bool intersects( range a, range b )
{
    if( a > b ) std::swap(a,b) ;
    return a.second >= b.first ;
}


Vector3 NormalizedToWorld(Image *height, Terrain *terrain, Vector2 normalized)
{
    if(!terrain || !height) return Vector2(0,0);
    Vector3 spacing=terrain->GetSpacing();
    int patchSize=terrain->GetPatchSize();
    IntVector2 numPatches=IntVector2((height->GetWidth()-1) / patchSize, (height->GetHeight()-1) / patchSize);
    Vector2 patchWorldSize=Vector2(spacing.x_*(float)(patchSize*numPatches.x_), spacing.z_*(float)(patchSize*numPatches.y_));
    Vector2 patchWorldOrigin=Vector2(-0.5f * patchWorldSize.x_, -0.5f * patchWorldSize.y_);
    return Vector3(patchWorldOrigin.x_+normalized.x_*patchWorldSize.x_, 0, patchWorldOrigin.y_+normalized.y_*patchWorldSize.y_);
}

